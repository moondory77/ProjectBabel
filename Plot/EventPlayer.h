#ifndef __EventPlayer_H__
#define __EventPlayer_H__

#define MAXINDEX 8
#include "cocos2d.h"
#include "Plot/EventBox.h"
#include "Plot/EventList.h"
#include <string>

using std::string;
USING_NS_CC;


//*************************** #Scene - Lobby	***************************************// 
/*
(Lobby 화면에서) Start버튼을 눌렀을 때 캐릭터 액션.
로비 화면의 주인공 icon을 이용하기에, 따로 layer 없음
*/
class MenuLobby : public cocos2d::CCNode
{
	static MenuLobby* MenuStart_instance;

	EventList *list_menu_lobby;
	Layer *layer_menu_lobby;

	Sprite *spr_icon_hero;	//target으로 삼아
	Sprite *spr_icon_title;	//extra_target으로 삼아

	Sprite *spr_icon_start;
	Sprite *spr_icon_record;
	Sprite *spr_icon_character;
	Sprite *spr_icon_setting;


public:
	static MenuLobby*  getInstance();

	MenuLobby() {};

	EventList* getList() { return this->list_menu_lobby; };
	Layer* getLayer() { return this->layer_menu_lobby; };

	Sprite* getHeroTarget() { return this->spr_icon_hero; }
	Sprite* getTitleTarget() { return this->spr_icon_title; }
	Sprite* getStartTarget() { return this->spr_icon_start; }
	Sprite* getRecordTarget() { return this->spr_icon_record; }
	Sprite* getCharacterTarget() { return this->spr_icon_character; }
	Sprite* getSettingTarget() { return this->spr_icon_setting; }

	void initMenuLobby();

	//** 항상 사용 후에 콜 하기로..
	void quitMenuLobby() {
		list_menu_lobby->~EventList();
	};
};


//*************************** #Scene - CharacterInfo	**************************************// 

// (로비화면에서) Character 버튼을 터치 시에 발생하는 캐릭터액션 이벤트리스트
// 실제로 Character 씬으로 트랜지션 한 다음, 발생
class MenuCharacter : public cocos2d::CCNode
{
	static MenuCharacter* MenuCha_instance;

	EventList *list_menu_cha;
	Layer *layer_menu_cha;
	
	Sprite *spr_icon_hero;		///액션 모션을 취하는 캐릭터 스프라이트
	Sprite *spr_icon_weapon;	///슬라이딩액션으로 등장하는 현재무기 스프라이트
	Sprite *spr_icon_back;		///뒤로가기 버튼
	Sprite *spr_icon_sword;
	Sprite *spr_icon_bat;

public:
	static MenuCharacter* getInstance();

	MenuCharacter() {};

	Sprite* tmp;

	EventList* getList() { return this->list_menu_cha; };
	Layer* getLayer() { return this->layer_menu_cha; };
	
	Sprite* getHeroTarget() { return this->spr_icon_hero; };
	Sprite* getWeaponTarget() { return this->spr_icon_weapon; };
	Sprite* getBackTarget() { return this->spr_icon_back; };
	Sprite *getSwordTarget() { return this->spr_icon_sword; };
	Sprite *getBatTarget() { return this->spr_icon_bat; };

	void initMenuCharacter();

	//** 항상 사용 후에 콜 하기로..
	void quitMenuCharater() {
		list_menu_cha->~EventList();
	};
};


//*************************** #Scene - OnReady	***************************************// 

// OnReady 화면에서 이루어지는 이벤트들
class MenuOnReady : public cocos2d::CCNode
{
	static MenuOnReady* MenuOnReady_instance;

	EventList *list_menu_onReady;
	Layer *layer_menu_onReady;
	
	Sprite *spr_icon_hero;
	Sprite *spr_icon_logo;
	Sprite *spr_icon_start;
	Sprite *spr_icon_trial;
	Sprite *spr_icon_buff;

public:
	static MenuOnReady* getInstance();

	Label *labelResult[MAXINDEX];			//** 해당 message를 출력할 라벨

	MenuOnReady() {};
	EventList* getList() { return list_menu_onReady; }
	Layer* getLayer() { return layer_menu_onReady; }
	
	Sprite* getHeroTarget() { return spr_icon_hero; }
	Sprite* getStartTarget() { return spr_icon_start; }
	Sprite* getTrialTarget() { return spr_icon_trial; }
	Sprite* getBuffTarget() { return spr_icon_buff; }

	void initMenuOnReady();
	void quitMenuOnReady() {
		list_menu_onReady->~EventList();
	};

};




//*************************** #Scene - Result	***************************************// 

class MenuResult : public cocos2d::CCNode
{
	static MenuResult* MenuResult_instance;

	EventList *list_menu_result;
	Layer *layer_menu_result;

	Sprite *spr_icon_logo;
	Sprite *spr_icon_grade;
	Sprite *spr_icon_confirm;
	Sprite *spr_icon_hero;
public:
	static MenuResult* getInstance();

	MenuResult() {};
	EventList* getList() { return list_menu_result; }
	Layer* getLayer() { return layer_menu_result; }
	Sprite* getLogoTarget() { return spr_icon_logo; }
	Sprite* getHeroTarget() { return spr_icon_hero; }
	Sprite* getConfirmTarget() { return spr_icon_confirm; }
	Sprite* getGradeTarget() { return spr_icon_grade; }

	void initMenuResult();
	void quitMenuResult()
	{
		list_menu_result->~EventList();
	}
};




#endif 