#ifndef __SCENE_ON_READY_H__
#define __SCENE_ON_READY_H__

#include "cocos2d.h"
#include "Event/EventThread.h"

USING_NS_CC;
using namespace std;

class OnReady : public cocos2d::Scene
{
	//��� UI ����Ʈ
	enum IconName {
		HERO, START, TRIAL, BUFF
	};

	enum EventName {
		InOutMotion,
	};

	Layer* subLayer;

	EventThread* eventList[1];
	Sprite* iconTarget[6];
	int iconTouchID[6];

	void initBG();
	void initUI();	//layer ���� UI ��������Ʈ ����


public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	

	EventListenerTouchOneByOne *listenerOnReady;
	virtual bool onTouchBegan_OnReady(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_OnReady(Touch* touch, Event *unused_event);

	CREATE_FUNC(OnReady);
};


#endif // !__OnReady_SCENE_H__
