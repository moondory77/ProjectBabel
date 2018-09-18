#ifndef __SCENE_RESULT_H__
#define __SCENE_RESULT_H__

#include "cocos2d.h"
#include "Event/EventThread.h"

USING_NS_CC;
using namespace std;

class Result : public Scene
{
	//사용 UI 목록
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
	void initUI();	//layer 위에 UI 스프라이트 맵핑


public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(Result);

	
	EventListenerTouchOneByOne *listenerResult;
	virtual bool onTouchBegan_Result(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_Result(Touch* touch, Event *unused_event);
};






#endif