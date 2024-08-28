#!/bin/bash

if [ ! -f ./scripts/chillerbot_test.sh ] || [ ! -f CMakeLists.txt ]; then
	echo "Error: make sure your are in the root of the repo"
	exit 1
fi

gdb=''
arg_build_dir=''
arg_end_args=0
arg_asan=0

for arg in "$@"; do
	if [[ "${arg::1}" == "-" ]] && [[ "$arg_end_args" == "0" ]]; then
		if [ "$arg" == "-h" ] || [ "$arg" == "--help" ]; then
			echo "usage: $(basename "$0") [OPTION..] [build dir]"
			exit 0
		elif [ "$arg" == "--gdb" ]; then
			gdb='gdb -ex=run -ex=bt -ex=quit --args'
		elif [ "$arg" == "--asan" ]; then
			arg_asan=1
		elif [ "$arg" == "--" ]; then
			arg_end_args=1
		else
			echo "Error: unknown arg '$arg'"
			exit 1
		fi
	else
		arg_build_dir="$arg"
	fi
done

function is_cmd() {
	[ -x "$(command -v "$1")" ] && return 0
}

function get_cores() {
	local cores
	if is_cmd nproc; then
		cores="$(nproc)"
	elif is_cmd sysctl; then
		cores="$(sysctl -n hw.ncpu)"
	fi
	if [ "$cores" -lt "1" ]; then
		cores=1
	fi
	echo "$cores"
}

if [ "$arg_build_dir" != "" ]; then
	if [ ! -d "$arg_build_dir" ]; then
		echo "Error: directory not found '$arg_build_dir'"
		exit 1
	fi
else
	arg_build_dir=test-chillerbot
	mkdir -p test-chillerbot
fi

cd "$arg_build_dir" || exit 1
if [ "$arg_asan" == "1" ]; then
	cp ../valgrind.supp .
	cp ../ubsan.supp .
	cp ../valgrind.supp .
	export UBSAN_OPTIONS=suppressions=./ubsan.supp:log_path=./SAN:print_stacktrace=1:halt_on_errors=0
	export ASAN_OPTIONS=log_path=./SAN:print_stacktrace=1:check_initialization_order=1:detect_leaks=1:halt_on_errors=0
	export CC=clang
	export CXX=clang++
	export CXXFLAGS="-fsanitize=address,undefined -fsanitize-recover=address,undefined -fno-omit-frame-pointer"
	export CFLAGS="-fsanitize=address,undefined -fsanitize-recover=address,undefined -fno-omit-frame-pointer"
	rm SAN.* &> /dev/null
fi
if [ ! -f Makefile ]; then
	cmake .. -DHEADLESS_CLIENT=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo || exit 1
fi
make -j"$(get_cores)" || exit 1

function cleanup() {
	if test -n "$(find . -maxdepth 1 -name 'SAN.*' -print -quit)"; then
		cat SAN.*
	fi
	echo "shutting down test clients and server ..."
	sleep 1
	echo "shutdown" > server.fifo
	echo "quit" > client1.fifo
	echo "quit" > client2.fifo
}

trap cleanup EXIT

{
	echo $'add_path $CURRENTDIR'
	echo $'add_path $DATADIR'
} > storage.cfg

# war foo
mkdir -p chillerbot/warlist/war/foo
echo foo > chillerbot/warlist/war/foo/names.txt
echo fooslongalt >> chillerbot/warlist/war/foo/names.txt
echo client1 >> chillerbot/warlist/war/foo/names.txt
echo "bullied me in school" > chillerbot/warlist/war/foo/reason.txt
echo "teamfoo" > chillerbot/warlist/warlist_clans.txt
# team bar
mkdir -p chillerbot/warlist/team/bar
echo bar > chillerbot/warlist/team/bar/names.txt
echo clientbar >> chillerbot/warlist/team/bar/names.txt
echo "# i joined foo" >> chillerbot/warlist/warlist_clans.txt
echo "# teamfoo" >> chillerbot/warlist/warlist_clans.txt

function wait_for_fifo() {
	local fifo="$1"
	local tries="$2"
	local fails=0
	# give the client time to launch and create the fifo file
	# but assume after X secs that the client crashed before
	# being able to create the file
	while [[ ! -p "$fifo" ]]; do
		fails="$((fails + 1))"
		# if [ "$arg_verbose" == "1" ]
		# then
		# 	echo "[!] client fifo not found (attempts $fails/$tries)"
		# fi
		if [ "$fails" -gt "$tries" ]; then
			print_results
			echo "[-] Error: client possibly crashed on launch"
			exit 1
		fi
		sleep 1
	done
}

