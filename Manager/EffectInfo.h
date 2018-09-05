#ifndef __EFFECT_INFORMATION_H__
#define __EFFECT_INFORMATION_H__

#include "cocos2d.h"
using namespace std;
USING_NS_CC;

enum EffectName {

	BLADE_WIND_1ST = 0,
	BLADE_WIND_2ND = 1,
	BLADE_SLASH = 2,
	BLADE_SPECIAL = 3,

	SHIELD_CRASH = 4,
	ENERGY_CHARGE_1ST = 5,

	DASH_1ST = 6,
	DASH_2ND = 7,
	DASH_3RD = 8,

	JUMP_1ST = 9,
	JUMP_2ND = 10,

	SCORE_NORMAL = 11,
	ATTACK_CHARGE = 13
};


//(캐쉬를 통해) Effect 리소스 관리 위한 데이터 클래스
class EffectInfo
{
public:
	const string address;				//텍스쳐(시트)의 (확장자 제외) full address
	Texture2D& texture;					//참조하는 텍스쳐에 대한 포인터(texture cache 관리를 위해)

	EffectInfo(string& plist_tex_name, Texture2D& tex) : address(plist_tex_name), texture(tex) {};
	virtual ~EffectInfo() {};
	virtual bool isRunning() = 0;
};


class SprEffectInfo : public EffectInfo
{
public:

	SprEffectInfo(string& plist_tex_name, Texture2D& tex) : EffectInfo(plist_tex_name, tex) {};
	~SprEffectInfo() { frames.clear(); };

	Vector<SpriteFrame*> frames = {};	//Texture Cache로부터 받아온 각 Frame을 Parse하여 저장
	Sprite* sprEffect = NULL;

	bool isRunning() { return sprEffect != NULL; };
};


class LabelEffectInfo : public EffectInfo
{
public:
	LabelEffectInfo(string& plist_tex_name, Texture2D& tex) :EffectInfo(plist_tex_name, tex) {};
	~LabelEffectInfo() {};

	Label* labelEffect = NULL;
	bool isRunning() { return labelEffect != NULL; };
};

#endif
