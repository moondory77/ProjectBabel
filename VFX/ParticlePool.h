#ifndef __PARTICLE_POOL__H_
#define __PARTICLE_POOL__H_
#include "cocos2d.h"
#include "VFX/ParticleCustom.h"

USING_NS_CC;
using namespace std;


class ParticlePool : public CCNode
{
private:
	const string texName;	//(texture �ּ� ������ ����) texture ���ϸ�  
	Texture2D& texture;

	const int subNum;		//pool�� ���� ��ƼŬ(plist) ���� ��
	vector<Vector<SpriteFrame*>> subFrame = {};	//�ش� Pool�� �̿�Ǵ� ������ ��ƼŬ ������ ����
	ParticleCustom** protoType;
	ParticleBatchNode* batchNode = NULL;

	struct {
		int min;
		int max;
		int cur;
	}PoolSize;

	vector<ParticleCustom*> unitPool = {};			//��ü ��ƼŬ ���� ���
	vector<int> availableStack = {};				//������ ������ ��ƼŬ�� �ε������												
	int cursor;	//available pool�� top �ε��� (availablePool�� (0 ~ top-1)��ġ�� ��ƼŬ�� ���� ��밡��)

public:

	ParticlePool(Texture2D& p_tex, string tex_name, int elem_num);
	~ParticlePool();

	const string getTexName() { return texName; }
	Texture2D& getTexture() { return texture; }
	ParticleBatchNode* getBatchNode() { return batchNode; }


	//���� ��ƼŬ ������ pool�� ����
	void setProtoType(ParticleCustom* target, int elem_idx, int frame_num);
	void runSpawning(int min_size, int max_size);


	int getMinSize() { return PoolSize.min; }
	int getMaxSize() { return PoolSize.max; }
	inline int getCurSize() { return unitPool.size(); }
	inline void setMaxSize(int update_size) {
		if (update_size > PoolSize.max) { this->PoolSize.max = update_size; }
	}

	void pushToAvailableStack(int unit_idx);
	void pushParticle(int sort_idx);
	void popParticle();

	void playParticleEffect(Point& world_pos);
};


#endif // !1__PARTICLE_POOL__H_
