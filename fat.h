#ifndef __FAT_H__
#define __FAT_H__

#include <psptypes.h>

#ifndef INVALID
#define INVALID ((u32)-1)
#endif

struct _fat_mbr_dpt {
	u8 active;
	u8 start[3];
	u8 id;
	u8 ending[3];
	u32 start_sec;
	u32 total_sec;
} __attribute__((packed));

struct _fat_mbr {
	u8 mb_data[0x1BE];
	struct _fat_mbr_dpt dpt[4];
	u16 ending_flag;
} __attribute__((packed));
typedef struct _fat_mbr t_fat_mbr, * p_fat_mbr;

struct _fat_dbr {
	u8 jmp_boot[3];
	char oem_name[8];
	u16 bytes_per_sec;
	u8 sec_per_clus;
	u16 reserved_sec;
	u8 num_fats;
	u16 root_entry;
	u16 total_sec;
	u8 media_type;
	u16 sec_per_fat;
	u16 sec_per_track;
	u16 heads;
	u32 hidd_sec;
	u32 big_total_sec;
	union {
		struct {
			u8 drv_num;
			u8 reserved;
			u8 boot_sig;
			u8 vol_id[4];
			char vol_lab[11];
			char file_sys_type[8];
		} __attribute__((packed)) fat16;
		struct {
			u32 sec_per_fat;
			u16 extend_flag;
			u16 sys_ver;
			u32 root_clus;
			u16 info_sec;
			u16 back_sec;
			u8 reserved[10];
		} __attribute__((packed)) fat32;
	} __attribute__((packed)) ufat;
	u8 exe_code[448];
	u16 ending_flag;
} __attribute__((packed));
typedef struct _fat_dbr t_fat_dbr, * p_fat_dbr;

struct _fat_normentry {
	char filename[8];
	char fileext[3];
	u8 attr;
	u8 flag;
	u8 cr_time_msec;
	u16 cr_time;
	u16 cr_date;
	u16 last_visit;
	u16 clus_high;
	u16 last_mod_time;
	u16 last_mod_date;
	u16 clus;
	u32 filesize;
} __attribute__((packed));

struct _fat_longfile {
	u8 order;
	u16 uni_name[5];
	u8 sig;
	u8 reserved;
	u8 checksum;
	u16 uni_name2[6];
	u16 clus;
	u16 uni_name3[2];
} __attribute__((packed));

union _fat_entry {
	struct _fat_normentry norm;
	struct _fat_longfile longfile;
} __attribute__((packed));
typedef union _fat_entry t_fat_entry, * p_fat_entry;

#define FAT_FILEATTR_READONLY	0x01
#define FAT_FILEATTR_HIDDEN		0x02
#define FAT_FILEATTR_SYSTEM		0x04
#define FAT_FILEATTR_VOLUME		0x08
#define FAT_FILEATTR_DIRECTORY	0x10
#define FAT_FILEATTR_ARCHIVE	0x20

#define MAX_FILENAME_LENGTH 1024
typedef struct {
	char filename[MAX_FILENAME_LENGTH];
	char longname[MAX_FILENAME_LENGTH];
	u32 filesize;
	u16 cdate;
	u16 ctime;
	u16 mdate;
	u16 mtime;
	u32 clus;
	u8 attr;
} t_fat_info, * p_fat_info;

#ifdef __cplusplus
extern "C" {
#endif

int fat_init(int devkitVersion);
int fat_locate(const char * name, char * sname, u32 clus, p_fat_entry info);
u32 fat_readdir(const char * dir, char * sdir, p_fat_info * info);
void fat_free();

#ifdef __cplusplus
}
#endif

#endif
