#include "base/CCEventDispatcher.h"
#include <string.h>
#include "System/GameManager.h"
#include "System/BGManager.h"
#include "Event/Events.h"

#include "_PlayerInfo_Scene.h"


Scene* PlayerInfo::createScene()
{
	return PlayerInfo::create();
}


bool PlayerInfo::init()
{
	if (!Scene::init()) {
		return false;
	}

	_eventDispatcher->removeAllEventListeners();
	initBG();

	listener_Status = EventListenerTouchOneByOne::create();
	listener_Status->setSwallowTouches(true);
	listener_Status->onTouchBegan = CC_CALLBACK_2(PlayerInfo::onTouchBegan_Status, this);
	listener_Status->onTouchEnded = CC_CALLBACK_2(PlayerInfo::onTouchEnded_Status, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener_Status, this);
	listener_Status->setEnabled(false);

	listener_WpChange = EventListenerTouchOneByOne::create();
	listener_WpChange->setSwallowTouches(true);
	listener_WpChange->onTouchBegan = CC_CALLBACK_2(PlayerInfo::onTouchBegan_WpChange, this);
	listener_WpChange->onTouchEnded = CC_CALLBACK_2(PlayerInfo::onTouchEnded_WpChange, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener_WpChange, this);
	listener_WpChange->setEnabled(false);

	//threadSwingWeapon = new LobbyStatusHeroMotion();
	//threadSwingWeapon->initThread({sprIcon[HERO], sprIcon[SWORD], sprIcon[BAT]});

	return true;
}


void PlayerInfo::onEnterTransitionDidFinish()
{
	threadSwingWeapon->playBox();

	sprIcon[WP_TAB]->runAction(MoveBy::create(1.5f, Point(winSize().width*0.8f, 0)));
	sprIcon[BACK]->runAction(FadeIn::create(1.5f));

	this->runAction(Sequence::create(DelayTime::create(1.2f),
		CallFunc::create([&]() {listener_Status->setEnabled(true); }), nullptr));
}


void PlayerInfo::initBG()
{
	//# Layer 0 - Karma BG 로딩
	BGManager::getInstance()->loadBG();
	this->addChild(BGManager::getInstance()->getParallax());

	//# Layer 1 - Main Hill BG 로딩
	auto layer_back = Layer::create();
	layer_back->setAnchorPoint(Point::ZERO);
	//layer_back->setOpacity(0);
	this->addChild(layer_back);

	auto spr_hill = Sprite::create("hill.png");
	spr_hill->setScale(DivForHorizontal(spr_hill), DivForVertical(spr_hill)*0.6f);
	spr_hill->setAnchorPoint(Point::ZERO);
	spr_hill->setPosition(Point::ZERO);
	layer_back->addChild(spr_hill);

	sprIcon[HERO] = Sprite::create("motions/for_events/menu_cha/menu_character_1.png");
	sprIcon[HERO]->setScale(DivForHorizontal(sprIcon[HERO])*0.3f);
	sprIcon[HERO]->setAnchorPoint(Point(0.5, 0));
	sprIcon[HERO]->setPosition(Point(winSize().width*0.5f, winSize().height*0.5f));
	layer_back->addChild(sprIcon[HERO]);

	//# Layer 2 - UI Layer
	layerUI = Layer::create();
	layerUI->setAnchorPoint(Point::ZERO);
	this->addChild(layerUI);

	sprIcon[WP_TAB] = Sprite::create("layout/character_info/weapon.png");
	sprIcon[WP_TAB]->setScale(DivForHorizontal(sprIcon[WP_TAB])*0.8f, DivForVertical(sprIcon[WP_TAB])*0.1f);
	sprIcon[WP_TAB]->setAnchorPoint(Point::ZERO);
	sprIcon[WP_TAB]->setPosition(-winSize().width*0.8f, winSize().height*0.8f);
	layerUI->addChild(sprIcon[WP_TAB]);

	sprIcon[BACK] = Sprite::create("layout/character_info/back.png");
	sprIcon[BACK]->setScale(DivForHorizontal(sprIcon[BACK])*0.23f, DivForVertical(sprIcon[BACK])*0.08f);
	sprIcon[BACK]->setPosition(winSize().width*0.5f, winSize().height*0.15f);
	sprIcon[BACK]->setOpacity(0);
	layerUI->addChild(sprIcon[BACK]);


	sprIcon[SWORD] = Sprite::create("weapon/sword.png");
	sprIcon[SWORD]->setScale(DivForHorizontal(sprIcon[SWORD])*0.18f);
	sprIcon[SWORD]->setAnchorPoint(Point(0.5, 0));
	sprIcon[SWORD]->setPosition(Point(winSize().width*0.32f, winSize().height*0.22f));
	sprIcon[SWORD]->setOpacity(0);
	layerUI->addChild(sprIcon[SWORD]);

	sprIcon[BAT] = Sprite::create("weapon/bat.png");
	sprIcon[BAT]->setScale(DivForHorizontal(sprIcon[BAT])*0.2f, DivForVertical(sprIcon[BAT])*0.26f);
	sprIcon[BAT]->setAnchorPoint(Point(0.5, 0));
	sprIcon[BAT]->setPosition(Point(winSize().width*0.68f, winSize().height*0.22f));
	sprIcon[BAT]->setOpacity(0);
	layerUI->addChild(sprIcon[BAT]);
}



