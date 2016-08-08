#ifndef __BKDEBUG_H__
#define __BKDEBUG_H__

#ifndef PSP
#include <malloc.h>
#define MEMINFO (((struct mallinfo)mallinfo()).uordblks)
#else
#ifdef _DEBUGBOOKR
#include <malloc.h>
#define MEMINFO (((struct mallinfo)mallinfo()).uordblks)
#else
#define MEMINFO 0
#endif
#endif


#ifdef PSP
#ifdef _DEBUGBOOKR

#define DEBUGBOOKR(args...)					\
  do {								\
    FILE* outf = fopen("ms0:/bookr_gai.debug","a");		\
    if(outf!=NULL){						\
      fprintf(outf,args);					\
      fclose(outf);						\
    }								\
  }while(0)

#define DEBUGBOOKR_VA(fmt,ap)				\
  do {								\
    FILE* outf = fopen("ms0:/bookr_gai.debug","a");		\
    if(outf!=NULL){						\
      vfprintf(outf,fmt,ap);					\
      fclose(outf);						\
    }								\
  }while(0)
#define DEBUGBOOKR_F(outp, args...)				\
  do {								\
    FILE* outf = fopen("ms0:/bookr_gai.debug","a");		\
    if(outf!=NULL){						\
      fprintf(outf,args);					\
      fclose(outf);						\
    }								\
  }while(0)

#define DEBUGBOOKR_VAF(outp, fmt,ap)				\
  do {								\
    FILE* outf = fopen("ms0:/bookr_gai.debug","a");		\
    if(outf!=NULL){						\
      vfprintf(outf,fmt,ap);					\
      fclose(outf);						\
    }								\
  }while(0)

#else /* _DEBUGBOOKR */
#define DEBUGBOOKR(...)
#define DEBUGBOOKR_F(...)
#define DEBUGBOOKR_VA(...)
#define DEBUGBOOKR_VAF(...)

#endif /* _DEBUGBOOKR */
#else /* PSP */
#include <time.h>
#define DEBUGBOOKR(...)						\
  do {							\
    FILE* outf = fopen("/tmp/bookr_gai.debug","a");	\
    if(outf!=NULL){					\
      time_t t = time(NULL);				\
      struct tm* lt = localtime(&t);				\
      fprintf(outf,"%d:%d:%d.%d - BOOKR: ",			\
	      lt->tm_hour,					\
	      lt->tm_min,					\
	      lt->tm_sec,					\
	      0 );						\
      fprintf(outf,__VA_ARGS__);				\
      fclose(outf);						\
    }								\
  }while(0)
#define DEBUGBOOKR_VA(fmt, ap)			\
  do {							\
    FILE* outf = fopen("/tmp/bookr_gai.debug","a");	\
    if(outf!=NULL){					\
      vfprintf(outf,fmt,ap);					\
      fclose(outf);						\
    }								\
  }while(0)
#define DEBUGBOOKR_F(outp, ...)				\
  do {							\
    FILE* outf = fopen("/tmp/bookr_gai.debug","a");	\
    if(outf!=NULL){					\
      fprintf(outf,__VA_ARGS__);				\
      fclose(outf);						\
    }								\
  }while(0)
#define DEBUGBOOKR_VAF(outp, fmt, ap)			\
  do {							\
    FILE* outf = fopen("/tmp/bookr_gai.debug","a");	\
    if(outf!=NULL){					\
      vfprintf(outf,fmt,ap);					\
      fclose(outf);						\
    }								\
  }while(0)


#endif /* PSP */

#endif /* __BKDEBUG_H__ */
