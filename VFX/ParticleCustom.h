#ifndef __PARTICLE_CUSTOM__H_
#define __PARTICLE_CUSTOM__H_
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

enum ParticleType
{
	RUIN_COMPANY = 100,
	RUIN_APARTMENT = 101,
	RUIN_GOTHIC = 102,
	RUIN_MYSTERY = 103,
	SPARK_SWING = 201,
};

class ParticlePool;

//각 파티클의 활성/비활성 여부를 트랙킹하기 위한 wrapper class
class ParticleCustom : public ParticleSystemQuad
{
	friend class ParticlePool;
	const string plistName;

protected:

	int unitID;					//pool 안에서의 고유 index
	ParticlePool* host;
	bool isRunning = false;

public:

	static ParticleCustom* create(const string& plist_name);
	ParticleCustom(const string& plist_name) : plistName(plist_name) {};
	virtual ~ParticleCustom() {};

	//ParticleCustom(ParticlePool& host_pool, int unit_idx) : container(host_pool), unitIdx(unit_idx) {};
	void setID(int id) { this->unitID = id; }
	int getID() { return this->unitID; }
	void setHost(ParticlePool* my_host) { this->host = my_host; }
	ParticlePool* getHost() { return this->host; }

	virtual ParticleCustom* spawnChild(int id);

	//stop 후, 다시 가용스택으로 옮기는 코드 삽입
	virtual void updateParticleQuads() override;
	virtual void resetSystem();
};



//
////프레임 애니메이션 효과를 동반하는 파티클 시스템
//class ParticleAnimation : public ParticleCustom
//{
//protected:
//
//	Vector<SpriteFrame*> animFrames = {};
//
//public:
//
//	static ParticleAnimation* create(ParticlePool& host, int unit_idx, const string& plist_name);
//
//	ParticleAnimation(ParticlePool& host, int unit_idx) : ParticleCustom(host, unit_idx) {};
//
//
//	void initFrames(ParticleType p_type, int frame_cnt);
//
//	virtual ~ParticleAnimation() {};
//	virtual void update(float deltaTime);
//
//};


#endif // !1__PARTICLE_CUSTOM__H_
