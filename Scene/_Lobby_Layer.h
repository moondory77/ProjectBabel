#ifndef __LAYER_LOBBY_H__
#define __LAYER_LOBBY_H__

#include "cocos2d.h"
#include "Event/EventThread.h"

USING_NS_CC;
using namespace std;


class MainLobby : public LayerColor
{
	//��� UI ���
	enum IconName{
		HERO, TITLE_LOGO,
		START, RECORD, PLAYER_INFO, SETTING
	};

	Sprite* iconTarget[6];
	int iconTouchID[6];
	void initUI();	//layer ���� UI ��������Ʈ ����

	EventThread* threadHeroInOut;

public:

	//create Func ->retain ������
	static MainLobby* create();
	virtual bool init();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();


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

	Sprite* iconTarget[6];
	int iconTouchID[6];
	void initUI();	//layer ���� UI ��������Ʈ ����

	EventThread* threadHeroInOut;
	EventThread* threadWeaponChange;

public:

	static StatusLobby* create();
	virtual bool init();
	virtual void onEnterTransitionDidFinish();


	EventListenerTouchOneByOne* listenerStatus;
	bool onTouchBegan_Status(Touch* touch, Event *unused_event);
	void onTouchEnded_Status(Touch* touch, Event *unused_event);

	EventListenerTouchOneByOne* listenerWpChange;
	bool onTouchBegan_WpChange(Touch* touch, Event* unused_event);
	void onTouchEnded_WpChange(Touch* touch, Event *unused_event);

};




class RecordLobby : public LayerColor
{

};


#endif