./DDNet-Server "sv_input_fifo server.fifo;sv_port 17822;sv_spamprotection 0;sv_spam_mute_duration 0" > server.log &

# support chillerbot-zx
# shellcheck disable=SC2211
./chillerbot-* \
	"cl_input_fifo client1.fifo;
	cl_shownotifications 0;
	snd_volume 0;
	cl_chat_spam_filter 0;
	player_name client1;
	cl_tabbed_out_msg 0;
	cl_war_list_advanced 1;
	connect localhost:17822" > client1.log &

wait_for_fifo client1.fifo 10

# shellcheck disable=SC2211
$gdb ./chillerbot-* \
	'cl_input_fifo client2.fifo;
	cl_shownotifications 0;
	snd_volume 0;
	cl_chat_spam_filter 0;
	player_name client2;
	cl_tabbed_out_msg 0;
	connect localhost:17822;
	player_clan "Chilli.*";
	cl_war_list_advanced 1;
	inp_mousesens 1000' > client2.log &

wait_for_fifo client2.fifo 10

sleep 1

ins=()
outs=()
# should not crash on chat cmd parse too many args
ins+=('!a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a')
outs+=('')
# should not crash on chat cmd parse too long arg
ins+=('!a aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa')
outs+=('')
ins+=('I HAVE A QUESTION client2')
outs+=('client1 If you have a question just ask.')
ins+=('client2: how many jumpos do you have')
outs+=('client1 I currently have 1 out of 1 jumps')
ins+=('client2: what is your problem')
outs+=('client1 has war because: bullied me in school')
ins+=('client2 i has questio')
outs+=('client1 If you have a question just ask.')
ins+=('client2: got dj?')
outs+=('client1 Yes. I currently have 1 out of 1 jumps')
ins+=('client2: ?????')
outs+=('client1 has war because: bullied me in school')
ins+=('client2: do you play on windows or linux?')
outs+=('client1 I currently use linux.')
ins+=('client2: what??!?!?¿??')
outs+=('client1 has war because: bullied me in school')
ins+=('client2: who is clan war?')
outs+=('client1 I war those clans: teamfoo')
ins+=('client2: ?')
outs+=('client1 has war because: bullied me in school')
# ins+=('client2: is bar your friend?');outs+=('client1 yes')
ins+=('client2: is neutralPlyr on war list?')
outs+=("client1: 'neutralPlyr' is not on my warlist.")
ins+=('client2: who is against you?')
outs+=('client1 1 of my 1 enemies are online: client1')
ins+=('client2: who is with you?')
outs+=('client1 currently 0 of my 1 friends are connected')
ins+=('client2: u war me?')
outs+=('client1 has war because: bullied me in school')
ins+=('client2 who is with you?')
outs+=('client1 currently 0 of my 1 friends are connected')
ins+=('kan i di was frage client2?')
outs+=('client1 frag! Aber es kann sein, dass ich nicht antworte.')
ins+=('client2: you war me?')
outs+=('client1: client1 has war because: bullied me in school')
ins+=('client2: who do you war?')
outs+=('client1 1 of my 1 enemies are online: client1')
ins+=('client2: do you even have any friends?')
outs+=('client1 currently 0 of my 1 friends are connected')
ins+=('client2: why foo war?')
outs+=('client1: foo has war because: bullied me in school')
ins+=('client2: warum hat       foo war?')
outs+=('client1: foo has war because: bullied me in school')
ins+=('client2 hast du eig war mit samplename300 ?')
outs+=("client1: 'samplename300' is not on my warlist.")
ins+=('client2 do you war foo?')
outs+=('client1: foo has war because: bullied me in school')
ins+=('hi client2')
outs+=('hi client1')
ins+=('client2: are we peace')
outs+=('client1 you have war because: bullied me in school')
ins+=('client2 how are you?')
outs+=('client1 good, and you? :)')
ins+=('wats ur inp_mousesens? client2')
outs+=('client1 my current inp_mousesens is 1000')
ins+=('client2: why?')
outs+=('client1 has war because: bullied me in school')
ins+=('client2: why kill my friend foo')
outs+=('client1: foo has war because: bullied me in school')
ins+=('can i ask you smtin client2')
outs+=('client1 yes but I might not answer')
ins+=('can i ask you something client2')
outs+=('client1 yes but I might not answer')
ins+=('why do you war foo client2')
outs+=('client1: foo has war because: bullied me in school')
ins+=('kann ich dich was fragen client2?')
outs+=('client1 frag! Aber es kann sein, dass ich nicht antworte.')
ins+=('can me ask question client2')
outs+=('client1 yes but I might not answer')
ins+=('u have nade client2?')
outs+=('client1 No I got those weapons: hammer, gun')
ins+=('client2 hast du hammer?')
outs+=('client1 Yes I got those weapons: hammer, gun')
ins+=('why do you war fooslongalt CLIENT2')
outs+=('client1: fooslongalt has war because: bullied me in school')
ins+=('why do you war fooslongalt cLIEnT2???????')
outs+=('client1: fooslongalt has war because: bullied me in school')
ins+=('how do you chat so fast?? client2')
outs+=('client1 I bound the chillerbot-ux command "reply_to_last_ping" to automate chat')
ins+=('client2: how do always drop money?')
outs+=('client1 I auto drop money using "auto_drop_money" in chillerbot-ux')
ins+=('client2 why is my name red')
outs+=('client1 has war because: bullied me in school')
ins+=('client2 ah nice where can i download chillerbot?')
outs+=('client1 I use chillerbot-ux ( https://chillerbot.github.io )')
ins+=('client2 me is join your clan? yes?')
outs+=('client1 Chilli.* is a fun clan everybody that uses the skin greensward can join')
ins+=('client2: i is enemi?')
outs+=('client1 has war because: bullied me in school')

