#ifndef __PARTICLE_POOL__H_
#define __PARTICLE_POOL__H_
#include "cocos2d.h"
#include "VFX/ParticleCustom.h"

USING_NS_CC;
using namespace std;


class ParticlePool : public CCNode
{
private:
	const ParticleType type;
	const int sortNum;		//pool�� ���� ��ƼŬ(plist) ����	
	const string texName;	//(texture �ּ� ������ ����) texture ���ϸ�  
	Texture2D& texture;
	Vector<SpriteFrame*> frames = {};	//��ƼŬ texture�� ���� ��� frame ��������

	struct {
		int min;
		int max;
		int cur;
		//const int minSize;		//pool�� �����ϴ� �ּ� ��ƼŬ
	}PoolSize;


	ParticleBatchNode* batchNode = NULL;
	ParticleCustom** protoType;

	vector<ParticleCustom*> unitPool = {};			//��ü ��ƼŬ ���� ���
	vector<int> availableStack = {};				//������ ������ ��ƼŬ�� �ε������												
	int cursor;	//available pool�� top �ε��� (availablePool�� (0 ~ top-1)��ġ�� ��ƼŬ�� ���� ��밡��)


public:


	ParticlePool(ParticleType p_type, Texture2D& p_tex, string tex_name, int sort_num);
	~ParticlePool();

	const string getTexName() { return texName; }
	ParticleBatchNode* getBatchNode() { return batchNode; }

	//���� ��ƼŬ ������ pool�� ����
	void setProtoType(initializer_list<ParticleCustom*> targets);
	void initPool(int min_size, int max_size);


	void pushParticle(int sort_idx);


	int getMinSize() { return PoolSize.min; }
	int getMaxSize() { return PoolSize.max; }
	inline int getCurSize() { return unitPool.size(); }
	inline void setMaxSize(int update_size) {
		if (update_size > PoolSize.max) { this->PoolSize.max = update_size; }
	}

	void pushToAvailableStack(int unit_idx);
	void popParticle();

	void playParticleEffect(Point& world_pos);
};


#endif // !1__PARTICLE_POOL__H_
