#include <string>
#include "Plot/EventList.h"
#include "Plot/EventBox.h"

using std::string;
USING_NS_CC;


//seq��° box ��ȯ
Box* EventList::scanBox(int seq)
{
	Box* scan = head;

	if (seq < 0 || seq > size)
	{
		//CCLOG("�߸��� �ε��� ����. idx: %d, ListSize: %d", seq, size);
		return NULL;
	}
	else {
		for (int i = 0; i < seq; i++)
			scan = scan->next;

		//CCLOG("Box ����. idx : %d, ListSize: %d", seq, size);
		return scan;
	}
}


//����Ʈ�� seq ��° Box�� ����
void EventList::deleteBox(int seq)
{
	if (seq == 0) {
		//CCLOG("����� ������ �� �����ϴ�.");
	}
	else
	{
		Box* scan = scanBox(seq);
		//seq��° Box�� ���� �������� ��,
		if (scan != NULL)
		{
			scan->prev->next = scan->next;
			scan->next->prev = scan->prev;

			delete(scan);
			size--;

			//if (seq == size)
				//CCLOG("Box Pop ����! Current size : %d", size);
			//else
				//CCLOG("Box ���� ����! ���� idx: %d, Current size : %d", seq, size);
		}
	}
	return;
}

//seq ��°�� Box ���� 
void EventList::insertBox(int seq, Box* box)
{
	Box* scan = scanBox(seq);

	//seq��° Box�� ���� �������� ��,
	if (scan != NULL)
	{
		box->next = scan;
		box->prev = scan->prev;

		scan->prev->next = box;
		scan->prev = box;

		size++;

		//push�� ��츦 ����ؼ� ��¸޽���
		//if (seq == 0)
			//CCLOG("Box Push ����! Current size : %d", size);
		//else
			//CCLOG("Box ���� ����! ���� idx: %d, Current size : %d", seq, size);
	}
	return;
}

void EventList::insertBox(int seq, Box box){
	
	insertBox(seq, &box);
	return;
}


//����Ʈ �� ���� Box ���� 
void EventList::pushBox(Box* box)
{
	insertBox(0, box);
	return;
}

void EventList::pushBox(Box box)
{
	insertBox(0, &box);
}

//�� ���� Box�� ����
void EventList::popBox()
{
	deleteBox(size);
	return;
}


//���� Ŀ���� ����Ʈ�� seq ��°�� �̵�
void EventList::moveCurTo(int seq)
{
	Box* scan = scanBox(seq);

	if (scan != NULL)
	{
		cur = scan;
		//CCLOG("Ŀ�� �̵� ����! ���� Ŀ�� idx: %d", seq);
	}
	//else
		//CCLOG("Ŀ�� �̵� ����!");

	return;
}

void EventList::moveCurNext() {
	cur = cur->next;
	return;
}

void EventList::moveCurBack() {
	cur = cur->prev;
	return;
}

Box* EventList::scanCur() {
	return cur;
}






