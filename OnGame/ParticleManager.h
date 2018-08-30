#ifndef __PARTICLE_MANAGER__H_
#define __PARTICLE_MANAGER__H_
#include "cocos2d.h"
#include "OnGame/ParticlePool.h"

USING_NS_CC;
using namespace std;


//파티클 Pool이 복수 공존할 때, 최적화 필요
class ParticleManager : public CCNode
{
private:
	static bool isExist;
	set<string> AddressTable = {};	//Cache에 로드 여부를 확인하기 위한, (파일명)주소 테이블


public:
	map<ParticleType, ParticlePool*> Pools = {};
	void initPool(ParticleType p_type, const char* plist_tex_name, int elem_num, int default_size);
	void removePool(ParticleType p_type);


	ParticleManager(){
		assert(!isExist);
		isExist = true;
	}

	~ParticleManager(){
		for (auto iter = Pools.begin(); iter != Pools.end(); iter++) {
			removePool((*iter).first);
		}
		///cache가 모두 비워졌는지 확인
		assert(AddressTable.empty());
		isExist = false;
	}

};


#endif // !__ParticleManager__H_
