// FIXME: fix or move these tests

#include <external/chillerbot_reply/include/chillerbot_reply/langparser.h>
#include <gtest/gtest.h>

TEST(Lang, Greetings)
{
	// proper greetings
	EXPECT_TRUE(LangParser::IsGreeting("hi"));
	EXPECT_TRUE(LangParser::IsGreeting("hi!"));

	// possible false positives
	EXPECT_FALSE(LangParser::IsGreeting("Do you know the streamer Hallowed1986?"));
	EXPECT_FALSE(LangParser::IsGreeting("Ich hoffe das wird ein guter tag"));
}

TEST(Lang, Bye)
{
	// proper greetings
	EXPECT_TRUE(LangParser::IsBye("bye"));
	EXPECT_TRUE(LangParser::IsBye("bye!"));
	EXPECT_TRUE(LangParser::IsBye("ChillerDragon: bye"));

	// possible false positives
	EXPECT_FALSE(LangParser::IsBye("bye: hello!"));
}

TEST(Lang, Insult)
{
	// proper greetings
	EXPECT_TRUE(LangParser::IsInsult("fuck your relatives"));
	EXPECT_TRUE(LangParser::IsInsult("DELETE THE GAME"));

	// possible false positives
	EXPECT_FALSE(LangParser::IsInsult("fuck yeah! that was awesome"));
	EXPECT_FALSE(LangParser::IsInsult("dogshit i died"));
}

TEST(Lang, Why)
{
	// proper greetings
	// EXPECT_TRUE(Parser.IsQuestionWhy("wai u do dis"));
	EXPECT_TRUE(LangParser::IsQuestionWhy("why did you!"));
	EXPECT_TRUE(LangParser::IsQuestionWhy("warum ist das passiert?!"));

	// possible false positives
	// EXPECT_FALSE(Parser.IsQuestionWhy("did he tell you why?"));
	EXPECT_FALSE(LangParser::IsQuestionWhy("when did what happen?"));
}

TEST(Lang, StrFindOrder)
{
	EXPECT_TRUE(LangParser::StrFindOrder("can i ask you something", 2, "can", "ask"));
	EXPECT_TRUE(LangParser::StrFindOrder("foobarbaz", 3, "foo", "oobar", "barbaz"));
	EXPECT_TRUE(
		LangParser::StrFindOrder(
			"foo, bar, baz, qux, quux, quuz, corge, grault, garply, waldo, fred, plugh, xyzzy, and thud",
			14,
			"foo", "bar", "baz", "qux", "quux", "quuz", "corge", "grault", "garply", "waldo", "fred", "plugh", "xyzzy", "thud"));

	EXPECT_FALSE(LangParser::StrFindOrder("i ask you can something", 2, "can", "ask"));
	EXPECT_FALSE(LangParser::StrFindOrder("foo baz bar", 3, "foo", "bar", "baz"));
}

TEST(Lang, StrFindIndex)
{
	EXPECT_EQ(LangParser::StrFindIndex("foo bar", "bar"), 4);
	EXPECT_EQ(LangParser::StrFindIndex("foo bar", "404"), -1);
	EXPECT_EQ(LangParser::StrFindIndex("foo bar", "foo"), 0);
}
