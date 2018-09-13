#include "cocos2d.h"
#include "EventThread.h"


EventThread::EventThread() 
{
	head = new EventBox([]() {});
	this->addChild(head);

	cur = tail = head;
	targetList.clear();
};

void EventThread::pushBox(EventBox* box)
{
	this->addChild(box);

	tail->next = box;
	box->prev = tail;
	tail = tail->next;
}

void EventThread::popBox()
{
	if (tail != head){	
		if (cur == tail)
			cur = cur->prev;	
		tail = tail->prev;	
		tail->next->removeFromParent();
		tail->next->release();
		tail->next = NULL;
	}
}
