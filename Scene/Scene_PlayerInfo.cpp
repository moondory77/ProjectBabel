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

	_eventDispatcher->removeAllEventListeners();		//���� ���õǾ� �ִ� ��� �̺�Ʈ ������ ����

	//** �ش� ���� ��� �� ������ �ʱ�ȭ
	listener_player_info = EventListenerTouchOneByOne::create();
	listener_player_info->setSwallowTouches(true);
	listener_player_info->onTouchBegan = CC_CALLBACK_2(PlayerInfo::onTouchBegan_PlayerInfo, this);
	listener_player_info->onTouchEnded = CC_CALLBACK_2(PlayerInfo::onTouchEnded_PlayerInfo, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener_player_info, 1);

	//** �ش� �� ���� �ʱ�ȭ
	initBG();

	//** main ��� ����
	BGManager::getInstance()->loadBG();
	this->addChild(BGManager::getInstance()->getParallax());

	//** �޴� Ŭ���� ����̺�Ʈ�� ���� �̺�Ʈ����Ʈ ����
	//** EventList �ʱ�ȭ 
	MenuCharacter::getInstance()->initMenuCharacter();
	this->addChild(MenuCharacter::getInstance()->getLayer(), 5);	

	cur_list = MenuCharacter::getInstance()->getList();
	for (int i = 0; i < 2; i++)
	{
		cur_list->moveCurNext();
		cur_list->scanCur()->setEvent();
		cur_list->scanCur()->startEvent();
	}

	//** �̺�Ʈ �ִϸ��̼� �ð� 1.5�� ��ŭ ������ ��ŵ�
	auto intro_delay = EventManager::getInstance()->getListenerSleep(1.5f, listener_player_info);
	this->runAction(intro_delay);

	return true;
}


void PlayerInfo::initBG()
{
	//** #1 ��� ��� layer
	layer_bg_hill = LayerColor::create(Color4B::WHITE);
	layer_bg_hill->setAnchorPoint(Point::ZERO);
	layer_bg_hill->setOpacity(0);
	this->addChild(layer_bg_hill, 2);

	//** #2 ��� ��� sprite
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

	// 1) CharacterInfo �ʱ�ȭ�鿡��,
	if (!is_weapon_on)
	{
		//�ڷΰ��� ��ư Ŭ�� ��
		if (MenuCharacter::getInstance()->getBackTarget()->getBoundingBox().containsPoint(touchPoint))
		{
			cur_list->moveCurTo(5);	//Menu_Character_Outro_0
			for (int i = 0; i < 2; i++)
			{
				cur_list->scanCur()->setEvent();
				cur_list->scanCur()->startEvent();
				cur_list->moveCurNext();
			}
	
			//** 1.5�� ��,
			this->runAction(EventManager::getInstance()->sceneTransferAction(1.5f, LOBBY));
		}
		//������ �� Ŭ�� ��
		else if (MenuCharacter::getInstance()->getWeaponTarget()->getBoundingBox().containsPoint(touchPoint))
		{
			cur_list->moveCurTo(3);	//Menu_Character_WTab_0
			cur_list->scanCur()->setEvent();
			cur_list->scanCur()->startEvent();

			is_weapon_on = true;
			//** �̺�Ʈ �ִϸ��̼� �ð� 1�� ��ŭ ������ ��ŵ�
			auto tap_delay = EventManager::getInstance()->getListenerSleep(1.0f, listener_player_info);
			this->runAction(tap_delay);
		}
	}
	// 2) ������ sub ȭ�鿡��
	else
	{
		//������ �� Ŭ�� ��
		if (MenuCharacter::getInstance()->getWeaponTarget()->getBoundingBox().containsPoint(touchPoint))
		{
			
			cur_list->moveCurTo(4);	//Menu_Character_WTab_1
			cur_list->scanCur()->setEvent();
			cur_list->scanCur()->startEvent();

			is_weapon_on = false;
			//** �̺�Ʈ �ִϸ��̼� �ð� 1�� ��ŭ ������ ��ŵ�
			auto tap_delay = EventManager::getInstance()->getListenerSleep(1.0f, listener_player_info);
			this->runAction(tap_delay);
		}
	}

}

/*
//**�ݹ��Լ�. �κ� �� ��ȯ��
void CharacterInfo::sceneTransfer()
{
	Director::getInstance()->replaceScene(Lobby::createScene());
}

//** sceneTransfer �ݹ��Լ� �̿�. delay �Ŀ� �� ��ȯ�ϴ� �׼� ��ȯ
//** �ش� delay ���� �����ʵ� �����
Action* CharacterInfo::sceneTransferAction(float delay)
{
	//** ��ȯ�׼ǵ��� Ű�Է� ������ ����, ��� ������ ���� 
	//** -> ������ ���� ������ ������ Ȱ��ȭ
	_eventDispatcher->removeAllEventListeners();

	auto timelapse = DelayTime::create(delay);
	auto trans = CallFunc::create(CC_CALLBACK_0(CharacterInfo::sceneTransfer, this));

	CCLOG("After %.2f seconds, scene transfer", delay);
	return Sequence::create(timelapse, trans, NULL);
}



//**�ݹ��Լ�.  �Ű������� ���� ������Ʈ �����ʸ� ��밡���ϰ���
void CharacterInfo::eventOnToggle(Ref *sender, void *object)
{
	auto listener = (EventListener*)object;

	listener->setEnabled(true);
}

//** eventToggle �ݹ��Լ� �̿�.  delay ���� �ش� �����ʸ� ������Ű�� �׼� ��ȯ
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
