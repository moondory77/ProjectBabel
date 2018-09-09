#ifndef __PARTICLE_MANAGER__H_
#define __PARTICLE_MANAGER__H_

#include "cocos2d.h"
#include "VFX/ParticlePool.h"
#include "Util/TextureBatcher.h"

USING_NS_CC;
using namespace std;

//�Ź� onGame �� ���Ը���, ���� �ʱ�ȭ
class ParticleManager : public CCNode
{
private:
	static bool isExist;
	TextureBatcher* texPool;
	
public:
	map<ParticleType, ParticlePool*> PoolTable = {};

	void initPool(ParticleType p_type, const char* tex_name, int elem_num, int default_size);
	void removePool(ParticleType p_type);

	ParticleManager(){
		assert(!isExist);
		texPool = new TextureBatcher();
		isExist = true;
	}

	~ParticleManager(){
		while (!PoolTable.empty()){
			removePool(PoolTable.begin()->first);
		}
		texPool->release();
		//CCLOG(TextureCache::getInstance()->getCachedTextureInfo().c_str());
		isExist = false;
	}
};


#endif // !__ParticleManager__H_
