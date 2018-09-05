#ifndef __SCENE_ON_READY_H__
#define __SCENE_ON_READY_H__

#include "cocos2d.h"
#include "Plot/EventList.h"
#include "Scene/_Scene_Lobby.h"

USING_NS_CC;

class OnReady : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	
	CREATE_FUNC(OnReady);

	bool is_popup_on = false;

	//Box *eve_cur;
	EventList *cur_list;

	EventListenerTouchOneByOne *listener_on_ready;
	virtual bool onTouchBegan_OnReady(Touch* touch, Event *unused_event);
	//virtual void onTouchMoved_OnReady(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_OnReady(Touch* touch, Event *unused_event);

	Layer *layer_bg;
	Sprite *spr_bg_hill;
	Sprite *spr_bg_back;
	Sprite *spr_bg_on_ready;
	Layer *layer_grid;

	void initBG();

	//** delta 만큼 시간 딜레이 후에, 인트로 이벤트시작하는 콜백함수
	void initScheduleCallback(float delta);

};


#endif // !__OnReady_SCENE_H__
