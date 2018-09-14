#include "_Lobby_Layer.h"
#include "System/GameManager.h"
#include "Event/Events.h"


///////////////////////////////////////////////////////////////////////////
/*****************************Main Layer**********************************/
///////////////////////////////////////////////////////////////////////////
bool MainLobby::init()
{
	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 0), winSize().width, winSize().height)) {
		return false;
	}
	listenerMain = EventListenerTouchOneByOne::create();
	listenerMain->setSwallowTouches(true);
	listenerMain->onTouchBegan = CC_CALLBACK_2(MainLobby::onTouchBegan_Main, this);
	listenerMain->onTouchEnded = CC_CALLBACK_2(MainLobby::onTouchEnded_Main, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerMain, this);
	listenerMain->setEnabled(false);

	for (int i = 0; i < 6; i++) {
		iconTouchID[i] = -1;
	}
	initUI();	//각 아이콘 스프라이트 생성/배치
	return true;
}
void MainLobby::initUI()
{
	iconTarget[TITLE_LOGO] = Sprite::create("layout/main/title.png");
	iconTarget[TITLE_LOGO]->setScale(DivForHorizontal(iconTarget[TITLE_LOGO])*0.9f);
	iconTarget[TITLE_LOGO]->setPosition(Point(winSize().width*0.5f, winSize().height*0.85f));
	iconTarget[TITLE_LOGO]->setOpacity(0);
	this->addChild(iconTarget[TITLE_LOGO]);

	iconTarget[START] = Sprite::create("layout/main/start.png");
	iconTarget[START]->setScale(DivForHorizontal(iconTarget[START])*0.7f);
	iconTarget[START]->setAnchorPoint(Point(0.5, 0));
	iconTarget[START]->setPosition(Point(winSize().width*0.5f, 0));
	iconTarget[START]->setOpacity(0);
	this->addChild(iconTarget[START]);

	iconTarget[RECORD] = Sprite::create("layout/main/record.png");
	iconTarget[RECORD]->setScale(DivForHorizontal(iconTarget[RECORD])*0.25f);
	iconTarget[RECORD]->setAnchorPoint(Point(0, 0.5));
	iconTarget[RECORD]->setPosition(Point(0, winSize().height*0.25f));
	iconTarget[RECORD]->setOpacity(0);
	this->addChild(iconTarget[RECORD]);

	iconTarget[STATUS] = Sprite::create("layout/main/character.png");
	iconTarget[STATUS]->setScale(DivForHorizontal(iconTarget[STATUS])*0.25f);
	iconTarget[STATUS]->setAnchorPoint(Point(0, 0.5));
	iconTarget[STATUS]->setPosition(Point(0, winSize().height*0.4f));
	iconTarget[STATUS]->setOpacity(0);
	this->addChild(iconTarget[STATUS]);

	iconTarget[SETTING] = Sprite::create("layout/main/setting.png");
	iconTarget[SETTING]->setScale(DivForHorizontal(iconTarget[SETTING])*0.25f);
	iconTarget[SETTING]->setAnchorPoint(Point(1, 0.5));
	iconTarget[SETTING]->setPosition(Point(winSize().width, winSize().height*0.3f));
	iconTarget[SETTING]->setOpacity(0);
	this->addChild(iconTarget[SETTING]);
}


//해당 Sub Layer가 addChild 될 때마다
void MainLobby::onEnterTransitionDidFinish()
{
	iconTarget[START]->runAction(FadeIn::create(1.5f));
	iconTarget[RECORD]->runAction(FadeIn::create(1.5f));
	iconTarget[STATUS]->runAction(FadeIn::create(1.5f));
	iconTarget[SETTING]->runAction(FadeIn::create(1.5f));

	//터치 리스너는, 인트로 모션 후부터 사용 가능
	this->runAction(Sequence::create(DelayTime::create(1.5f),
		CallFunc::create([&]() {listenerMain->setEnabled(true); }), nullptr));
}

