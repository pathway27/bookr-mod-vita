#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pspkernel.h>
#include "fat.h"
#include "miniconv.h"

static int fwVersion;
static int fatfd = -1;
static t_fat_dbr dbr;
static t_fat_mbr mbr;
static u32 * fat_table = NULL;
static u64 dbr_pos = 0;
static u64 root_pos = 0;
static u64 data_pos = 0;
static u64 bytes_per_clus = 0;
static u32 loadcount = 0;
static u32 clus_max = 0;
static enum {
	fat12,
	fat16,
	fat32
} fat_type = fat16;

int fat_init(int devkitVersion)
{
	fwVersion = devkitVersion;
	fatfd = sceIoOpen("msstor:", PSP_O_RDONLY, 0777);
	if(fatfd < 0)
		return 0;
	if(sceIoRead(fatfd, &mbr, sizeof(mbr)) != sizeof(mbr))
	{
		sceIoClose(fatfd);
		fatfd = -1;
		return 0;
	}
	
	dbr_pos = mbr.dpt[0].start_sec * 0x200;
	
	if(sceIoLseek32(fatfd, dbr_pos, PSP_SEEK_SET) != dbr_pos || sceIoRead(fatfd, &dbr, sizeof(dbr)) < sizeof(dbr))
	{
		dbr_pos = 0;
		if(sceIoLseek32(fatfd, dbr_pos, PSP_SEEK_SET) != dbr_pos || sceIoRead(fatfd, &dbr, sizeof(dbr)) < sizeof(dbr)) 
		{
			sceIoClose(fatfd);
			fatfd = -1;
			return 0;
		}
	}
	
	u64 total_sec = ( dbr.total_sec == 0 ) ? dbr.big_total_sec : dbr.total_sec;
	u64 fat_sec = (dbr.sec_per_fat == 0 ) ? dbr.ufat.fat32.sec_per_fat : dbr.sec_per_fat;
	u64 root_sec = ( dbr.root_entry * 32 + dbr.bytes_per_sec - 1 ) / dbr.bytes_per_sec; 
	u64 data_sec = total_sec - dbr.reserved_sec - ( dbr.num_fats * fat_sec ) - root_sec;
	u64 data_clus = data_sec / dbr.sec_per_clus ;
	
	if ( data_clus < 4085 )
	{
		fat_type = fat12;
		clus_max = 0x0FF0;
	}
	else if ( data_clus < 65525 )
	{
		fat_type = fat16;
		clus_max = 0xFFF0;
	}
	else
	{
		fat_type = fat32;
		clus_max = 0x0FFFFFF0;
	}
	
//	if ( dbr.root_entry == 0 )
//	{
//		fat_type = fat32;
//		clus_max = 0x0FFFFFF0;
//	}
//	else if(mbr.dpt[0].id == 0x1)
//	{
//		fat_type = fat12;
//		clus_max = 0x0FF0;
//	}
//	else //if(mbr.dpt[0].id == 0x4 || mbr.dpt[0].id == 0x6 || mbr.dpt[0].id == 0xE || mbr.dpt[0].id == 0xB )
//	{
//		fat_type = fat16;
//		clus_max = 0xFFF0;
//	}
	
	bytes_per_clus = 1ull * dbr.sec_per_clus * dbr.bytes_per_sec;
	if(fat_type == fat32)
	{
		data_pos = 1ull * dbr_pos;
		data_pos += 1ull * (dbr.ufat.fat32.sec_per_fat * dbr.num_fats + dbr.reserved_sec) * dbr.bytes_per_sec;
		root_pos = data_pos;
		root_pos += 1ull * bytes_per_clus * dbr.ufat.fat32.root_clus;
	}
	else
	{
		root_pos = 1ull * dbr_pos;
		root_pos += 1ull * (dbr.num_fats * dbr.sec_per_fat + dbr.reserved_sec) * dbr.bytes_per_sec;
		data_pos = root_pos;
		data_pos += 1ull * dbr.root_entry * sizeof(t_fat_entry);
	}
	sceIoClose(fatfd);
	return 1;
}

