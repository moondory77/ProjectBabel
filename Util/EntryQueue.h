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

	//��⿭ ���� ��, ���� �ν��Ͻ��� ��� ����
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


	void pushBack(T obj_ptr);		//�� Obstacle�� ��⿭ ���� ���� ��Ŵ
	T popFront();				//��⿭ �� �� Obstacle�� ����

	T getFront();
	float getFrontWeight();
	void updateWeights(float deltaTime);	//(�ܺ�)�����췯�� �ð� ����ġ ������Ʈ
};



#endif
