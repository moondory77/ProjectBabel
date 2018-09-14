#ifndef __SCENE_LOBBY_H__
#define __SCENE_LOBBY_H__

#include "cocos2d.h"
#include "Scene/_Lobby_Layer.h"
#include "Event/EventThread.h"
USING_NS_CC;
using namespace std;


class Lobby : public cocos2d::Scene
{
	enum SubLobby {
		MAIN_LOBBY,
		STATUS_LOBBY,
		RECORD_LOBBY
	};

	enum EventName {
		InOutMotion
	};

	Layer* subLobby[3];
	EventThread* eventList[1];
	void initBG();
	Sprite* iconHero;

public:
	static cocos2d::Scene* createScene();
	virtual bool init();	
	virtual ~Lobby(){
		for (int i = 0; i < 3; i++) { if (subLobby[i]) subLobby[i]->release(); }
		for (int i = 0; i < 1; i++) { if (eventList[i]) eventList[i]->release(); }
	};
	CREATE_FUNC(Lobby);

	////Setting 메뉴를 위한 터치 리스너
	//EventListenerTouchOneByOne *listener_setting;
	//virtual bool onTouchBegan_Setting(Touch* touch, Event *unused_event);
	////virtual void onTouchMoved_Lobby(Touch* touch, Event *unused_event);
	//virtual void onTouchEnded_Setting(Touch* touch, Event *unused_event);
};

#endif

