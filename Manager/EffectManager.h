#ifndef __EFFECT_MANAGER_H__
#define __EFFECT_MANAGER_H__

#include "cocos2d.h"
#include "Manager/EffectInfo.h"
#include "Manager/GameManager.h"
#include "OnGame/Character.h"

using namespace std;
USING_NS_CC;


class EffectManager : public CCNode
{
	static EffectManager* EffectManager_instance;
	EffectManager() {};
	~EffectManager() {};

	map< EffectName, EffectInfo*> EffectTable = {};

public:

	static EffectManager* getInstance();

	void initEffect(EffectName spr_effect, const char* plist_tex_name, int frame_num);
	void initEffect(EffectName label_effect, const char* tex_name);
	void removeEffect(EffectName effect_name);	
	bool isRunningEffect(EffectName effect_name) 
	{
		if (EffectTable.find(effect_name) != EffectTable.end())
			return EffectTable[effect_name]->isRunning();
		else
			return false;

	};


	//실제 사용 API (모두 Manager를 통해서 인터페이스)
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









