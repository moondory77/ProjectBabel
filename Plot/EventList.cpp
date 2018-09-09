#include <string>
#include "Plot/EventList.h"
#include "Plot/EventBox.h"

using std::string;
USING_NS_CC;


//seq번째 box 반환
Box* EventList::scanBox(int seq)
{
	Box* scan = head;

	if (seq < 0 || seq > size)
	{
		//CCLOG("잘못된 인덱스 접근. idx: %d, ListSize: %d", seq, size);
		return NULL;
	}
	else {
		for (int i = 0; i < seq; i++)
			scan = scan->next;

		//CCLOG("Box 접근. idx : %d, ListSize: %d", seq, size);
		return scan;
	}
}


//리스트의 seq 번째 Box를 삭제
void EventList::deleteBox(int seq)
{
	if (seq == 0) {
		//CCLOG("헤더는 삭제할 수 없습니다.");
	}
	else
	{
		Box* scan = scanBox(seq);
		//seq번째 Box에 접근 성공했을 때,
		if (scan != NULL)
		{
			scan->prev->next = scan->next;
			scan->next->prev = scan->prev;

			delete(scan);
			size--;

			//if (seq == size)
				//CCLOG("Box Pop 성공! Current size : %d", size);
			//else
				//CCLOG("Box 삭제 성공! 삭제 idx: %d, Current size : %d", seq, size);
		}
	}
	return;
}

//seq 번째에 Box 삽입 
void EventList::insertBox(int seq, Box* box)
{
	Box* scan = scanBox(seq);

	//seq번째 Box에 접근 성공했을 때,
	if (scan != NULL)
	{
		box->next = scan;
		box->prev = scan->prev;

		scan->prev->next = box;
		scan->prev = box;

		size++;

		//push일 경우를 고려해서 출력메시지
		//if (seq == 0)
			//CCLOG("Box Push 성공! Current size : %d", size);
		//else
			//CCLOG("Box 삽입 성공! 삽입 idx: %d, Current size : %d", seq, size);
	}
	return;
}

void EventList::insertBox(int seq, Box box){
	
	insertBox(seq, &box);
	return;
}


//리스트 맨 끝에 Box 삽입 
void EventList::pushBox(Box* box)
{
	insertBox(0, box);
	return;
}

void EventList::pushBox(Box box)
{
	insertBox(0, &box);
}

//맨 끝의 Box를 삭제
void EventList::popBox()
{
	deleteBox(size);
	return;
}


//현재 커서를 리스트의 seq 번째로 이동
void EventList::moveCurTo(int seq)
{
	Box* scan = scanBox(seq);

	if (scan != NULL)
	{
		cur = scan;
		//CCLOG("커서 이동 성공! 현재 커서 idx: %d", seq);
	}
	//else
		//CCLOG("커서 이동 실패!");

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






