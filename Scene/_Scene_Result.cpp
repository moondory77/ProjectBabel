#include "System/GameManager.h"
#include "System/BGManager.h"
#include "Scene/_Scene_Result.h"
#include "Scene/_Scene_Lobby.h"
#include "Plot/EventPlayer.h"
#include "Plot/EventBox.h"
//#include "LabelTypingEffect.h"
#include "base/CCEventDispatcher.h"
#include <string.h>

USING_NS_CC;

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

	_eventDispatcher->removeAllEventListeners();

	//** Result ���� ���� ������ ���� �� ���
	listener_result = EventListenerTouchOneByOne::create();
	listener_result->setSwallowTouches(true);
	listener_result->onTouchBegan = CC_CALLBACK_2(Result::onTouchBegan_Result, this);
	listener_result->onTouchEnded = CC_CALLBACK_2(Result::onTouchEnded_Result, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener_result, 1);
	///�ӽ÷� �켱���� 1

	// �����Ͻ� ���. hill ��� ����
	BGManager::getInstance()->loadBG();
	this->addChild(BGManager::getInstance()->getParallax());
	initBG();

	// EventPlayer �ʱ�ȭ��Ű��, ���� ���̾�� ����
	MenuResult::getInstance()->initMenuResult();
	cur_list = MenuResult::getInstance()->getList();
	this->addChild(MenuResult::getInstance()->getLayer(), 5);

	//3.2�� ���� �����ʸ� ��ŵ� 
	this->runAction(EventManager::getInstance()->getListenerSleep(3.2f, listener_result));
	//0.8�� �� ��Ʈ�� �̺�Ʈ ����(��ũ ���߱� ����)
	this->scheduleOnce(schedule_selector(Result::initScheduleCallback), 0.8f);

	return true;
}

void Result::initScheduleCallback(float delta)
{
	cur_list->moveCurNext();	//Menu_Result_Intro_0
	cur_list->scanCur()->setEvent();
	cur_list->scanCur()->startEvent();
}

void Result::initBG()
{
	//** #1 ��� ��淹�̾�
	layer_bg_hill = LayerColor::create(Color4B::WHITE);
	layer_bg_hill->setAnchorPoint(Point::ZERO);
	layer_bg_hill->setPosition(Point::ZERO);
	layer_bg_hill->setOpacity(0);
	this->addChild(layer_bg_hill);

	//** #2 ���� ������� �ణ �������� ���̾�
	spr_bg_back = Sprite::create("layout/result/bg_black.png");
	spr_bg_back->setScale(DivForHorizontal(spr_bg_back), DivForVertical(spr_bg_back));
	//spr_bg_back->setAnchorPoint(Point::ZERO);
	spr_bg_back->setPosition(Point(winSize().width*0.5f, winSize().height*0.5f));
	spr_bg_back->setOpacity(100);
	layer_bg_hill->addChild(spr_bg_back);

	//** #3 ��� ��������Ʈ
	spr_bg_hill = Sprite::create("layout/result/hill.png");
	spr_bg_hill->setScale(DivForHorizontal(spr_bg_hill)*1.2f, DivForVertical(spr_bg_hill)*0.45f);
	spr_bg_hill->setAnchorPoint(Point::ZERO);
	//spr_bg_hill->setRotation(2);
	spr_bg_hill->setPosition(Point(-winSize().width*0.2f, 0));
	layer_bg_hill->addChild(spr_bg_hill);
}


bool Result::onTouchBegan_Result(Touch* touch, Event *unused_event)
{
	return true;
}
void Result::onTouchEnded_Result(Touch* touch, Event *unused_event)
{
	Point touchPoint = touch->getLocation();

	//��� ��� �߱� ���̸�,
	if (!is_confirm_on)
	{
		cur_list->moveCurTo(2);	//Menu_Result_Grading_0
		cur_list->scanCur()->setEvent();
		cur_list->scanCur()->startEvent();
		
		//�̺�Ʈ ����, ������ 3�� ���
		this->runAction(EventManager::getInstance()->getListenerSleep(3.0f, listener_result));
		is_confirm_on = true;
	}
	//��� ��� �� �ĸ�,
	else
	{
		if (MenuResult::getInstance()->getConfirmTarget()->getBoundingBox().containsPoint(touchPoint))
		{
			cur_list->moveCurTo(3);	//Menu_Result_Outro_0
			cur_list->scanCur()->setEvent();
			cur_list->scanCur()->startEvent();

			_eventDispatcher->removeAllEventListeners();
			EventManager::getInstance()->setFromResult(true);
			this->runAction(EventManager::getInstance()->sceneTransferAction(4.8f, LOBBY));
		}
	}

}
