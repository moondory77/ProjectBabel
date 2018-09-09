#include "System/GameManager.h"
#include "System/BGManager.h"
#include "base/CCEventDispatcher.h"
#include <string.h>
#include "_Scene_Record.h"


Scene* Record::createScene()
{
	return Record::create();
}


bool Record::init()
{
	if (!Scene::init())
	{
		return false;
	}

	_eventDispatcher->removeAllEventListeners();

	listener_record = EventListenerTouchOneByOne::create();
	listener_record->setSwallowTouches(true);
	listener_record->onTouchBegan = CC_CALLBACK_2(Record::onTouchBegan_Record, this);
	listener_record->onTouchEnded = CC_CALLBACK_2(Record::onTouchEnded_Record, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener_record, 1);

	initBG();
	initIcon();

	BGManager::getInstance()->loadBG();
	this->addChild(BGManager::getInstance()->getParallax(), 1);

	/*
	Menu_Character::getInstance()->initMenu_Character();
	this->addChild(Menu_Character::getInstance()->getLayer(), 5);

	auto eve_cur = Menu_Character::getInstance()->getList()->scanBoxCur();
	eve_cur->setBoxTarget(Menu_Character::getInstance()->getTarget());
	eve_cur->setEvent();
	eve_cur->startEvent();
	*/

	return true;
}

void Record::initBG()
{

	//** #2 언덕 배경 layer
	layer_bg_hill = LayerColor::create(Color4B::WHITE);
	layer_bg_hill->setAnchorPoint(Point::ZERO);
	layer_bg_hill->setPosition(Point::ZERO);
	layer_bg_hill->setOpacity(0);


	spr_bg_hill = Sprite::create("hill.png");
	spr_bg_hill->setScale(DivForHorizontal(spr_bg_hill), DivForVertical(spr_bg_hill)*0.6f);
	spr_bg_hill->setAnchorPoint(Point::ZERO);
	//spr_bg_hill->setRotation(25);
	spr_bg_hill->setPosition(Point::ZERO);
	layer_bg_hill->addChild(spr_bg_hill);
	
	this->addChild(layer_bg_hill, 2);


	//** #3 grid 레이어 생성
	layer_grid = LayerColor::create(Color4B::WHITE);
	layer_grid->setAnchorPoint(Point::ZERO);
	layer_grid->setOpacity(0);

	auto spr_grid = Sprite::create("grid.png");
	spr_grid->setScale(DivForHorizontal(spr_grid)*2.0f, DivForVertical(spr_grid));
	spr_grid->setAnchorPoint(Point::ZERO);
	spr_grid->setPosition(Point::ZERO);

	layer_grid->addChild(spr_grid);
	//this->addChild(layer_grid, 3);
}

void Record::initIcon()
{
	//** icon_layer: z-order 3

	//** #1 아이콘 layer 생성
	layer_icon = LayerColor::create(Color4B::WHITE);
	layer_icon->setAnchorPoint(Point::ZERO);
	layer_icon->setOpacity(0);
	this->addChild(layer_icon, 3);


	//** start 아이콘 스프라이트 생성해서 배치
	spr_icon_start = Sprite::create("layout/main/start.png");
	spr_icon_start->setScale(DivForHorizontal(spr_icon_start)*0.7f);
	spr_icon_start->setAnchorPoint(Point(0.5, 0));
	//icon_start->setRotation(45);
	spr_icon_start->setPosition(Point(winSize().width*0.5f, 0));
	layer_icon->addChild(spr_icon_start);


	//** Record 아이콘 스프라이트 생성해서 배치
	spr_icon_record = Sprite::create("layout/main/record_over.png");
	spr_icon_record->setScale(DivForHorizontal(spr_icon_record)*0.25f);
	spr_icon_record->setAnchorPoint(Point(0, 0.5));
	//icon_start->setRotation(45);
	spr_icon_record->setPosition(Point(0, winSize().height*0.25f));
	layer_icon->addChild(spr_icon_record);


	//** Character 아이콘 스프라이트 생성해서 배치
	spr_icon_character = Sprite::create("layout/main/character.png");
	spr_icon_character->setScale(DivForHorizontal(spr_icon_character)*0.25f);
	spr_icon_character->setAnchorPoint(Point(0, 0.5));
	//icon_start->setRotation(45);
	spr_icon_character->setPosition(Point(0, winSize().height*0.4f));
	layer_icon->addChild(spr_icon_character);


	//** Setting 아이콘 스프라이트 생성해서 배치
	spr_icon_setting = Sprite::create("layout/main/setting.png");
	spr_icon_setting->setScale(DivForHorizontal(spr_icon_setting)*0.25f);
	spr_icon_setting->setAnchorPoint(Point(1, 0.5));
	//icon_start->setRotation(45);
	spr_icon_setting->setPosition(Point(winSize().width, winSize().height*0.3f));
	layer_icon->addChild(spr_icon_setting);
}


bool Record::onTouchBegan_Record(Touch* touch, Event *unused_event)
{
	if (!is_popup_on)
	{
		return true;
	}
	return false;
}


void Record::onTouchEnded_Record(Touch* touch, Event *unused_event)
{
	Point touchPoint = touch->getLocation();

	if (!is_popup_on)
	{
		if (spr_icon_record->getBoundingBox().containsPoint(touchPoint))
		{
			Director::getInstance()->replaceScene(Lobby::createScene());
		}
	}
}

