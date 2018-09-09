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


//(ĳ���� ����) Effect ���ҽ� ���� ���� ������ Ŭ����
class EffectInfo : public CCNode
{
public:
	const EffectType type;
	const string texName;				//�ؽ���(��Ʈ)�� (Ȯ���� ����) full address

	EffectInfo(Texture2D& tex, EffectType e_type, string& plist_tex_name) : type(e_type), texName(plist_tex_name) {};
	virtual ~EffectInfo() {};
	virtual bool isRunning() = 0;
};


class SprEffectInfo : public EffectInfo
{
public:

	SprEffectInfo(Texture2D& tex, EffectType e_type, string& tex_name, int frame_num);
	~SprEffectInfo();

	Vector<SpriteFrame*> frames = {};	//Texture Cache�κ��� �޾ƿ� �� Frame�� Parse�Ͽ� ����
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
