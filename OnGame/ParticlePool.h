#ifndef __PARTICLE_POOL__H_
#define __PARTICLE_POOL__H_
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

enum ParticleType
{
	RUIN_COMPANY = 1,
	RUIN_APARTMENT = 2
};


//flag를 통해, 활성/비활성을 트랙킹하기 위한 wrapper class
class ParticleCustomUnit : public ParticleSystemQuad
{
	typedef ParticleSystem super;	//override 하는 과정에서 중복을 위한 super지정
	friend class ParticlePool;
	ParticlePool& container;
	bool isRunning = false;	//파티클 이펙트 후, 다시 가용 pool로 옮겨오기 위한 플래그

protected:
	const int unitIdx;

public:
	static ParticleCustomUnit* create(ParticlePool& host, int unit_idx, const string& filename);
	ParticleCustomUnit(ParticlePool& host_pool, int unit_idx) : container(host_pool), unitIdx(unit_idx) {};
	//ParticleCustomUnit(const ParticleCustomUnit& clone) : container(clone.container){};

	virtual ~ParticleCustomUnit() {};
	virtual void update(float deltaTime);
};


class ParticlePool : public CCNode
{
private:
	const ParticleType type;
	const string address;
	const int elementNum;
	const int defaultSize;

	Vector<SpriteFrame*> frames = {};
	ParticleBatchNode* batchNode = NULL;

	vector<int> availableStack = {};				//가용한 상태의 파티클의 unitPool 상의 인덱스 저장
	vector<ParticleCustomUnit*> unitPool = {};
	int cursor;	//available pool 상의 top 위치 (availablePool의 (0 ~ top-1)위치의 파티클은 현재 사용가능)

public:
	void pushToAvailableStack(int unit_idx);
	void pushParticle();
	void popParticle();


	ParticlePool(ParticleType type, string plist_tex_address, int elem_sort, int default_size);
	~ParticlePool();

	void playParticleEffect(Point& world_pos);
	const string& getAddress() { return address; }
	ParticleBatchNode* getBatchNode() { return batchNode; }
};



#endif // !1__PARTICLE_POOL__H_
