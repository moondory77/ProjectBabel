#ifndef __ABS_OBSTACLE_CONTAINER_H__
#define __ABS_OBSTACLE_CONTAINER_H__

#include "cocos2d.h"
#include "VFX/ParticleManager.h"
#include "Obstacle/ObsBatchManager.h"
#include "OnGame/Character.h"


USING_NS_CC;
using namespace std;


class ObsContainer : public CCNode
{
protected:

	//ObsTexType texType;
	bool featuredFlag = false;
	bool isAliveFlag = false;				//(생성 후)인스턴스의  여부 플래그
	Point initPos = Point::ZERO;					//생성 위치
	float scaleFactor = -1.0f;				//인스턴스의 크기(스크린 width 기준)
	
public:

	ObsContainer(const ObsBatchUnit& batch_unit, Character& player, ParticlePool& ruins_pool) 
		: texBatchUnit(batch_unit), mainChar(player), ruinsPool(ruins_pool) {};
	
	~ObsContainer() {};

	//생성 시 기본적으로 참조해야 할 정보
	const ObsBatchUnit& texBatchUnit;
	Character& mainChar;
	ParticlePool& ruinsPool;

	bool isAlive() { return isAliveFlag; }
	float getScaleFactor() { return scaleFactor; }
	
	void setScaleFactor(float scale_factor) { this->scaleFactor = scale_factor; }
	virtual void update(float deltaTime) = 0;


	//private:
//Sprite* sprite;
//float gravity;
//float velocity;
//float MaxVelocity;
//Point direction;//방향벡터(x^2 + y^2 = 1)
//Obstacle(ObstacleType initType);
//Obstacle(ObstacleType initType, Point initPos, float initG, float initV, Point initD, Layer* parent);
//void setPosition(Point newPos) { sprite->setPosition(newPos); }
//public:
	//빌딩의 original size는 1168 * 2020 
	//빌딩에 대해, 상호작용이 가능한 타겟 캐릭터를 설정


//Point getPosition() { return sprite->getPosition(); }
//bool isCollision(Charactor c);
//	virtual Point movePattern(Point nowPos, Point direction);//장애물 별로 따로 설정

};






#endif