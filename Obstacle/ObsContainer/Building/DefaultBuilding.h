#ifndef __DEFAULT_BUILDING_H__
#define __DEFAULT_BUILDING_H__

#include "cocos2d.h"
#include "Obstacle/ObsContainer/Building/_T_BuildContainer.h"

USING_NS_CC;
using namespace std;

class DefaultBuilding : public BuildContainer
{
private:
	int prevSpecialID;
	float prevSpecialDamage;

public:

	DefaultBuilding(BatchElement& batch_elem, Character& main_char, ParticlePool& ruins_pool);
	DefaultBuilding(const BuildContainer& bld);

	virtual ~DefaultBuilding() {
		CCLOG("Success to remove Default Building");
	};

	void updateVelocity(float deltaTime);
	virtual void update(float deltaTime) override;
	virtual BuildContainer& spawnChild(BuildContainer& mate) override;


};



#endif