static int convert_table_fat12()
{
	u16 * otable = (u16 *)malloc(dbr.sec_per_fat * dbr.bytes_per_sec);
	if(otable == NULL)
		return 0;
	memcpy(otable, fat_table, dbr.sec_per_fat * dbr.bytes_per_sec);
	int i, j, entrycount = dbr.sec_per_fat * dbr.bytes_per_sec / sizeof(u16) * 4 / 3;
	free((void *)fat_table);
	fat_table = (u32 *)malloc(sizeof(u32) * entrycount);
	if(fat_table == NULL)
	{
		free((void *)otable);
		return 0;
	}
	for(i = 0, j = 0; i < entrycount; i += 4, j += 3)
	{
		fat_table[i] = otable[j] & 0x0FFF;
		fat_table[i + 1] = ((otable[j] >> 12) & 0x000F) | ((otable[j + 1] & 0x00FF) << 4);
		fat_table[i + 2] = ((otable[j + 1] >> 8) & 0x00FF) | ((otable[j + 2] & 0x000F) << 8);
		fat_table[i + 3] = otable[j + 2] >> 4;
	}
	free((void *)otable);
	return 1;
}

static int convert_table_fat16()
{
	u16 * otable = (u16 *)malloc(dbr.sec_per_fat * dbr.bytes_per_sec);
	if(otable == NULL)
		return 0;
	memcpy(otable, fat_table, dbr.sec_per_fat * dbr.bytes_per_sec);
	int i, entrycount = dbr.sec_per_fat * dbr.bytes_per_sec / sizeof(u16);
	free((void *)fat_table);
	fat_table = (u32 *)malloc(sizeof(u32) * entrycount);
	if(fat_table == NULL)
	{
		free((void *)otable);
		return 0;
	}
	for(i = 0; i < entrycount; i ++)
		fat_table[i] = otable[i];
	free((void *)otable);
	return 1;
}

static int fat_load_table()
{
	if(loadcount > 0)
	{
		loadcount ++;
		return 1;
	}
	fatfd = sceIoOpen("msstor:", PSP_O_RDONLY, 0777);
	if(fatfd < 0)
		return 0;
		
	u32 fat_table_size = ((fat_type == fat32) ? dbr.ufat.fat32.sec_per_fat : dbr.sec_per_fat) * dbr.bytes_per_sec;
	if(sceIoLseek32(fatfd, dbr_pos + dbr.reserved_sec * dbr.bytes_per_sec, PSP_SEEK_SET) != dbr_pos + dbr.reserved_sec * dbr.bytes_per_sec || (fat_table = (u32 *)malloc(fat_table_size)) == NULL)
	{
		sceIoClose(fatfd);
		fatfd = -1;
		return 0;
	}
	if( (sceIoRead(fatfd, fat_table, fat_table_size) != fat_table_size) || (fat_type == fat12 && !convert_table_fat12()) || ( fat_type == fat16 && !convert_table_fat16()))
	{
		sceIoClose(fatfd);
		fatfd = -1;
		free((void *)fat_table);
		fat_table = NULL;
		return 0;
	}
	loadcount = 1;
	return 1;
}

static void fat_free_table()
{
	if(loadcount > 0)
	{
		loadcount --;
		if(loadcount > 0)
			return;
		if(fat_table != NULL)
		{
			free((void *)fat_table);
			fat_table = NULL;
		}
		sceIoClose(fatfd);
	}
}

static u8 fat_calc_chksum(p_fat_entry info)
{
	u8 * n = (u8 *)&info->norm.filename[0];
	u8 chksum = 0;
	u32 i;
	for(i = 0; i < 11; i ++)
		chksum = ((chksum & 1) ? 0x80 : 0) + (chksum >> 1) + n[i];
	return chksum;
}

