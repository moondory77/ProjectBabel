#include "EntryQueue.h"

template <typename T>
EntryQueue<T>::EntryQueue() : queueSize(DEFAULT_QUEUE_SIZE + 1)
{
	front = 0;
	rear = 0;
	Entry = new T[queueSize];
	weightEntry = new float[queueSize];
};


template <typename T>
EntryQueue<T>::EntryQueue(int max_size) : queueSize(max_size + 1)
{
	front = 0;
	rear = 0;
	Entry = new T[queueSize];
	weightEntry = new float[queueSize];
}


template <typename T>
void EntryQueue<T>::pushBack(T obj_ptr)
{
	if (!isFull()) {
		rear = (rear + 1) % queueSize;
		Entry[rear] = obj_ptr;
		weightEntry[rear] = 0;
	}
}


template <typename T>
T EntryQueue<T>::popFront()
{
	if (!isEmpty())
	{
		front = (front + 1) % queueSize;
		return Entry[front];
	}
	else
		return NULL;
}



template <typename T>
T EntryQueue<T>::getFront()
{
	if (!isEmpty())
		return Entry[(front + 1) % queueSize];
	else
		return NULL;
}


template <typename T>
float EntryQueue<T>::getFrontWeight()
{
	if (!isEmpty())
	{
		return weightEntry[(front + 1) % queueSize];
	}
	else
		return -1;
};


template <typename T>
void EntryQueue<T>::updateWeights(float deltaTime)
{
	if (!isEmpty())
	{
		int cursor = front;
	
		do {
			cursor = (cursor + 1) % queueSize;
			weightEntry[cursor] += deltaTime;
		} while (cursor != rear);		
	}
}




//template <typename T>
//T EntryQueue<T>::activateFront()
//{
//	if (!isEmpty()) {
//
//		front = (front + 1) % maxSize;
//		auto spawned = queue[front];
//		spawned->initBlocks();
//		activeSet.insert(spawned);
//	
//		queue[front] = NULL;
//		return spawned;
//	}
//	else
//		return NULL;
//}