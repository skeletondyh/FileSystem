#include "BufManager.h"
#include <assert.h>

int main()
{
	/* ��������� */
	char cmd[100];
	char buffer[2000];				//	$path�Ļ�����
	char temp[2000];
	char currentdir[2000];			//  ��ǰ·�� �ݹ�
	char content[PAGE_SIZE];		//	������ļ��Ļ�����

	super_block * sp;
	FileManager * filem;
	BufManager * Bufm;
	
	/* �ļ�ϵͳ��ǰ��״̬����Ի�����˵�� */
	int i_index;					// ��ǰ�����ڵ����ڻ���ҳ���±�
	int data_index;					// ��ǰ���ݿ����ڻ���ҳ���±�
	dir_block * dirb;				// ��ǰĿ¼
	inode * currentINode;			// ��ǰ�����ڵ�
	char * currentipage;			// ��ǰ�����ڵ�����ҳ

	/* ʵ�����ļ��е�λ�� */
	//int i_pageID = SUPER_NUM;							// ʵ�������ڵ�����ҳ��pageID��
	//int data_pageID = SUPER_NUM + INDEX_PAGE_NUM;		// ʵ������ҳ��pageID��
	int currentiid = 0;										// ʵ�������ڵ��id��
	int dataid = 0;											// ʵ�����ݿ��id;

	filem = new FileManager();
	Bufm = new BufManager(filem);
	sp = new super_block();

	memset((void *)sp, 0, sizeof(super_block));

	/* ��ȡ�����ڵ�����ݿ��ռ����� */
	filem->readPage(0, (char *)sp->inode_bitmap);
	filem->readPage(1, (char *)sp->block_bitmap);

	/* ��ʼ��ҳ�� װ��CAPACITYҳ */
	for (int i = 2; i < CAPACITY + 2; i++) {
		Bufm->getPage(i, data_index);
	}

	/* ��ʼ��ʱ��ǰĿ¼Ϊ��Ŀ¼ ��ʼ�������� */
	char * root = Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM, data_index);
	dirb = (dir_block *)root;
	currentiid = dirb->dirs[0].inode_id;
	assert(currentiid == 0);
	currentipage = Bufm->getPage(SUPER_NUM, i_index);
	currentINode = (inode *)currentipage;
	assert(currentINode->i_id == 0);

	memset((void *)currentdir, 0, sizeof(currentdir));
	currentdir[0] = '/';


	/* ��ʼ������������ */
	while (true) {

		/* �����ǰĿ¼ */
		printf("%s", currentdir);
		printf(" >>");

		scanf("%s", cmd);
		if (strcmp(cmd, "pwd") == 0) {
			printf("%s\n", currentdir);            // ��ǰĿ¼���0������ʾ��ǰĿ¼����
		}
		else if (strcmp(cmd, "exit") == 0) {
			printf("byebye\n");
			break;
		}
		/*else if (strcmp(cmd, "ls") == 0) {                 // ls���ⲻ��
			int dirnum = currentINode->i_file_size / DIR_ENTRY_SIZE;
			if (currentINode->i_id == 0) {
				printf("%s\n", dirb->dirs[1].name);
			}
			for (int i = 2; i < dirnum; i++) {
				printf("%s\n", dirb->dirs[i].name);
			}
		}
		else if (strcmp(cmd, "cd") == 0) {

		}*/
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

			if (buffer[0] == '/') {							// ����·��

				printf("Absolute Path\n");

				p = p + 1;
				//int tempi_index;
				//int tempdata_index;

				/* �Ӹ�Ŀ¼��ʼ�������� */
				temproot = (dir_block *)Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM, tempdata_index);	
				//printf("test cd in start: %s\n", temproot->dirs[0].name);
				inodepage = Bufm->getPage(SUPER_NUM, tempi_index);
				pnode = (inode *)inodepage;
				assert(pnode->i_id == 0);

				while (*p != '\0') {

					/* ��Ŀ¼�� */
					memset((void *)temp, 0, sizeof(temp));
					int k = 0;
					while (*p != '\0' && *p != '/') {
						temp[k] = *p;
						p++;
						k++;
					}	

					int dirnum = pnode->i_file_size / DIR_ENTRY_SIZE;
					int i;

					/* ������̽Ŀ¼�ĺϷ��� */
					int tempiid;
					char *tempinodepage;
					inode *temppnode;

					for (i = 1; i < dirnum; i++) {						// Ŀ¼��� 1 ���ʾ��Ŀ¼������Ŀ¼û�и�Ŀ¼��
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
							//printf("*(p + 1) is : %c\n", *(p + 1));
							*p = '\0';
							printf("%s No such directory\n", buffer);
							err = true;
						}
						else {										// ����Ŀ¼
							if (dirnum < DIR_NUM) {

								if (strcmp(cmd, "mkdir") == 0) {
									mkdir = true;
								}
								else if (strcmp(cmd, "echo") == 0) {
									mkfile = true;
								}
							}
							p = p + 1;
								/* �����޸ĵ�ǰĿ¼�͵�ǰ�����ڵ� */
								/*memset(temproot->dirs[i].name, 0, DIR_SIZE);
								strcpy(temproot->dirs[i].name, temp);
								printf("new dir: %s\n", temproot->dirs[i].name);

								for (int k = 0; k < INDEX_NUM; k++) {
									if (sp->inode_bitmap[k] == false) {
										sp->inode_bitmap[k] = true;
										temproot->dirs[i].inode_id = k;
										break;
									}
								}
								pnode->i_file_size += DIR_ENTRY_SIZE;
								Bufm->markDirty(tempdata_index);
								Bufm->markDirty(tempi_index);*/

								/* Ȼ��õ������Ľڵ�ҳ */
								/*char * newinodepage;
								inode *newnode;
								dir_block *newdir;

								newinodepage = Bufm->getPage(SUPER_NUM + temproot->dirs[i].inode_id / INDEX_NUM_PER_PAGE, tempi_index);
								newnode = (inode *)(newinodepage + INDEX_SIZE * (temproot->dirs[i].inode_id % INDEX_NUM_PER_PAGE));
								newnode->i_file_size = DIR_ENTRY_SIZE;
								newnode->i_mode = 0;
								newnode->i_id = temproot->dirs[i].inode_id;
								for (int k = 0; k < PAGE_SIZE; k++) {
									if (sp->block_bitmap[k] == false) {
										sp->block_bitmap[k] = true;
										newnode->i_block = k;
										break;
									}
								}
								Bufm->markDirty(tempi_index);*/
								
								/* �õ�������Ŀ¼�� */
								/*newdir = (dir_block *)Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + newnode->i_block, tempdata_index);
								newdir->dirs[0].inode_id = newnode->i_id;
								strcpy(newdir->dirs[0].name, temp);

								newdir->dirs[1].inode_id = pnode->i_id;
								strcpy(newdir->dirs[1].name, temproot->dirs[0].name);
								printf("new father dir: %s\n", newdir->dirs[1].name);

								Bufm->markDirty(tempdata_index);
								break;*/
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
						/*else if (noneExit) {
							printf("%s No such directory\n", buffer);
						}*/
						else {

							printf("test ls : %s\n", temproot->dirs[0].name);
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
						/*else if (noneExit) {
							printf("%s No such directory\n", buffer);
						}*/
						else {
							currentiid = pnode->i_id;
							dataid = pnode->i_block;
							strcpy(currentdir, buffer);
						}
					}
					else if (strcmp(cmd, "cat") == 0) {
						if (noneExit) {
							printf("%s No such file\n", buffer);
						}
						else if (!isFile) {
							printf("%s is not a file\n", buffer);
						}
						/*else if (noneExit) {
							printf("%s No such file\n", buffer);
						}*/
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
						else {															// ɾ�������ļ�

						}
					}
					else if (strcmp(cmd, "echo") == 0) {
						if (mkfile) {

							/* �޸ĵ�ǰĿ¼�������ڵ� */
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

							/* �õ������Ľڵ�ҳ */
							char * newinodepage;
							inode *newnode;

							newinodepage = Bufm->getPage(SUPER_NUM + temproot->dirs[newindex].inode_id / INDEX_NUM_PER_PAGE, tempi_index);
							newnode = (inode *)(newinodepage + INDEX_SIZE * (temproot->dirs[newindex].inode_id % INDEX_NUM_PER_PAGE));
							newnode->i_file_size = PAGE_SIZE;				 // ������׷���ļ��Ĳ���  ֱ�Ӹ���
							newnode->i_mode = 1;							 // 1��ʾ�ļ�
							newnode->i_id = temproot->dirs[newindex].inode_id;
							newnode->parent = pnode->i_id;					 // �������׽ڵ�
							for (int k = 0; k < PAGE_SIZE; k++) {
								if (sp->block_bitmap[k] == false) {
									sp->block_bitmap[k] = true;
									newnode->i_block = k;
									break;
								}
							}
							Bufm->markDirty(tempi_index);

							/* �õ�����������ҳ */
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

							/* �޸ĵ�ǰĿ¼�͵�ǰ�����ڵ� */
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

							/* �õ������Ľڵ�ҳ */
							char * newinodepage;
							inode *newnode;
							dir_block *newdir;

							newinodepage = Bufm->getPage(SUPER_NUM + temproot->dirs[newindex].inode_id / INDEX_NUM_PER_PAGE, tempi_index);
							newnode = (inode *)(newinodepage + INDEX_SIZE * (temproot->dirs[newindex].inode_id % INDEX_NUM_PER_PAGE));
							newnode->i_file_size = 2 * DIR_ENTRY_SIZE;       // ��ǰĿ¼�븸Ŀ¼ 
							newnode->i_mode = 0;
							newnode->i_id = temproot->dirs[newindex].inode_id;
							newnode->parent = pnode->i_id;					 // �������׽ڵ�
							for (int k = 0; k < PAGE_SIZE; k++) {
								if (sp->block_bitmap[k] == false) {
									sp->block_bitmap[k] = true;
									newnode->i_block = k;
									break;
								}
							}
							Bufm->markDirty(tempi_index);

							/* �õ�������Ŀ¼�� */
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
			else {																// ���·��

				/* �Ƿ���Ҫ����Ĵ��룿 
					����������
				*/

			}
		}


		dirb = (dir_block *)Bufm->getPage(SUPER_NUM + INDEX_PAGE_NUM + dataid, data_index);
		assert(currentiid == dirb->dirs[0].inode_id);
		currentipage = Bufm->getPage(SUPER_NUM + currentiid / INDEX_NUM_PER_PAGE, i_index);
		currentINode = (inode *)(currentipage + INDEX_SIZE * (currentiid % INDEX_NUM_PER_PAGE));
	}

	getchar();
	getchar();

	/* д��BitMap */
	filem->writePage(0, (char *)sp->inode_bitmap);
	filem->writePage(1, (char *)sp->block_bitmap);
	delete Bufm;
	delete sp;
	return 0;
}