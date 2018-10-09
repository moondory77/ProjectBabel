#ifndef __PARTICLE_POOL__H_
#define __PARTICLE_POOL__H_
#include "cocos2d.h"
#include "VFX/ParticleCustom.h"

USING_NS_CC;
using namespace std;


class ParticlePool : public CCNode
{
private:
	const string texName;	//(texture 주소 참조를 위한) texture 파일명  
	Texture2D& texture;

	const int subNum;		//pool에 들어가는 파티클(plist) 종류 수
	vector<Vector<SpriteFrame*>> subFrame = {};	//해당 Pool에 이용되는 종류별 파티클 프레임 저장
	ParticleCustom** protoType;
	ParticleBatchNode* batchNode = NULL;

	struct {
		int min;
		int max;
		int cur;
	}PoolSize;

	vector<ParticleCustom*> unitPool = {};			//전체 파티클 저장 목록
	vector<int> availableStack = {};				//가용한 상태의 파티클의 인덱스목록												
	int cursor;	//available pool의 top 인덱스 (availablePool의 (0 ~ top-1)위치의 파티클은 현재 사용가능)

public:

	ParticlePool(Texture2D& p_tex, string tex_name, int elem_num);
	~ParticlePool();

	const string getTexName() { return texName; }
	Texture2D& getTexture() { return texture; }
	ParticleBatchNode* getBatchNode() { return batchNode; }


	//구성 파티클 원형을 pool에 장착
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
