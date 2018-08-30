#ifndef __SCENE_RESULT_H__
#define __SCENE_RESULT_H__

#include "cocos2d.h"
#include "Plot/EventPlayer.h"
#include "Plot/EventBox.h"
#include "Plot/EventList.h"
#include "Scene/Scene_Lobby.h"

USING_NS_CC;

class Result : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	CREATE_FUNC(Result);

	bool is_confirm_on = false;
	
	EventList* cur_list;

	EventListenerTouchOneByOne *listener_result;
	virtual bool onTouchBegan_Result(Touch* touch, Event *unused_event);
	//virtual void onTouchMoved_CharacterInfo(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_Result(Touch* touch, Event *unused_event);

	Layer *layer_bg_hill;
	Sprite *spr_bg_hill;
	Sprite *spr_bg_back;
	
	void initBG();

	void initScheduleCallback(float delta);
};






#endif