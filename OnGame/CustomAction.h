#ifndef __CUSTOM_ACTION_H__
#define __CUSTOM_ACTION_H__

#include "cocos2d.h"
USING_NS_CC;

class CustomSequence : public Sequence
{
private:
	float skewFactor = 1.0f;	//기존의 (interval)액션 진행 시간에 대해, 왜곡시키는 비율

public:
	static CustomSequence* create(FiniteTimeAction *action1, ...) CC_REQUIRES_NULL_TERMINATION;
	static CustomSequence* createWithVariableList(FiniteTimeAction *action1, va_list args);
	static CustomSequence* createWithTwoActions(FiniteTimeAction *actionOne, FiniteTimeAction *actionTwo);

	void setSkewFactor(float skew_factor) { this->skewFactor = skew_factor; };
	float getSkewFactor() { return this->skewFactor; }
	virtual void step(float dt);
};


#endif
