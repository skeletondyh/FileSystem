#ifndef FIND_PLACE_H
#define FIND_PLACE_H

#include "LinkList.h"

class FindPlace {
	
	LinkList * mylist;

public:

	void access(int index) {
		mylist->del(index);
	}

	void freepage(int index) {
		mylist->freepage(index);
	}

	int find() {
		int index = mylist->getFirst();
		mylist->del(index);
		return index;
	}

	FindPlace(int c) {
		mylist = new LinkList(c);
	}

	~FindPlace()
	{
		delete mylist;
	}
};

#endif // !FIND_PLACE_H

