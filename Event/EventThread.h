#ifndef __EVENT_THREAD_H__
#define __EVENT_THREAD_H__
#include "cocos2d.h"
using namespace std;
USING_NS_CC;

/*
EventBox�� ������ Ŭ����
�� ���� ����(EventBox)���� ��, �ϳ��� ������(EventThread)�� �����Ѵ�.
���� ������ ������, ������ �ð����� ���Ѵ�.
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

	//��üȭ ��, ���� �̺�Ʈ ��������, initThread���� �����ϴ� �ɷ�..
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

	void pushBox(EventBox* box);				//thread �� ���� Box ����
	void popBox();								//thread �� ���� Box ����

};

#endif // !__EVENT_THREAD_H__
