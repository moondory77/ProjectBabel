#ifndef __ENTRY_QUEUE_H__
#define __ENTRY_QUEUE_H__

#include "cocos2d.h"
#define DEFAULT_QUEUE_SIZE 5

USING_NS_CC;
using namespace std;


template <typename T>
class EntryQueue
{
private :

	const int queueSize;
	int front;
	int rear;
	
	T* Entry = NULL;
	float* weightEntry = NULL;

public:

	EntryQueue(int max_size);
	EntryQueue();

	//대기열 삭제 시, 내부 인스턴스들 모두 삭제
	~EntryQueue() 
	{
		while (!isEmpty()) { 
			//release(popFront()); 
			delete(popFront());
		}
		delete(Entry);
		delete(weightEntry);
	}

	bool isFull() { return (rear + 1) % queueSize == front; }
	bool isEmpty() { return rear == front; }
	
	const int getMaxSize() { return queueSize - 1; }
	int getCurSize(){
		if (front <= rear)	
			return (rear - front);
		else 
			return (rear + queueSize - front);
	}


	void pushBack(T obj_ptr);		//새 Obstacle을 대기열 끝에 포함 시킴
	T popFront();				//대기열 맨 앞 Obstacle을 제거

	T getFront();
	float getFrontWeight();
	void updateWeights(float deltaTime);	//(외부)스케쥴러에 시간 가중치 업데이트
};



#endif
