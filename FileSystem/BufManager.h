#ifndef BUF_MANAGER_H
#define BUF_MANAGER_H

#include "FileManager.h"
#include "FindPlace.h"

#define CAPACITY 1024   // 页表大小


class BufManager {


	FileManager * fm;
	FindPlace *replace; // 决定页的替换顺序

	/* ID均从 0 开始 */
	bool *dirty;        // 该页是否为脏页（被修改过）
	char ** table;      // 页表；
	int * IDinfile;		// 每一页对应的文件中的pageID
	int * IDincache;    // 文件中的每一页对应的cache中的PageID
	int last;           // 最近访问的页表ID

	/* 分配新页 进行替换 若有必要则writeback 
	   此时PageID 在页表中必然没有对应的缓存页 */
	char * fetchPage(int PageID, int &index) {
		index = replace->find();
		if (table[index] == NULL) { 
			table[index] = new char[PAGE_SIZE];
			IDinfile[index] = PageID;
			IDincache[PageID] = index;
			return table[index];
		}
		else {
			if (dirty[index] == true) { // 若为脏页则写回
				int id = IDinfile[index];
				fm->writePage(id, table[index]);
				dirty[index] = false;
			}

			/* 否则只需进行简单的替换 */
			int temp = IDinfile[index];
			IDinfile[index] = PageID;
			IDincache[PageID] = index;
			IDincache[temp] = -1;
			return table[index];
		}
	}

public:

	void access(int index) {
		if (index == last)
			return;
		replace->access(index);
		last = index;
	}

	void markDirty(int index) {
		dirty[index] = true;
		access(index);
	}

	void release(int index) {
		dirty[index] = false;
		replace->freepage(index);
	}

	void remove(int pageID) {
		if (IDincache[pageID] != -1) {
			release(IDincache[pageID]);
		}
	}

	/* 为文件中的一个Page在Buf中获取一页 */
	char * getPage(int PageID, int &index, bool ifread = true) {
		if (IDincache[PageID] != -1) {  // 若页表中有该页 （一定是最新的）
			index = IDincache[PageID];
			access(index);
			return table[index];
		}
		else {
			char * temp = fetchPage(PageID, index);  // 若没有则分配一新页
			if (ifread)
				fm->readPage(PageID, temp);
			return temp;
		}
	}

	/* 初始化操作 */
	BufManager(FileManager * p) {
		fm = p;
		replace = new FindPlace(CAPACITY);
		dirty = new bool[CAPACITY];
		table = new char*[CAPACITY];
		IDinfile = new int[CAPACITY];
		last = -1;
		for (int i = 0; i < CAPACITY; i++) {
			table[i] = NULL;
			dirty[i] = false;
			IDinfile[i] = -1;
		}

		IDincache = new int[PAGE_NUM];
		for (int k = 0; k < PAGE_NUM; k++) {
			IDincache[k] = -1;
		}
	}

	~BufManager()
	{
		for (int i = 0; i < CAPACITY; i++) {
			if (table[i] != NULL) {
				if (dirty[i] == true) {
					int pageid = IDinfile[i];
					fm->writePage(pageid, table[i]);
				}
				delete[] table[i];
			}
		}
		delete[] table;
		delete fm;
		delete replace;
		delete[] dirty;
		delete[] IDinfile;
		delete[] IDincache;
	}
};


#endif // 

