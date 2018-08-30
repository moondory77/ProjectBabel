#include "base/CCEventDispatcher.h"
#include <string.h>
#include "Manager/GameManager.h"
#include "Manager/BGManager.h"
#include "Scene_PlayerInfo.h"

Scene* PlayerInfo::createScene()
{
	return PlayerInfo::create();
}

bool PlayerInfo::init()
{
	if (!Scene::init())
	{
		return false;
	}

	_eventDispatcher->removeAllEventListeners();		//현재 세팅되어 있는 모든 이벤트 리스너 해제

	//** 해당 씬에 사용 될 리스너 초기화
	listener_player_info = EventListenerTouchOneByOne::create();
	listener_player_info->setSwallowTouches(true);
	listener_player_info->onTouchBegan = CC_CALLBACK_2(PlayerInfo::onTouchBegan_PlayerInfo, this);
	listener_player_info->onTouchEnded = CC_CALLBACK_2(PlayerInfo::onTouchEnded_PlayerInfo, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener_player_info, 1);

	//** 해당 씬 배경들 초기화
	initBG();

	//** main 배경 갱신
	BGManager::getInstance()->loadBG();
	this->addChild(BGManager::getInstance()->getParallax());

	//** 메뉴 클릭시 모션이벤트를 위한 이벤트리스트 생성
	//** EventList 초기화 
	MenuCharacter::getInstance()->initMenuCharacter();
	this->addChild(MenuCharacter::getInstance()->getLayer(), 5);	

	cur_list = MenuCharacter::getInstance()->getList();
	for (int i = 0; i < 2; i++)
	{
		cur_list->moveCurNext();
		cur_list->scanCur()->setEvent();
		cur_list->scanCur()->startEvent();
	}

	//** 이벤트 애니메이션 시간 1.5초 만큼 리스너 잠궈둠
	auto intro_delay = EventManager::getInstance()->getListenerSleep(1.5f, listener_player_info);
	this->runAction(intro_delay);

	return true;
}


void PlayerInfo::initBG()
{
	//** #1 언덕 배경 layer
	layer_bg_hill = LayerColor::create(Color4B::WHITE);
	layer_bg_hill->setAnchorPoint(Point::ZERO);
	layer_bg_hill->setOpacity(0);
	this->addChild(layer_bg_hill, 2);

	//** #2 언덕 배경 sprite
	spr_bg_hill = Sprite::create("hill.png");
	spr_bg_hill->setScale(DivForHorizontal(spr_bg_hill), DivForVertical(spr_bg_hill)*0.6f);
	spr_bg_hill->setAnchorPoint(Point::ZERO);
	//spr_bg_hill->setRotation(25);
	spr_bg_hill->setPosition(Point::ZERO);
	layer_bg_hill->addChild(spr_bg_hill);

	spr_bg_logo = Sprite::create("layout/character_info/logo.png");
	spr_bg_logo->setScale(DivForHorizontal(spr_bg_logo)*0.8f, DivForVertical(spr_bg_logo)*0.2f);
	spr_bg_logo->setPosition(Point(winSize().width*0.5f, winSize().height*0.85f));
	//layer_bg_hill->addChild(spr_bg_logo);
}

bool PlayerInfo::onTouchBegan_PlayerInfo(Touch* touch, Event *unused_event)
{
	if (!is_popup_on)
	{
		return true;
	}
	return false;
}


void PlayerInfo::onTouchEnded_PlayerInfo(Touch* touch, Event *unused_event)
{
	Point touchPoint = touch->getLocation();

	// 1) CharacterInfo 초기화면에서,
	if (!is_weapon_on)
	{
		//뒤로가기 버튼 클릭 시
		if (MenuCharacter::getInstance()->getBackTarget()->getBoundingBox().containsPoint(touchPoint))
		{
			cur_list->moveCurTo(5);	//Menu_Character_Outro_0
			for (int i = 0; i < 2; i++)
			{
				cur_list->scanCur()->setEvent();
				cur_list->scanCur()->startEvent();
				cur_list->moveCurNext();
			}
	
			//** 1.5초 후,
			this->runAction(EventManager::getInstance()->sceneTransferAction(1.5f, LOBBY));
		}
		//무기목록 탭 클릭 시
		else if (MenuCharacter::getInstance()->getWeaponTarget()->getBoundingBox().containsPoint(touchPoint))
		{
			cur_list->moveCurTo(3);	//Menu_Character_WTab_0
			cur_list->scanCur()->setEvent();
			cur_list->scanCur()->startEvent();

			is_weapon_on = true;
			//** 이벤트 애니메이션 시간 1초 만큼 리스너 잠궈둠
			auto tap_delay = EventManager::getInstance()->getListenerSleep(1.0f, listener_player_info);
			this->runAction(tap_delay);
		}
	}
	// 2) 무기목록 sub 화면에서
	else
	{
		//무기목록 탭 클릭 시
		if (MenuCharacter::getInstance()->getWeaponTarget()->getBoundingBox().containsPoint(touchPoint))
		{
			
			cur_list->moveCurTo(4);	//Menu_Character_WTab_1
			cur_list->scanCur()->setEvent();
			cur_list->scanCur()->startEvent();

			is_weapon_on = false;
			//** 이벤트 애니메이션 시간 1초 만큼 리스너 잠궈둠
			auto tap_delay = EventManager::getInstance()->getListenerSleep(1.0f, listener_player_info);
			this->runAction(tap_delay);
		}
	}

}

/*
//**콜백함수. 로비 씬 전환용
void CharacterInfo::sceneTransfer()
{
	Director::getInstance()->replaceScene(Lobby::createScene());
}

//** sceneTransfer 콜백함수 이용. delay 후에 씬 전환하는 액션 반환
//** 해당 delay 동안 리스너도 잠재움
Action* CharacterInfo::sceneTransferAction(float delay)
{
	//** 변환액션동안 키입력 방지를 위해, 모든 리스너 해제 
	//** -> 어차피 다음 씬에서 리스너 활성화
	_eventDispatcher->removeAllEventListeners();

	auto timelapse = DelayTime::create(delay);
	auto trans = CallFunc::create(CC_CALLBACK_0(CharacterInfo::sceneTransfer, this));

	CCLOG("After %.2f seconds, scene transfer", delay);
	return Sequence::create(timelapse, trans, NULL);
}



//**콜백함수.  매개변수로 받은 오브젝트 리스너를 사용가능하게함
void CharacterInfo::eventOnToggle(Ref *sender, void *object)
{
	auto listener = (EventListener*)object;

	listener->setEnabled(true);
}

//** eventToggle 콜백함수 이용.  delay 동안 해당 리스너를 중지시키는 액션 반환
Action* CharacterInfo::eventOnToggleAction(float delay, EventListener* listener)
{
	listener->setEnabled(false);

	auto timelapse = DelayTime::create(delay);
	auto toggle = CallFuncN::create(CC_CALLBACK_1(CharacterInfo::eventOnToggle, this, listener));

	CCLOG("During %.2f seconds, listener sleep", delay);
	//auto total = Sequence::create(timelapse, toggle_delay, NULL);

	return Sequence::create(timelapse, toggle, NULL);
}
*/
