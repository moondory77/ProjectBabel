#include "Events.h"
#include "System/GameManager.h"


void MainLobby_InOutMotion::initThread(initializer_list<Sprite*> targets)
{
	// 0 - hero, 1 - target_logo
	setTarget(targets);

	//#1 - 캐릭터 걸어 올라오는..
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


	//#2 - 캐릭터 걸어 내려가는..
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

		targetList[HERO]->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("walk_down_0.png"));
		targetList[HERO]->runAction(Sequence::create(
			DelayTime::create(0.3f),
			walk_down,
			CallFunc::create([&]()
		{
			targetList[HERO]->stopAllActions();
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, OnReady::createScene()));	//onReady 화면으로
		}),
			nullptr));
	}));

	moveCurNext();	//유효한 첫번째 이벤트박스로 커서이동
}


void StatusLobby_InOutMotion::initThread(initializer_list<Sprite*> targets)
{
	// 0 - hero, 1 - sword, 2 - bat
	setTarget(targets);

	//#1 - 검 Swing Intro
	pushBox(new EventBox([&]()
	{
		Vector<SpriteFrame*> anim_frame;
		for (int i = 0; i < 9; i++) {
			anim_frame.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("WeaponSwing_%d.png", i+1)));
		}
		auto animation = Animation::createWithSpriteFrames(anim_frame);
		animation->setDelayPerUnit(0.04f);
		auto act = Sequence::create(DelayTime::create(1.2f), Animate::create(animation), nullptr);
		targetList[HERO]->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("WeaponSwing_0.png"));
		targetList[HERO]->runAction(act);

		//원 rotate 액션 정의
		auto circle_rot_1 = RotateTo::create(0, 45.0f);	//시작 시, 원 각도 약간 젖히기
		auto circle_rot_2 = RotateBy::create(0.12f, -90.0f);
		auto circle_rot_3 = RotateBy::create(0.2f, -223.0f);
		auto circle_sequence = Sequence::create(DelayTime::create(1.2f), circle_rot_1, circle_rot_2, circle_rot_3, nullptr);
		targetList[HERO_CIRCLE]->runAction(circle_sequence);

		//무기 rotate 액션 정의
		auto wp_rot_1 = RotateTo::create(0, 6.0f);
		auto wp_rot_2 = RotateBy::create(0.12f, 74.0f);
		auto wp_rot_3 = RotateBy::create(0.2f, 120.0f);
		auto wp_rotation = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3, nullptr);
	
		auto max_wp_size = 0.16f * DivForHorizontal(targetList[HERO_WEAPON]) / targetList[HERO]->getScale();
		auto wp_scale_1 = ScaleTo::create(0.12f, max_wp_size *0.3f);
		auto wp_scale_2 = ScaleTo::create(0.2f, max_wp_size);
		auto wp_scaling = Sequence::create(wp_scale_1, wp_scale_2, nullptr);

		auto wp_swing = Spawn::create(wp_rotation, wp_scaling, nullptr);
		auto wp_sequence 
			= Sequence::create(DelayTime::create(1.2f), 
				CallFunc::create([&]()
		{
			targetList[HERO_WEAPON]->setOpacity(255);
			targetList[HERO_WEAPON]->setScale(0.2f);
		}), wp_swing, nullptr);
		
		targetList[HERO_WEAPON]->runAction(wp_sequence);
		moveCurNext();
	}));


	//#2 - 검 갈무리 outro
	pushBox(new EventBox([&]()
	{
		Vector<SpriteFrame*> anim_frame;
		for (int i = 0; i < 9; i++) {
			anim_frame.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("WeaponSwing_%d.png", i + 1)));
		}
		auto animation = Animation::createWithSpriteFrames(anim_frame);
		animation->setDelayPerUnit(0.04f);
		auto act = Sequence::create(
			Animate::create(animation)->reverse(),
			DelayTime::create(1.2f),
			CallFunc::create([&]()
		{
			targetList[HERO_WEAPON]->setOpacity(0);
			targetList[HERO]->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("default_back.png"));
		}), nullptr);
		targetList[HERO]->runAction(act);

		//원 rotate액션 정의
		auto circle_rot_1 = RotateBy::create(0.12f, -90.0f);
		auto circle_rot_2 = RotateBy::create(0.2f, -223.0f);
		auto circle_sequence = Sequence::create(circle_rot_1, circle_rot_2, nullptr)->reverse();
		targetList[HERO_CIRCLE]->runAction(circle_sequence);

		//무기 rotate 액션 정의
		auto wp_rot_1 = RotateBy::create(0.12f, 74.0f);
		auto wp_rot_2 = RotateBy::create(0.2f, 120.0f);
		auto wp_rotation = Sequence::create(wp_rot_1, wp_rot_2, nullptr)->reverse();	
	
		auto wp_scale_1 = ScaleTo::create(0.12f, 2.0f);
		auto wp_scale_2 = ScaleTo::create(0.2f, 1.4f);
		auto wp_scaling = Sequence::create(wp_scale_1, wp_scale_2, nullptr);
	
		auto wp_sequence = Spawn::create(wp_rotation, wp_scaling, nullptr);
		targetList[HERO_WEAPON]->runAction(wp_sequence);
	
		moveCurPrev();
	}));

	moveCurNext();	//유효한 첫번째 이벤트박스로 커서이동
}
void StatusLobby_WeaponChange::initThread(initializer_list<Sprite*> targets)
{
	// 0 - hero, 1 - hero_circle, 2 - hero_weapon
	setTarget(targets);

	//#1 - Swing Motion
	pushBox(
		new EventBox([&]()
	{

		Vector<SpriteFrame*> anim_frame;
		for (int i = 0; i < 9; i++) {
			anim_frame.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("WeaponSwing_%d.png", i + 1)));
		}
		auto animation = Animation::createWithSpriteFrames(anim_frame);
		animation->setDelayPerUnit(0.04f);
		targetList[HERO]->runAction(Animate::create(animation));

		//원 rotate 액션 정의
		auto circle_rot_1 = RotateTo::create(0, 45.0f);	//시작 시, 원 각도 약간 젖히기
		auto circle_rot_2 = RotateBy::create(0.12f, -90.0f);
		auto circle_rot_3 = RotateBy::create(0.2f, -223.0f);
		auto circle_sequence = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3, nullptr);
		targetList[HERO_CIRCLE]->runAction(circle_sequence);

		//무기 rotate 액션 정의
		auto wp_rot_1 = RotateTo::create(0, 6.0f);
		auto wp_rot_2 = RotateBy::create(0.12f, 74.0f);
		auto wp_rot_3 = RotateBy::create(0.2f, 120.0f);
		auto wp_rotation = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3, nullptr);

		auto max_wp_size = 0.16f * DivForHorizontal(targetList[HERO_WEAPON]) / targetList[HERO]->getScale();
		auto wp_scale_1 = ScaleTo::create(0.12f, max_wp_size * 0.3f);
		auto wp_scale_2 = ScaleTo::create(0.2f, max_wp_size);
		auto wp_scaling = Sequence::create(wp_scale_1, wp_scale_2, nullptr);

		auto wp_swing = Spawn::create(wp_rotation, wp_scaling, nullptr);
		auto wp_sequence = Sequence::create(
			CallFunc::create([&]()
		{
			targetList[HERO_WEAPON]->setOpacity(255);
			targetList[HERO_WEAPON]->setScale(0.2f);
		}), wp_swing, nullptr);

		targetList[HERO_WEAPON]->runAction(wp_sequence);
	}));

	moveCurNext();
}



