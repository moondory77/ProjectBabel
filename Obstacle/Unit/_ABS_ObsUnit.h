#ifndef __ABS_OBSTACLE_UNIT_H__
#define __ABS_OBSTACLE_UNIT_H__

#include "cocos2d.h"
USING_NS_CC;
using namespace std;

class ObsUnit : public CCNode
{
private:

protected:
	bool isAliveFlag;		//현재 작동 유효 flag	

	ObsUnit() {};
	virtual ~ObsUnit() {};

	Sprite* sprUnit;						
	Vector<SpriteFrame*> sprUnitFrames = {};

public:

	bool isAlive() { return this->isAliveFlag; };

	float getTopY() { return sprUnit->getBoundingBox().getMaxY(); }
	float getBottomY() { return sprUnit->getBoundingBox().getMinY(); }
	float getLeftX() { return sprUnit->getBoundingBox().getMinX(); }
	float getRightX() { return sprUnit->getBoundingBox().getMaxX(); }
	float getCenterX() { return sprUnit->getBoundingBox().getMidX(); }
	float getCenterY() { return sprUnit->getBoundingBox().getMidY(); }
	float getWidth() { return (sprUnit->getBoundingBox().getMaxX() - sprUnit->getBoundingBox().getMinX()); }
	float getHeight() { return (sprUnit->getBoundingBox().getMaxY() - sprUnit->getBoundingBox().getMinY()); }

	virtual void stateUpdate(float deltaTime) = 0;
	virtual void positionUpdate(float deltaTime) = 0;	
	

	Point getPosition() { return Point(sprUnit->getBoundingBox().getMidX(), sprUnit->getBoundingBox().getMidY()); }
	float getPositionX() { return sprUnit->getBoundingBox().getMidX(); }
	float getPositionY() { return sprUnit->getBoundingBox().getMidY(); }

	virtual void setPosition(Point pos) = 0;
	virtual void setPositionX(float newX) = 0;
	virtual void setPositionY(float newY) = 0;
	virtual void breakSelf() = 0;
};



#endif