#ifndef __SCENE_PLAYER_INFORMATION__H__
#define __SCENE_PLAYER_INFORMATION__H__

#include "cocos2d.h"
#include "Scene/_Lobby_Scene.h"
#include "Event/EventThread.h"


USING_NS_CC;

class PlayerInfo : public Scene
{
	enum IconName{
		HERO, WP_TAB, BACK,
		SWORD, BAT
	};

	Sprite* sprIcon[5];
	int isTouchedIcon[5];	//touchID로 이벤트 터치 구별

	EventThread *threadSwingWeapon;
	Layer* layerUI;

	bool is_popup_on = false;
	bool is_weapon_on = false;
	void initBG();

public:

	static Scene* createScene();
	virtual bool init();
	virtual void onEnterTransitionDidFinish();
	CREATE_FUNC(PlayerInfo);


	EventListenerTouchOneByOne* listener_Status;
	bool onTouchBegan_Status(Touch* touch, Event *unused_event);
	//void onTouchMoved_Status(Touch* touch, Event *unused_event);
	void onTouchEnded_Status(Touch* touch, Event *unused_event);
	
	EventListenerTouchOneByOne* listener_WpChange;
	bool onTouchBegan_WpChange(Touch* touch, Event* unused_event);
	//void onTouchMoved_WpChange(Touch* touch, Event *unused_event);
	void onTouchEnded_WpChange(Touch* touch, Event *unused_event);





};

#endif