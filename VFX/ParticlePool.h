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
	const int sortNum;		//pool에 들어가는 파티클(plist) 종류	
	const string texName;	//(texture 주소 참조를 위한) texture 파일명  
	Texture2D& texture;
	Vector<SpriteFrame*> frames = {};	//파티클 texture에 대한 사용 frame 개별저장

	struct {
		int min;
		int max;
		int cur;
		//const int minSize;		//pool에 존재하는 최소 파티클
	}PoolSize;


	ParticleBatchNode* batchNode = NULL;
	ParticleCustom** protoType;

	vector<ParticleCustom*> unitPool = {};			//전체 파티클 저장 목록
	vector<int> availableStack = {};				//가용한 상태의 파티클의 인덱스목록												
	int cursor;	//available pool의 top 인덱스 (availablePool의 (0 ~ top-1)위치의 파티클은 현재 사용가능)


public:


	ParticlePool(ParticleType p_type, Texture2D& p_tex, string tex_name, int sort_num);
	~ParticlePool();

	const string getTexName() { return texName; }
	ParticleBatchNode* getBatchNode() { return batchNode; }

	//구성 파티클 원형을 pool에 장착
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