bool PlayerInfo::onTouchBegan_Status(Touch* touch, Event *unused_event)
{
	Point touchPos = touch->getLocation();

	if (sprIcon[WP_TAB]->getBoundingBox().containsPoint(touchPos)) {
		isTouchedIcon[WP_TAB] = touch->getID();
		return true;
	}
	if (sprIcon[BACK]->getBoundingBox().containsPoint(touchPos)) {
		isTouchedIcon[BACK] = touch->getID();
		return true;
	}
	return false;
}



void PlayerInfo::onTouchEnded_Status(Touch* touch, Event *unused_event)
{
	Point touchPos = touch->getLocation();

	// -> Weapon Change 화면으로
	if (sprIcon[WP_TAB]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == isTouchedIcon[WP_TAB]))
	{
		//(현재무기 탭) 메뉴 바 약간좌측으로 '감추는' 액션(spr_icon_weapon)
		sprIcon[WP_TAB]->runAction(MoveBy::create(1.0f, Point(-winSize().width*0.6f, 0)));
		sprIcon[SWORD]->runAction(FadeIn::create(1.2f));
		sprIcon[BAT]->runAction(FadeIn::create(1.2f));
		sprIcon[BACK]->runAction(FadeOut::create(1.2f));
	
		listener_Status->setEnabled(false);
		this->runAction(Sequence::create(DelayTime::create(1.5f),
			CallFunc::create([&]() {listener_WpChange->setEnabled(true); }), nullptr));
	}
	// -> 다시 Lobby로
	else if (sprIcon[BACK]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == isTouchedIcon[BACK]))
	{
		//threadSwingWeapon->playBox();	//무기 갈무리
		//this->runAction(EventManager::getInstance()->sceneTransferAction(1.5f, LOBBY));
	}

	isTouchedIcon[WP_TAB] = -1;
	isTouchedIcon[BACK] = -1;
}



bool PlayerInfo::onTouchBegan_WpChange(Touch* touch, Event* unused_event)
{
	Point touchPos = touch->getLocation();

	if (sprIcon[WP_TAB]->getBoundingBox().containsPoint(touchPos)) {
		isTouchedIcon[WP_TAB] = touch->getID();
		return true;
	}
	if (sprIcon[SWORD]->getBoundingBox().containsPoint(touchPos)) {
		isTouchedIcon[SWORD] = touch->getID();
		return true;
	}
	if (sprIcon[BAT]->getBoundingBox().containsPoint(touchPos)) {
		isTouchedIcon[BAT] = touch->getID();
		return true;
	}
	return false;
}



void PlayerInfo::onTouchEnded_WpChange(Touch* touch, Event *unused_event)
{
	Point touchPos = touch->getLocation();

	if (sprIcon[WP_TAB]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == isTouchedIcon[WP_TAB]))
	{
		//(현재무기 탭) 메뉴 바 약간좌측으로 '감추는' 액션(spr_icon_weapon)
		sprIcon[WP_TAB]->runAction(MoveBy::create(1.0f, Point(winSize().width*0.6f, 0)));
		sprIcon[SWORD]->runAction(FadeOut::create(1.2f));
		sprIcon[BAT]->runAction(FadeOut::create(1.2f));
		sprIcon[BACK]->runAction(FadeIn::create(1.2f));

		listener_WpChange->setEnabled(false);
		this->runAction(Sequence::create(DelayTime::create(1.2f),
			CallFunc::create([&]() {listener_Status->setEnabled(true); }), nullptr));

	}

}

