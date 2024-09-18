// ChillerDragon 2020 - chillerbot ux

#include <base/system.h>
#include <engine/shared/linereader.h>
#include <game/client/components/chat.h>
#include <game/client/gameclient.h>
#include <game/generated/protocol.h>

#include "chillpw.h"

void CChillPw::OnMapLoad()
{
	m_LoginOffset[0] = 0;
	m_LoginOffset[1] = 0;
	m_ChatDelay[0] = time_get() + time_freq() * 2;
	m_ChatDelay[1] = time_get() + time_freq() * 2;

	const char *pAddrSkipProtocol = g_Config.m_UiServerAddress;
	if(str_startswith(pAddrSkipProtocol, "tw-0.7+udp://"))
		pAddrSkipProtocol += str_length("tw-0.7+udp://");
	if(str_startswith(pAddrSkipProtocol, "tw-0.6+udp://"))
		pAddrSkipProtocol += str_length("tw-0.6+udp://");

	str_copy(m_aCurrentServerAddr, pAddrSkipProtocol, sizeof(m_aCurrentServerAddr));
	str_copy(m_aCurrentServerAddrNoPort, pAddrSkipProtocol, sizeof(m_aCurrentServerAddrNoPort));
	for(int i = 0; i < str_length(m_aCurrentServerAddr); i++)
	{
		if(m_aCurrentServerAddr[i] == ':')
		{
			m_aCurrentServerAddrNoPort[i] = '\0';
			break;
		}
	}
}

void CChillPw::OnConsoleInit()
{
	Console()->Register("chillpw", "?s[status|dump_host|reload]", CFGFLAG_CLIENT, ConChillpw, this, "");
}

void CChillPw::ConReload()
{
	LoadPasswords();
}

void CChillPw::ConStatus()
{
	char aBuf[512];
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillpw", "~~~ chillpw status ~~~");
	str_format(aBuf, sizeof(aBuf), "file: %s", g_Config.m_ClPasswordFile);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillpw", aBuf);
	str_format(aBuf, sizeof(aBuf), "loaded passwords: %d", m_NumLoadedPasswords);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillpw", aBuf);

	int Found = 0;
	int FoundDummy = 0;
	for(int i = 0; i < MAX_PASSWORDS; i++)
	{
		if(m_aaHostnames[i][0] == '\0')
			continue;
		if(!str_find(m_aaHostnames[i], ":") || str_find(m_aaHostnames[i], ":*"))
		{
			if(str_comp(m_aCurrentServerAddrNoPort, m_aaHostnames[i]))
				continue;
		}
		else
		{
			if(str_comp(m_aCurrentServerAddr, m_aaHostnames[i]))
			{
				// if it has a port and it does not match skip to next entry
				if(str_find(m_aCurrentServerAddr, ":"))
					continue;

				// skip all non default port entrys
				if(!str_endswith(m_aaHostnames[i], ":8303"))
					continue;

				// if hostname without port does not match skip
				if(!str_startswith(m_aaHostnames[i], m_aCurrentServerAddr) || m_aaHostnames[i][str_length(m_aCurrentServerAddr)] != ':')
					continue;
			}
		}
		if(m_aDummy[i] == 0)
			Found++;
		else
			FoundDummy++;
	}
	str_format(aBuf, sizeof(aBuf), "curret host: '%s'", m_aCurrentServerAddr);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillpw", aBuf);
	// main
	str_format(aBuf, sizeof(aBuf), "  [main] known passwords: %d", Found);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillpw", aBuf);
	str_format(aBuf, sizeof(aBuf), "  [main] attempted passwords: %d", m_LoginOffset[0]);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillpw", aBuf);
	// dummy
	str_format(aBuf, sizeof(aBuf), "  [dummy] known passwords: %d", FoundDummy);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillpw", aBuf);
	str_format(aBuf, sizeof(aBuf), "  [dummy] attempted passwords: %d", m_LoginOffset[1]);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillpw", aBuf);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillpw", "see also 'chillpw dump_host'");
}

void CChillPw::ConDumpHost()
{
	for(int i = 0; i < MAX_PASSWORDS; i++)
	{
		if(m_aaHostnames[i][0] == '\0')
			continue;
		if(!str_find(m_aaHostnames[i], ":") || str_find(m_aaHostnames[i], ":*"))
		{
			if(str_comp(m_aCurrentServerAddrNoPort, m_aaHostnames[i]))
				continue;
		}
		else
		{
			if(str_comp(m_aCurrentServerAddr, m_aaHostnames[i]))
			{
				// if it has a port and it does not match skip to next entry
				if(str_find(m_aCurrentServerAddr, ":"))
					continue;

				// skip all non default port entrys
				if(!str_endswith(m_aaHostnames[i], ":8303"))
					continue;

				// if hostname without port does not match skip
				if(!str_startswith(m_aaHostnames[i], m_aCurrentServerAddr) || m_aaHostnames[i][str_length(m_aCurrentServerAddr)] != ':')
					continue;
			}
		}
		char aBuf[2048];
		str_format(aBuf, sizeof(aBuf), "dummy=%d host=%s pass: %s", m_aDummy[i], m_aaHostnames[i], m_aaPasswords[i]);
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillpw", aBuf);
	}
}

