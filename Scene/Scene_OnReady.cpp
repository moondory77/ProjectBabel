//#include "Util/LabelTypingEffect.h"
#include "Manager/GameManager.h"
#include "Manager/BGManager.h"

#include "Plot/EventPlayer.h"
#include "base/CCEventDispatcher.h"

#include "Scene/Scene_OnReady.h"
#include "Scene/Scene_Lobby.h"
#include <string.h>

USING_NS_CC;

Scene* OnReady::createScene()
{
	return OnReady::create();
}

bool OnReady::init()
{
	if (!Scene::init())
	{
		return false;
	}
	
	//이전 등록된 이벤트 리스너 모두 해제
	_eventDispatcher->removeAllEventListeners();

	// OnReady씬 리스너를 생성, 등록
	listener_on_ready = EventListenerTouchOneByOne::create();
	listener_on_ready->setSwallowTouches(true);
	listener_on_ready->onTouchBegan = CC_CALLBACK_2(OnReady::onTouchBegan_OnReady, this);
	listener_on_ready->onTouchEnded = CC_CALLBACK_2(OnReady::onTouchEnded_OnReady, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener_on_ready, 1);

	// 진도암시 배경. hill 배경 셋팅
	BGManager::getInstance()->loadBG();
	this->addChild(BGManager::getInstance()->getParallax());
	initBG();
	
	//** EventPlayer 초기화시키고, 씬에 레이어로 삽입
	MenuOnReady::getInstance()->initMenuOnReady();
	cur_list = MenuOnReady::getInstance()->getList();
	this->addChild(MenuOnReady::getInstance()->getLayer(), 5);

	BGManager::getInstance()->moveBG(400);

	//리스너 3.2초만큼 잠금
	this->runAction(EventManager::getInstance()->getListenerSleep(3.2f, listener_on_ready));
	// 1초 기다렸다가 시작(자막과 싱크 맞추기 위해)
	this->scheduleOnce(schedule_selector(OnReady::initScheduleCallback), 1.0f);
	return true;
}

void OnReady::initScheduleCallback(float delta)
{
	for (int i = 0; i < 1; i++)
	{
		cur_list->moveCurNext();
		cur_list->scanCur()->setEvent();
		cur_list->scanCur()->startEvent();
	}
}

void OnReady::initBG()
{
	//#1 배경 스프라이트를 담는 레이어
	layer_bg = LayerColor::create(Color4B::WHITE);
	layer_bg->setAnchorPoint(Point::ZERO);
	layer_bg->setPosition(Point::ZERO);
	layer_bg->setOpacity(0);	
	this->addChild(layer_bg);

	//** 화면 전체를 감싸는 불투명 스프라이트
	spr_bg_back = Sprite::create("layout/on_ready/bg_black.png");
	spr_bg_back->setScale(DivForHorizontal(spr_bg_back), DivForVertical(spr_bg_back));
	spr_bg_back->setAnchorPoint(Point::ZERO);
	spr_bg_back->setPosition(Point::ZERO);
	spr_bg_back->setOpacity(100);
	layer_bg->addChild(spr_bg_back);
	
	//** 언덕 스프라이트
	spr_bg_hill = Sprite::create("layout/on_ready/hill_2.png");
	spr_bg_hill->setScale(DivForHorizontal(spr_bg_hill), DivForVertical(spr_bg_hill));
	spr_bg_hill->setAnchorPoint(Point::ZERO);
	spr_bg_hill->setPosition(Point(0, -winSize().height*0.12));
	layer_bg->addChild(spr_bg_hill);

	/*
	//** 아이템 로고 스프라이트
	spr_bg_on_ready = Sprite::create("UI/OnReady/item_title.png");
	DivisionForScreen_vertical = winSize.width / (spr_bg_on_ready->getContentSize().width);
	DivisionForScreen_horizontal = winSize.height / (spr_bg_on_ready->getContentSize().height);
	spr_bg_on_ready->setScale(DivisionForScreen_vertical*0.8f, DivisionForScreen_horizontal*0.2f);
	spr_bg_on_ready->setPosition(Point(winSize.width*0.5f, winSize.height*0.85f));
	layer_bg->addChild(spr_bg_on_ready);
	*/
}


bool OnReady::onTouchBegan_OnReady(Touch* touch, Event *unused_event)
{
	if (!is_popup_on)
	{
		return true;
	}
	return false;
}
void OnReady::onTouchEnded_OnReady(Touch* touch, Event* unused_event)
{
	Point touchPoint = touch->getLocation();

	if (!is_popup_on)
	{
		//시작 버튼 클릭 시
		if (MenuOnReady::getInstance()->getStartTarget()->getBoundingBox().containsPoint(touchPoint))
		{
			cur_list->moveCurTo(2);	// Menu_OnReady_Outro_0
			cur_list->scanCur()->setEvent();
			cur_list->scanCur()->startEvent();
			
			BGManager::getInstance()->moveBG(800);

			_eventDispatcher->removeAllEventListeners();
			///이벤트 액션동안 리스너 잠굼
			this->runAction(EventManager::getInstance()->sceneTransferAction(2.9f, ON_GAME));
			/// 2.88초 후에 씬 전환
		}
		//시련 버튼 클릭 변경 시
		else if (MenuOnReady::getInstance()->getTrialTarget()->getBoundingBox().containsPoint(touchPoint))
		{
			if (EventManager::getInstance()->getTrialNum() == 1)
			{
				//시련 번호 변경
				//EventManager::getInstance()->setTrialNum(2);

				//cur_list->moveCurTo(2);	// Menu_OnReady_Gear_0

				//for (int i = 0; i < 1; i++)
				//{
				//	cur_list->scanCur()->setEvent();
				//	cur_list->scanCur()->startEvent();
				//	cur_list->moveCurNext();
				//}

				//** 톱니바퀴 시련_A
				/*eve_cur = MenuOnReady::getInstance()->getList()->scanBox(4);
				eve_cur->setEvent();
				eve_cur->startEvent();*/

				/*
				//** 톱니바퀴 시련_M
				eve_cur = MenuOnReady::getInstance()->getList()->scanBox(5);
				eve_cur->setBoxLayer(MenuOnReady::getInstance()->getLayer());
				//eve_cur->setBoxLocation(Point(winSize.width*0.3, winSize.height*0.75f));
				eve_cur->setEvent();
				eve_cur->startEvent();
				*/
			}
			//** 다른 시련이면.. (4, 5)
			else
			{
				//** 시련 번호 변경
				//EventManager::getInstance()->setTrialNum(1);

				//cur_list->moveCurTo(2);	//Menu_OnReady_Meteor_0

				//for (int i = 0; i < 2; i++)
				//{
				//	cur_list->scanCur()->setEvent();
				//	cur_list->scanCur()->startEvent();
				//	cur_list->moveCurNext();
				//}

				/* 
				//** 운석의 시련_M
				eve_cur = MenuOnReady::getInstance()->getList()->scanBox(3);
				eve_cur->setBoxLayer(MenuOnReady::getInstance()->getLayer());
				//eve_cur->setBoxLocation(Point(winSize.width*0.3, winSize.height*0.75f));
				eve_cur->setEvent();
				eve_cur->startEvent();
				*/
				//** 시련 번호 변경
				//EventManager::getInstance()->setTrialNum(1);
			}
		}
	}
}







