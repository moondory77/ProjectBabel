#ifndef __ABS_OBSTACLE_CONTAINER_H__
#define __ABS_OBSTACLE_CONTAINER_H__

#include "cocos2d.h"
#include "OnGame/Character.h"
#include "OnGame/ParticleManager.h"
#include "Obstacle/ObsTexBatcher.h"


USING_NS_CC;
using namespace std;


class ObsContainer : public CCNode
{
protected:

	//ObsTexType texType;
	bool featuredFlag = false;
	bool isAliveFlag = false;				//(���� ��)�ν��Ͻ���  ���� �÷���
	Point initPos = Point::ZERO;					//���� ��ġ
	float scaleFactor = -1.0f;				//�ν��Ͻ��� ũ��(��ũ�� width ����)
	
public:

	ObsContainer(BatchElement& batch_elem, Character& player, ParticlePool& ruins_pool) : texBatcher(batch_elem), mainChar(player), ruinsPool(ruins_pool) {};
	~ObsContainer() {};

	//���� �� �⺻������ �����ؾ� �� ����
	BatchElement& texBatcher;
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
//Point direction;//���⺤��(x^2 + y^2 = 1)
//Obstacle(ObstacleType initType);
//Obstacle(ObstacleType initType, Point initPos, float initG, float initV, Point initD, Layer* parent);
//void setPosition(Point newPos) { sprite->setPosition(newPos); }
//public:
	//������ original size�� 1168 * 2020 
	//������ ����, ��ȣ�ۿ��� ������ Ÿ�� ĳ���͸� ����


//Point getPosition() { return sprite->getPosition(); }
//bool isCollision(Charactor c);
//	virtual Point movePattern(Point nowPos, Point direction);//��ֹ� ���� ���� ����

};






#endif