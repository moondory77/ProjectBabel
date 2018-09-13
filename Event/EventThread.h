#ifndef __EVENT_THREAD_H__
#define __EVENT_THREAD_H__
#include "cocos2d.h"
using namespace std;
USING_NS_CC;

/*
EventBox의 시퀀스 클래스
각 유닛 동작(EventBox)들이 모여, 하나의 시퀀스(EventThread)를 수행한다.
유닛 동작의 기준은, 동일한 시간으로 정한다.
*/

class EventBox : public CCNode
{
	friend class EventThread;

	function<void()> unitProc;
	EventBox* next = NULL;
	EventBox* prev = NULL;

public:

	EventBox(function<void()> lamda_func) { unitProc = lamda_func; }
	~EventBox() {}
	void play() { this->unitProc(); };
};



class EventThread : public CCNode
{
protected:

	EventBox *head, *tail;
	EventBox *cur;
	vector<Sprite*> targetList;

	//구체화 한, 실제 이벤트 쓰레드의, initThread에서 생성하는 걸로..
	inline void setTarget(initializer_list<Sprite*> targets) {
		for (auto iter = targets.begin(); iter != targets.end(); iter++) {
			targetList.push_back(*iter);
		}
	};

public:
	EventThread();

	virtual ~EventThread() {
		while (head != tail) { popBox(); }
		head->removeFromParent();
		head->release();
		targetList.clear();
	};

	virtual void initThread(initializer_list<Sprite*> targets) = 0;
	inline void playBox() { if (cur) cur->play(); }
	inline void moveCurNext() { if (cur->next != NULL) cur = cur->next; }
	inline void moveCurPrev() { if (cur->prev != NULL) cur = cur->prev; }

	void pushBox(EventBox* box);				//thread 맨 끝에 Box 삽입
	void popBox();								//thread 맨 끝의 Box 삭제

};

#endif // !__EVENT_THREAD_H__
