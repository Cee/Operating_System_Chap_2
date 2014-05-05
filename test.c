#include <stdio.h>

//TypeDef
typedef unsigned char byte_1;
typedef unsigned short byte_2;
typedef unsigned int byte_4;

int TotalSection;				//记录总扇区数目

#pragma pack (1)				//对齐

//Define the BIOS Parameter Block
struct BPB {
	byte_2	BPB_BytsPerSec;		//每扇区字节数
	byte_1	BPB_SecPerClus;		//每簇扇区数
	byte_2	BPB_RsvdSecCnt;		//Boot记录占用多少扇区
	byte_1	BPB_NumFATs;		//共有多少FAT表
	byte_2	BPB_RootEntCnt;		//根目录文件数最大值
	byte_2	BPB_TotSec16;		//扇区总数
	byte_1	BPB_Media;			//介质描述符
    byte_2	BPB_FATSz16; 		//每FAT扇区数  
    byte_2	BPB_SecPerTrk;		//每磁道扇区数
    byte_2	BPB_NumHeads;		//磁头数
    byte_4	BPB_HiddSec;		//隐藏扇区数
    byte_4  BPB_TotSec32;		//如果BPB_TotSec16为0 则由这个值记录扇区数
};

//Define the root directory
struct RootDirectory {
	char	DIR_Name[11];		//文件名8字节扩展名3字节
	byte_1	DIR_Attr;			//文件属性
	char	DIR_Reserved[10];	//保留位
	byte_2	DIR_WrtTime;		//最后一次写入时间
	byte_2	DIR_WrtDate;		//最后一次写入日期
	byte_2	DIR_FstClus;		//此条目对应的开始簇号
	byte_4	DIR_FileSize;		//文件大小
};

#pragma pack ()					//取消对齐

void my_print(char *, int);

int main(){

	FILE *file = fopen("a.img", "rb");		//打开 a.img

	struct BPB bpb;							//创建 BPB 
	struct BPB* p = &bpb; 					//指向 BPB 的指针
	fseek(file, 11, SEEK_SET);				//指针指向第11个Byte 忽略短跳转指令(3)和厂商名(8)
	fread(p, 1, 25, file);					//读取后25个Byte作为 BPB

	struct RootDirectory rd;				//创建根目录区条目
	struct RootDirectory* q = &rd;			//指向根目录区条目的指针

	TotalSection = (p->BPB_FATSz16 == 0) ? p->BPB_TotSec32 : p->BPB_FATSz16;
											//判断扇区数目

	int first_byte = (p->BPB_RsvdSecCnt + p->BPB_NumFATs * TotalSection) * p->BPB_BytsPerSec;
											//确定开始位置
	//遍历
	int i = 0;
	for(i = 0; i < p->BPB_RootEntCnt; i++){

		fseek(file, first_byte, SEEK_SET);	//定位到开始位置
		fread(q, 1, 32, file);				//每次读取32个字节 作为RootDirectory
		first_byte += 32;					//指针定位到下一个位置

		//若文件名空 则说明无文件
		if (q->DIR_Name[0] == '\0'){
			continue;
		}

		//若文件名除了英文数字空格 则忽略
		int j = 0;
		for (j = 0; j < 11; j++){
			if (! ((q->DIR_Name[j] >= '0' && q->DIR_Name[j] <= '9') 
				|| (q->DIR_Name[j] >= 'a' && q->DIR_Name[j] <= 'z')
				|| (q->DIR_Name[j] >= 'A' && q->DIR_Name[j] <= 'Z')
				|| q->DIR_Name[j] == ' ')){
				break;
			} 
		}
		if (j < 11){
			continue;
		}

		//处理正常的文件目录
		char name[12];						//保存文件名或者目录名称
		
		j = 0;
		int count = 0;
		bool isDictionary = false;			//标记是否是目录 是目录则需要遍历 
		for (j = 0; j < 11; j++){
			if (q->DIR_Name[j] != ' '){
				name[count] = q->DIR_Name[j];
			} else {
				//根据DIR_Attr判断是文件还是目录
				if ((q->DIR_Attr & 0x10) != 0){
					//目录
					isDictionary = true;
					count++;
					break;
				} else {
					//文件
					name[count] = '.';
					while (q->DIR_Name[j] == ' '){
						j++;
					}
					j--;
				}
			}
			count++;
		}
		name[count] = '\0';
		printf("%s\n", name);





	}

	fclose(file);							//关闭 a.img

}