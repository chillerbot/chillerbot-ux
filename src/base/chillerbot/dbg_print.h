#ifndef BASE_CHILLERBOT_DBG_PRINT_H
#define BASE_CHILLERBOT_DBG_PRINT_H

#include <base/str.h>

/*
    DBG_..

    The debug printer macro family :)

    This macro is intended to be called multiple times
    for example in the tick function or in some other loop
    but it only prints if the values updated to avoid spam

    It uses the args to define new variable names so give them short and nice names
    otherwise the print looks ugly or the code will not compile if you have some -> in your
    variable for example. Here is an example on how to use it:

    int curX = m_pClient->m_aClients[ClientId].m_RenderCur.m_X;
    int mx = 5;
    DBG_II(curX, mx);

    Make sure the variable names do not conflict it will generate a static
    si_<thenameyoupicked> under the hood to keep track of changes

    Variadic macros seems annoying
    https://stackoverflow.com/questions/1872220/is-it-possible-to-iterate-over-arguments-in-variadic-macros

    So there is one macro for every need.

    DBG_I(var1) - for printing one ints
    DBG_II(var1, var2) - for printing two ints
    DBG_III(var1, var2, var3) - for printing three ints
    DBG_IIII(var1, var2, var3) - for printing three ints

    DBG_S(var1) - for printing one string
    DBG_SS(var1, var2) - for printing two strings
    DBG_SSS(var1, var2, var3) - for printing three strings

    DBG_SI(var1, var2) - for printing one string and one int
    DBG_IS(var1, var2) - for printing one int and one string
    DBG_SII(var1, var2, var3) - for printing one string and two ints
    DBG_SIS(var1, var2, var3) - for printing one string, one int and one string
    DBG_ISS(var1, var2, var3) - for printing one int and two strings
    DBG_IIS(var1, var2, var3) - for printing two ints and one string
*/

