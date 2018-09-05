#include "Manager/GameManager.h"
#include "Manager/BGManager.h"
#include "Manager/PopUps.h"

#include "Plot/EventPlayer.h"
#include "Plot/EventList.h"
#include "Plot/EventBox.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"
#include <string.h>

#include "Scene/_Scene_Lobby.h"


USING_NS_CC;

Scene* Lobby::createScene()
{
	return Lobby::create();
}

bool Lobby::init()
{
	if (!Scene::init()){
		return false;
	}
	_eventDispatcher->removeAllEventListeners();	//현재 세팅되어 있는 모든 이벤트 리스너 해제
	
	//** Lobby 씬에 대한 리스너 생성 및 등록
	listener_lobby = EventListenerTouchOneByOne::create();
	listener_lobby->setSwallowTouches(true);
	listener_lobby->onTouchBegan = CC_CALLBACK_2(Lobby::onTouchBegan_Lobby, this);
	listener_lobby->onTouchEnded = CC_CALLBACK_2(Lobby::onTouchEnded_Lobby, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener_lobby, 3);

	//진도  암시 배경, hill 배경 셋팅
	//BGManager::getInstance()->loadBG(); 
	//this->addChild(BGManager::getInstance()->getParallax());


	
	initBG();

	//** EventPlayer 초기화시키고, Lobby 씬에 레이어로 삽입
	MenuLobby::getInstance()->initMenuLobby();
	this->addChild(MenuLobby::getInstance()->getLayer());

	this->runAction(EventManager::getInstance()->getListenerSleep(3.5f, listener_lobby));
	//BGManager::getInstance()->moveBG(500);
	cur_list = MenuLobby::getInstance()->getList();
	
	for (int i = 0; i < 2; i++){
		cur_list->moveCurNext();
		cur_list->scanCur()->setEvent();
		cur_list->scanCur()->startEvent();
	}

	////** Case 1) 스테이지 클리어 후, 귀환하는 경우
	//if (EventManager::getInstance()->getFromResult())
	//{
	//	//** 이벤트 애니메이션 시간 3.5초 만큼 리스너 슬립	
	//	this->runAction(EventManager::getInstance()->getListenerSleep(3.5f, listener_lobby));
	//	eve_cur = MenuLobby::getInstance()->getList()->scanBox(2); 	//MenuLobbyBox_Return_A
	//	eve_cur->setEvent();
	//	eve_cur->startEvent();
	//}
	////** Case 2) 그 밖의 다른 Scene에서 진입하는 경우
	//else
	//{
	//	//** 이벤트 애니메이션 시간 1.5초 만큼 리스너 잠궈둠
	//	this->runAction(EventManager::getInstance()->getListenerSleep(1.7f, listener_lobby));
	//	eve_cur = MenuLobby::getInstance()->getList()->scanBox(1);	//MenuLobbyBox_Intro_A
	//	eve_cur->setEvent();
	//	eve_cur->startEvent();
	//}

	//** 다시 플래그 갱신
	//EventManager::getInstance()->setFromResult(false);
	return true;
}


// 배경 레이어(언덕그림 sub) 초기화
void Lobby::initBG()
{
	BGManager::getInstance()->loadBG();
	this->addChild(BGManager::getInstance()->getParallax());

	//** #2 언덕 스프라이트(고정) layer
	layer_bg_hill = LayerColor::create(Color4B::WHITE);
	layer_bg_hill->setAnchorPoint(Point::ZERO);
	layer_bg_hill->setOpacity(0);
	this->addChild(layer_bg_hill);

	spr_bg_hill = Sprite::create("hill.png");	
	spr_bg_hill->setScale(DivForHorizontal(spr_bg_hill), DivForVertical(spr_bg_hill)*0.6f);
	spr_bg_hill->setAnchorPoint(Point::ZERO);
	//spr_bg_hill->setRotation(25);
	spr_bg_hill->setPosition(Point::ZERO);
	layer_bg_hill->addChild(spr_bg_hill);

	//CCLOG("winsize : width %f, height %f", winSize().width, winSize().height);
}


