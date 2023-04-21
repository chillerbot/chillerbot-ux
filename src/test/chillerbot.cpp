#include "test.h"

#include <gtest/gtest.h>

#include <base/system.h>
#include <engine/shared/chillerbot/langparser.h>

// #include <base/chillerbot/pad_utf8.h>

// #include <engine/shared/rust_version.h>

// TEST(ChillerBot, PadUtf8)
// {
// 	// char aBuf[128];
// 	// str_copy(aBuf, "♥♥♥", sizeof(aBuf));
// 	// str_pad_right_utf8(aBuf, sizeof(aBuf), 6);
// 	// EXPECT_STREQ(aBuf, "♥♥♥   ");

// 	// str_copy(aBuf, "[Block] Ki-o | Myr писька ♥ ~~~~~~~~~~~~~~~~~~~~~~~~", sizeof(aBuf));
// 	// str_pad_right_utf8(aBuf, sizeof(aBuf), 60);
// 	// EXPECT_STREQ(aBuf, "[Block] Ki-o | Myr писька ♥ ~~~~~~~~~~~~~~~~~~~~~~~~        ");

// 	// str_copy(aBuf, "DDNet GER2 - Brutal", sizeof(aBuf));
// 	// str_pad_right_utf8(aBuf, sizeof(aBuf), 60);
// 	// EXPECT_STREQ(aBuf, "DDNet GER2 - Brutal                                         ");

// 	// str_copy(aBuf, "DDNet GER10 [ger10.ddnet.org whitelist] - Novice", sizeof(aBuf));
// 	// str_pad_right_utf8(aBuf, sizeof(aBuf), 60);
// 	// EXPECT_STREQ(aBuf, "DDNet GER10 [ger10.ddnet.org whitelist] - Novice            ");

// 	// str_copy(aBuf, "困困困", sizeof(aBuf));
// 	// str_pad_right_utf8(aBuf, sizeof(aBuf), 6);
// 	// EXPECT_STREQ(aBuf, "困困困   ");

// 	// str_copy(aBuf, "困困困", sizeof(aBuf));
// 	// str_pad_right_utf8(aBuf, sizeof(aBuf), 60);
// 	// EXPECT_STREQ(aBuf, "困困困困                                                         ");
// 	//                  1234

// 	// int width = str_width_unicode("困");
// 	// EXPECT_EQ(width, 2);

// 	// str_copy(aBuf, "困", sizeof(aBuf));
// 	// str_pad_right_utf8(aBuf, sizeof(aBuf), 60);
// 	// EXPECT_STREQ(aBuf, "困                                                          ");

// 	// str_copy(aBuf, "困难", sizeof(aBuf));
// 	// str_pad_right_utf8(aBuf, sizeof(aBuf), 60);
// 	// EXPECT_STREQ(aBuf, "困难                                                        ");

// 	// str_copy(aBuf, "困难 [上海]", sizeof(aBuf));
// 	// str_pad_right_utf8(aBuf, sizeof(aBuf), 60);
// 	// EXPECT_STREQ(aBuf, "困难 [上海]                                                  ");

// 	// str_copy(aBuf, "「中国社区服」Gores - 困难 [上海]", sizeof(aBuf));
// 	// str_pad_right_utf8(aBuf, sizeof(aBuf), 60);
// 	// EXPECT_STREQ(aBuf, "「中国社区服」Gores - 困难 [上海]                               ");

// 	// str_copy(aBuf, "[私服]黑子的感染模式服务器", sizeof(aBuf));
// 	// str_pad_right_utf8(aBuf, sizeof(aBuf), 60);
// 	// EXPECT_STREQ(aBuf, "[私服]黑子的感染模式服务器                                      ");

// 	// str_copy(aBuf, "|*KoG*| GER #3 - Main Gores [kog.tw]", sizeof(aBuf));
// 	// str_pad_right_utf8(aBuf, sizeof(aBuf), 60);
// 	// EXPECT_STREQ(aBuf, "|*KoG*| GER #3 - Main Gores [kog.tw]                        ");
// }

TEST(ChillerBot, LangParserAskToAsk)
{
	CLangParser LangParser;
	bool AskToAsk = LangParser.IsAskToAsk("Can I ask you a question?");
	EXPECT_EQ(AskToAsk, true);
	AskToAsk = LangParser.IsAskToAsk("Kann ich dich was fragen?");
	EXPECT_EQ(AskToAsk, true);
}

TEST(ChillerBot, LangParserAskToAskStatement)
{
	CLangParser LangParser;
	bool AskToAsk = LangParser.IsAskToAsk("I have a question");
	EXPECT_EQ(AskToAsk, true);
	AskToAsk = LangParser.IsAskToAsk("Ich hab eine frage");
	EXPECT_EQ(AskToAsk, true);
}

TEST(ChillerBot, LangParserWantedToAskFalsePositive)
{
	CLangParser LangParser;
	bool AskToAsk = LangParser.IsAskToAsk("I wanted To ask you something, but then I did not care");
	EXPECT_EQ(AskToAsk, false);

	AskToAsk = LangParser.IsAskToAsk("I just wanted to ask how are you");
	EXPECT_EQ(AskToAsk, false);

	AskToAsk = LangParser.IsAskToAsk("I just wanted to ask");
	EXPECT_EQ(AskToAsk, false);
}

TEST(ChillerBot, LangParserWantedToAskFalsePositiveGer)
{
	CLangParser LangParser;
	bool AskToAsk = LangParser.IsAskToAsk("ich wollte dich fragen, doch dann wars mir egal");
	EXPECT_EQ(AskToAsk, false);

	AskToAsk = LangParser.IsAskToAsk("Ich wollt dich nur fragen wie es dir geht");
	EXPECT_EQ(AskToAsk, false);

	AskToAsk = LangParser.IsAskToAsk("das wollte ich dich schon immer mal fragen");
	EXPECT_EQ(AskToAsk, false);
}
