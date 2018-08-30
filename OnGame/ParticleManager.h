#ifndef __PARTICLE_MANAGER__H_
#define __PARTICLE_MANAGER__H_
#include "cocos2d.h"
#include "OnGame/ParticlePool.h"

USING_NS_CC;
using namespace std;


//��ƼŬ Pool�� ���� ������ ��, ����ȭ �ʿ�
class ParticleManager : public CCNode
{
private:
	static bool isExist;
	set<string> AddressTable = {};	//Cache�� �ε� ���θ� Ȯ���ϱ� ����, (���ϸ�)�ּ� ���̺�


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
		///cache�� ��� ��������� Ȯ��
		assert(AddressTable.empty());
		isExist = false;
	}

};


#endif // !__ParticleManager__H_
