#ifndef __EFFECT_INFORMATION_H__
#define __EFFECT_INFORMATION_H__

#include "cocos2d.h"
using namespace std;
USING_NS_CC;

enum EffectType {

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
class EffectInfo : public CCNode
{
public:
	const EffectType type;
	const string texName;				//텍스쳐(시트)의 (확장자 제외) full address

	EffectInfo(Texture2D& tex, EffectType e_type, string& plist_tex_name) : type(e_type), texName(plist_tex_name) {};
	virtual ~EffectInfo() {};
	virtual bool isRunning() = 0;
};


class SprEffectInfo : public EffectInfo
{
public:

	SprEffectInfo(Texture2D& tex, EffectType e_type, string& tex_name, int frame_num);
	~SprEffectInfo();

	Vector<SpriteFrame*> frames = {};	//Texture Cache로부터 받아온 각 Frame을 Parse하여 저장
	Sprite* sprEffect = NULL;
	bool isRunning() { return sprEffect != NULL; };
};


class LabelEffectInfo : public EffectInfo
{
public:
	Texture2D& texture;				

	LabelEffectInfo(Texture2D& tex, EffectType e_type, string& plist_tex_name) : EffectInfo(tex, e_type, plist_tex_name), texture(tex) {};
	~LabelEffectInfo() {};

	Label* labelEffect = NULL;
	bool isRunning() { return labelEffect != NULL; };
};

#endif
