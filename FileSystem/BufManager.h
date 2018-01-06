#ifndef BUF_MANAGER_H
#define BUF_MANAGER_H

#include "FileManager.h"
#include "FindPlace.h"

#define CAPACITY 1024   // ҳ���С


class BufManager {


	FileManager * fm;
	FindPlace *replace; // ����ҳ���滻˳��

	/* ID���� 0 ��ʼ */
	bool *dirty;        // ��ҳ�Ƿ�Ϊ��ҳ�����޸Ĺ���
	char ** table;      // ҳ��
	int * IDinfile;		// ÿһҳ��Ӧ���ļ��е�pageID
	int * IDincache;    // �ļ��е�ÿһҳ��Ӧ��cache�е�PageID
	int last;           // ������ʵ�ҳ��ID

	/* ������ҳ �����滻 ���б�Ҫ��writeback 
	   ��ʱPageID ��ҳ���б�Ȼû�ж�Ӧ�Ļ���ҳ */
	char * fetchPage(int PageID, int &index) {
		index = replace->find();
		if (table[index] == NULL) { 
			table[index] = new char[PAGE_SIZE];
			IDinfile[index] = PageID;
			IDincache[PageID] = index;
			return table[index];
		}
		else {
			if (dirty[index] == true) { // ��Ϊ��ҳ��д��
				int id = IDinfile[index];
				fm->writePage(id, table[index]);
				dirty[index] = false;
			}

			/* ����ֻ����м򵥵��滻 */
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

	/* Ϊ�ļ��е�һ��Page��Buf�л�ȡһҳ */
	char * getPage(int PageID, int &index, bool ifread = true) {
		if (IDincache[PageID] != -1) {  // ��ҳ�����и�ҳ ��һ�������µģ�
			index = IDincache[PageID];
			access(index);
			return table[index];
		}
		else {
			char * temp = fetchPage(PageID, index);  // ��û�������һ��ҳ
			if (ifread)
				fm->readPage(PageID, temp);
			return temp;
		}
	}

	/* ��ʼ������ */
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

