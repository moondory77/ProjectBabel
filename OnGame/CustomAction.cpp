#include "CustomAction.h"


//Sequence Action의 타입만 바꾸는 포워딩
CustomSequence* CustomSequence::create(FiniteTimeAction *action1, ...)
{
	va_list params;
	va_start(params, action1);
	CustomSequence *ret = CustomSequence::createWithVariableList(action1, params);
	va_end(params);
	return ret;
}

CustomSequence* CustomSequence::createWithVariableList(FiniteTimeAction *action1, va_list args)
{
	FiniteTimeAction *now;
	FiniteTimeAction *prev = action1;
	bool bOneAction = true;

	while (action1)
	{
		now = va_arg(args, FiniteTimeAction*);
		if (now) {
			prev = createWithTwoActions(prev, now);
			bOneAction = false;
		}
		else {
			if (bOneAction) {
				auto extra_action = new (std::nothrow) FiniteTimeAction();
				assert(extra_action != NULL);
				extra_action->autorelease();
				prev = createWithTwoActions(prev, extra_action);
			}
			break;
		}
	}
	return ((CustomSequence*)prev);
}


CustomSequence* CustomSequence::createWithTwoActions(FiniteTimeAction *actionOne, FiniteTimeAction *actionTwo)
{
	CustomSequence *sequence = new (std::nothrow) CustomSequence();
	if (sequence && sequence->initWithTwoActions(actionOne, actionTwo))
	{
		sequence->autorelease();
		return sequence;
	}
	delete sequence;
	return nullptr;
}



void CustomSequence::step(float dt)
{
	//왜곡할만큼 deltaTime을 변형하여 update 진행
	ActionInterval::step(dt * skewFactor);
}
