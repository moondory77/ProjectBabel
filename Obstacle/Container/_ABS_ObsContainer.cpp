#include "_ABS_ObsContainer.h"
#include "random"



//
//Obstacle::Obstacle(obType initType) {
//	type = initType;
//	//sprite �ʱ�ȭ
//}
//


//Obstacle::Obstacle(obType initType, Point initPos, float initG, float initV, Point initD, Layer* canvas) {
//	type = initType;
//	gravity = initG;
//	velocity = initV;
//	direction = Point(initD.x,initD.y);
//	MaxVelocity = 150;
//	if (type == obType::BUILDING)
//	{
//
//	}
//	else if(type == obType::DIAGFRAGMENT)
//	{
//		//sprite �ʱ�ȭ
//		char buf[100];
//		random_device rd;
//		mt19937_64 rng(rd());
//		uniform_int_distribution<long long> dist(1, 2);
//		uniform_int_distribution<long long> dist2(800, 2000);
//		uniform_real_distribution<double> dist3(-0.2, 0.2);
//		uniform_real_distribution<double> dist4(Director::getInstance()->getWinSize().width / 3, 2 * Director::getInstance()->getWinSize().width / 3);
//		direction.x = 1 * dist3(rng);
//		sprintf(buf, "DiagFragment/meteroite_%d.png", dist(rng));
//		velocity = dist2(rng);
//		DiagF = Sprite::create(buf);
//		DiagF->setName("DiagF");
//		DiagF->setScale(1);
//
//		float temp = direction.x;
//		if (temp < 0)
//			temp = temp*-1;
//
//		double angle = atan2(1, temp)*57.2957951;
//		log("angle : %f", angle);
//
//		if (direction.x>0)
//		{
//			angle = 270 + angle;
//		}
//		else if (direction.x<0)
//		{
//			angle = 90 - angle;
//		}
//		else
//		{
//			angle = 0;
//		}
//
//		DiagF->setRotation(angle);
//		DiagF->setAnchorPoint(Point(0.5, 0));
//		DiagF->setPosition(dist4(rng), Director::getInstance()->getWinSize().height * 3);
//		DiagF->setVisible(true);
//		canvas->addChild(DiagF, 5, "DiagF");
//		log("create");
//	}
//	else
//	{
//
//	}
//}
//
//

//Action* ObsContainer::runAction(Action* a) 
//{
//	Sprite::runAction();
//};



//
//void Obstacle::move(float delta) {
//	if (type == obType::BUILDING) {
//		//�ǹ��� �������� ���� ó��
//	}
//	else {
//		sprite = DiagF;
//		if (sprite == NULL)
//			return;
//
//
//		//test�غ� �� �ƴϴ� ��������� Ȯ�� ���
//		Point nowPos = sprite->getPosition();
//										//������ gravity ���x
//		double newX = nowPos.x + delta * velocity * direction.x,
//			newY = nowPos.y + delta * velocity*-1;
//		DiagF->setPosition(Point(newX, newY));
//		//log("direction");
//		//log("%f %f %f",direction.x,delta,velocity);
////		direction = movePattern(sprite->getPosition(),direction);
//	}
//}

/*
bool Obstacle::isCollision(Charactor c) {
	//ĳ���ͼ���+�� �߰��ؾ� ����� © �� ������
	//�ӽ÷� onGame�� �ִ°� �����ͼ� test ����
}*/












