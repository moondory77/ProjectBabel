#include "System/GameManager.h"
#include "System/BGManager.h"
#include "Scene/_Scene_Result.h"
#include "Scene/_Scene_Lobby.h"
#include "Plot/EventPlayer.h"
#include "Plot/EventBox.h"
//#include "LabelTypingEffect.h"
#include "base/CCEventDispatcher.h"
#include <string.h>

USING_NS_CC;

Scene* Result::createScene()
{
	return Result::create();
}


bool Result::init()
{
	if (!Scene::init())
	{
		return false;
	}

	_eventDispatcher->removeAllEventListeners();

	//** Result 씬에 대한 리스너 생성 및 등록
	listener_result = EventListenerTouchOneByOne::create();
	listener_result->setSwallowTouches(true);
	listener_result->onTouchBegan = CC_CALLBACK_2(Result::onTouchBegan_Result, this);
	listener_result->onTouchEnded = CC_CALLBACK_2(Result::onTouchEnded_Result, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener_result, 1);
	///임시로 우선순위 1

	// 진도암시 배경. hill 배경 셋팅
	BGManager::getInstance()->loadBG();
	this->addChild(BGManager::getInstance()->getParallax());
	initBG();

	// EventPlayer 초기화시키고, 씬에 레이어로 삽입
	MenuResult::getInstance()->initMenuResult();
	cur_list = MenuResult::getInstance()->getList();
	this->addChild(MenuResult::getInstance()->getLayer(), 5);

	//3.2초 동안 리스너를 잠궈둠 
	this->runAction(EventManager::getInstance()->getListenerSleep(3.2f, listener_result));
	//0.8초 후 인트로 이벤트 시작(싱크 맞추기 위해)
	this->scheduleOnce(schedule_selector(Result::initScheduleCallback), 0.8f);

	return true;
}

void Result::initScheduleCallback(float delta)
{
	cur_list->moveCurNext();	//Menu_Result_Intro_0
	cur_list->scanCur()->setEvent();
	cur_list->scanCur()->startEvent();
}

void Result::initBG()
{
	//** #1 언덕 배경레이어
	layer_bg_hill = LayerColor::create(Color4B::WHITE);
	layer_bg_hill->setAnchorPoint(Point::ZERO);
	layer_bg_hill->setPosition(Point::ZERO);
	layer_bg_hill->setOpacity(0);
	this->addChild(layer_bg_hill);

	//** #2 메인 배경위의 약간 불투명한 레이어
	spr_bg_back = Sprite::create("layout/result/bg_black.png");
	spr_bg_back->setScale(DivForHorizontal(spr_bg_back), DivForVertical(spr_bg_back));
	//spr_bg_back->setAnchorPoint(Point::ZERO);
	spr_bg_back->setPosition(Point(winSize().width*0.5f, winSize().height*0.5f));
	spr_bg_back->setOpacity(100);
	layer_bg_hill->addChild(spr_bg_back);

	//** #3 언덕 스프라이트
	spr_bg_hill = Sprite::create("layout/result/hill.png");
	spr_bg_hill->setScale(DivForHorizontal(spr_bg_hill)*1.2f, DivForVertical(spr_bg_hill)*0.45f);
	spr_bg_hill->setAnchorPoint(Point::ZERO);
	//spr_bg_hill->setRotation(2);
	spr_bg_hill->setPosition(Point(-winSize().width*0.2f, 0));
	layer_bg_hill->addChild(spr_bg_hill);
}


bool Result::onTouchBegan_Result(Touch* touch, Event *unused_event)
{
	return true;
}
void Result::onTouchEnded_Result(Touch* touch, Event *unused_event)
{
	Point touchPoint = touch->getLocation();

	//결과 등급 뜨기 전이면,
	if (!is_confirm_on)
	{
		cur_list->moveCurTo(2);	//Menu_Result_Grading_0
		cur_list->scanCur()->setEvent();
		cur_list->scanCur()->startEvent();
		
		//이벤트 동안, 리스너 3초 잠금
		this->runAction(EventManager::getInstance()->getListenerSleep(3.0f, listener_result));
		is_confirm_on = true;
	}
	//결과 등급 뜬 후면,
	else
	{
		if (MenuResult::getInstance()->getConfirmTarget()->getBoundingBox().containsPoint(touchPoint))
		{
			cur_list->moveCurTo(3);	//Menu_Result_Outro_0
			cur_list->scanCur()->setEvent();
			cur_list->scanCur()->startEvent();

			_eventDispatcher->removeAllEventListeners();
			EventManager::getInstance()->setFromResult(true);
			this->runAction(EventManager::getInstance()->sceneTransferAction(4.8f, LOBBY));
		}
	}

}
