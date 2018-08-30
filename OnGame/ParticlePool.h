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


//flag�� ����, Ȱ��/��Ȱ���� Ʈ��ŷ�ϱ� ���� wrapper class
class ParticleCustomUnit : public ParticleSystemQuad
{
	typedef ParticleSystem super;	//override �ϴ� �������� �ߺ��� ���� super����
	friend class ParticlePool;
	ParticlePool& container;
	bool isRunning = false;	//��ƼŬ ����Ʈ ��, �ٽ� ���� pool�� �Űܿ��� ���� �÷���

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

	vector<int> availableStack = {};				//������ ������ ��ƼŬ�� unitPool ���� �ε��� ����
	vector<ParticleCustomUnit*> unitPool = {};
	int cursor;	//available pool ���� top ��ġ (availablePool�� (0 ~ top-1)��ġ�� ��ƼŬ�� ���� ��밡��)

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
