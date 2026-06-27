#ifndef GAME_CLIENT_COMPONENTS_CHILLERBOT_CHATHELPER_REPLYTOPING_H
#define GAME_CLIENT_COMPONENTS_CHILLERBOT_CHATHELPER_REPLYTOPING_H

/*
	CReplyToPing

	Description:
		One instance per message and reply.
		Tries to respond to common teeworlds chat messages.

	Example:
		char aResponse[1024];
		CReplyToPing ReplyToPing = CReplyToPing(this, aName, m_pClient->m_aClients[ClientId].m_aClan, pLine, aResponse, sizeof(aResponse));
		if(ReplyToPing.Reply())
			m_pClient->m_Chat.Say(0, aResponse);

	Public methods:
		Reply()
*/
class CReplyToPing
{
	class CChatHelper *m_pChatHelper;
	class CChatHelper *ChatHelper() { return m_pChatHelper; }
	class CGameClient *GameClient();

	const char *m_pMessageAuthor;
	const char *m_pMessageAuthorClan;
	const char *m_pMessage;
	char *m_pResponse;
	long unsigned int m_SizeOfResponse;

	bool WhatOs();
	// where are you
	bool Where();
	// check all wars "who is on your warlist?"
	bool ListWars();
	bool SmallTalk(int NameLen, int MsgLen);
	bool WhyWar(const char *pVictim, bool IsCheck = false);
	bool IsWarName();
	bool NameIsWar();
	/*
		WhyDoYouKillMe

		Covering all kinds of questions like only "?"
		But only triggering an response if its coming from an enemy.
	*/
	bool WhyDoYouKillMe(int NameLen, int MsgLen);
	/*
		DoYouWarMe

		intentionally check for being on warlist
		also expecting an no if not
	*/
	bool DoYouWarMe(int NameLen, int MsgLen);
	/*
		GetSuffixLen

		if pStr ends with pSuffix return length of pSuffix
	*/
	int GetSuffixLen(const char *pStr, const char *pSuffix);
	/*
		StripSpacesAndPunctuationAndOwnName

		If the input pStr "yourname: hello, world  ??! ? !?"
		is given the pStripped will point to "hello, world"
	*/
	void StripSpacesAndPunctuationAndOwnName(const char *pStr, char *pStripped, int SizeOfStripped);
	/*
		IsEmptyStr

		returns true if pStr is null "" or " " or "			 "
	*/
	bool IsEmptyStr(const char *pStr);

	/*
		IsWarCheckSuffix

		Check if the given string could be a message checking
		if another player has war. By only looking at the suffix.
		And only looking at the format "name is war?" not "is war name?"

		Returns the suffix length that was found
	*/
	int IsWarCheckSuffix(const char *pStr);

public:
	CReplyToPing(class CChatHelper *pChatHelper, const char *pMessageAuthor, const char *pMessageAuthorClan, const char *pMessage, char *pResponse, long unsigned int SizeOfResponse);

	/*
		Reply

		Return:
			true - if known message found and got a reply
			false - if no known message found

		Side effect:
			Fills pResponse buffer with the response message
	*/
	bool Reply();
};

#endif
