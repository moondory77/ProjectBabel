#ifndef __PARTICLE_MANAGER__H_
#define __PARTICLE_MANAGER__H_
#include "cocos2d.h"
#include "OnGame/ParticlePool.h"

USING_NS_CC;
using namespace std;


//���� Pool�� ������ ��, ����ȭ �ʿ�
//���� type�� pool�� ���� �� ���� �����ϰ� �� ��, �Ǵ� �ʿ�. ����� �Ұ���
class ParticleManager : public CCNode
{
private:
	static bool isExist;

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
		isExist = false;
	}
};


#endif // !__ParticleManager__H_
