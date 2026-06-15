// Langugage parser by ChillerDragon

#include "langparser.h"

#include <base/str.h>

#include <cstdarg>
#include <cstdio>

const char *CLangParser::StrFindOrder(const char *pHaystack, int NumNeedles, ...)
{
	va_list Args;
	va_start(Args, NumNeedles);
	// dbg_msg("langparser", "strfindorder called with %d needles", NumNeedles);
	const char *pSearch = pHaystack;
	bool Found = true;
	for(int i = 0; i < NumNeedles; i++)
	{
		const char *pNeedle = va_arg(Args, const char *);
		// dbg_msg("needle", "%s", pNeedle);
		if(!(pSearch = str_find_nocase(pSearch, pNeedle)))
		{
			Found = false;
			break;
		}
	}
	va_end(Args);
	return Found ? pSearch : nullptr;
}

int CLangParser::StrFindIndex(const char *pHaystack, const char *pNeedle)
{
	int HaystackLen = str_length(pHaystack);
	int i = 0;
	for(i = 0; i < HaystackLen; i++)
		if(str_startswith(pHaystack + i, pNeedle))
			return i;
	return -1;
}

const char *CLangParser::FindWord(const char *pText, const char *pWord)
{
	const char *pHL = str_find_nocase(pText, pWord);
	while(pHL)
	{
		int Length = str_length(pWord);

		bool BoundaryStart = pText == pHL || pHL[-1] == ' ';
		bool BoundaryEnd =
			pHL[Length] == 0 ||
			pHL[Length] == ' ' ||
			pHL[Length] == '!' ||
			pHL[Length] == '?' ||
			pHL[Length] == '.' ||
			pHL[Length] == ',' ||
			pHL[Length] == '1' ||
			pHL[Length] == pHL[Length - 1];
		if(BoundaryStart && BoundaryEnd)
			return pHL;
		pHL = str_find_nocase(pHL + 1, pWord);
	}
	return nullptr;
}

bool CLangParser::IsAskToAskGerman(const char *pMessage, const char *pMessageAuthor, char *pResponse, int SizeOfResponse)
{
	if(pResponse)
		pResponse[0] = '\0';
	// ich habe eine frage
	if(!str_find(pMessage, "?"))
	{
		const char *pHave = str_find_nocase(pMessage, "hab ");
		if(!pHave)
			pHave = str_find_nocase(pMessage, "habe ");
		if(pHave)
		{
			if(str_find_nocase(pHave, "frage"))
			{
				if(pResponse)
					str_format(pResponse, SizeOfResponse, "%s frag einfach wenn du eine frage hast.", pMessageAuthor ? pMessageAuthor : "");
				return true;
			}
		}
	}
	// kann ich dich etwas
	// kan i di was
	const char *pCanSomething = StrFindOrder(pMessage, 2, "kan", "was");
	if(!pCanSomething)
		return false;
	if(str_find_nocase(pCanSomething, "frag"))
	{
		if(pResponse)
			str_format(pResponse, SizeOfResponse, "%s frag! Aber es kann sein, dass ich nicht antworte.", pMessageAuthor ? pMessageAuthor : "");
		return true;
	}
	return false;
}

bool CLangParser::IsAskToAsk(const char *pMessage, const char *pMessageAuthor, char *pResponse, int SizeOfResponse)
{
	if(pResponse)
		pResponse[0] = '\0';
	// i have a question
	if(!str_find(pMessage, "?"))
	{
		const char *pHave = str_find_nocase(pMessage, "have ");
		if(!pHave)
			pHave = str_find_nocase(pMessage, "has ");
		if(!pHave)
			pHave = str_find_nocase(pMessage, "i ");
		if(pHave)
		{
			if(str_find_nocase(pHave, "questio") || str_find_nocase(pHave, "qustion"))
			{
				if(pResponse)
					str_format(pResponse, SizeOfResponse, "%s If you have a question just ask.", pMessageAuthor ? pMessageAuthor : "");
				return true;
			}
		}
	}
	// can i ask a question
	const char *pCanAsk = StrFindOrder(pMessage, 2, "can", "ask");
	if(!pCanAsk)
		return IsAskToAskGerman(pMessage, pMessageAuthor, pResponse, SizeOfResponse);
	if(str_find_nocase(pCanAsk, "smt") ||
		str_find_nocase(pCanAsk, "sume") ||
		str_find_nocase(pCanAsk, "some") ||
		str_find_nocase(pCanAsk, "thing") ||
		str_find_nocase(pCanAsk, "question"))
	{
		if(pResponse)
			str_format(pResponse, SizeOfResponse, "%s yes but I might not answer", pMessageAuthor ? pMessageAuthor : "");
		return true;
	}
	return false;
}

