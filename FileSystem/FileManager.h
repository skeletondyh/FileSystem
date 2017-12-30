#ifndef FILE_MANAGER
#define FILE_MANAGER

#include "stdio.h"
#include <cstring>

#define PAGE_SIZE 4096  		// 每页Bytes
#define PAGE_SIZE_INT 1024		// 每页INT(PAGE_SIZE / 4)
#define PAGE_NUM 4130 			// 文件一共有多少页
#define INDEX_SIZE 32   		// 索引节点字节数
#define INDEX_NUM 4096			// 索引节点个数
#define INDEX_PAGE_NUM 32 		// 索引节点所占块数
#define SUPER_NUM 2  			// 超级节点所占块数


typedef struct 
{
	int i_id;     											// 索引节点的编号
	int i_mode; 											// directory or file
	int i_file_size;    									// 文件大小 Bytes
	int i_block;      										// 数据block的位置
	char i_place_holder[INDEX_SIZE - 4 * sizeof(int)];    	// 占位
}inode;

/* 超级节点 */
typedef struct super_block
{
	bool inode_bitmap[PAGE_SIZE];
	bool block_bitmap[PAGE_SIZE];

	super_block() {
		for(int i = 0; i < PAGE_SIZE; i++) {
			inode_bitmap[i] = false;
			block_bitmap[i] = false;
		}

		printf("Construct super_block\n");
	}
}super_block;

/* 目录项 */
typedef struct
{
	char name[252];
	int inode_id;
}dir_entry;

/* 目录块 */
typedef struct
{
	dir_entry dirs[16];
}dir_block;


class FileManager {

private:

	FILE * fp; 					// 文件流指针
	super_block sp_block;       // BitMap
	dir_entry currentDir;

	/* 若文件不存在则创建并进行初始化 */
	void _createFile() {

		fp = fopen("DATA", "wb+");

		unsigned char * initbuf = new unsigned char[PAGE_SIZE * PAGE_NUM];
		memset((void *)initbuf, 0, PAGE_SIZE * PAGE_NUM);

		/* 初始化0号索引节点 */
		unsigned int * offset = (unsigned int *)(initbuf + 2 * PAGE_SIZE);
		inode * temp = (inode *)offset;
		temp->i_id = 0;
		temp->i_mode = 0;   // 0 means directory
		temp->i_file_size = 256;
		temp->i_block = SUPER_NUM + INDEX_PAGE_NUM;

		/* 初始化根目录节点 */
		unsigned char * pchar = initbuf + PAGE_SIZE * temp->i_block;
		dir_block * dirblock = (dir_block *)pchar;
		dirblock->dirs[0].name[0] = '/';
		dirblock->dirs[0].inode_id = temp->i_id;

		/* 初始化Super Block 
		   初始化后不用写入文件
		   关闭文件时最后写入 */
		sp_block.inode_bitmap[0] = true;
		sp_block.block_bitmap[0] = true;


		int count = fwrite((void *)initbuf, sizeof(unsigned char), PAGE_NUM * PAGE_SIZE, fp);
		//fflush(fp);
		printf("Write Count: %d\n", count);

		//fseek(fp, 0, SEEK_SET);
		//fwrite((void *)(&sp_block), sizeof(bool), sizeof(super_block), fp);
		//fflush(fp);

		printf("ftell: %ld\n", ftell(fp));

		delete[] initbuf;
	}

public:

	FileManager() {

		currentDir.inode_id = 0;
		currentDir.name[0] = '/';
		currentDir.name[1] = '\0';

		fp = fopen("DATA", "r");
		if(fp == NULL) {
			_createFile();
		}
		else {
			fclose(fp);
			fp = fopen("DATA", "rb+");
			//printf("ftell2: %ld\n", ftell(fp));
			fseek(fp, 0, SEEK_SET);
			//printf("ftell3: %ld\n", ftell(fp));
			int temp = fread((void *)(&sp_block), sizeof(bool), sizeof(super_block), fp);
			//printf("ftell4: %ld\n", ftell(fp));
			printf("Read Super Block: %d\n", temp);
			//printf("%d%d", sp_block.inode_bitmap[0], sp_block.block_bitmap[0]);
			//if (sp_block.inode_bitmap[0] == true && sp_block.block_bitmap[0] == true) {
				//printf("Save Success\n");
			//}
		}
	}

	~FileManager() {
		rewind(fp);
		fwrite((void *)(&sp_block), sizeof(bool), sizeof(super_block), fp);
		//int error = fseek(fp, 0, SEEK_SET);
		//if (error) {
			//printf("ERROR!\n");
		//}
		//printf("superblock size: %d\n", sizeof(super_block));
		//printf("\n%d%d\n", sp_block.inode_bitmap[0], sp_block.block_bitmap[0]);

		//FILE * testf = fopen("hehe", "w");
		//fclose(fp);
		//fp = fopen("DATA", "w");
		//int * test = new int[1000];
		//for (int i = 0; i < 1000; i++) {
			//test[i] = 7;
		//}
		//int count1 = fwrite((void *)(test), sizeof(int), 1000, fp);
		//fclose(testf);
		//int count2 = fwrite((void *)(sp_block.block_bitmap), sizeof(char), PAGE_SIZE, fp);
		//fflush(fp);
		//printf("count: %d\n", count1);
		fclose(fp);
		getchar();
		getchar();
	}

	void Show() {
		printf(currentDir.name);
		//putchar(' ');
		//putchar('>');
		//putchar('>');
	}

	void TestRead(int PageId, unsigned char * buf) {
		int error = fseek(fp, PageId * PAGE_SIZE, SEEK_SET);
		if(error) {
			printf("Error!\n");
		}
		int temp = fread((void *)buf, sizeof(unsigned char), PAGE_SIZE, fp);
		printf("%d\n", temp);
	}
};

#endif
