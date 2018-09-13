#ifndef __SCENE_LOBBY_H__
#define __SCENE_LOBBY_H__

#include "cocos2d.h"
#include "Scene/_Lobby_Layer.h"

USING_NS_CC;
using namespace std;

class Lobby : public cocos2d::Scene
{

	//사용 UI 목록
	enum LayerName {
		MAIN, STATUS, RECORD
	};

	Layer* layerLobby[3];
	Sprite* iconHero;
	void initBG();	

public:

	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(Lobby);

	EventListenerTouchOneByOne *listener_lobby;
	virtual bool onTouchBegan_Lobby(Touch* touch, Event *unused_event);
	//virtual void onTouchMoved_Lobby(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_Lobby(Touch* touch, Event *unused_event);


	//Layer *layer_bg_hill;
	//Sprite *spr_bg_hill;
	////Setting 메뉴를 위한 터치 리스너
	//EventListenerTouchOneByOne *listener_setting;
	//virtual bool onTouchBegan_Setting(Touch* touch, Event *unused_event);
	////virtual void onTouchMoved_Lobby(Touch* touch, Event *unused_event);
	//virtual void onTouchEnded_Setting(Touch* touch, Event *unused_event);

};

#endif