static int fat_dir_list(u32 clus, u32 * count, p_fat_entry * entrys)
{
	if(clus == 0)
		return 0;
	if(clus < 2)
	{
		* count = dbr.root_entry;
		if((* entrys = (p_fat_entry)malloc(* count * sizeof(t_fat_entry))) == NULL)
			return 0;
		if(sceIoLseek(fatfd, root_pos, PSP_SEEK_SET) != root_pos || sceIoRead(fatfd, * entrys, * count * sizeof(t_fat_entry)) != * count * sizeof(t_fat_entry))
		{
			free((void *)* entrys);
			return 0;
		}
	}
	else
	{
		if(fat_table[clus] < 2)
			return 0;
		u32 c2 = clus;
		* count = 1;
		while(fat_table[c2] < clus_max && fat_table[c2] > 1)
		{
			c2 = fat_table[c2];
			(* count) ++;
		}
		c2 = clus;
		u32 epc = (bytes_per_clus / sizeof(t_fat_entry)), ep = 0;
		(* count) *= epc;
		if((* entrys = (p_fat_entry)malloc(* count * sizeof(t_fat_entry))) == NULL)
			return 0;
		do {
			u64 epos = data_pos + 1ull * (c2 - 2) * bytes_per_clus;
			if(sceIoLseek(fatfd, epos, PSP_SEEK_SET) != epos || sceIoRead(fatfd, &(* entrys)[ep], bytes_per_clus) != bytes_per_clus)
			{
				free((void *)* entrys);
				return 0;
			}
			ep += epc;
			c2 = fat_table[c2];
		} while(c2 < clus_max && c2 > 1);
	}
	return 1;
}

static int fat_get_longname(p_fat_entry entrys, u32 cur, char * longnamestr)
{
	u16 chksum = fat_calc_chksum(&entrys[cur]);
	u32 j = cur;
	u16 longname[MAX_FILENAME_LENGTH];
	memset(longname, 0, MAX_FILENAME_LENGTH * sizeof(u16));
	while(j > 0)
	{
		j --;
		if(entrys[j].norm.attr != 0x0F || entrys[j].longfile.checksum != chksum || entrys[j].norm.filename[0] == 0 || (u8)entrys[j].norm.filename[0] == 0xE5)
			return 0;
		u32 order = entrys[j].longfile.order & 0x3F;
		if(order > 20)
			return 0;
		u32 ppos = (order - 1) * 13;
		u32 k;
		for(k = 0; k < 5; k ++)
			longname[ppos ++] = entrys[j].longfile.uni_name[k];
		for(k = 0; k < 6; k ++)
			longname[ppos ++] = entrys[j].longfile.uni_name2[k];
		for(k = 0; k < 2; k ++)
			longname[ppos ++] = entrys[j].longfile.uni_name3[k];
		if((entrys[j].longfile.order & 0x40) > 0)
			break;
	}
	if(entrys[j].norm.attr != 0x0F || (entrys[j].longfile.order & 0x40) == 0)
		return 0;
	longname[MAX_FILENAME_LENGTH - 1] = 0;
	memset(longnamestr, 0, MAX_FILENAME_LENGTH);

	char* temp = miniConvUTF16LEConv(longname);
	if(temp) {
		strncpy(longnamestr, temp, MAX_FILENAME_LENGTH - 1);
	}
	return 1;
}

static void fat_get_shortname(p_fat_entry entry, char * shortnamestr)
{
	static int chartable[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x00
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x00
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x00
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x30
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x40
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 0x50
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x60
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 0x70
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x80
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x90
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xA0
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xB0
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xC0
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xD0
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xE0
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0   // 0xF0
	};
	u32 i = 0;
	u8 abit = 0;
	if((entry->norm.flag & 0x08) > 0)
		abit = 0x20;
	while(i < 8 && entry->norm.filename[i] != 0x20)
	{
		*shortnamestr ++ = entry->norm.filename[i] | ((chartable[(u8)entry->norm.filename[i]]) ? abit : 0);
		i ++;
	}
	if(entry->norm.fileext[0] != 0x20)
	{
		*shortnamestr ++ = '.';
		i = 0;
		while(i < 3 && entry->norm.fileext[i] != 0x20)
			*shortnamestr ++ = entry->norm.fileext[i ++] | abit;
	}
	*shortnamestr = 0;
}

