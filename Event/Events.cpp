#include "Events.h"
#include "System/GameManager.h"



void MainLobby_InOutMotion::initThread(initializer_list<Sprite*> targets)
{
	// 0 - hero, 1 - target_logo
	setTarget(targets);

	//#1 - ĳ���� �ɾ� �ö����..
	pushBox(new EventBox([&]()
	{
		Vector<SpriteFrame*> anim_frame;
		for (int i = 0; i < 5; i++) {
			anim_frame.pushBack(SpriteFrameCache::getInstance()
				->getSpriteFrameByName(StringUtils::format("walk_up_%d.png", i)));
		}
		auto animation = Animation::createWithSpriteFrames(anim_frame);
		animation->setDelayPerUnit(0.13f);
		auto walk_motion = RepeatForever::create(Animate::create(animation));
		auto up = MoveBy::create(3.3f, Point(0, winSize().height*0.7));

		targetList[HERO]->runAction(walk_motion);
		targetList[HERO]->runAction(
			Sequence::create(up,
				CallFunc::create([&]() {
			targetList[HERO]->stopAllActions();
			targetList[HERO]->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("default_back.png"));
			moveCurNext();
		}), nullptr));
	}));


	//#2 - ĳ���� �ɾ� ��������..
	pushBox(new EventBox([&]()
	{
		Vector<SpriteFrame*> anim_frame;
		for (int i = 0; i < 5; i++) {
			anim_frame.pushBack(SpriteFrameCache::getInstance()
				->getSpriteFrameByName(StringUtils::format("walk_down_%d.png", i + 1)));
		}
		auto animation = Animation::createWithSpriteFrames(anim_frame);
		animation->setDelayPerUnit(0.13f);
		auto walk_motion = Repeat::create(Animate::create(animation), 6);
		auto down = MoveBy::create(3.3f, Point(0, -winSize().height*0.7));
		auto walk_down = Spawn::create(walk_motion, down, nullptr);

		targetList[HERO]->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("walk_down_1.png"));
		targetList[HERO]->runAction(Sequence::create(
			DelayTime::create(0.3f), 
			walk_down,
			CallFunc::create([&]() 
		{
			targetList[HERO]->stopAllActions();
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, OnReady::createScene()));
		}),
			nullptr));
	}));

	moveCurNext();	//��ȿ�� ù��° �̺�Ʈ�ڽ��� Ŀ���̵�
}


void StatusLobby_InOutMotion::initThread(initializer_list<Sprite*> targets)
{
	// 0 - hero, 1 - sword, 2 - bat
	setTarget(targets);

	//#1 - �� �ֵθ� Intro
	pushBox(
		new EventBox([&]()
	{
		Vector<SpriteFrame*> anim_frame;

		for (int i = 0; i < 8; i++) {
			anim_frame.pushBack(SpriteFrameCache::getInstance()
				->getSpriteFrameByName(StringUtils::format("menu_character_%d.png", i + 1)));
		}

		auto animation = Animation::createWithSpriteFrames(anim_frame);
		animation->setDelayPerUnit(0.09f);
		auto delay = DelayTime::create(1.5f);
		auto act = Sequence::create(delay, Animate::create(animation),
			CallFunc::create([&]() { moveCurNext(); }), nullptr);
		targetList[HERO]->runAction(act);
	}));

	//#2 - �� ������ outro
	pushBox(new EventBox([&]()
	{
		Vector<SpriteFrame*> anim_frame;
		for (int i = 0; i < 8; i++) {
			anim_frame.pushBack(SpriteFrameCache::getInstance()
				->getSpriteFrameByName(StringUtils::format("menu_character_%d.png", i + 1)));
		}

		auto animation = Animation::createWithSpriteFrames(anim_frame);
		animation->setDelayPerUnit(0.09f);

		auto act = Sequence::create(
			DelayTime::create(0.7f),
			Animate::create(animation)->reverse(),
			DelayTime::create(0.5f),
			CallFunc::create([&]()
		{
			targetList[HERO]->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("default_back.png"));
			moveCurPrev();
		}),
			NULL);
		targetList[HERO]->runAction(act);
	}));

	moveCurNext();	//��ȿ�� ù��° �̺�Ʈ�ڽ��� Ŀ���̵�
}




