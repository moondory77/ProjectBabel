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
	SPARK = 201,
};

class ParticlePool;


/*
추가 기능 
1. 활성상태 추적
2. animation frame 변환
*/

class ParticleCustom : public ParticleSystemQuad
{
	friend class ParticlePool;

protected:

	const string plistName;		//개체 복사를 위해 필요한 Particle 주소
	Vector<SpriteFrame*>* subFrame;

	int frameCnt;				//파티클 구성 frame 갯수
	int unitID;					//고유 index (pool 내부에서)
	ParticlePool* host;
	bool isRunning = false;

public:

	static ParticleCustom* create(const string& plist_name);
	
	ParticleCustom(const string& plist_name) : plistName(plist_name) {};
	virtual ~ParticleCustom() {};

	void setID(int id) { this->unitID = id; }
	int getID() { return this->unitID; }

	void setHost(ParticlePool* my_host) { this->host = my_host; }
	ParticlePool* getHost() { return this->host; }

	//구성 frame배열 포인터
	void setSubFrame(Vector<SpriteFrame*>& p_frame) {
		this->subFrame = &p_frame;
		this->frameCnt = subFrame->size();
	};	

	ParticleCustom* spawnChild(int unit_id);

	//stop 후, 다시 가용스택으로 옮기는 코드 삽입
	virtual void updateParticleQuads() override;
	virtual void resetSystem();
};


#endif // !1__PARTICLE_CUSTOM__H_