bool CLangParser::IsGreeting(const char *pMsg)
{
	const char aGreetings[][128] = {
		"hi",
		"hay",
		"hey",
		"heey",
		"heeey",
		"heeeey",
		"heyho",
		"haay",
		"haaay",
		"haaaay",
		"haaaaay",
		"henlo",
		"helo",
		"hello",
		"halo",
		"hallo",
		"hellu",
		"hallu",
		"helu",
		"henlu",
		"hemnlo",
		"herro",
		"ahoi",
		"ahoy",
		"moin",
		"servus",
		"guten tag",
		"good morning",
		"guten morgen",
		"priviet",
		"zdorova", // hello
		"zdarova", // hello
		"здарова", // hello
		"ку", // hi ???
		"ola",
		"ay",
		"ayy",
		"ayyy",
		"ayyyy",
		"aayyy",
		"aaay",
		"aaaay",
		"yo",
		"yoyo",
		"yoyoyo",
		"yoo",
		"yooo",
		"salut",
		"slt",
		"sup",
		"szia",
		"salam",
		"selam"};
	for(const auto &aGreeting : aGreetings)
	{
		if(FindWord(pMsg, aGreeting))
			return true;
	}
	return false;
}

bool CLangParser::IsGreetingQq(const char *pMsg)
{
	if(FindWord(pMsg, "q"))
		return true;
	if(FindWord(pMsg, "qq"))
		return true;
	return false;
}

bool CLangParser::IsGreetingRus(const char *pMsg)
{
	const char aWords[][128] = {
		"Здравствуйте",
		"zdravstvuyte",
		"Доброе утро",
		"dobroye utro",
		"Добрый день",
		"dobriy den",
		"Привет",
		"привет",
		"privet"};
	for(const auto &aWord : aWords)
	{
		if(FindWord(pMsg, aWord))
			return true;
	}
	return false;
}

bool CLangParser::IsBye(const char *pMsg)
{
	const char aByes[][128] = {
		"bb",
		"see you",
		"leaving",
		"have a nice day",
		"have an nice day",
		"quit",
		"bye"};
	for(const auto &aBye : aByes)
	{
		if(FindWord(pMsg, aBye))
			return true;
	}
	return false;
}

bool CLangParser::IsInsult(const char *pMsg)
{
	const char aInsults[][128] = {
		"DELETE THE GAME",
		"GAYASS",
		"NIGGER",
		"NIGGA",
		"GAYNIGGER",
		"GAYNIGGA",
		"your mother",
		"ur mom",
		"fuck your",
		"fucking idiot",
		"piece of shit"};
	for(const auto &aInsult : aInsults)
	{
		if(FindWord(pMsg, aInsult))
			return true;
	}
	// /me
	if(str_startswith(pMsg, "### '"))
	{
		if(str_endswith(pMsg, "' DELETED"))
			return true;
		if(str_endswith(pMsg, "' RRRRREEEEEEEEEEEEEEEEEEEEEEEEE"))
			return true;
	}
	return false;
}

bool CLangParser::IsQuestionWhy(const char *pMsg)
{
	const char aWhys[][128] = {
		"warum",
		"whyy",
		"whyyy",
		"whyyyy",
		"w hyyyy",
		"whhy",
		"whhyy",
		"whhyyy",
		"wtf?",
		"why"};
	for(const auto &pWhy : aWhys)
	{
		if(FindWord(pMsg, pWhy))
			return true;
	}
	return false;
}

bool CLangParser::IsQuestionHow(const char *pMsg)
{
	const char aHows[][128] = {
		"wie",
		"wiee",
		"wieee",
		"wiemach",
		"how",
		"hoow",
		"hooww",
		"explain",
		"erklär",
		"tell me",
		"howto",
		"i need to know",
		"i want to know",
		"howw",
		"howww",
		"howwww"};
	for(const auto &pHow : aHows)
	{
		if(FindWord(pMsg, pHow))
			return true;
	}
	return false;
}

bool CLangParser::IsQuestionWhichWhat(const char *pMsg)
{
	const char aHows[][128] = {
		"which",
		"wich",
		"wihc",
		"wihch",
		"wat",
		"what"};
	for(const auto &pHow : aHows)
	{
		if(FindWord(pMsg, pHow))
			return true;
	}
	return false;
}

bool CLangParser::IsQuestionWhoWhichWhat(const char *pMsg)
{
	const char aHows[][128] = {
		"wen",
		"who",
		"whoo",
		"whu",
		"which",
		"wich",
		"wihc",
		"wihch",
		"wat",
		"what"};
	for(const auto &pHow : aHows)
	{
		if(FindWord(pMsg, pHow))
			return true;
	}
	return false;
}
