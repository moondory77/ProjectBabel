#ifndef __T_ORDEAL_CONTAINER_H__
#define __T_ORDEAL_CONTAINER_H__

#include "cocos2d.h"
#include "Obstacle/Container/_ABS_ObsContainer.h"

USING_NS_CC;
using namespace std;


class OrdContainer : public ObsContainer
{
private:

protected:

public:

	OrdContainer(Point init_pos, float scale_factor, ObsBatcher& obs_batcher, Character& player, ParticlePool& ruin_pool);
	virtual ~OrdContainer();


	virtual void update(float deltaTime) {};
	virtual int getAliveCnt() { return 0; };

	//void move(float delta);
	//static DiagFragment* create();
	//DiagFragment(Point initPos,Size initSize, Layer* parent);
	//~DiagFragment();
	//virtual Point movePattern(Point nowPos, Point direction);
};

#endif