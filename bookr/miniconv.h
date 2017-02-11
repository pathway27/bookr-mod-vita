#ifndef __MINICONV_H__
#define __MINICONV_H__

typedef char* (*utf8_convert_function)(const unsigned char*);

#ifdef __cplusplus
extern "C" {
#endif

char* utf16le_to_utf8(const unsigned short* utf16le);
char* utf16be_to_utf8(const unsigned short* utf16be);
#ifdef GBK_CONV
char* gbk_to_utf8(const unsigned char* gbk);
#endif
#ifdef BIG5_CONV
char* big5_to_utf8(const unsigned char* big5);
#endif
#ifdef SJIS_CONV
char* sjis_to_utf8(const unsigned char* sjis);
#endif
#ifdef EUC_KR_CONV
char* euc_kr_to_utf8(const unsigned char* euc_kr);
#endif
#ifdef MS_EE_CONV
char* ms_ee_to_utf8(const unsigned char* ms_ee);
#endif
#ifdef MS_CYRL_CONV
char* ms_cyrl_to_utf8(const unsigned char* ms_cyrl);
#endif
#ifdef MS_ARAB_CONV
char* ms_arab_to_utf8(const unsigned char* ms_cyrl);
#endif
#ifdef ISO8859_6_CONV
char* iso8859_6_to_utf8(const unsigned char* iso8859_6);
#endif
#ifdef ISO8859_2_CONV
char* iso8859_2_to_utf8(const unsigned char* iso8859_2);
#endif
#ifdef ISO8859_1_CONV
char* iso8859_1_to_utf8(const unsigned char* iso8859_1);
#endif
char* miniConvUTF16LEConv(const unsigned short* utf16le);
char* miniConvUTF16BEConv(const unsigned short* utf16be);
int miniConvGetConvCount();
char* miniConvGetConvCharset(int index);
void miniConvSetFileSystemConv(const char* charset);
int miniConvHaveFileSystemConv();
char* miniConvFileSystemConv(const unsigned char* s);
void miniConvSetDefaultSubtitleConv(const char* charset);
int miniConvHaveDefaultSubtitleConv();
int miniConvHaveSubtitleConv(const char* charset);
char* miniConvDefaultSubtitleConv(const unsigned char* s);
char* miniConvSubtitleConv(const unsigned char* s, const char* charset);
void miniConvFreeMemory(void* mem);
#ifdef __cplusplus
}
#endif

#include "bkdebug.h"

#endif
