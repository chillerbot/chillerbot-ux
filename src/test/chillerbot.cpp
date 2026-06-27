#include "test.h"

#include <base/str.h>

#include <external/chillerbot_reply/include/chillerbot_reply/langparser.h>
#include <gtest/gtest.h>

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