void CChillPw::ConChillpw(IConsole::IResult *pResult, void *pUserData)
{
	CChillPw *pSelf = (CChillPw *)pUserData;

	if(!str_comp_nocase(pResult->GetString(0), "dump_host"))
		pSelf->ConDumpHost();
	else if(!str_comp_nocase(pResult->GetString(0), "reload"))
		pSelf->ConReload();
	else
		pSelf->ConStatus();
}

void CChillPw::OnRender()
{
	if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
		return;

	if(Client()->DummyConnecting())
	{
		m_LoginOffset[1] = 0;
		m_ChatDelay[1] = time_get() + time_freq() * 2;
	}
	for(int i = 0; i < NUM_DUMMIES; i++)
	{
		if(!m_ChatDelay[i])
			continue;
		if(m_ChatDelay[i] > time_get())
			continue;
		if(g_Config.m_ClDummy != i)
			continue;

		if(AuthChatAccount(i, m_LoginOffset[i] + 1))
			m_ChatDelay[i] = time_get() + time_freq() * 2;
		else
			m_ChatDelay[i] = 0;
		m_LoginOffset[i]++;
	}
}

void CChillPw::LoadPasswords()
{
	m_NumLoadedPasswords = 0;
	IOHANDLE File = Storage()->OpenFile(g_Config.m_ClPasswordFile, IOFLAG_READ, IStorage::TYPE_ALL);
	char aBuf[128];
	int Line = 0;
	if(!File)
	{
		str_format(aBuf, sizeof(aBuf), "failed to open '%s'", g_Config.m_ClPasswordFile);
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillerbot", aBuf);
		return;
	}

	CLineReader LineReader;
	if(!LineReader.OpenFile(File))
	{
		str_format(aBuf, sizeof(aBuf), "failed to open '%s'", g_Config.m_ClPasswordFile);
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillerbot", aBuf);
		return;
	}

	for(char *aPassword : m_aaPasswords)
		aPassword[0] = '\0';
	for(char *aHostname : m_aaHostnames)
		aHostname[0] = '\0';

	const char *pLine;
	while((pLine = LineReader.Get()) && Line < MAX_PASSWORDS)
	{
		if(pLine[0] == '#' || pLine[0] == '\0')
			continue;

		char aLine[2048];
		str_copy(aLine, pLine);
		char *pRow1 = aLine;
		if(GotInvalidToken(pRow1, Line, aLine))
		{
			break;
		}
		char *pRow2 = (char *)str_find((const char *)pRow1 + 1, ",");
		if(GotInvalidToken(pRow2, Line, aLine))
		{
			break;
		}
		char *pRow3 = (char *)str_find((const char *)pRow2 + 1, ",");
		if(GotInvalidToken(pRow3, Line, aLine))
		{
			break;
		}
		str_copy(m_aaPasswords[Line], pRow3 + 1, sizeof(m_aaPasswords[Line]));
		pRow3[0] = '\0';
		m_aDummy[Line] = atoi(pRow2 + 1);
		pRow2[0] = '\0';
		str_copy(m_aaHostnames[Line], pRow1, sizeof(m_aaHostnames[Line]));
		// if the format is "hostname:*" cut of the globbed port to "hostname"
		for(int i = 0; i < str_length(pRow1); i++)
			if(pRow1[i] == ':' && pRow1[i + 1] == '*')
				m_aaHostnames[Line][i] = '\0';
		Line++;
	}
	m_NumLoadedPasswords = Line;
	str_format(aBuf, sizeof(aBuf), "loaded %d passwords from '%s'", m_NumLoadedPasswords, g_Config.m_ClPasswordFile);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chillerbot", aBuf);
}

void CChillPw::OnInit()
{
	LoadPasswords();
}

void CChillPw::SavePassword(const char *pServer, const char *pPassword)
{
	IOHANDLE File = Storage()->OpenFile(g_Config.m_ClPasswordFile, IOFLAG_WRITE, IStorage::TYPE_SAVE);
	if(File)
	{
		io_write(File, pServer, str_length(pServer));
		io_write(File, ";", 1);
		io_write(File, pPassword, str_length(pPassword));
		io_write_newline(File);
		io_close(File);
	}
}

bool CChillPw::AuthChatAccount(int Dummy, int Offset)
{
	int Found = 0;
	for(int i = 0; i < MAX_PASSWORDS; i++)
	{
		if(m_aaHostnames[i][0] == '\0')
			continue;
		if(!str_find(m_aaHostnames[i], ":") || str_find(m_aaHostnames[i], ":*"))
		{
			if(str_comp(m_aCurrentServerAddrNoPort, m_aaHostnames[i]))
				continue;
		}
		else
		{
			if(str_comp(m_aCurrentServerAddr, m_aaHostnames[i]))
			{
				// if it has a port and it does not match skip to next entry
				if(str_find(m_aCurrentServerAddr, ":"))
					continue;

				// skip all non default port entrys
				if(!str_endswith(m_aaHostnames[i], ":8303"))
					continue;

				// if hostname without port does not match skip
				if(!str_startswith(m_aaHostnames[i], m_aCurrentServerAddr) || m_aaHostnames[i][str_length(m_aCurrentServerAddr)] != ':')
					continue;
			}
		}
		if(Dummy != m_aDummy[i])
			continue;
		if(Offset > ++Found)
			continue;
		Console()->ExecuteLine(m_aaPasswords[i]);
		return true;
	}
	return false;
}