void OnReady_InOutMotion::initThread(initializer_list<Sprite*> targets)
{
	// 0 - hero
	setTarget(targets);

	// #1 - 씬 진입 시, 캐릭터 왼쪽에서 천천히 걸어 나오는 모션
	pushBox(new EventBox([&]()
	{
		Vector<SpriteFrame*> anim_frame;
		for (int i = 0; i < 6; i++) {
			anim_frame.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("walk_right_%d.png", i)));
		}
		auto animation = Animation::createWithSpriteFrames(anim_frame);
		animation->setDelayPerUnit(0.17f);
		auto walk_motion = Repeat::create(Animate::create(animation), 2);
		auto right = MoveBy::create(2.0f, Point(winSize().width*0.34f, 0));
		auto walk_right = Spawn::create(walk_motion, right, nullptr);
		targetList[HERO]->runAction(
			Sequence::create(walk_right, CallFunc::create([&]() { moveCurNext(); }), nullptr));
	}));


	// #2 - 캐릭터 오른쪽으로 빠르게 걸어나가는 모션
	pushBox(new EventBox([&]()
	{
		Vector<SpriteFrame*> anim_frame;
		for (int i = 0; i < 6; i++) {
			anim_frame.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("walk_right_%d.png", i)));
		}
		auto animation = Animation::createWithSpriteFrames(anim_frame);
		animation->setDelayPerUnit(0.11f);
		auto walk_motion = RepeatForever::create(Animate::create(animation));
		targetList[HERO]->runAction(walk_motion);

		auto move_right = MoveBy::create(0.822f, Point(winSize().width*0.24f, 0));
		auto move_diagonal = MoveBy::create(2.058f, Point(winSize().width*0.62f, -winSize().height*0.3f));
		auto total_move = Sequence::create(move_right, move_diagonal, NULL);

		targetList[HERO]->runAction(Sequence::create(total_move,
			CallFunc::create([&]()
		{
			targetList[HERO]->stopAllActions();
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, OnGame::createScene()));
		}), nullptr));

	}));

	moveCurNext();	//유효한 첫번째 이벤트박스로 커서이동


}



