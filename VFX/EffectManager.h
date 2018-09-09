#ifndef __EFFECT_MANAGER_H__
#define __EFFECT_MANAGER_H__

#include "cocos2d.h"
#include "VFX/EffectInfo.h"
#include "System/GameManager.h"
#include "OnGame/Character.h"
#include "Util/TextureBatcher.h"

using namespace std;
USING_NS_CC;


class EffectManager : public CCNode
{
	static EffectManager* EffectManager_instance;
	
	EffectManager() { texPool = new TextureBatcher(); };
	~EffectManager() { 
		while (!EffectTable.empty()){
			removeEffect(EffectTable.begin()->first);
		}
		texPool->release(); 
	};

	TextureBatcher* texPool;
	map< EffectType, EffectInfo*> EffectTable = {};

public:

	static EffectManager* getInstance();

	void initEffect(EffectType spr_effect, const char* tex_name, int frame_num);
	void initEffect(EffectType label_effect, const char* tex_name);
	void removeEffect(EffectType effect_name);
	
	bool isRunningEffect(EffectType effect_name){
		if (EffectTable.find(effect_name) != EffectTable.end())
			return EffectTable[effect_name]->isRunning();
		else
			return false;
	};


	//���� ��� API (��� Manager�� ���ؼ� �������̽�)
	void runBladeWind(Point position, Vec2 direction, float size, bool clockwise, Layer& target_canvas);
	void runBladeSlash(Point position, Vec2 direction, float size, Layer& target_canvas);

	void runAttackCharge(float max_size, Sprite& target_canvas);
	void stopAttackCharge();

	void displayAtkScore(int score, Point position, float size, Layer& target_canvas);


	//void runAttackEffect(Character* mainChar, Layer* canvas, Vec2 direction, float size, bool clockwise);
	//void runDamageEffect(Layer* canvas, int damage, int input_id);
	//void runSpecialEffect(Character* mainChar, Layer* canvas, float start_delay);
	//void callBackSpcEnd(Ref* sender, Character* mainChar);
	//void runDefenseEffect(Character* mainChar, Layer* canvas);
};

#endif









