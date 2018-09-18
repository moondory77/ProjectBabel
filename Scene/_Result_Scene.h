#ifndef __SCENE_RESULT_H__
#define __SCENE_RESULT_H__

#include "cocos2d.h"
#include "Event/EventThread.h"

USING_NS_CC;
using namespace std;

class Result : public Scene
{
	//��� UI ���
	enum IconName {
		HERO, RESULT_LOGO,
		NEXT, GRADE
	};

	enum EventName {
		InOutMotion, Grading
	};

	Layer* subLayer;
	EventThread* eventList[2];
	Sprite* iconTarget[6];
	int iconTouchID[6];

	void initBG();
	void initUI();	//layer ���� UI ��������Ʈ ����


public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(Result);

	
	EventListenerTouchOneByOne *listenerResult;
	virtual bool onTouchBegan_Result(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_Result(Touch* touch, Event *unused_event);
};






#endif