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

///��ƼŬ�� Ȱ�����¸� �����ϱ� ���� Wrapping Class
class ParticleCustom : public ParticleSystemQuad
{
	friend class ParticlePool;

protected:

	const string plistName;		//��ü ���縦 ���� �ʿ��� Particle �ּ�
	int unitID;					//���� index (pool ���ο���)

	ParticlePool* host;
	bool isRunning = false;

public:

	static ParticleCustom* create(const string& plist_name);
	
	ParticleCustom(const string& plist_name) : plistName(plist_name) {};
	virtual ~ParticleCustom() {};

	void setID(int id) { this->unitID = id; }
	int getID() { return this->unitID; }

	
	virtual void setHost(ParticlePool* my_host) { this->host = my_host; }
	ParticlePool* getHost() { return this->host; }
	virtual ParticleCustom* spawnChild(int id);

	//stop ��, �ٽ� ���뽺������ �ű�� �ڵ� ����
	virtual void updateParticleQuads() override;
	virtual void resetSystem();
};




//������ �ִϸ��̼� ȿ���� �����ϴ� ��ƼŬ �ý���
class ParticleAnimation : public ParticleCustom
{
protected:
	Vector<SpriteFrame*> animFrames = {};
	void initFrames();
public:

	static ParticleAnimation* create(const string& plist_name, int frame_cnt);
	ParticleAnimation(const string& plist_name, int frame_cnt) : ParticleCustom(plist_name) {
		animFrames.reserve(frame_cnt);
	};
	virtual ~ParticleAnimation() {};
	

	virtual void setHost(ParticlePool* my_host) {
		this->host = my_host;
		initFrames();
	};
	virtual ParticleCustom* spawnChild(int id);
	virtual void updateParticleQuads();
};


#endif // !1__PARTICLE_CUSTOM__H_
