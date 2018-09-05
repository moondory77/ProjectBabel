#ifndef __SCENE_LOBBY_H__
#define __SCENE_LOBBY_H__

#include "cocos2d.h"
#include "Plot/EventBox.h"
#include "Plot/EventList.h"
#include "Scene/_Scene_Result.h"
#include "Scene/_Scene_Record.h"
#include "Scene/_Scene_OnReady.h"
#include "Scene/_Scene_PlayerInfo.h"

USING_NS_CC;


class Lobby : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(Lobby);

	bool is_popup_on = false;
	//bool is_from_result = true;

	EventList* cur_list;

	EventListenerTouchOneByOne *listener_lobby;
	virtual bool onTouchBegan_Lobby(Touch* touch, Event *unused_event);
	//virtual void onTouchMoved_Lobby(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_Lobby(Touch* touch, Event *unused_event);


	EventListenerTouchOneByOne *listener_prac;
	//virtual bool onTouchBegan_Prac(Touch* touch, Event *unused_event);
	//virtual void onTouchMoved_Prac(Touch* touch, Event *unused_event);
	//virtual void onTouchEnded_Prac(Touch* touch, Event *unused_event);

	void initBG();
	Layer *layer_bg_hill;
	Sprite *spr_bg_hill;

	//Setting 메뉴를 위한 터치 리스너
	EventListenerTouchOneByOne *listener_setting;
	virtual bool onTouchBegan_Setting(Touch* touch, Event *unused_event);
	//virtual void onTouchMoved_Lobby(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_Setting(Touch* touch, Event *unused_event);

};

#endif

