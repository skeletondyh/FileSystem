#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "stdio.h"
#include "string.h"

#define PAGE_SIZE 4096  		// 每页Bytes
#define PAGE_SIZE_INT 1024		// 每页INT(PAGE_SIZE / 4)
#define PAGE_NUM 4130 			// 文件一共有多少页
#define INDEX_SIZE 32   		// 索引节点字节数
#define INDEX_NUM 4096			// 索引节点个数
#define INDEX_PAGE_NUM 32 		// 索引节点所占页数
#define INDEX_NUM_PER_PAGE 128  // 一页里有多少索引节点
#define SUPER_NUM 2  			// 超级节点所占页数
#define DIR_ENTRY_SIZE 256      // 目录项的大小
#define DIR_SIZE 252			// 目录名的长度
#define DIR_NUM 16              // 一个块中的目录数

/* 索引节点 */
struct inode 
{
	int i_id;     											// 索引节点的编号
	int parent;												// 父亲索引节点的编号
	int i_mode; 											// directory or file
	int i_file_size;    									// 文件大小 Bytes
	int i_block;      										// 数据block的位置
	char i_place_holder[INDEX_SIZE - 5 * sizeof(int)];    	// 占位
};

/* 超级节点 */
struct super_block
{
	bool inode_bitmap[PAGE_SIZE];
	bool block_bitmap[PAGE_SIZE];
};

/* 目录项 */
struct dir_entry
{
	char name[DIR_SIZE];          // 目录的名称
	int inode_id;                 // 目录对应的索引节点的ID
};

/* 目录块 */
struct dir_block
{
	dir_entry dirs[DIR_NUM];      // 一个目录块有16条记录
};


class FileManager {

private:

	FILE * fp; 					// 文件流指针

	/* 若文件不存在则创建并进行初始化 */
	void _createFile() {

		fp = fopen("DATA", "wb+");

		unsigned char * initbuf = new unsigned char[PAGE_SIZE * PAGE_NUM];
		memset((void *)initbuf, 0, PAGE_SIZE * PAGE_NUM);

		super_block * sp = (super_block *)initbuf;
		sp->inode_bitmap[0] = true;
		sp->block_bitmap[0] = true;

		/* 初始化0号索引节点 */
		unsigned char * offset = initbuf + 2 * PAGE_SIZE;
		inode * temp = (inode *)offset;
		temp->parent = -1;
		temp->i_id = 0;
		temp->i_mode = 0;										// 0 means directory
		temp->i_file_size = DIR_ENTRY_SIZE;
		temp->i_block = 0;										// 0 号索引节点对应 0 号数据块

		/* 初始化根目录节点 */
		unsigned char * pchar = initbuf + PAGE_SIZE * (SUPER_NUM + INDEX_PAGE_NUM + temp->i_block);
		dir_block * dirblock = (dir_block *)pchar;
		dirblock->dirs[0].name[0] = '/';
		dirblock->dirs[0].name[1] = '\0';
		dirblock->dirs[0].inode_id = temp->i_id;


		/* 初始化操作完成 全部写回 */
		int count = fwrite((void *)initbuf, sizeof(unsigned char), PAGE_NUM * PAGE_SIZE, fp);

		//printf("Initial Write Count: %d\n", count);
		//printf("ftell: %ld\n", ftell(fp));

		delete[] initbuf;
	}

public:

	FileManager() {

		fp = fopen("DATA", "r");
		if(fp == NULL) {
			_createFile();
		}
		fclose(fp);
		fp = fopen("DATA", "rb+");
		//printf("rb+ open where: %ld\n", ftell(fp));
		fseek(fp, 0, SEEK_SET);
	}

	~FileManager() {

		/* To Be Continued ....   需要额外操作吗？  */

		fclose(fp);
	}

	/* PageID 表示文件中的页号 */
	void readPage(int PageID, char * buf) {
		int error = fseek(fp, PageID * PAGE_SIZE, SEEK_SET);
		if (error) {
			printf("readPage Error!\n");
		}
		int temp = fread((void *)buf, sizeof(char), PAGE_SIZE, fp);
		//printf("Read Count: %d\n", temp);
	}

	/* PageID 表示文件中的页号 */
	void writePage(int PageID, char * buf) {
		int error = fseek(fp, PageID * PAGE_SIZE, SEEK_SET);
		if (error) {
			printf("writePage Error!\n");
		}
		int temp = fwrite((void *)buf, sizeof(char), PAGE_SIZE, fp);
		//printf("Write Count: %d\n", temp);
	}
};

#endif
