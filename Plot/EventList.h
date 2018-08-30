#ifndef  __EventList_H__
#define __EventList_H__

#include "cocos2d.h"
#include "Plot/EventBox.h"

USING_NS_CC;


/* EventBox들의 SequenceList   
 * Event 전개 자료구조
 * 하나의 EventList는 하나의 EventCanvas에 대해 작용 
 * 각 EventList 클래스는 싱글톤
 * EventBox 인스턴스의 생성/삭제는 모두 EventList 내에서 이루어진다.
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

	Box* scanBox(int seq);	//seq 번째의 Box 포인터 반환
	
	void insertBox(int seq, Box* box);	//seq 번째에 Box 삽입
	void insertBox(int seq, Box box);

	void deleteBox(int seq); //seq 번째 Box 삭제
	
	void pushBox(Box* box); //리스트 맨 끝에 Box 삽입
	void pushBox(Box box);

	void popBox();	//리스트 맨 끝 Box를 삭제 

	void moveCurTo(int seq);	// seq 번째로 커서 이동
	void moveCurNext();	// curBox 반환 cursor 한단계 진행,
	void moveCurBack();	// curBox 반환 cursor 한단계 뒤로,


	Box* scanCur();	//curBox 반환

	~EventList() {
		int last = size;
		while (last > 0) {
			deleteBox(last);
			last--;
		}
		delete head;	//마지막 header까지 삭제
	}
};




#endif // ! __EventList_H__

