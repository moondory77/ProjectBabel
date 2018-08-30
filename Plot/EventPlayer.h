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
(Lobby ȭ�鿡��) Start��ư�� ������ �� ĳ���� �׼�.
�κ� ȭ���� ���ΰ� icon�� �̿��ϱ⿡, ���� layer ����
*/
class MenuLobby : public cocos2d::CCNode
{
	static MenuLobby* MenuStart_instance;

	EventList *list_menu_lobby;
	Layer *layer_menu_lobby;

	Sprite *spr_icon_hero;	//target���� ���
	Sprite *spr_icon_title;	//extra_target���� ���

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

	//** �׻� ��� �Ŀ� �� �ϱ��..
	void quitMenuLobby() {
		list_menu_lobby->~EventList();
	};
};


//*************************** #Scene - CharacterInfo	**************************************// 

// (�κ�ȭ�鿡��) Character ��ư�� ��ġ �ÿ� �߻��ϴ� ĳ���;׼� �̺�Ʈ����Ʈ
// ������ Character ������ Ʈ������ �� ����, �߻�
class MenuCharacter : public cocos2d::CCNode
{
	static MenuCharacter* MenuCha_instance;

	EventList *list_menu_cha;
	Layer *layer_menu_cha;
	
	Sprite *spr_icon_hero;		///�׼� ����� ���ϴ� ĳ���� ��������Ʈ
	Sprite *spr_icon_weapon;	///�����̵��׼����� �����ϴ� ���繫�� ��������Ʈ
	Sprite *spr_icon_back;		///�ڷΰ��� ��ư
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

	//** �׻� ��� �Ŀ� �� �ϱ��..
	void quitMenuCharater() {
		list_menu_cha->~EventList();
	};
};


//*************************** #Scene - OnReady	***************************************// 

// OnReady ȭ�鿡�� �̷������ �̺�Ʈ��
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

	Label *labelResult[MAXINDEX];			//** �ش� message�� ����� ��

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