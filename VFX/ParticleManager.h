#ifndef __PARTICLE_MANAGER__H_
#define __PARTICLE_MANAGER__H_

#include "cocos2d.h"
#include "VFX/ParticlePool.h"
#include "Util/TextureBatcher.h"

USING_NS_CC;
using namespace std;

//매번 onGame 씬 진입마다, 새로 초기화
class ParticleManager : public CCNode
{
private:
	static bool isExist;
	TextureBatcher* texPool;

public:
	map<ParticleType, ParticlePool*> PoolTable = {};	//각 타입 별, pool은 단 하나만 존재

	void createPool(ParticleType p_type, const char* tex_name, int sort_num);
	void removePool(ParticleType p_type);



	ParticleManager() {
		assert(!isExist);
		texPool = new TextureBatcher();
		isExist = true;
	}

	~ParticleManager() {
		while (!PoolTable.empty()) {
			removePool(PoolTable.begin()->first);
		}
		texPool->release();
		//CCLOG(TextureCache::getInstance()->getCachedTextureInfo().c_str());
		isExist = false;
	}
};


#endif // !__ParticleManager__H_
