#include <stddef.h>
#include "Manager/GameManager.h"
#include "base/CCEventDispatcher.h"

USING_NS_CC;

Size winSize(){
	return Director::getInstance()->getWinSize();
} 

//현재스크린의 종횡비
float winAspectRatio()
{
	return winSize().height / winSize().width;
}

float DivForHorizontal(Sprite* target)
{
	return winSize().width / (target->getContentSize().width);
}

float DivForVertical(Sprite* target)
{
	return winSize().height / (target->getContentSize().height);
}


//***************************	#EventManager	**************************************// 
EventManager* EventManager::EventManager_instance = NULL;
EventManager* EventManager::getInstance()
{
	if (!EventManager_instance)
	{
		EventManager_instance = new EventManager();
	}
	return EventManager_instance;
}


//** delay 시간 후에, 씬 전환하는 액션 반환. 해당 시간 동안, 모든 리스너도 잠재운다.
Action* EventManager::sceneTransferAction(float delay, sceneNum scene_num)
{
	auto timelapse = DelayTime::create(delay);
	auto trans = CallFuncN::create(CC_CALLBACK_1(EventManager::callback_sceneTransfer, this, scene_num));


	CCLOG("After %.2f seconds, scene transfer", delay);
	return Sequence::create(timelapse, trans, NULL);
}
//** 콜백함수. 액션으로 만들기 위해 생성
void EventManager::callback_sceneTransfer(Ref *sender, sceneNum scene_num)
{
	Scene *dst;

	switch (scene_num)
	{
	case LOBBY:
		dst = TransitionFade::create(2, Lobby::createScene());
		break;
	case ON_READY:
		dst = TransitionFade::create(2, OnReady::createScene());
		break;
	case ON_GAME:
		dst = TransitionFade::create(2, OnGame::createScene());
		break;
	case RESULT:
		dst = TransitionFade::create(2, Result::createScene());
		break;
	case PLAYER_INFO:
		dst = TransitionFade::create(2, PlayerInfo::createScene());
		break;
	case RECORD:
		dst = TransitionFade::create(2, Record::createScene());
		break;
	}

	Director::getInstance()->replaceScene(dst);
}


//** delay 시간동안 해당 리스너 중지시키는 액션 반환. eventOnToggle 콜백 이용
Action* EventManager::getListenerSleep(float delay, EventListener *listener)
{
	listener->setEnabled(false);
	auto timelapse = DelayTime::create(delay);
	auto toggle_delay = CallFuncN::create(CC_CALLBACK_1(EventManager::callback_listenerAwake, this, listener));

	CCLOG("%.2f seconds, listener sleep", delay);
	//auto total = Sequence::create(timelapse, toggle_delay, NULL);

	return Sequence::create(timelapse, toggle_delay, NULL);
}
//** 콜백함수, 매개변수로 받는 object 리스너를 enable 시킴
void EventManager::callback_listenerAwake(Ref *sender, void *object)
{
	auto listener = (EventListener*)object;

	listener->setEnabled(true);
}



//***************************	#GridLayer	**************************************// 

GridLayer* GridLayer::GridLayer_instance = NULL;

GridLayer* GridLayer::getInstance()
{
	if (!GridLayer_instance)
	{
		GridLayer_instance = new GridLayer();
	}
	return GridLayer_instance;
}

//** 그리드 레이어를 생성하여 반환
Layer* GridLayer::getLayer()
{

	layer_grid = LayerColor::create(Color4B::WHITE);
	layer_grid->setAnchorPoint(Point::ZERO);
	layer_grid->setPosition(Point::ZERO);
	layer_grid->setOpacity(0);

	auto spr_grid = Sprite::create("grid.png");
	auto DivisionForScreen_vertical = winSize().width / (spr_grid->getContentSize().width);
	auto DivisionForScreen_horizontal = winSize().height / (spr_grid->getContentSize().height);
	spr_grid->setScale(DivisionForScreen_vertical*2.0f, DivisionForScreen_horizontal);
	spr_grid->setAnchorPoint(Point::ZERO);
	spr_grid->setPosition(Point::ZERO);
	layer_grid->addChild(spr_grid);


	return layer_grid;
}

