#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROOT_START 0x2600
#define DATA_START 0x3e00

typedef int BOOL;
#define TRUE 1
#define FALSE 0

typedef struct {
	char	BPB_BytsPerSec[2];													//每扇区字节数
	char	BPB_SecPerClus[1];													//每簇扇区数
	char	BPB_RsvdSecCnt[2];													//Boot记录占用多少扇区
	char	BPB_NumFATs[1];														//共有多少FAT表
	char	BPB_RootEntCnt[2];													//根目录文件数最大值
	char	BPB_TotSec16[2];													//扇区总数
	char	BPB_Media[1];														//介质描述符
	char	BPB_FATSz16[2];														//每FAT扇区数  
	char	BPB_SecPerTrk[2];													//每磁道扇区数
	char	BPB_NumHeads[2];													//磁头数
	char	BPB_HiddSec[4];														//隐藏扇区数
	char	BPB_TotSec32[4];													//如果BPB_TotSec16为0 则由这个值记录扇区数
} BPB;

typedef struct {
	char	DIR_Name[11];														//文件名8字节扩展名3字节
	char	DIR_Attr[1];														//文件属性
	char	DIR_Reserved[10];													//保留位
	char	DIR_WrtTime[2];														//最后一次写入时间
	char	DIR_WrtDate[2];														//最后一次写入日期
	char	DIR_FstClus[2];														//此条目对应的开始簇号
	char	DIR_FileSize[4];													//文件大小
} RootDir;

void readFile(FILE* file, char* path, int offset, RootDir* rd);
void addToPath(char* path, char* oldPath);
void printFile(char* path, RootDir* rd);
int TotalSection;
int main(){
	FILE *file = fopen("a.img", "r");											//打开 a.img
	RootDir* rd = (RootDir*)malloc(sizeof(RootDir));							//分配地址空间
	memset(rd, 0, sizeof(RootDir));

	readFile(file, "", ROOT_START, rd);

	free(rd);
	fclose(file);
	return 0;
}

void readFile(FILE* file, char* path, int offset, RootDir* rd){

	int RD_Size = sizeof(RootDir);

	long point = ftell(file);
	fseek(file, offset, SEEK_SET);
	do {
		memset(rd, 0, RD_Size);
		fread(rd, RD_Size, 1, file);
		if (isDir(rd)){
			int fst_Clus = rd->DIR_FstClus[0] + rd->DIR_FstClus[1] * 0x10;
			char* newPath = (char*)malloc(strlen(path) + 0x08);
			memset(newPath, 0, strlen(newPath));
			memcpy(newPath, path, strlen(path));
			RootDir oldRd;
			memcpy(&oldRd, rd, RD_Size);
			addToPath(newPath, rd->DIR_Name);
			readFile(file, newPath, (fst_Clus * 0x200 + DATA_START), rd);
			memcpy(rd, &oldRd, RD_Size);
			free(newPath);
		}else{
			if (isValid(rd)){
				// printf("%s%s\n",path, rd->DIR_Name);
				printFile(path, rd);
			}
		}
	}while(!isEmpty(rd));
	fseek(file, point, SEEK_SET);

}

void addToPath(char* path, char* oldPath){
	int p = 0;
	while (path[p] != 0)
		p += 8;
	int i = 0;
	for (i = 0; i < 8; i++){
		path[p + i] = oldPath[i];
	}
}

BOOL isEmpty(RootDir* rd){
	int i = 0;
	for (i = 0; i < 0x20; i++){
		if(((char*)rd)[i] > 0){
			return FALSE;
		}
	}
	return TRUE;
}
BOOL isDir(RootDir* rd){
	char c = rd->DIR_Name[0];
	if(c < 0x30 || c > 0x7a)
		return FALSE;

	int i = 0x00;
	for (i = 0x00; i < 0x08; i++){
		c = rd->DIR_Name[i];
		if ( c < 0x20 || c > 0x7d){
			return FALSE;
		}
	}
	for (i = 0x08; i < 0x0a; i++){
		c = rd->DIR_Name[i];
		if( c < 0x20 || c > 0x7d){
			return FALSE;
		} else if( c != 0x20 ){
			return FALSE;
		}
	}
	return TRUE;
}
BOOL isValid(RootDir* rd){
	int j = 0;
		for (j = 0; j < 11; j++){
			char c = rd->DIR_Name[j];
			if (! ((c >= '0' && c <= '9') 
				|| (c >= 'a' && c <= 'z')
				|| (c >= 'A' && c <= 'Z')
				|| c == ' ')){
				return FALSE;
			} 
		}
	return TRUE;
}

void printFile(char* path, RootDir* rd){
	int i = 0;
	while (i < strlen(path)){
		if (path[i] != ' '){
			printf("%c", path[i]);
		}
		if (i % 8 == 7){
			printf("/");
		}
		i++;
	}

	for (i = 0; i < 8; i++){
		if (rd->DIR_Name[i] == ' '){
			break;
		}
		printf("%c", rd->DIR_Name[i]);
	}
	printf(".");
	for (i = 8; i < 11; i++){
		if (rd->DIR_Name[i] == ' '){
			break;
		}
		printf("%c", rd->DIR_Name[i]);
	}
	printf("\n");
}