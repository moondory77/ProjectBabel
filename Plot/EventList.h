#ifndef  __EventList_H__
#define __EventList_H__

#include "cocos2d.h"
#include "Plot/EventBox.h"

USING_NS_CC;


/* EventBox���� SequenceList   
 * Event ���� �ڷᱸ��
 * �ϳ��� EventList�� �ϳ��� EventCanvas�� ���� �ۿ� 
 * �� EventList Ŭ������ �̱���
 * EventBox �ν��Ͻ��� ����/������ ��� EventList ������ �̷������.
 */

class EventList : public cocos2d::CCNode
{
	//String* name;
	Box* head;
	Box* cur;
	int size;

public:
	EventList() {
		head = new Box(NULL, NULL);
		head->next = head;
		head->prev = head;

		cur = head;
		size = 0;
	}

	int getSize() { return this->size; }
	Box* getHead() { return this->head; }

	Box* scanBox(int seq);	//seq ��°�� Box ������ ��ȯ
	
	void insertBox(int seq, Box* box);	//seq ��°�� Box ����
	void insertBox(int seq, Box box);

	void deleteBox(int seq); //seq ��° Box ����
	
	void pushBox(Box* box); //����Ʈ �� ���� Box ����
	void pushBox(Box box);

	void popBox();	//����Ʈ �� �� Box�� ���� 

	void moveCurTo(int seq);	// seq ��°�� Ŀ�� �̵�
	void moveCurNext();	// curBox ��ȯ cursor �Ѵܰ� ����,
	void moveCurBack();	// curBox ��ȯ cursor �Ѵܰ� �ڷ�,


	Box* scanCur();	//curBox ��ȯ

	~EventList() {
		int last = size;
		while (last > 0) {
			deleteBox(last);
			last--;
		}
		delete head;	//������ header���� ����
	}
};




#endif // ! __EventList_H__

