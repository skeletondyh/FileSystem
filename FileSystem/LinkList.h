#ifndef LINKLIST_H
#define LINKLIST_H

/* ����ڵ� �±����ָ�� */
struct Node {
	int prev;
	int next;
};

class LinkList {
	
	int cap;          // ����ͷβ�ڱ�
	Node * ListNodes;

public:

	/* 1 �� cap �ֱ��Ӧҳ���� 0 �� cap - 1 ҳ */
	LinkList(int c) {
		cap = c;
		ListNodes = new Node[cap + 2];
		ListNodes[0].prev = -1;
		ListNodes[cap + 1].next = -1;
		for (int i = 1; i < cap + 1; i++) {
			ListNodes[i].prev = i - 1;
			ListNodes[i - 1].next = i;
			ListNodes[i].next = i + 1;
			ListNodes[i + 1].prev = i;
		}
	}

	~LinkList()
	{
		delete[] ListNodes;
	}

	/* index Ϊҳ���е�����ֵ ��Ҫ��һ */
	void del(int index) {
		if (ListNodes[1 + index].next == cap + 1) {
			return;
		}
		ListNodes[ListNodes[1 + index].prev].next = ListNodes[1 + index].next;
		ListNodes[ListNodes[1 + index].next].prev = ListNodes[1 + index].prev;
		ListNodes[1 + index].next = cap + 1;
		ListNodes[1 + index].prev = ListNodes[1 + cap].prev;
		ListNodes[ListNodes[1 + cap].prev].next = 1 + index;
		ListNodes[1 + cap].prev = 1 + index;
	}

	/* index Ϊҳ���е�����ֵ ��Ҫ��һ */
	void freepage(int index) {
		if (ListNodes[1 + index].prev == 0) {
			return;
		}
		ListNodes[ListNodes[1 + index].prev].next = ListNodes[1 + index].next;
		ListNodes[ListNodes[1 + index].next].prev = ListNodes[1 + index].prev;
		ListNodes[1 + index].prev = 0;
		ListNodes[1 + index].next = ListNodes[0].next;
		ListNodes[ListNodes[0].next].prev = 1 + index;
		ListNodes[0].next = 1 + index;
	}

	int getFirst() {
		return ListNodes[0].next - 1;
	}
};

#endif // !LINKLIST_H
