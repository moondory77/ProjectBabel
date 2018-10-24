#ifndef __ABS_OBSTACLE_UNIT_H__
#define __ABS_OBSTACLE_UNIT_H__

#include "cocos2d.h"
USING_NS_CC;
using namespace std;


/*
#issue
unit ��ü�� sprite ��� Ŭ������ ���� ���,
position update ��, unit�� �����Ͱ� �ƴ� ��ü ��ü�� ���ÿ� �ö󰡸鼭 �����÷ο� �߻�
������ �Ǵ� ��Ʈ�� ������ ���� sprite�� ������ �� ��.
*/
class ObsUnit : public CCNode
{
private:

protected:
	bool isAliveFlag;		//���� �۵� ��ȿ flag	

	ObsUnit() {};
	virtual ~ObsUnit() {};

	Sprite* sprUnit;	//������ ������ ��
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