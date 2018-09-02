#include "cocos2d.h"
#include "Manager/GameManager.h"
#include "OnGame/Character.h"

using namespace std;

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

//plist로부터 각 Frame을 Effect별로 나누어 저장
class EffectInfo {
	
public:
	const string address;	//확장자를 제외한, 이미지 주소 string
	Vector<SpriteFrame*> frames = {};	//Cache로부터 받아온 애니메이션 프레임
	
	Sprite* sprEffect = NULL;

	EffectInfo(string& plist_address) : address(plist_address){};
	~EffectInfo() {};

};


class EffectManager : public CCNode
{
	static EffectManager* EffectManager_instance;
	EffectManager() {};
	set<string> AddressTable = {};	//Cache에 로드 여부를 확인하기 위한, (파일명)주소 테이블

	///싱글톤 내, 모든 Effect에 대한 목록은 처음부터 모두 존재
	map< EffectName, EffectInfo*> Effects = {
		{ BLADE_WIND_1ST, NULL },
		{ BLADE_WIND_2ND , NULL },
		{ BLADE_SLASH, NULL },
		{ BLADE_SPECIAL, NULL },
		{ SHIELD_CRASH, NULL },
		{ ENERGY_CHARGE_1ST, NULL },

		{ DASH_1ST, NULL },
		{ DASH_2ND, NULL },
		{ DASH_3RD, NULL },
		{ JUMP_1ST, NULL },
		{ JUMP_2ND, NULL },
		{ SCORE_NORMAL, NULL },
		{ ATTACK_CHARGE , NULL}
	};

	//int curAttackID = -1;	//1회 공격 - 이펙트 대응을 보장하는 ID값
public:
	static EffectManager* getInstance();

	//싱글톤 객체에 처음 EffectInfo를 로드 (런타임 시, 한번만)
	void initEffectInfo(EffectName effect_name, const char* plist_name, int frame_num);
	//더이상 해당 런타임에서 필요없을 때, 캐쉬에서 해제
	void removeEffectInfo(EffectName effect_name);
	

	inline bool isRunningEffect(EffectName effect_name) {
		return (Effects[effect_name] != NULL) 
			&& (Effects[effect_name]->sprEffect != NULL);
	}



	void runBladeWind(Point position, Vec2 direction, float size, bool clockwise, Layer& target_canvas);
	void runBladeSlash(Point position, Vec2 direction, float size, Layer& target_canvas);
	
	void runAttackCharge(float max_size, Sprite& target_canvas);
	void stopAttackCharge();
	
	//void callBack_AttackCharge(float dt);
	
	
	//void runAttackEffect(Character* mainChar, Layer* canvas, Vec2 direction, float size, bool clockwise);
	//void runDamageEffect(Layer* canvas, int damage, int input_id);
	//void runSpecialEffect(Character* mainChar, Layer* canvas, float start_delay);

	//void callBackSpcEnd(Ref* sender, Character* mainChar);
	//void runDefenseEffect(Character* mainChar, Layer* canvas);
};