void Result_InOutMotion::initThread(initializer_list<Sprite*> targets)
{
	// 0 - hero
	setTarget(targets);

	// #1 - 씬 진입 시, 캐릭터 오른쪽에서 천천히 걸어 나오는 모션
	pushBox(new EventBox([&]()
	{
		Vector<SpriteFrame*> anim_frame;
		for (int i = 0; i < 6; i++) {
			anim_frame.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("walk_left_%d.png", i)));
		}
		auto animation = Animation::createWithSpriteFrames(anim_frame);
		animation->setDelayPerUnit(0.17f);
	
		auto walk_motion = Repeat::create(Animate::create(animation), 2);
		auto left = MoveBy::create(2.1f, Point(-winSize().width*0.28f, 0));
		auto walk_left = Spawn::create(walk_motion, left, nullptr);
		targetList[HERO]->runAction(
			Sequence::create(walk_left, CallFunc::create([&]() { moveCurNext(); }), nullptr));
	}));

	// #2 - 빠르게 왼쪽으로 언덕 걸어올라가며 로비화면으로..
	pushBox(new EventBox([&]()
	{
		Vector<SpriteFrame*> anim_frame;
		for (int i = 0; i < 6; i++) {
			anim_frame.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("walk_left_%d.png", i)));
		}
		auto animation = Animation::createWithSpriteFrames(anim_frame);
		animation->setDelayPerUnit(0.18f);

		auto walk_motion = RepeatForever::create(Animate::create(animation));
		targetList[HERO]->runAction(walk_motion);

		auto move_left_start = MoveBy::create(0.8f, Point(-winSize().width*0.15f, 0));
		auto move_diagonal = MoveBy::create(3.0f, Point(-winSize().width*0.55f, winSize().height*0.3f));
		auto move_left_end = MoveBy::create(1.0f, Point(-winSize().width*0.2f, 0));
		auto total_move = Sequence::create(move_left_start, move_diagonal, move_left_end, nullptr);

		targetList[HERO]->runAction(Sequence::create(total_move,
			CallFunc::create([&]()
		{
			targetList[HERO]->stopAllActions();
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, Lobby::createScene()));
		}), nullptr));

	}));

	moveCurNext();	//유효한 첫번째 이벤트박스로 커서이동
}
void Result_Grading::initThread(initializer_list<Sprite*> targets) 
{
	// 0 - grade
	setTarget(targets);

	// #1 - 씬 진입 시, 캐릭터 오른쪽에서 천천히 걸어 나오는 모션
	pushBox(new EventBox([&]()
	{
		////** 액션0 - 해당 스테이지 결과 등급을 보여주는 액션
		//auto expand = ScaleBy::create(3.0f, 3.0f);
		//auto fade_in = FadeIn::create(2.6f);
		//auto resulting = Spawn::create(expand, fade_in, NULL);

		////** 액션1 - 확인 버튼을 띄워주는 액션
		//auto delay_confirm = DelayTime::create(2.5f);
		//auto fade_in_confirm = FadeTo::create(2.0f, 200);
		//auto act_confirm = Sequence::create(delay_confirm, fade_in_confirm, NULL);

		//targetList[GRADE] = Sprite::create("layout/result/grade_s.png");
		//targetList[GRADE]->setScale(DivForHorizontal(targetList[GRADE])*0.25f);
		//targetList[GRADE]->setOpacity(0);
		//targetList[GRADE]->setPosition(Point(winSize().width*0.5f, winSize().height*0.55f));
		//layer_menu_result->addChild(spr_icon_grade);
	}));

	moveCurNext();
}