bool MainLobby::onTouchBegan_Main(Touch* touch, Event *unused_event)
{
	Point touchPos = touch->getLocation();

	//터치 시, 터치된 아이콘의 id 획득 후 이벤트 넘긴다
	if (iconTarget[START]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[START] = touch->getID();
		return true;
	}
	if (iconTarget[RECORD]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[RECORD] = touch->getID();
		return true;
	}
	if (iconTarget[STATUS]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[STATUS] = touch->getID();
		return true;
	}
	if (iconTarget[SETTING]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[SETTING] = touch->getID();
		return true;
	}
	return false;
}
void MainLobby::onTouchEnded_Main(Touch* touch, Event *unused_event)
{
	Point touchPos = touch->getLocation();

	if (iconTarget[START]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[START]))
	{
		listenerMain->setEnabled(false);
		getParent()->getEventDispatcher()->dispatchCustomEvent("RushToField");
	
		iconTarget[RECORD]->runAction(FadeOut::create(1.2f));
		iconTarget[STATUS]->runAction(FadeOut::create(1.2f));
		iconTarget[SETTING]->runAction(FadeOut::create(1.2f));
		iconTarget[START]->runAction(Sequence::create(FadeOut::create(1.2f),
			CallFunc::create([&]() { this->removeFromParent(); }), nullptr));
	}
	else if (iconTarget[RECORD]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[RECORD]))
	{
		listenerMain->setEnabled(false);
		getParent()->getEventDispatcher()->dispatchCustomEvent("ChangeToRecordLobby");
	
		iconTarget[START]->runAction(FadeOut::create(1.2f));
		iconTarget[STATUS]->runAction(FadeOut::create(1.2f));
		iconTarget[SETTING]->runAction(FadeOut::create(1.2f));
		iconTarget[RECORD]->runAction(Sequence::create(FadeOut::create(1.2f),
			CallFunc::create([&]() { this->removeFromParent(); }), nullptr));
	}
	else if (iconTarget[STATUS]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[STATUS]))
	{
		listenerMain->setEnabled(false);
		getParent()->getEventDispatcher()->dispatchCustomEvent("ChangeToStatusLobby");
		
		iconTarget[START]->runAction(FadeOut::create(1.2f));
		iconTarget[RECORD]->runAction(FadeOut::create(1.2f));
		iconTarget[SETTING]->runAction(FadeOut::create(1.2f));
		iconTarget[STATUS]->runAction(Sequence::create(FadeOut::create(1.2f),
			CallFunc::create([&]() { this->removeFromParent(); }), nullptr));

	}
	else if (iconTarget[SETTING]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[SETTING]))
	{

	}

}


///////////////////////////////////////////////////////////////////////////
/****************************Status Layer*********************************/
///////////////////////////////////////////////////////////////////////////

bool StatusLobby::init()
{
	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 0))) {
		return false;
	}

	listenerStatus = EventListenerTouchOneByOne::create();
	listenerStatus->setSwallowTouches(true);
	listenerStatus->onTouchBegan = CC_CALLBACK_2(StatusLobby::onTouchBegan_Status, this);
	listenerStatus->onTouchEnded = CC_CALLBACK_2(StatusLobby::onTouchEnded_Status, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerStatus, this);
	listenerStatus->setEnabled(false);

	listenerWpChange = EventListenerTouchOneByOne::create();
	listenerWpChange->setSwallowTouches(true);
	listenerWpChange->onTouchBegan = CC_CALLBACK_2(StatusLobby::onTouchBegan_WpChange, this);
	listenerWpChange->onTouchEnded = CC_CALLBACK_2(StatusLobby::onTouchEnded_WpChange, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerWpChange, this);
	listenerWpChange->setEnabled(false);

	for (int i = 0; i < 5; i++) {
		iconTouchID[i] = -1;
	}

	initUI();	//아이콘 스프라이트 생성/배치

	//필요한 이벤트 쓰레드 생성 및 초기화
	eventList[InOutMotion] = new StatusLobby_InOutMotion();
	eventList[InOutMotion]->initThread({ iconTarget[HERO] });
	return true;
}
void StatusLobby::initUI()
{
	iconTarget[WEAPON_TAB] = Sprite::create("layout/character_info/weapon.png");
	iconTarget[WEAPON_TAB]->setScale(DivForHorizontal(iconTarget[WEAPON_TAB])*0.8f, DivForVertical(iconTarget[WEAPON_TAB])*0.1f);
	iconTarget[WEAPON_TAB]->setAnchorPoint(Point::ZERO);
	iconTarget[WEAPON_TAB]->setPosition(-winSize().width*0.8f, winSize().height*0.8f);
	this->addChild(iconTarget[WEAPON_TAB]);

	iconTarget[BACK] = Sprite::create("layout/character_info/back.png");
	iconTarget[BACK]->setScale(DivForHorizontal(iconTarget[BACK])*0.23f, DivForVertical(iconTarget[BACK])*0.08f);
	iconTarget[BACK]->setPosition(winSize().width*0.5f, winSize().height*0.15f);
	iconTarget[BACK]->setOpacity(0);
	this->addChild(iconTarget[BACK]);

	iconTarget[SWORD] = Sprite::create("weapon/sword.png");
	iconTarget[SWORD]->setScale(DivForHorizontal(iconTarget[SWORD])*0.18f);
	iconTarget[SWORD]->setAnchorPoint(Point(0.5, 0));
	iconTarget[SWORD]->setPosition(Point(winSize().width*0.32f, winSize().height*0.22f));
	iconTarget[SWORD]->setOpacity(0);
	this->addChild(iconTarget[SWORD]);

	iconTarget[BAT] = Sprite::create("weapon/bat.png");
	iconTarget[BAT]->setScale(DivForHorizontal(iconTarget[BAT])*0.2f, DivForVertical(iconTarget[BAT])*0.26f);
	iconTarget[BAT]->setAnchorPoint(Point(0.5, 0));
	iconTarget[BAT]->setPosition(Point(winSize().width*0.68f, winSize().height*0.22f));
	iconTarget[BAT]->setOpacity(0);
	this->addChild(iconTarget[BAT]);
}


