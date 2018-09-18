//#include "Util/LabelTypingEffect.h"
#include "System/GameManager.h"
#include "System/BGManager.h"
#include "_OnReady_Scene.h"
#include "Event/Events.h"


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
	eventList[InOutMotion] = new OnReady_InOutMotion();

	//이전 등록된 이벤트 리스너 모두 해제
	_eventDispatcher->removeAllEventListeners();
	initBG();
	initUI();
	
	eventList[InOutMotion]->initThread({ iconTarget[HERO] });
	eventList[InOutMotion]->playBox();
	iconTarget[START]->runAction(FadeIn::create(2.0f));

	// OnReady씬 리스너를 생성, 등록
	listenerOnReady = EventListenerTouchOneByOne::create();
	listenerOnReady->setSwallowTouches(true);
	listenerOnReady->onTouchBegan = CC_CALLBACK_2(OnReady::onTouchBegan_OnReady, this);
	listenerOnReady->onTouchEnded = CC_CALLBACK_2(OnReady::onTouchEnded_OnReady, this);
	_eventDispatcher->addEventListenerWithFixedPriority(listenerOnReady, 1);
	listenerOnReady->setEnabled(false);

	this->runAction(Sequence::create(
		DelayTime::create(2.1f), 
		CallFunc::create([&]() {listenerOnReady->setEnabled(true); }), 
		nullptr));

	return true;
}


void OnReady::initBG()
{
	BGManager::getInstance()->loadBG();
	this->addChild(BGManager::getInstance()->getParallax());

	auto spr_black_back = Sprite::create("layout/on_ready/bg_black.png");
	spr_black_back->setScale(DivForHorizontal(spr_black_back), DivForVertical(spr_black_back));
	spr_black_back->setAnchorPoint(Point::ZERO);
	spr_black_back->setPosition(Point::ZERO);
	spr_black_back->setOpacity(100);
	this->addChild(spr_black_back);
	
	auto spr_hill = Sprite::create("layout/on_ready/hill_2.png");
	spr_hill->setScale(DivForHorizontal(spr_hill), DivForVertical(spr_hill));
	spr_hill->setAnchorPoint(Point::ZERO);
	spr_hill->setPosition(Point(0, -winSize().height*0.12));
	this->addChild(spr_hill);

	iconTarget[HERO] = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("walk_right_5.png"));
	iconTarget[HERO]->setScale(DivForHorizontal(iconTarget[HERO])*0.2f);
	iconTarget[HERO]->setAnchorPoint(Point(0.5, 0));
	iconTarget[HERO]->setPosition(-winSize().width*0.1f, winSize().height*0.34f);
	this->addChild(iconTarget[HERO]);
}


void OnReady::initUI()
{
	subLayer = Layer::create();
	subLayer->setAnchorPoint(Point::ZERO);
	this->addChild(subLayer);

	iconTarget[START] = Sprite::create("layout/on_ready/start.png");
	iconTarget[START]->setScale(DivForHorizontal(iconTarget[START])*0.25f, DivForVertical(iconTarget[START])*0.085f);
	iconTarget[START]->setPosition(winSize().width*0.5f, winSize().height*0.1f);
	iconTarget[START]->setOpacity(0);
	subLayer->addChild(iconTarget[START]);
}


bool OnReady::onTouchBegan_OnReady(Touch* touch, Event *unused_event)
{
	Point touchPos = touch->getLocation();

	if (iconTarget[START]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[START] = touch->getID();
		return true;
	}
	return false;
}


void OnReady::onTouchEnded_OnReady(Touch* touch, Event* unused_event)
{
	Point touchPos = touch->getLocation();

	if (iconTarget[START]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[START]))
	{
		this->eventList[InOutMotion]->playBox();
		listenerOnReady->setEnabled(false);
		iconTarget[START]->runAction(Sequence::create(FadeOut::create(2.0f), nullptr));
	}
}