bool Lobby::onTouchBegan_Lobby(Touch* touch, Event *unused_event)
{
	if (!is_popup_on)
	{
		return true;
	}
	return false;
}
void Lobby::onTouchEnded_Lobby(Touch* touch, Event *unused_event)
{
	Point touchPoint = touch->getLocation();

	if (!is_popup_on)
	{
		//** #1 - CharacterInfo 버튼을 눌렀을 때,
		if (MenuLobby::getInstance()->getCharacterTarget()
							->getBoundingBox().containsPoint(touchPoint))
		{	
			_eventDispatcher->removeAllEventListeners();

			cur_list->moveCurTo(3);	//Menu_Lobby_Outro_0
			cur_list->scanCur()->setEvent();
			cur_list->scanCur()->startEvent();

			this->runAction(EventManager::getInstance()->sceneTransferAction(1.5f, PLAYER_INFO));
			// 1.5초 대기 후, CharacterInfo 씬으로 이동
		}
		//** #2 - Record 버튼을 눌렀을 때,
		else if (MenuLobby::getInstance()->getRecordTarget()
							->getBoundingBox().containsPoint(touchPoint))
		{
			_eventDispatcher->removeAllEventListeners();

			cur_list->moveCurTo(3);	//Menu_Lobby_Outro_0
			cur_list->scanCur()->setEvent();
			cur_list->scanCur()->startEvent();

			// 1.5초 대기 후, Record 씬
			this->runAction(EventManager::getInstance()->sceneTransferAction(1.5f, RECORD));
		}
		//** #3 - Start 버튼을 눌렀을 때,
		else if(MenuLobby::getInstance()->getStartTarget()
								->getBoundingBox().containsPoint(touchPoint))
		{
			BGManager::getInstance()->moveBG(400);

			_eventDispatcher->removeAllEventListeners();

			cur_list->moveCurTo(3);	//Menu_Lobby_Outro_0
			for (int i = 0; i < 2; i++) {
				cur_list->scanCur()->setEvent();
				cur_list->scanCur()->startEvent();
				cur_list->moveCurNext();
			}
			// 4 초 후, OnReady 씬 (fade 2초)
			this->runAction(EventManager::getInstance()->sceneTransferAction(4, ON_READY));
		}
		//** #4 - Setting 버튼을 눌렀을 때, (팝업)
		else if(MenuLobby::getInstance()->getSettingTarget()
					->getBoundingBox().containsPoint(touchPoint)){
		
			//** 셋팅 팝업 레이어에 대한 리스너를 생성하고 콜백설정까지
			listener_setting = EventListenerTouchOneByOne::create();
			listener_setting->setSwallowTouches(true);
			listener_setting->onTouchBegan = CC_CALLBACK_2(Lobby::onTouchBegan_Setting, this);
			listener_setting->onTouchEnded = CC_CALLBACK_2(Lobby::onTouchEnded_Setting, this);

			Director::getInstance()->getEventDispatcher()
				->addEventListenerWithSceneGraphPriority(listener_setting, this);

			PopUpSetting::getInstance()->loadPopUp();
			this->addChild(PopUpSetting::getInstance()->getLayer(), 5);
			is_popup_on = true;
		}
		//** 그 밖의 경우 -> (임시) 없애야 함
		else
		{
			//BGManager::getInstance()->moveBG(1100);
			this->runAction(EventManager::getInstance()->getListenerSleep(4.0, listener_lobby));
		}
	}

}



////////////////////////////////////********셋팅 이벤트 리스너*********////////////////////////////////
bool Lobby::onTouchBegan_Setting(Touch* touch, Event *unused_event)
{
	
	Point touchPoint = touch->getLocation();
	/*
	if (PopUpSetting::getInstance()->
						spr_icon_bgm->getBoundingBox().containsPoint(touchPoint))
		return true;
	else if (PopUpSetting::getInstance()->
						spr_icon_effect->getBoundingBox().containsPoint(touchPoint))
		return true;
	else if (PopUpSetting::getInstance()->
						spr_icon_vibrate->getBoundingBox().containsPoint(touchPoint))
		return true;
	else if (PopUpSetting::getInstance()->
						spr_icon_close->getBoundingBox().containsPoint(touchPoint))
		return true;
	else
	*/
		return true;
	
}
void Lobby::onTouchEnded_Setting(Touch* touch, Event *unused_event)
{
	Point touchPoint = touch->getLocation();


	if (PopUpSetting::getInstance()->spr_icon_bgm->getBoundingBox().containsPoint(touchPoint))
	{
		PopUpSetting::getInstance()->bgmToggle();
	}
	else if (PopUpSetting::getInstance()->spr_icon_effect->getBoundingBox().containsPoint(touchPoint))
	{
		PopUpSetting::getInstance()->effectToggle();
	}

	else if (PopUpSetting::getInstance()->spr_icon_vibrate->getBoundingBox().containsPoint(touchPoint))
	{
		PopUpSetting::getInstance()->vibrateToggle();
	}
	else if (PopUpSetting::getInstance()->spr_icon_close->getBoundingBox().containsPoint(touchPoint))
	{
		this->removeChild(PopUpSetting::getInstance()->getLayer());
		_eventDispatcher->removeEventListener(listener_setting);
		is_popup_on = false;
	}
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


