#include "BufManager.h"
#include <assert.h>
#include <string>

void RemoveDir(BufManager * Bufm, super_block * sp, inode * pnode) {

	/* 保存当前状态  页表在递归过程中随时会被更新 */
	int tempblock = pnode->i_block;					
	int tempi = pnode->i_id;

	int iindex;
	int dataindex;

	int dirnum = pnode->i_file_size / DIR_ENTRY_SIZE;
	dir_block * cdir = (dir_block *)Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + pnode->i_block, dataindex);

	for (int i = 2; i < dirnum; i++) {
		char * nodepage = Bufm->getPage(SUPER_NUM + cdir->dirs[i].inode_id / INDEX_NUM_PER_PAGE, iindex);
		pnode = (inode *)(nodepage + INDEX_SIZE * (cdir->dirs[i].inode_id % INDEX_NUM_PER_PAGE));
		if (pnode->i_mode == 0) {
			RemoveDir(Bufm, sp, pnode);
		}
		else {
			sp->inode_bitmap[pnode->i_id] = false;
			sp->block_bitmap[pnode->i_block] = false;
			Bufm->remove(SUPER_NUM + INDEX_PAGE_NUM + pnode->i_block);
		}

		cdir = (dir_block *)Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + tempblock, dataindex);
		nodepage = Bufm->getPage(SUPER_NUM + tempi / INDEX_NUM_PER_PAGE, iindex);
		pnode = (inode *)(nodepage + INDEX_SIZE * (tempi % INDEX_NUM_PER_PAGE));
	}

	sp->inode_bitmap[tempi] = false;
	sp->block_bitmap[tempblock] = false;
	Bufm->release(dataindex);
	return;
}