int fat_locate(const char * name, char * sname, u32 clus, p_fat_entry info)
{
	u32 count;
	p_fat_entry entrys;
	if(!fat_dir_list(clus, &count, &entrys))
		return 0;
	SceUID dl = sceIoDopen(sname);
	if(dl < 0)
		return 0;
	char shortname[11];
	int onlylong = 0;
	u32 nlen = strlen(name);
	if(nlen > 12)
		onlylong = 1;
	else
	{
		char * dot = strrchr(name, '.');
		if((dot == NULL && nlen < 9) || (dot - name < 9 && nlen - 1 - (dot - name) < 4))
		{
			memset(&shortname[0], 0x20, 11);
			memcpy(&shortname[0], name, (dot == NULL) ? nlen : (dot - name));
			if(dot != NULL)
				memcpy(&shortname[8], dot + 1, nlen - 1 - (dot - name));
		}
		else
			onlylong = 1;
	}
	SceIoDirent sid;
	u32 i;
	for(i = 0; i < count; i ++) {
		if((entrys[i].norm.attr & FAT_FILEATTR_VOLUME) == 0 && entrys[i].norm.filename[0] != 0 && (u8)entrys[i].norm.filename[0] != 0xE5)
		{
			memset(&sid, 0, sizeof(SceIoDirent));
			int result;
			while((result = sceIoDread(dl, &sid)) > 0 && (sid.d_stat.st_attr & 0x08) > 0);
			if(result == 0)
				break;
			if(entrys[i].norm.filename[0] == 0x05)
				entrys[i].norm.filename[0] = 0xE5;
			if(!onlylong && strncasecmp(shortname, &entrys[i].norm.filename[0], 11) == 0)
			{
				if((u8)entrys[i].norm.filename[0] == 0xE5)
					entrys[i].norm.filename[0] = 0x05;
				memcpy(info, &entrys[i], sizeof(t_fat_entry));
				free((void *)entrys);
				if (fwVersion < 0x04000110)
					strcat(sname, sid.d_name);
				else {
					char short_name[MAX_FILENAME_LENGTH];
					memset(short_name, 0, MAX_FILENAME_LENGTH);
                                        fat_get_shortname(info, short_name);
                                        strcat(sname, short_name);
				}
				if((info->norm.attr & FAT_FILEATTR_DIRECTORY) > 0)
					strcat(sname, "/");
				sceIoDclose(dl);
				return 1;
			}
			if((u8)entrys[i].norm.filename[0] == 0xE5)
				entrys[i].norm.filename[0] = 0x05;
			char longnames[MAX_FILENAME_LENGTH];
			if(!fat_get_longname(entrys, i, longnames))
				continue;
			if(strcasecmp(name, longnames) == 0)
			{
				memcpy(info, &entrys[i], sizeof(t_fat_entry));
				free((void *)entrys);
				if (fwVersion < 0x04000110)
					strcat(sname, sid.d_name);
				else {
					char short_name[MAX_FILENAME_LENGTH];
					memset(short_name, 0, MAX_FILENAME_LENGTH);
                                        fat_get_shortname(info, short_name);
                                        strcat(sname, short_name);
				}
				if((info->norm.attr & FAT_FILEATTR_DIRECTORY) > 0)
					strcat(sname, "/");
				sceIoDclose(dl);
				return 1;
			}
		}
	}
	free((void *)entrys);
	sceIoDclose(dl);
	return 0;
}

static u32 fat_dir_clus(const char * dir, char * shortdir)
{
	if(!fat_load_table() || fatfd < 0)
		return 0;
	char rdir[MAX_FILENAME_LENGTH];
	strcpy(rdir, dir);
	char * partname = strtok(rdir, "/\\");
	if(partname == NULL)
	{
		fat_free_table();
		return 0;
	}
	if(strcmp(partname, "ms0:") != 0 && strcmp(partname, "fatms:") != 0 && strcmp(partname, "fatms0:") != 0)
	{
		fat_free_table();
		return 0;
	}
	strcpy(shortdir, partname);
	strcat(shortdir, "/");
	partname = strtok(NULL, "/\\");
	u32 clus = (fat_type == fat32) ? dbr.ufat.fat32.root_clus : 1;
	t_fat_entry entry;
	while(partname != NULL) {
		if(partname[0] != 0)
		{
			if(fat_locate(partname, shortdir, clus, &entry))
			{
				clus = entry.norm.clus_high;
				clus = ((fat_type == fat32) ? (clus << 16) : 0) + entry.norm.clus;
			}
			else
			{
				fat_free_table();
				return 0;
			}
		}
		partname = strtok(NULL, "/\\");
	}
	fat_free_table();
	return clus;
}

