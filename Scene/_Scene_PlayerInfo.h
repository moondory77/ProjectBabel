#ifndef __SCENE_PLAYER_INFORMATION__H__
#define __SCENE_PLAYER_INFORMATION__H__

#include "cocos2d.h"
#include "Plot/EventPlayer.h"
#include "Plot/EventBox.h"
#include "Plot/EventList.h"
#include "Scene/_Scene_Lobby.h"

USING_NS_CC;

class PlayerInfo : public cocos2d::Scene
{

public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(PlayerInfo);

	Box *eve_cur;
	EventList *cur_list;

	bool is_popup_on = false;

	EventListenerTouchOneByOne *listener_player_info;
	virtual bool onTouchBegan_PlayerInfo(Touch* touch, Event *unused_event);
	//virtual void onTouchMoved_CharacterInfo(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_PlayerInfo(Touch* touch, Event *unused_event);
	
	Layer *layer_bg_hill;
	Sprite *spr_bg_hill;
	Sprite *spr_bg_logo;
	Layer *layer_grid;
	void initBG();

	bool is_weapon_on = false;

	/*
	//** 주인공 애니메이션 관련
	bool is_moving_on = false;
	Sprite *spr_icon_hero;
	*/
};

#endif