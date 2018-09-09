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


//�� ��ƼŬ�� Ȱ��/��Ȱ�� ���θ� Ʈ��ŷ�ϱ� ���� wrapper class
class ParticleCustomUnit : public ParticleSystemQuad
{
	typedef ParticleSystem super;	//override �ϴ� �������� �ߺ��� ���� super����
	friend class ParticlePool;
	ParticlePool& container;
	bool isRunning = false;	//��ƼŬ ����Ʈ ��, �ٽ� ���� pool�� �Űܿ��� ���� �÷���

protected:
	const int unitIdx;	//pool �ȿ����� ���� index

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

	vector<int> availableStack = {};				//������ ������ ��ƼŬ�� unitPool ���� �ε��� ����
	vector<ParticleCustomUnit*> unitPool = {};

	//available pool�� top �ε��� (availablePool�� (0 ~ top-1)��ġ�� ��ƼŬ�� ���� ��밡��)
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
