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


//각 파티클의 활성/비활성 여부를 트랙킹하기 위한 wrapper class
class ParticleCustomUnit : public ParticleSystemQuad
{
	typedef ParticleSystem super;	//override 하는 과정에서 중복을 위한 super지정
	friend class ParticlePool;
	ParticlePool& container;
	bool isRunning = false;	//파티클 이펙트 후, 다시 가용 pool로 옮겨오기 위한 플래그

protected:
	const int unitIdx;	//pool 안에서의 고유 index

public:
	static ParticleCustomUnit* create(ParticlePool& host, int unit_idx, const string& plist_name);
	ParticleCustomUnit(ParticlePool& host_pool, int unit_idx) : container(host_pool), unitIdx(unit_idx) {};

	virtual ~ParticleCustomUnit() {};
	virtual void update(float deltaTime);
};



class ParticlePool : public CCNode
{
private:
	const ParticleType type;
	const string texName;

	const int elementNum;
	const int minSize;
	int maxSize;

	Vector<SpriteFrame*> frames = {};
	ParticleBatchNode* batchNode = NULL;

	vector<int> availableStack = {};				//가용한 상태의 파티클의 unitPool 상의 인덱스 저장
	vector<ParticleCustomUnit*> unitPool = {};

	//available pool의 top 인덱스 (availablePool의 (0 ~ top-1)위치의 파티클은 현재 사용가능)
	int cursor;

public:

	ParticlePool(Texture2D& tex_in_cache, ParticleType type, string tex_name, int elem_sort, int default_size);
	~ParticlePool();

	const string getTexName() { return texName; }
	ParticleBatchNode* getBatchNode() { return batchNode; }

	int getMinSize() { return minSize; }
	int getMaxSize() { return maxSize; }
	inline int getCurSize() { return unitPool.size(); }
	inline void setMaxSize(int update_size){
		if (update_size > minSize) { maxSize = update_size; }
	}

	void pushToAvailableStack(int unit_idx);
	void pushParticle();
	void popParticle();

	void playParticleEffect(Point& world_pos);
};


#endif // !1__PARTICLE_POOL__H_
