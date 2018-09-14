#ifndef __LAYER_LOBBY_H__
#define __LAYER_LOBBY_H__

#include "cocos2d.h"
#include "Event/EventThread.h"

USING_NS_CC;
using namespace std;


class MainLobby : public LayerColor
{
	//�ش� subLayer���� �����ϴ� icon target
	enum IconName {
		HERO, TITLE_LOGO,
		START, RECORD, STATUS, SETTING
	};

	Sprite* iconTarget[6];
	int iconTouchID[6];
	void initUI();	

public:

	MainLobby(Sprite* hero) {
		iconTarget[HERO] = hero; 
		assert(init());
	};
	~MainLobby() {};

	virtual bool init();
	virtual void onEnterTransitionDidFinish();

	EventListenerTouchOneByOne* listenerMain;
	bool onTouchBegan_Main(Touch* touch, Event *unused_event);
	void onTouchEnded_Main(Touch* touch, Event *unused_event);

};


class StatusLobby : public LayerColor
{
	//��� UI ���
	enum IconName {
		HERO, WEAPON_TAB, BACK,
		SWORD, BAT, AXE
	};

	enum EventName {
		InOutMotion,
		WeaponChangeMotion
	};

	EventThread* eventList[2];	
	Sprite* iconTarget[6];
	int iconTouchID[6];
	void initUI();	//layer ���� UI ��������Ʈ ����


public:
	virtual bool init();
	virtual void onEnterTransitionDidFinish();

	StatusLobby(Sprite* hero) {
		iconTarget[HERO] = hero; 
		assert(init());
	};	
	~StatusLobby() {
		for (int i = 0; i < 1; i++) {
			eventList[i]->release();
		}
	};

	EventListenerTouchOneByOne* listenerStatus;
	bool onTouchBegan_Status(Touch* touch, Event *unused_event);
	void onTouchEnded_Status(Touch* touch, Event *unused_event);

	EventListenerTouchOneByOne* listenerWpChange;
	bool onTouchBegan_WpChange(Touch* touch, Event* unused_event);
	void onTouchEnded_WpChange(Touch* touch, Event *unused_event);
};



//class RecordLobby : public LayerColor
//{
//
//};


#endif