u32 fat_readdir(const char * dir, char * sdir, p_fat_info * info)
{

  DEBUGBOOKR("entering fat_readdir: dir:%s sdir:%s\n",dir, sdir);
	if(!fat_load_table() || fatfd < 0)
		return INVALID;
	u32 clus = fat_dir_clus(dir, sdir);
	SceUID dl = 0;
	if(clus == 0 || (dl = sceIoDopen(sdir)) < 0)
	{
		fat_free_table();
		sceIoDclose(dl);
		return INVALID;
	}
	u32 ecount = 0;
	p_fat_entry entrys;
	if(!fat_dir_list(clus, &ecount, &entrys))
	{
		fat_free_table();
		sceIoDclose(dl);
		return INVALID;
	}
	u32 count = 0, cur = 0, i;
	for(i = 0; i < ecount; i ++)
	{
		if((entrys[i].norm.attr & FAT_FILEATTR_VOLUME) != 0 || entrys[i].norm.filename[0] == 0 || (u8)entrys[i].norm.filename[0] == 0xE5 || (entrys[i].norm.filename[0] == '.' && entrys[i].norm.filename[1] == 0x20))
			continue;
		count ++;
	}
	if(count == 0 || (*info = (p_fat_info)malloc(count * sizeof(t_fat_info))) == NULL)
	{
		free(entrys);
		fat_free_table();
		sceIoDclose(dl);
		return INVALID;
	}
	SceIoDirent sid;
	for(i = 0; i < ecount; i ++)
	{
		if((entrys[i].norm.attr & FAT_FILEATTR_VOLUME) != 0 || entrys[i].norm.filename[0] == 0 || (u8)entrys[i].norm.filename[0] == 0xE5 || (entrys[i].norm.filename[0] == '.' && entrys[i].norm.filename[1] == 0x20))
			continue;
		memset(&sid, 0, sizeof(SceIoDirent));
		int result;
		while((result = sceIoDread(dl, &sid)) > 0 && ((sid.d_stat.st_attr & 0x08) > 0 || (sid.d_name[0] == '.' && sid.d_name[1] == 0)));
		if(result == 0)
			break;
		p_fat_info inf = &((*info)[cur]);
		fat_get_shortname(&entrys[i], inf->filename);
		if(inf->filename[0] == 0x05)
			inf->filename[0] = 0xE5;
		if(!fat_get_longname(entrys, i, inf->longname))
			strcpy(inf->longname, inf->filename);
		if (fwVersion < 0x04000110)
			strcpy(inf->filename, sid.d_name);
		inf->filesize = entrys[i].norm.filesize;
		inf->cdate = entrys[i].norm.cr_date;
		inf->ctime = entrys[i].norm.cr_time;
		inf->mdate = entrys[i].norm.last_mod_date;
		inf->mtime = entrys[i].norm.last_mod_time;
		inf->clus = ((fat_type == fat32) ? (((u32)entrys[i].norm.clus_high) << 16) : 0) + entrys[i].norm.clus;
		inf->attr = entrys[i].norm.attr;
		cur ++;
	}
	free(entrys);
	fat_free_table();
	sceIoDclose(dl);
	DEBUGBOOKR("exiting fat_readdir: dir:%s sdir:%s\n",dir, sdir);
	return cur;
}

void fat_free()
{
	if(fatfd >= 0)
	{
		sceIoClose(fatfd);
		fatfd = -1;
	}
	memset(&dbr, 0, sizeof(dbr));
	memset(&mbr, 0, sizeof(mbr));
	root_pos = 0;
	data_pos = 0;
	bytes_per_clus = 0;
	loadcount = 0;
	clus_max = 0;
	fat_type = fat16;
}
