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
