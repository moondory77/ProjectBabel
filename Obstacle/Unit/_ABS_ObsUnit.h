#ifndef __ABS_OBSTACLE_UNIT_H__
#define __ABS_OBSTACLE_UNIT_H__

#include "cocos2d.h"
USING_NS_CC;
using namespace std;


/*
#issue
unit 자체를 sprite 상속 클래스로 만들 경우,
position update 시, unit의 포인터가 아닌 객체 자체가 스택에 올라가면서 오버플로우 발생
렌더링 되는 파트를 나누어 따로 sprite로 만들어야 할 듯.
*/
class ObsUnit : public CCNode
{
private:

protected:
	bool isAliveFlag;		//현재 작동 유효 flag	

	ObsUnit() {};
	virtual ~ObsUnit() {};

	Sprite* sprUnit;	//유닛의 렌더링 부
	Vector<SpriteFrame*> sprUnitFrames = {};

public:

	bool isAlive() { return this->isAliveFlag; };
	float getTopY() { return sprUnit->getBoundingBox().getMaxY(); }
	float getBottomY() { return sprUnit->getBoundingBox().getMinY(); }
	float getLeftX() { return sprUnit->getBoundingBox().getMinX(); }
	float getRightX() { return sprUnit->getBoundingBox().getMaxX(); }
	float getMidX() { return sprUnit->getBoundingBox().getMidX(); }
	float getMidY() { return sprUnit->getBoundingBox().getMidY(); }
	float getWidth() { return sprUnit->getBoundingBox().size.width; }
	float getHeight() { return sprUnit->getBoundingBox().size.height; }

	virtual void updateState(float deltaTime) = 0;
	virtual void updatePosition(float deltaTime) = 0;
	
	Point getPosition() { return Point(sprUnit->getBoundingBox().getMidX(), sprUnit->getBoundingBox().getMidY()); }
	float getPositionX() { return sprUnit->getBoundingBox().getMidX(); }
	float getPositionY() { return sprUnit->getBoundingBox().getMidY(); }

	virtual void setPosition(Point pos) = 0;
	virtual void setPositionX(float newX) = 0;
	virtual void setPositionY(float newY) = 0;
	virtual void breakSelf() = 0;
};



#endif