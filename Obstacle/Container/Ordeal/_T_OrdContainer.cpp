#include "_T_OrdContainer.h"
#include "random"


OrdContainer::OrdContainer(Point init_pos, float scale_factor, ObsBatcher& obs_batcher, Character& player, ParticlePool& ruin_pool)
	: ObsContainer(obs_batcher, player, ruin_pool)
{
	//initPosX = init_pos.x;
	//initPosX = init_pos.y;
	initPos = init_pos;
	scaleFactor = scale_factor;
	//curDeltaVelocity = 0.0f;

	//Obstacle(ObstacleType::DIAG_FRAGMENT, initPos, 1.f, 5, Point(1 / sqrt(2), -1 / sqrt(2)), parent)	
	random_device rd;
	mt19937_64 rng(rd());
	uniform_int_distribution<long long> dist(-0, 0);
	//startPosX = dist(rng);
	//startPosY = dist(rng);	
}

OrdContainer::~OrdContainer()
{
};

/*
DiagFragment* DiagFragment::create()
{
	DiagFragment *ch = new DiagFragment(Point((float)0, (float)0), (Size)0);
	char route[100] = { 0 };
	sprintf(route, "test.png");

	ch->setTexture(route);

	return nullptr;
}
*/


/*
Point DiagFragment::movePattern(Point nowPos, Point direction) {
	//현재 위치(nowPos)와 winSize 비교하여 direction의 변경사항 있으면 변경할 것
		if (nowPos.x > winSize.width || nowPos.x <= 0)

		direction.x = direction.x*-1;

	return direction;
}*/