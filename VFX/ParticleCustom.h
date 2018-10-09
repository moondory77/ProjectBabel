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
�߰� ��� 
1. Ȱ������ ����
2. animation frame ��ȯ
*/

class ParticleCustom : public ParticleSystemQuad
{
	friend class ParticlePool;

protected:

	const string plistName;		//��ü ���縦 ���� �ʿ��� Particle �ּ�
	Vector<SpriteFrame*>* subFrame;

	int frameCnt;				//��ƼŬ ���� frame ����
	int unitID;					//���� index (pool ���ο���)
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

	//���� frame�迭 ������
	void setSubFrame(Vector<SpriteFrame*>& p_frame) {
		this->subFrame = &p_frame;
		this->frameCnt = subFrame->size();
	};	

	ParticleCustom* spawnChild(int unit_id);

	//stop ��, �ٽ� ���뽺������ �ű�� �ڵ� ����
	virtual void updateParticleQuads() override;
	virtual void resetSystem();
};


#endif // !1__PARTICLE_CUSTOM__H_
