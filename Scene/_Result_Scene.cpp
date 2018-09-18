#include "_Result_Scene.h"
#include "System/GameManager.h"
#include "System/BGManager.h"
#include "Event/Events.h"


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
	eventList[InOutMotion] = new Result_InOutMotion();

	_eventDispatcher->removeAllEventListeners();
	initBG();
	initUI();

	eventList[InOutMotion]->initThread({ iconTarget[HERO] });
	eventList[InOutMotion]->playBox();
	iconTarget[RESULT_LOGO]->runAction(FadeIn::create(2.0f));
	iconTarget[NEXT]->runAction(FadeIn::create(2.0f));

	//** Result 씬에 대한 리스너 생성 및 등록
	listenerResult = EventListenerTouchOneByOne::create();
	listenerResult->setSwallowTouches(true);
	listenerResult->onTouchBegan = CC_CALLBACK_2(Result::onTouchBegan_Result, this);
	listenerResult->onTouchEnded = CC_CALLBACK_2(Result::onTouchEnded_Result, this);
	_eventDispatcher->addEventListenerWithFixedPriority(listenerResult, 1);
	listenerResult->setEnabled(false);

	this->runAction(Sequence::create(
		DelayTime::create(2.1f), 
		CallFunc::create([&]() {listenerResult->setEnabled(true); }), 
		nullptr));

	return true;
}



void Result::initBG()
{
	// 진도암시 배경. hill 배경 셋팅
	BGManager::getInstance()->loadBG();
	this->addChild(BGManager::getInstance()->getParallax());

	auto spr_black_back = Sprite::create("layout/on_ready/bg_black.png");
	spr_black_back->setScale(DivForHorizontal(spr_black_back), DivForVertical(spr_black_back));
	spr_black_back->setAnchorPoint(Point::ZERO);
	spr_black_back->setPosition(Point::ZERO);
	spr_black_back->setOpacity(100);
	this->addChild(spr_black_back);

	auto spr_hill = Sprite::create("layout/result/hill.png");
	spr_hill->setScale(DivForHorizontal(spr_hill)*1.2f, DivForVertical(spr_hill)*0.45f);
	spr_hill->setAnchorPoint(Point::ZERO);
	//spr_bg_hill->setRotation(2);
	spr_hill->setPosition(Point(-winSize().width*0.2f, 0));
	this->addChild(spr_hill);

	iconTarget[HERO] = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("walk_left_5.png"));
	iconTarget[HERO]->setScale(DivForHorizontal(iconTarget[HERO])*0.2f);
	iconTarget[HERO]->setAnchorPoint(Point(0.5, 0));
	iconTarget[HERO]->setPosition(winSize().width*1.1f, winSize().height*0.04f);
	this->addChild(iconTarget[HERO]);
}


void Result::initUI()
{
	subLayer = Layer::create();
	subLayer->setAnchorPoint(Point::ZERO);
	this->addChild(subLayer);

	iconTarget[RESULT_LOGO] = Sprite::create("layout/result/result_logo.png");
	iconTarget[RESULT_LOGO]->setScale(DivForHorizontal(iconTarget[RESULT_LOGO])*0.6f, DivForVertical(iconTarget[RESULT_LOGO])*0.13f);
	iconTarget[RESULT_LOGO]->setPosition(winSize().width*0.5f, winSize().height*0.8f);
	iconTarget[RESULT_LOGO]->setOpacity(0);
	subLayer->addChild(iconTarget[RESULT_LOGO]);

	iconTarget[NEXT] = Sprite::create("layout/result/confirm.png");
	iconTarget[NEXT]->setScale(DivForHorizontal(iconTarget[NEXT])*0.3f);
	iconTarget[NEXT]->setAnchorPoint(Point(0.5, 0));
	iconTarget[NEXT]->setPosition(Point(winSize().width*0.5f, winSize().height*0.3f));
	iconTarget[NEXT]->setOpacity(0);
	subLayer->addChild(iconTarget[NEXT]);
}


bool Result::onTouchBegan_Result(Touch* touch, Event *unused_event)
{
	Point touchPos = touch->getLocation();

	//터치 시, 터치된 아이콘의 id 획득 후 이벤트 넘긴다
	if (iconTarget[NEXT]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[NEXT] = touch->getID();
		return true;
	}
	return false;
}


void Result::onTouchEnded_Result(Touch* touch, Event *unused_event)
{
	Point touchPos = touch->getLocation();

	if (iconTarget[NEXT]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[NEXT]))
	{
		this->eventList[InOutMotion]->playBox();
		listenerResult->setEnabled(false);
		iconTarget[NEXT]->runAction(Sequence::create(FadeOut::create(2.0f), nullptr));
	}
}