function run_tests() {
	local i
	local in_msg
	local out_msg
	local srv_log
	local line
	sleep 1
	for i in "${!ins[@]}"; do
		in_msg="${ins[$i]}"
		out_msg="${outs[$i]}"
		echo "say $in_msg" > client1.fifo
		sleep 0.2
		echo "reply_to_last_ping" > client2.fifo
		sleep 0.2
		srv_log="$(grep -F ' chat: ' server.log | tail -n2)"
		if ! echo "$srv_log" | grep -qF "$out_msg"; then
			# the sleep of 0.2 works fine on my laptop
			# this sleep of 1 and 2 seconds is needed in the CI
			# if the CI ever starts to fail it might be cleaner to
			# increase the base sleep from 0.2 to 0.5 again
			echo ""
			echo "Warning: message not found retrying with delay!"
			echo ""
			sleep 1
			echo "reply_to_last_ping" > client2.fifo
			sleep 2
			srv_log="$(grep -F ' chat: ' server.log | tail -n2)"
			if ! echo "$srv_log" | grep -qF "$out_msg"; then
				echo ""
				echo "Error: missing expected message in server log"
				echo ""
				echo "Sent:"
				echo "  $in_msg"
				echo "Expected:"
				echo "  $out_msg"
				echo "Got:"
				while read -r line; do
					echo "  $line"
				done < <(echo "$srv_log")
				if [ "$srv_log" == "" ]; then
					tail server.log
				fi
				echo ""
				exit 1
			else
				printf '.'
			fi
		else
			printf '.'
		fi
	done
}

run_tests

echo "player_name not_client1" > client1.fifo

ins=()
outs=()
ins+=('client2: are we peace')
outs+=('not_client1 your name is neither on my friend nor enemy list.')
ins+=('client2: is war foo?')
outs+=('not_client1: foo has war because: bullied me in school')
ins+=('client2: do you war me?')
outs+=("not_client1: 'not_client1' is not on my warlist.")
ins+=('client2: am i on warlist?')
outs+=("not_client1: 'not_client1' is not on my warlist.")
ins+=('client2 do you have war with client9 ?')
outs+=("not_client1: 'client9' is not on my warlist.")
ins+=('client2: where are you?')
outs+=('not_client1 i am literally next to you (on your left)')
ins+=('client2: client1 is ur war ?')
outs+=('not_client1: client1 has war because: bullied me in school')
ins+=('client2: do you have war with clientP ?')
outs+=("not_client1: 'clientP' is not on my warlist.")
ins+=('client2: im your enemy?')
outs+=('not_client1 your name is neither on my friend nor enemy list.')
ins+=('client2: is clientX on your warlist?')
outs+=("not_client1: 'clientX' is not on my warlist.")
ins+=('client2: client1 enemy?')
outs+=('not_client1: client1 has war because: bullied me in school')

run_tests

echo ""
echo "All tests passed :)"