#define DBG_I(var1) \
	{ \
		static int si_##var1 = 0; \
		if(si_##var1 != var1) \
		{ \
			si_##var1 = var1; \
			dbg_msg("chiller_dbg", #var1 "=%d", var1); \
		} \
	}

#define DBG_II(var1, var2) \
	{ \
		static int si_##var1 = 0; \
		static int si_##var2 = 0; \
		if(si_##var1 != var1 || si_##var2 != var2) \
		{ \
			si_##var1 = var1; \
			si_##var2 = var2; \
			dbg_msg("chiller_dbg", #var1 "=%d " #var2 "=%d", var1, var2); \
		} \
	}

#define DBG_III(var1, var2, var3) \
	{ \
		static int si_##var1 = 0; \
		static int si_##var2 = 0; \
		static int si_##var3 = 0; \
		if(si_##var1 != var1 || si_##var2 != var2 || si_##var3 != var3) \
		{ \
			si_##var1 = var1; \
			si_##var2 = var2; \
			si_##var3 = var3; \
			dbg_msg("chiller_dbg", #var1 "=%d " #var2 "=%d " #var3 "=%d", var1, var2, var3); \
		} \
	}

#define DBG_IIII(var1, var2, var3, var4) \
	{ \
		static int si_##var1 = 0; \
		static int si_##var2 = 0; \
		static int si_##var3 = 0; \
		static int si_##var4 = 0; \
		if(si_##var1 != var1 || si_##var2 != var2 || si_##var3 != var3 || si_##var4 != var4) \
		{ \
			si_##var1 = var1; \
			si_##var2 = var2; \
			si_##var3 = var3; \
			si_##var4 = var4; \
			dbg_msg("chiller_dbg", #var1 "=%d " #var2 "=%d " #var3 "=%d " #var3 "=%d", var1, var2, var3, var4); \
		} \
	}

#define DBG_S(var1) \
	{ \
		static char sa_##var1[2048]; \
		if(str_comp(sa_##var1, var1)) \
		{ \
			str_copy(sa_##var1, var1, sizeof(sa_##var1)); \
			dbg_msg("chiller_dbg", #var1 "=%s", var1); \
		} \
	}

#define DBG_SS(var1, var2) \
	{ \
		static char sa_##var1[2048]; \
		static char sa_##var2[2048]; \
		if(str_comp(sa_##var1, var1) || str_comp(sa_##var2, var2)) \
		{ \
			str_copy(sa_##var1, var1, sizeof(sa_##var1)); \
			str_copy(sa_##var2, var2, sizeof(sa_##var2)); \
			dbg_msg("chiller_dbg", #var1 "=%s " #var2 "=%s", var1, var2); \
		} \
	}

#define DBG_SSS(var1, var2, var3) \
	{ \
		static char sa_##var1[2048]; \
		static char sa_##var2[2048]; \
		static char sa_##var3[2048]; \
		if(str_comp(sa_##var1, var1) || str_comp(sa_##var2, var2) || str_comp(sa_##var3, var3)) \
		{ \
			str_copy(sa_##var1, var1, sizeof(sa_##var1)); \
			str_copy(sa_##var2, var2, sizeof(sa_##var2)); \
			str_copy(sa_##var3, var3, sizeof(sa_##var3)); \
			dbg_msg("chiller_dbg", #var1 "=%s " #var2 "=%s " #var2 "=%s", var1, var2, var3); \
		} \
	}

#define DBG_SI(var1, var2) \
	{ \
		static char sa_##var1[2048]; \
		static int si_##var2 = 0; \
		if(str_comp(sa_##var1, var1) || si_##var2 != var2) \
		{ \
			str_copy(sa_##var1, var1, sizeof(sa_##var1)); \
			si_##var2 = var2; \
			dbg_msg("chiller_dbg", #var1 "=%s " #var2 "=%d", var1, var2); \
		} \
	}

#define DBG_IS(var1, var2) \
	{ \
		static int si_##var1 = 0; \
		static char sa_##var2[2048]; \
		if(si_##var1 != var1 || str_comp(sa_##var2, var2)) \
		{ \
			si_##var1 = var1; \
			str_copy(sa_##var2, var2, sizeof(sa_##var2)); \
			dbg_msg("chiller_dbg", #var1 "=%d " #var2 "=%s", var1, var2); \
		} \
	}

#define DBG_SII(var1, var2, var3) \
	{ \
		static char sa_##var1[2048]; \
		static int si_##var2 = 0; \
		static int si_##var3 = 0; \
		if(str_comp(sa_##var1, var1) || si_##var2 != var2 || si_##var3 != var3) \
		{ \
			str_copy(sa_##var1, var1, sizeof(sa_##var1)); \
			si_##var2 = var2; \
			si_##var3 = var3; \
			dbg_msg("chiller_dbg", #var1 "=%s " #var2 "=%d " #var3 "=%d", var1, var2, var3); \
		} \
	}

#define DBG_SIS(var1, var2, var3) \
	{ \
		static char sa_##var1[2048]; \
		static int si_##var2 = 0; \
		static char sa_##var3[2048]; \
		if(str_comp(sa_##var1, var1) || si_##var2 != var2 || str_comp(sa_##var3, var3)) \
		{ \
			str_copy(sa_##var1, var1, sizeof(sa_##var1)); \
			si_##var2 = var2; \
			str_copy(sa_##var3, var3, sizeof(sa_##var3)); \
			dbg_msg("chiller_dbg", #var1 "=%s " #var2 "=%d " #var3 "=%s", var1, var2, var3); \
		} \
	}

#define DBG_ISS(var1, var2, var3) \
	{ \
		static int si_##var1 = 0; \
		static char sa_##var2[2048]; \
		static char sa_##var3[2048]; \
		if(si_##var1 != var1 || str_comp(sa_##var2, var2) || str_comp(sa_##var3, var3)) \
		{ \
			si_##var1 = var1; \
			str_copy(sa_##var2, var2, sizeof(sa_##var2)); \
			str_copy(sa_##var3, var3, sizeof(sa_##var3)); \
			dbg_msg("chiller_dbg", #var1 "=%d " #var2 "=%s " #var3 "=%s", var1, var2, var3); \
		} \
	}

#define DBG_IIS(var1, var2, var3) \
	{ \
		static int si_##var1 = 0; \
		static int si_##var2 = 0; \
		static char sa_##var3[2048]; \
		if(si_##var1 != var1 || si_##var2 != var2 || str_comp(sa_##var3, var3)) \
		{ \
			si_##var1 = var1; \
			si_##var2 = var2; \
			str_copy(sa_##var3, var3, sizeof(sa_##var3)); \
			dbg_msg("chiller_dbg", #var1 "=%d " #var2 "=%d " #var3 "=%s", var1, var2, var3); \
		} \
	}

#endif