int main()
{
	/* 命令处理所需 */
	char cmd[100];
	char buffer[2000];				//	$path的缓冲区
	char temp[2000];
	char currentdir[2000];			//  当前路径 递归
	char content[PAGE_SIZE];		//	输入的文件的缓冲区

	super_block * sp;
	FileManager * filem;
	BufManager * Bufm;
	
	/* 文件系统当前的状态（针对缓存来说） */
	int i_index;					// 当前索引节点所在缓存页的下标
	int data_index;					// 当前数据块所在缓存页的下标
	dir_block * dirb;				// 当前目录
	inode * currentINode;			// 当前索引节点
	char * currentipage;			// 当前索引节点所在页

	/* 实际在文件中的位置 */
	int currentiid = 0;										// 实际索引节点的id；
	int dataid = 0;											// 实际数据块的id;

	filem = new FileManager();
	Bufm = new BufManager(filem);
	sp = new super_block();

	memset((void *)sp, 0, sizeof(super_block));

	/* 读取索引节点和数据块的占用情况 */
	filem->readPage(0, (char *)sp->inode_bitmap);
	filem->readPage(1, (char *)sp->block_bitmap);

	/* 初始化页表 装入CAPACITY页 */
	for (int i = 2; i < CAPACITY + 2; i++) {
		Bufm->getPage(i, data_index);
	}

	/* 初始化时当前目录为根目录 初始化各变量 */
	char * root = Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM, data_index);
	dirb = (dir_block *)root;
	currentiid = dirb->dirs[0].inode_id;
	assert(currentiid == 0);
	currentipage = Bufm->getPage(SUPER_NUM, i_index);
	currentINode = (inode *)currentipage;
	assert(currentINode->i_id == 0);

	memset((void *)currentdir, 0, sizeof(currentdir));
	//currentdir[0] = '/';


	printf("Hello User X1054033!\n");
	printf("We are dingyh and dingry.\n");
	printf("Here is out file system!\n");
	printf("\n");

	/* 开始处理输入命令 */
	while (true) {

		/* 输出当前目录 */
		printf("/");
		printf("%s", currentdir);
		printf(" >>");

		scanf("%s", cmd);
		if (strcmp(cmd, "pwd") == 0) {
			printf("/");
			printf("%s\n", currentdir);            // 当前目录块的0索引表示当前目录名称
		}
		else if (strcmp(cmd, "exit") == 0) {
			printf("byebye\n");
			break;
		}
		else {

			if (strcmp(cmd, "echo") == 0) {
				memset((void *)content, 0, PAGE_SIZE);
				scanf("%s", content);
			}

			memset((void *)buffer, 0, sizeof(buffer));
			scanf("%s", buffer);
			char * p = buffer;

			dir_block * temproot;
			char *inodepage;
			inode *pnode;

			int tempi_index;
			int tempdata_index;

			bool err = false;
			bool noneExit = false;
			bool mkdir = false;
			bool mkfile = false;
			bool isFile = false;

			if (buffer[0] == '/') {							// 绝对路径

				//printf("Absolute Path\n");

				p = p + 1;

				/* 从根目录开始向下深入 */
				temproot = (dir_block *)Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM, tempdata_index);
				//printf("test cd in start: %s\n", temproot->dirs[0].name);
				inodepage = Bufm->getPage(SUPER_NUM, tempi_index);
				pnode = (inode *)inodepage;
				assert(pnode->i_id == 0);
			}
			else {

				//printf("Relative Path\n");

				temproot = dirb;
				inodepage = currentipage;
				pnode = currentINode;

			}

			while (*p != '\0') {

				/* 读目录名 */
				memset((void *)temp, 0, sizeof(temp));
				int k = 0;
				while (*p != '\0' && *p != '/') {
					temp[k] = *p;
					p++;
					k++;
				}

				if (k == 1 && temp[0] == '.') {
					p = p + 1;
					continue;
				}
				else if (strcmp(temp, "..") == 0) {
					assert(pnode->i_id == temproot->dirs[0].inode_id);
					//printf("Test : %s\n", temproot->dirs[0].name);
					if (temproot->dirs[0].inode_id != 0) {
						inodepage = Bufm->getPage(SUPER_NUM + pnode->parent / INDEX_NUM_PER_PAGE, tempi_index);
						pnode = (inode *)(inodepage + INDEX_SIZE * (pnode->parent % INDEX_NUM_PER_PAGE));
						temproot = (dir_block *)Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + pnode->i_block, tempdata_index);
						p = p + 1;
						continue;
					}
					else {
						*p = '\0';
						printf("%s No such directory\n", buffer);
						err = true;
						break;
					}
				}

				int dirnum = pnode->i_file_size / DIR_ENTRY_SIZE;
				int i;

				/* 用于试探目录的合法性 */
				int tempiid;
				char *tempinodepage;
				inode *temppnode;

				for (i = 1; i < dirnum; i++) {						// 目录块第 1 项表示父目录名（根目录没有父目录）
					if (i == 1 && temproot->dirs[0].inode_id != 0) {
						continue;
					}
					if (strcmp(temp, temproot->dirs[i].name) == 0) {
						tempiid = temproot->dirs[i].inode_id;
						tempinodepage = Bufm->getPage(SUPER_NUM + tempiid / INDEX_NUM_PER_PAGE, tempi_index);
						temppnode = (inode *)(inodepage + INDEX_SIZE * (tempiid % INDEX_NUM_PER_PAGE));
						if (temppnode->i_mode == 1 && *(p + 1) != '\0') {
							*p = '\0';
							err = true;
							printf("%s is not a directoty\n", buffer);
							break;
						}
						else {
							if (temppnode->i_mode == 1) {
								isFile = true;
							}
							temproot = (dir_block *)Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + temppnode->i_block, tempdata_index);
							inodepage = tempinodepage;
							pnode = temppnode;
							break;
						}
					}
				}

				if (i == dirnum) {

					noneExit = true;

					if (*(p + 1) != '\0') {
						*p = '\0';
						printf("%s No such directory\n", buffer);
						err = true;
					}
					else {										// 创建目录
						if (dirnum < DIR_NUM) {

							if (strcmp(cmd, "mkdir") == 0) {
								mkdir = true;
							}
							else if (strcmp(cmd, "echo") == 0) {
								mkfile = true;
							}
						}
						p = p + 1;
					}
				}
				else {
					assert(i < dirnum);
					if (*p == '/') {
						p = p + 1;
					}
				}
			}

			if (!err) {
				if (strcmp(cmd, "ls") == 0) {
					if (noneExit) {
						printf("%s No such directory\n", buffer);
					}
					else if (isFile) {
						printf("%s is not a directory\n", buffer);
					}
					else {

						//printf("test ls : %s\n", temproot->dirs[0].name);
						assert(pnode->i_id == temproot->dirs[0].inode_id);
						int dirnum = pnode->i_file_size / DIR_ENTRY_SIZE;
						if (dirnum > 1) {
							if (pnode->i_id == 0) {
								printf("%s\n", temproot->dirs[1].name);
							}
						}
						for (int j = 2; j < dirnum; j++) {
							printf("%s\n", temproot->dirs[j].name);
						}
					}
				}
				else if (strcmp(cmd, "cd") == 0) {
					if (noneExit) {
						printf("%s No such directory\n", buffer);
					}
					else if (isFile) {
						printf("%s is not a directory\n", buffer);
					}
					else {
						currentiid = pnode->i_id;
						dataid = pnode->i_block;
					}
				}
				else if (strcmp(cmd, "cat") == 0) {
					if (noneExit) {
						printf("%s No such file\n", buffer);
					}
					else if (!isFile) {
						printf("%s is not a file\n", buffer);
					}
					else {
						printf("%s\n", (char *)temproot);
					}
				}
				else if (strcmp(cmd, "rm") == 0) {
					if (noneExit) {
						printf("%s No such file\n", buffer);
					}
					else if (!isFile) {
						printf("%s is not a file\n", buffer);
					}
					else {															// 删除单个文件

						sp->block_bitmap[pnode->i_block] = false;					// 先在位图中标记为不占用
						sp->inode_bitmap[pnode->i_id] = false;

						Bufm->release(tempdata_index);								// 将其缓存页释放

						/* 得到父亲索引节点和父目录页 */
						char * parentipage = Bufm->getPage(SUPER_NUM + pnode->parent / INDEX_NUM_PER_PAGE, tempi_index);
						inode * parentnode = (inode *)(parentipage + INDEX_SIZE * (pnode->parent % INDEX_NUM_PER_PAGE));
						dir_block * parentdir = (dir_block *)(Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + parentnode->i_block, tempdata_index));

						/* 修改父亲索引节点和父目录页 */
						int dirnum = parentnode->i_file_size / DIR_ENTRY_SIZE;
						for (int i = 1; i < dirnum; i++) {
							if (i == 1 && parentdir->dirs[0].inode_id != 0) {
								continue;
							}
							if (strcmp(temp, parentdir->dirs[i].name) == 0) {
								for (int j = i; j < dirnum - 1; j++) {
									memcpy((void *)parentdir->dirs[j].name, (void *)parentdir->dirs[j + 1].name, DIR_ENTRY_SIZE);
								}
								break;
							}
						}

						parentnode->i_file_size -= DIR_ENTRY_SIZE;

						Bufm->markDirty(tempdata_index);
						Bufm->markDirty(tempi_index);
					}
				}
				else if (strcmp(cmd, "rmdir") == 0) {
					if (noneExit) {
						printf("%s No such directory\n", buffer);
					}
					else if (isFile) {
						printf("%s is not a directory\n", buffer);
					}
					else {

						if (temproot->dirs[0].inode_id != 0) {						// 无法删除根目录

							int tempparent = pnode->parent;							// 先把父亲节点存起来

							RemoveDir(Bufm, sp, pnode);

							/* 得到父亲索引节点和父目录页 */
							char * parentipage = Bufm->getPage(SUPER_NUM + tempparent / INDEX_NUM_PER_PAGE, tempi_index);
							inode * parentnode = (inode *)(parentipage + INDEX_SIZE * (tempparent % INDEX_NUM_PER_PAGE));
							dir_block * parentdir = (dir_block *)(Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + parentnode->i_block, tempdata_index));

							/* 修改父亲索引节点和父目录页 */
							int dirnum = parentnode->i_file_size / DIR_ENTRY_SIZE;
							for (int i = 1; i < dirnum; i++) {
								if (i == 1 && parentdir->dirs[0].inode_id != 0) {
									continue;
								}
								if (strcmp(temp, parentdir->dirs[i].name) == 0) {
									for (int j = i; j < dirnum - 1; j++) {
										memcpy((void *)parentdir->dirs[j].name, (void *)parentdir->dirs[j + 1].name, DIR_ENTRY_SIZE);
									}
									break;
								}
							}

							parentnode->i_file_size -= DIR_ENTRY_SIZE;

							Bufm->markDirty(tempdata_index);
							Bufm->markDirty(tempi_index);
						}

					}
				}
				else if (strcmp(cmd, "echo") == 0) {
					if (mkfile) {

						/* 修改当前目录和索引节点 */
						int newindex = pnode->i_file_size / DIR_ENTRY_SIZE;
						memset(temproot->dirs[newindex].name, 0, DIR_SIZE);
						strcpy(temproot->dirs[newindex].name, temp);
						printf("new file: %s\n", temproot->dirs[newindex].name);

						for (int k = 0; k < INDEX_NUM; k++) {
							if (sp->inode_bitmap[k] == false) {
								sp->inode_bitmap[k] = true;
								temproot->dirs[newindex].inode_id = k;
								break;
							}
						}
						pnode->i_file_size += DIR_ENTRY_SIZE;
						Bufm->markDirty(tempdata_index);
						Bufm->markDirty(tempi_index);

						/* 得到新增的节点页 */
						char * newinodepage;
						inode *newnode;

						newinodepage = Bufm->getPage(SUPER_NUM + temproot->dirs[newindex].inode_id / INDEX_NUM_PER_PAGE, tempi_index);
						newnode = (inode *)(newinodepage + INDEX_SIZE * (temproot->dirs[newindex].inode_id % INDEX_NUM_PER_PAGE));
						newnode->i_file_size = PAGE_SIZE;				 // 不存在追加文件的操作  直接覆盖
						newnode->i_mode = 1;							 // 1表示文件
						newnode->i_id = temproot->dirs[newindex].inode_id;
						newnode->parent = pnode->i_id;					 // 标明父亲节点
						for (int k = 0; k < PAGE_SIZE; k++) {
							if (sp->block_bitmap[k] == false) {
								sp->block_bitmap[k] = true;
								newnode->i_block = k;
								break;
							}
						}
						Bufm->markDirty(tempi_index);

						/* 得到新增的数据页 */
						char * newdata = Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + newnode->i_block, tempdata_index);
						memcpy((void *)newdata, (void *)content, PAGE_SIZE);
						Bufm->markDirty(tempdata_index);
					}
					else if (!isFile) {
						printf("%s is not a file\n", buffer);
					}
					else {
						memcpy((void *)temproot, (void *)content, PAGE_SIZE);
						Bufm->markDirty(tempdata_index);
					}
				}
				else if (strcmp(cmd, "mkdir") == 0) {
					if (mkdir) {

						/* 修改当前目录和当前索引节点 */
						int newindex = pnode->i_file_size / DIR_ENTRY_SIZE;
						memset(temproot->dirs[newindex].name, 0, DIR_SIZE);
						strcpy(temproot->dirs[newindex].name, temp);
						printf("new dir: %s\n", temproot->dirs[newindex].name);

						for (int k = 0; k < INDEX_NUM; k++) {
							if (sp->inode_bitmap[k] == false) {
								sp->inode_bitmap[k] = true;
								temproot->dirs[newindex].inode_id = k;
								break;
							}
						}
						pnode->i_file_size += DIR_ENTRY_SIZE;
						Bufm->markDirty(tempdata_index);
						Bufm->markDirty(tempi_index);

						/* 得到新增的节点页 */
						char * newinodepage;
						inode *newnode;
						dir_block *newdir;

						newinodepage = Bufm->getPage(SUPER_NUM + temproot->dirs[newindex].inode_id / INDEX_NUM_PER_PAGE, tempi_index);
						newnode = (inode *)(newinodepage + INDEX_SIZE * (temproot->dirs[newindex].inode_id % INDEX_NUM_PER_PAGE));
						newnode->i_file_size = 2 * DIR_ENTRY_SIZE;       // 当前目录与父目录 
						newnode->i_mode = 0;
						newnode->i_id = temproot->dirs[newindex].inode_id;
						newnode->parent = pnode->i_id;					 // 标明父亲节点
						for (int k = 0; k < PAGE_SIZE; k++) {
							if (sp->block_bitmap[k] == false) {
								sp->block_bitmap[k] = true;
								newnode->i_block = k;
								break;
							}
						}
						Bufm->markDirty(tempi_index);

						/* 得到新增的目录块 */
						newdir = (dir_block *)Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + newnode->i_block, tempdata_index);
						newdir->dirs[0].inode_id = newnode->i_id;
						strcpy(newdir->dirs[0].name, temp);

						newdir->dirs[1].inode_id = pnode->i_id;
						strcpy(newdir->dirs[1].name, temproot->dirs[0].name);
						printf("new father dir: %s\n", newdir->dirs[1].name);

						Bufm->markDirty(tempdata_index);
					}
					else if (isFile) {
						printf("%s is not a directory\n", buffer);
					}
					else {
						printf("%s already exits\n", buffer);
					}
				}
			}
		}


		dirb = (dir_block *)Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + dataid, data_index);
		assert(currentiid == dirb->dirs[0].inode_id);
		currentipage = Bufm->getPage(SUPER_NUM + currentiid / INDEX_NUM_PER_PAGE, i_index);
		currentINode = (inode *)(currentipage + INDEX_SIZE * (currentiid % INDEX_NUM_PER_PAGE));

		char dirbuffer[2000];
		memset((void *)currentdir, 0, sizeof(currentdir));

		while (currentINode->i_id != 0) {
			memset((void *)dirbuffer, 0, sizeof(dirbuffer));
			int p = 0;
			while (dirb->dirs[0].name[p] != '\0') {
				dirbuffer[p] = dirb->dirs[0].name[p];
				p++;
			}
			dirbuffer[p] = '/';
			p++;
			strcpy(dirbuffer + p, currentdir);
			memcpy(currentdir, dirbuffer, sizeof(currentdir));

			currentipage = Bufm->getPage(SUPER_NUM + currentINode->parent / INDEX_NUM_PER_PAGE, i_index);
			currentINode = (inode *)(currentipage + INDEX_SIZE * (currentINode->parent % INDEX_NUM_PER_PAGE));
			dirb = (dir_block *)Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + currentINode->i_block, data_index);
		}

		dirb = (dir_block *)Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + dataid, data_index);
		assert(currentiid == dirb->dirs[0].inode_id);
		currentipage = Bufm->getPage(SUPER_NUM + currentiid / INDEX_NUM_PER_PAGE, i_index);
		currentINode = (inode *)(currentipage + INDEX_SIZE * (currentiid % INDEX_NUM_PER_PAGE));

	}

	//getchar();
	//getchar();

	/* 写回BitMap */
	filem->writePage(0, (char *)sp->inode_bitmap);
	filem->writePage(1, (char *)sp->block_bitmap);
	delete Bufm;
	delete sp;
	return 0;
}