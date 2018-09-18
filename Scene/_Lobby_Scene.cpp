#include "System/GameManager.h"
#include "System/BGManager.h"
#include "SimpleAudioEngine.h"

#include "Event/Events.h"
#include "_Lobby_Scene.h"


Scene* Lobby::createScene()
{
	return Lobby::create();
}

bool Lobby::init()
{
	if (!Scene::init()) {
		return false;
	}

	_eventDispatcher->removeAllEventListeners();
	initBG();

	subLobby[MAIN_LOBBY] = new MainLobby(iconHero);
	subLobby[STATUS_LOBBY] = new StatusLobby(iconHero);
	//subLobby[RECORD_LOBBY] = RecordLobby::create();

	auto lobby_trans_listener = EventListenerCustom::create("ChangeToMainLobby", [&](EventCustom*) { this->addChild(subLobby[MAIN_LOBBY]); });
	_eventDispatcher->addEventListenerWithSceneGraphPriority(lobby_trans_listener, this);

	lobby_trans_listener = EventListenerCustom::create("ChangeToStatusLobby", [&](EventCustom*) { this->addChild(subLobby[STATUS_LOBBY]); });
	_eventDispatcher->addEventListenerWithSceneGraphPriority(lobby_trans_listener, this);

	lobby_trans_listener = EventListenerCustom::create("ChangeToRecordLobby", [&](EventCustom*) { this->addChild(subLobby[RECORD_LOBBY]); });
	_eventDispatcher->addEventListenerWithSceneGraphPriority(lobby_trans_listener, this);

	lobby_trans_listener = EventListenerCustom::create("RushToField", [&](EventCustom*) { this->eventList[InOutMotion]->playBox(); });
	_eventDispatcher->addEventListenerWithSceneGraphPriority(lobby_trans_listener, this);

	eventList[InOutMotion] = new MainLobby_InOutMotion();
	eventList[InOutMotion]->initThread({ iconHero });
	eventList[InOutMotion]->playBox();

	this->addChild(subLobby[MAIN_LOBBY]);
	return true;
}



void Lobby::initBG()
{
	BGManager::getInstance()->loadBG();
	this->addChild(BGManager::getInstance()->getParallax());

	//# Layer 1 - BackGround Hill 로딩
	auto layer_hill = Layer::create();
	layer_hill->setAnchorPoint(Point::ZERO);
	this->addChild(layer_hill);

	auto spr_hill = Sprite::create("hill.png");
	spr_hill->setScale(DivForHorizontal(spr_hill), DivForVertical(spr_hill)*0.6f);
	spr_hill->setAnchorPoint(Point::ZERO);
	spr_hill->setPosition(Point::ZERO);
	layer_hill->addChild(spr_hill);

	iconHero = Sprite::create("motions/for_events/menu_cha/menu_character_1.png");
	iconHero->setScale(DivForHorizontal(iconHero)*0.22f);
	iconHero->setAnchorPoint(Point(0.5, 0));
	iconHero->setPosition(Point(winSize().width*0.5f, -winSize().height*0.2));
	this->addChild(iconHero, 3);
}





/*
///////////////////////////각 시나리오 이벤트 리스너에 대해서..////////////////////////////////////
bool Lobby::onTouchBegan_Prac(Touch* touch, Event *unused_event)
{
	return true;
}
void Lobby::onTouchEnded_Prac(Touch* touch, Event *unused_event)
{
	Point touchPoint = touch->getLocation();



		//** 다음 리스트 cursor에 대한 처리
		if (eve_cur != NULL)
		{
			if (!eve_cur->is_continuing)
				eve_cur->stopEvent();
		}

		//** 리스트의 다음 이벤트박스로 cursor 이동
		eve_cur = EvePrac::getInstance()->getPracList()->scanBoxCur();
		CCLOG("scanBox success");

		if (eve_cur->getBoxType() == 1)
		{
			auto a_cursor = dynamic_cast<ActionBox*>(eve_cur);
			a_cursor->setBoxTarget(EvePrac::getInstance()->getPracTarget());
			a_cursor->setEvent();

			if (!a_cursor->is_touchable)
			{
				CCLOG("touch off");
				//a_cursor->setListener(listener_prac);
				a_cursor->startEvent();

				//** move 관련 이벤트일 경우, move 진행 중에 다른 터치 입력을 방지
				this->runAction(EventManager::getInstance()
										->eventOnToggleAction(a_cursor->time_lapse, listener_prac));
			}
			else
				a_cursor->startEvent();

		}
		else if (eve_cur->getBoxType() == 2)
		{
			auto m_cursor = dynamic_cast<MsgBox*>(eve_cur);
			m_cursor->setBoxLayer(EvePrac::getInstance()->getPracLayer());
			m_cursor->setEvent();
			m_cursor->startEvent();
		}
}
*/


