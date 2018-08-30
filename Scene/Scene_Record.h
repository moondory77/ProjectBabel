#ifndef __SCENE_RECORD_H__
#define __SCENE_RECORD_H__

#include "cocos2d.h"
#include "Scene/Scene_Lobby.h"
#include "Plot/EventPlayer.h"

USING_NS_CC;

class Record : public cocos2d::Scene
{

public:
	static cocos2d::Scene* createScene();
	virtual bool init();

	CREATE_FUNC(Record);

	bool is_popup_on = false;

	EventList* cur_list;

	EventListenerTouchOneByOne *listener_record;
	virtual bool onTouchBegan_Record(Touch* touch, Event *unused_event);
	//virtual void onTouchMoved_Record(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_Record(Touch* touch, Event *unused_event);

	Layer *layer_bg_hill;
	Sprite *spr_bg_hill;
	Layer *layer_grid;
	void initBG();

	Layer *layer_icon;
	Sprite *spr_icon_start;
	Sprite *spr_icon_character;
	Sprite *spr_icon_record;
	Sprite *spr_icon_setting;
	void initIcon();

	//** 주인공 애니메이션 관련
	bool is_moving_on = false;
	Sprite *spr_icon_hero;

};


#endif