//씬 삽입 순간, 모션 이벤트 시작
void StatusLobby::onEnterTransitionDidFinish()
{
	eventList[InOutMotion]->playBox();
	iconTarget[WEAPON_TAB]->runAction(MoveBy::create(1.5f, Point(winSize().width*0.8f, 0)));
	iconTarget[BACK]->runAction(FadeIn::create(1.5f));

	//터치 리스너를 인트로 모션 후부터 사용 가능
	this->runAction(Sequence::create(DelayTime::create(1.2f),
		CallFunc::create([&]() {listenerStatus->setEnabled(true); }), nullptr));
}

bool StatusLobby::onTouchBegan_Status(Touch* touch, Event *unused_event)
{
	Point touchPos = touch->getLocation();

	//터치 시, 터치된 아이콘의 id 획득 후 이벤트 넘긴다
	if (iconTarget[WEAPON_TAB]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[WEAPON_TAB] = touch->getID();
		return true;
	}
	if (iconTarget[BACK]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[BACK] = touch->getID();
		return true;
	}
	return false;
}
void StatusLobby::onTouchEnded_Status(Touch* touch, Event *unused_event)
{
	Point touchPos = touch->getLocation();

	// -> Weapon Change 화면으로
	if (iconTarget[WEAPON_TAB]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[WEAPON_TAB]))
	{
		listenerStatus->setEnabled(false);

		//(현재무기 탭) 메뉴 바 약간좌측으로 '감추는' 액션(spr_icon_weapon)
		iconTarget[WEAPON_TAB]->runAction(MoveBy::create(1.0f, Point(-winSize().width*0.6f, 0)));
		iconTarget[SWORD]->runAction(FadeIn::create(1.2f));
		iconTarget[BAT]->runAction(FadeIn::create(1.2f));
		iconTarget[BACK]->runAction(FadeOut::create(1.2f));
		this->runAction(Sequence::create(DelayTime::create(1.5f),
			CallFunc::create([&]() {listenerWpChange->setEnabled(true); }), nullptr));
	}
	// -> 다시 Lobby로
	else if (iconTarget[BACK]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[BACK]))
	{
		listenerStatus->setEnabled(false);
		listenerWpChange->setEnabled(false);

		getParent()->getEventDispatcher()->dispatchCustomEvent("ChangeToMainLobby");	
		
		eventList[InOutMotion]->playBox();
		iconTarget[WEAPON_TAB]
			->runAction(Sequence::create(
				MoveBy::create(1.0f, Point(-winSize().width*0.7f, 0))
				, Place::create(Point(-winSize().width * 0.8f, winSize().height * 0.8f))
				, CallFunc::create([&]() {this->removeFromParent(); })
				, nullptr));	
	}

	iconTouchID[WEAPON_TAB] = -1;
	iconTouchID[BACK] = -1;
}

bool StatusLobby::onTouchBegan_WpChange(Touch* touch, Event* unused_event)
{
	Point touchPos = touch->getLocation();

	if (iconTarget[WEAPON_TAB]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[WEAPON_TAB] = touch->getID();
		return true;
	}
	if (iconTarget[SWORD]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[SWORD] = touch->getID();
		return true;
	}
	if (iconTarget[BAT]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[BAT] = touch->getID();
		return true;
	}
	return false;
}
void StatusLobby::onTouchEnded_WpChange(Touch* touch, Event *unused_event)
{
	Point touchPos = touch->getLocation();

	if (iconTarget[WEAPON_TAB]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[WEAPON_TAB]))
	{
		//(현재무기 탭) 메뉴 바 약간좌측으로 '감추는' 액션(spr_icon_weapon)
		iconTarget[WEAPON_TAB]->runAction(MoveBy::create(1.0f, Point(winSize().width*0.6f, 0)));
		iconTarget[SWORD]->runAction(FadeOut::create(1.2f));
		iconTarget[BAT]->runAction(FadeOut::create(1.2f));
		iconTarget[BACK]->runAction(FadeIn::create(1.2f));

		listenerWpChange->setEnabled(false);
		this->runAction(Sequence::create(DelayTime::create(1.2f),
			CallFunc::create([&]() {listenerStatus->setEnabled(true); }), nullptr));
	}
}




///////////////////////////////////////////////////////////////////////////
/****************************Record Layer*********************************/
///////////////////////////////////////////////////////////////////////////

