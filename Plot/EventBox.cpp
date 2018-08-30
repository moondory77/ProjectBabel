#include "Scene/Scene_Lobby.h"
#include "Plot/EventBox.h"
#include "Plot/EventPlayer.h"
#include "Manager/GameManager.h"

using std::string;
USING_NS_CC;

//*************************** #Scene - Lobby	***************************************// 

// 로비화면의 각 아이콘 페이드인 (1.5초 소모)
void Menu_Lobby_Intro_0::setEvent()
{
	//** 액션0 - 타이틀 로고(extra_target)이 나타나는 액션
	auto act_extra = FadeIn::create(1.5f);
	//** 액션1 - start 아이콘 나타나는 액션
	auto act_icon_start = FadeIn::create(1.5f);
	//** 액션2 - record 아이콘 나타나는 액션
	auto act_icon_record = FadeIn::create(1.5f);
	//** 액션3 - character 아이콘 나타나는 액션
	auto act_icon_character = FadeIn::create(1.5f);
	//** 액션4 - setting 아이콘 나타나는 액션
	auto act_icon_setting = FadeIn::create(1.5f);

	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuLobby::getInstance()->getTitleTarget(), act_extra, 0);
	setTarget(MenuLobby::getInstance()->getStartTarget(), act_icon_start, 1);
	setTarget(MenuLobby::getInstance()->getRecordTarget(), act_icon_record, 2);
	setTarget(MenuLobby::getInstance()->getCharacterTarget(), act_icon_character, 3);
	setTarget(MenuLobby::getInstance()->getSettingTarget(), act_icon_setting, 4);
}

// 주인공 위로 걸어올라가는 (3.3초 소모)
void Menu_Lobby_Intro_1::setEvent()
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/for_events/walk_up/walk_up.plist");
	animation = Animation::create();
	animation->setDelayPerUnit(0.11);

	for (int i = 0; i < 5; i++)
	{
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName
		(StringUtils::format("walk_up_%d.png", i));
		animation->addSpriteFrame(frame);
	}
	animate = Animate::create(animation);

	auto walk = Repeat::create(animate, 6);
	/// 반복하는 걷기 애니메이션
	auto up = MoveBy::create(3.3f, Point(0, winSize().height*0.7));
	/// 위로 이동하는 액션																
	auto walk_up = Spawn::create(walk, up, NULL);
	///걸어서 위로 올라가는 액션
	animation = Animation::create();
	animation->setDelayPerUnit(0.1f);
	animation->addSpriteFrameWithFile("default_back.png");
	animate = Animate::create(animation);
	auto stand_back = Repeat::create(animate, 1);
	///등지고 선, 이미지 스프라이트로 교체 액션

	//** 액션0 - 주인공(target)이 위로 올라가는 액션. 3.3초
	auto comeback = Sequence::create(walk_up, stand_back, NULL);
	//** 이벤트 레이어 타겟에 연결
	setTarget(MenuLobby::getInstance()->getHeroTarget(), comeback, 0);
}

//로비화면 icon & logo 페이드 아웃
void Menu_Lobby_Outro_0::setEvent()
{
	//** 액션0 - 타이틀 로고(extra_target)이 좀 빠르게 사라지는 액션
	auto act_extra = FadeOut::create(1.5f);
	//** 액션1 - start 아이콘 사라지는 액션
	auto act_icon_start = FadeOut::create(3.5f);
	//** 액션2 - record 아이콘 사라지는 액션
	auto act_icon_record = FadeOut::create(3.5f);
	//** 액션3 - character 아이콘 사라지는 액션
	auto act_icon_character = FadeOut::create(3.5f);
	//** 액션4 - setting 아이콘 사라지는 액션
	auto act_icon_setting = FadeOut::create(3.5f);

	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuLobby::getInstance()->getTitleTarget(), act_extra, 0);
	setTarget(MenuLobby::getInstance()->getStartTarget(), act_icon_start, 1);
	setTarget(MenuLobby::getInstance()->getRecordTarget(), act_icon_record, 2);
	setTarget(MenuLobby::getInstance()->getCharacterTarget(), act_icon_character, 3);
	setTarget(MenuLobby::getInstance()->getSettingTarget(), act_icon_setting, 4);
}

//아래로 걸어내려가는 캐릭터액션 이벤트 (3.3초 소모)
void Menu_Lobby_Outro_1::setEvent()
{
	//** 정면 캐릭터이미지 잠시동안, 보여주는 애니메이션을 먼저 생성
	//** 프레임 1장으로 이루어짐
	animation = Animation::create();
	animation->setDelayPerUnit(0.01);
	animation->addSpriteFrameWithFile("motions/for_events/walk_down/walk_down_0.png");
	animate = Animate::create(animation);

	auto stand = Repeat::create(animate, 1);
	///sub action 1) - stand
	//** 걸어가는 캐릭터 스프라이트 프레임캐쉬 생성
	//** walk_down 애니메이션은 5개의 프레임으로 이루어졌음
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/for_events/walk_down/walk_down.plist");
	animation = Animation::create();
	animation->setDelayPerUnit(0.11);
	for (int i = 0; i < 5; i++)
	{
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName
		(StringUtils::format("walk_down_%d.png", i + 1));
		animation->addSpriteFrame(frame);
	}
	animate = Animate::create(animation);

	auto walk = Repeat::create(animate, 6);	/// 반복하는 걷기 애니메이션
	auto down = MoveBy::create(3.3f, Point(0, -winSize().height*0.7));	/// 아래로 이동하는 액션
	auto walk_down = Spawn::create(walk, down, NULL);
	auto delay = DelayTime::create(0.5f);

	//** 액션0 - 주인공(target)이 걸어 내려가는 액션. 딜레이 포함 3.8초  
	auto act = Sequence::create(stand, delay, walk_down, NULL);
	/// total act = stand + delay + walk_down을 시퀀스로 연결

	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuLobby::getInstance()->getHeroTarget(), act, 0);
}



//*************************** #Scene - CharacterInfo	**************************************// 
// 주인공 칼 휘두름 (2.22초 소모)
void Menu_Character_Intro_0::setEvent()
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/for_events/menu_cha/menu_character.plist");
	animation = Animation::create();
	animation->setDelayPerUnit(0.09);
	
	//** 검 휘두르는 모션 프레임들 animation 객체에 셋팅
	for (int i = 0; i < 8; i++)
	{
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName
		(StringUtils::format("menu_character_%d.png", i+1));
		animation->addSpriteFrame(frame);
	}
	animate = Animate::create(animation);	
	
	//** 액션 0 - 조금 딜레이 후, 검 휘두르는 액션 생성 (icon_hero 타겟)
	auto delay = DelayTime::create(1.5f);
	auto play = Repeat::create(animate, 1);
	auto act = Sequence::create(delay, play, NULL);

	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuCharacter::getInstance()->getHeroTarget(), act, 0);
}

// 무기, 뒤로가기 탭 페이드인
void Menu_Character_Intro_1::setEvent()
{
	//** 액션 1 - (현재무기 탭) 메뉴 바를 우측으로  '펼쳐주는' 액션 생성(spr_icon_weapon 타겟)
	auto unfold_menu = MoveBy::create(2.0f, Point(winSize().width*0.8f, 0));
	//** 액션 2
	auto act_back = FadeIn::create(2.0f);

	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuCharacter::getInstance()->getWeaponTarget(), unfold_menu, 0);
	setTarget(MenuCharacter::getInstance()->getBackTarget(), act_back, 1);
}

// 무기 탭 클릭 시. 1.2초 소요
void Menu_Character_WTab_0::setEvent()
{
	//** (현재무기 탭) 메뉴 바 약간좌측으로 '감추는' 액션(spr_icon_weapon)
	auto fold_menu = MoveBy::create(1.0f, Point(-winSize().width*0.6f, 0));
	auto act_sword = FadeIn::create(1.2f);
	auto act_bat = FadeIn::create(1.2f);
	auto act_back = FadeOut::create(1.2f);	///뒤로가기 버튼 없어지게..

										
	setTarget(MenuCharacter::getInstance()->getWeaponTarget(), fold_menu, 0);
	setTarget(MenuCharacter::getInstance()->getSwordTarget(), act_sword, 1);
	setTarget(MenuCharacter::getInstance()->getBatTarget(), act_bat, 2);
	setTarget(MenuCharacter::getInstance()->getBackTarget(), act_back, 3);
}

// 탭을 다시 눌렀을 때, 캐릭터화면 복귀 (1.2초 소모)
void Menu_Character_WTab_1::setEvent()
{
	//** (현재무기 탭) 메뉴 바를 다시 우측으로 '펼쳐주는' Reverse 액션(spr_icon_weapon)
	auto unfold_menu = MoveBy::create(1.0f, Point(winSize().width*0.6f, 0));
	auto act_sword = FadeOut::create(1.2f);
	auto act_bat = FadeOut::create(1.2f);
	auto act_back = FadeIn::create(1.2f);

	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuCharacter::getInstance()->getWeaponTarget(), unfold_menu, 0);
	setTarget(MenuCharacter::getInstance()->getSwordTarget(), act_sword, 1);
	setTarget(MenuCharacter::getInstance()->getBatTarget(), act_bat, 2);
	setTarget(MenuCharacter::getInstance()->getBackTarget(), act_back, 3);
}

//주인공 다시 칼 집어넣는 (1.7초 소모)
void Menu_Character_Outro_0::setEvent()
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/for_events/menu_cha/menu_character.plist");
	animation = Animation::create();
	animation->setDelayPerUnit(0.09);

	for (int i = 0; i < 8; i++)
	{
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName
		(StringUtils::format("menu_character_%d.png", 8-i));
		animation->addSpriteFrame(frame);
	}
	animate = Animate::create(animation);

	auto delay = DelayTime::create(1.0f);
	auto play = Repeat::create(animate, 1);
	auto act = Sequence::create(play, delay, NULL);	

	setTarget(MenuCharacter::getInstance()->getHeroTarget(), act, 0);
}
//무기, 뒤로가기 탭 페이드 아웃
void Menu_Character_Outro_1::setEvent()
{
	// 액션 2 - (현재무기 탭) 메뉴바를 다시 좌측으로 '감추는' Reverse 액션 (spr_icon_weapon 타겟)
	auto fold_menu = MoveBy::create(1.7f, Point(-winSize().width*0.8f, 0));
	// 액션 3
	auto act_back = FadeOut::create(1.7f);

	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuCharacter::getInstance()->getWeaponTarget(), fold_menu, 0);
	setTarget(MenuCharacter::getInstance()->getBackTarget(), act_back, 1);
}





//*************************** #Scene - OnReady	***************************************// 

//씬 진입 시, 캐릭터 왼쪽에서 천천히 걸어 나오는 모션 (2.16초 소모)
void Menu_OnReady_Intro_0::setEvent()
{

	SpriteFrameCache::getInstance()
						->addSpriteFramesWithFile("motions/for_events/walk_right/walk_right.plist");
	animation = Animation::create();
	animation->setDelayPerUnit(0.18f);
	
	for (int i = 0; i < 6; i++)
	{
		auto frame = SpriteFrameCache::getInstance()
			->getSpriteFrameByName(StringUtils::format("walk_right_%d.png", i));
		animation->addSpriteFrame(frame);
	}
	animate = Animate::create(animation);
	
	//** spr_icon_hero를 타겟으로 하는 액션
	auto walk = Repeat::create(animate, 2);
	/// 3번 반복. 오른쪽으로 걷기 동작 모션
	auto move_right = MoveBy::create(2.16f, Point(winSize().width*0.34f, 0));
	/// 오른쪽으로 이동하는 액션
	auto act = Spawn::create(walk, move_right, NULL);
	/// 걸어서 왼쪽에서 들어오는, (스폰액션으로 묶어서)

	/*
	//** spr_icon_trial을 타겟으로 하는 액션 (회전)
	auto expand = ScaleBy::create(1.0f, 1.3f);
	auto shrink = expand->reverse();
	auto vibe = Sequence::create(expand, shrink, NULL);
	auto rotate = RotateBy::create(2.0f, 180);
	auto tint_red = TintTo::create(2.0f, 255, 0, 0);
	auto red_impulse = Spawn::create(vibe, rotate, tint_red, NULL);
	act_extra = RepeatForever::create(red_impulse);
	*/
	CCLOG("Action making sucess");

	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuOnReady::getInstance()->getHeroTarget(), act, 0);
}



//운석의 시련에 대한, 액션을 실행시켜주는 이벤트 
void Menu_OnReady_Meteor_0::setEvent()
{

	//** spr_icon_trial을 타겟으로 하는 액션 (회전)
	auto expand = ScaleTo::create(1.0f, 3.3f);
	auto shrink = ScaleTo::create(1.0f, 2.8f);
	auto vibe = Sequence::create(expand, shrink, NULL);
	auto rotate = RotateBy::create(2.0f, 180);
	auto tint_red = TintTo::create(2.0f, 255, 0, 0);
	auto red_impulse = Spawn::create(vibe, rotate, tint_red, NULL);
	//auto repeat_red_impulse = RepeatForever::create(red_impulse);
	auto act = RepeatForever::create(red_impulse);

	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuOnReady::getInstance()->getTrialTarget(), act, 0);

}

//운석의 시련에 대한, Message 출력 (시련 종류 설명)
void Menu_OnReady_Meteor_1::setEvent()
{
	string* tmp 
		= new string("[=================]_  운석의 시련을_   선택하셨습니다._[=================]"); 
	setBoxMsg(tmp);

	setTarget(MenuOnReady::getInstance()->getLayer());
	
	Point a = Point(winSize().width/2, winSize().height/2);
	setPrintLocation(&a);
	

	//** 출력 레이어에 빈 라벨 생성
	for (int i = 0; i < 6; i++)
	{
		target->removeChild(labelResult[i]);
		labelResult[i] = Label::createWithTTF("", "fonts/BareunDotum1.ttf", 20);
		labelResult[i]->setAnchorPoint(Point::ZERO);
		labelResult[i]->setPosition(Point(location->x, location->y - winSize().height*0.055f * i));
		labelResult[i]->setColor(Color3B::BLACK);
		target->addChild(labelResult[i]);
	}
}



//톱니바퀴의 시련에 대한, Action을 실행시켜주는 이벤트
void Menu_OnReady_Gear_0::setEvent()
{
	//** spr_icon_trial을 타겟으로 하는 액션 (회전)
	auto expand = ScaleTo::create(1.0f, 3.3f);
	auto shrink = ScaleTo::create(1.0f, 2.8f);
	auto vibe = Sequence::create(expand, shrink, NULL);
	auto rotate = RotateBy::create(2.0f, 180);
	auto tint_blue = TintTo::create(2.0f, 0, 0, 255);
	auto blue_impulse = Spawn::create(vibe, rotate, tint_blue, NULL);
	//auto repeat_blue_impulse = RepeatForever::create(blue_impulse);
	auto act = RepeatForever::create(blue_impulse);

	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuOnReady::getInstance()->getTrialTarget(), act, 0);
}

// 톱니바퀴 시련에 대한, Message 출력해주는 이벤트 (시련 종류 설명)
void Menu_OnReady_Gear_1::setEvent()
{
	string* tmp = new string("[=================]_  준비 중입니다._[=================]");
	setBoxMsg(tmp);

	setTarget(MenuOnReady::getInstance()->getLayer());

	Point a = Point(winSize().width / 2, winSize().height / 2);
	setPrintLocation(&a);

	for (int i = 0; i < 6; i++)
	{
		target->removeChild(labelResult[i]);
		labelResult[i] = Label::createWithTTF("", "fonts/BareunDotum1.ttf", 20);
		labelResult[i]->setAnchorPoint(Point::ZERO);
		labelResult[i]->setPosition(Point(location->x, location->y - winSize().height*0.055f * i));
		labelResult[i]->setColor(Color3B::BLACK);
		target->addChild(labelResult[i]);
	}
}


//출격버튼을 눌렀을 때, 캐릭터 오른쪽으로 빠르게 걸어나가는 모션 (2.88초 소모)
void Menu_OnReady_Outro_0::setEvent()
{	
	//SpriteFrameCache::getInstance()->removeSpriteFrames();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/for_events/walk_right/walk_right.plist");
	animation = Animation::create();
	animation->setDelayPerUnit(0.12f);

	for (int i = 0; i < 6; i++)
	{
		auto frame = SpriteFrameCache::getInstance()
								->getSpriteFrameByName(StringUtils::format("walk_right_%d.png", i));
		animation->addSpriteFrame(frame);
	}
	
	animate = Animate::create(animation);
	auto walk = Repeat::create(animate, 4);	///오른쪽으로 걷는 모션 5번 반복
	auto move_right = MoveBy::create(0.822f, Point(winSize().width*0.24f, 0));	///오른쪽 평행으로 이동하는 액션
	auto move_diag = MoveBy::create(2.058f, Point(winSize().width*0.62f, -winSize().height*0.3f));
	///대각선 아래로 이동하는 액션
	auto move = Sequence::create(move_right, move_diag, NULL);
	auto act = Spawn::create(walk, move, NULL);
	///걸어서 언덕 내려가는 모션액션(스폰액션으로 묶어서)
	/// 2.88초동안

	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuOnReady::getInstance()->getHeroTarget(), act, 0);
}








//*************************** #Scene - Result	***************************************// 

//씬 진입 시, 캐릭터 오른쪽에서 천천히 걸어 들어오는 모션 (2.16초 소모)
void Menu_Result_Intro_0::setEvent()
{	
	SpriteFrameCache::getInstance()->
						addSpriteFramesWithFile("motions/for_events/walk_left/walk_left.plist");
	animation = Animation::create();
	animation->setDelayPerUnit(0.18f);

	for (int i = 0; i < 6; i++)
	{
		auto frame = SpriteFrameCache::getInstance()
			->getSpriteFrameByName(StringUtils::format("walk_left_%d.png", i));
		animation->addSpriteFrame(frame);
	}
	animate = Animate::create(animation);


	//** 액션0 - spr_icon_hero를 타겟으로 하는 액션 
	auto walk = Repeat::create(animate, 2);
	/// 3번 반복. 오른쪽으로 걷기 동작 모션
	auto move_left = MoveBy::create(2.16f, Point(-winSize().width*0.28f, 0));
	/// 오른쪽으로 이동하는 액션
	auto act = Spawn::create(walk, move_left, NULL);
	/// 걸어서 왼쪽에서 들어오는, (스폰액션으로 묶어서)

	/*
	//** spr_icon_trial을 타겟으로 하는 액션 (회전)
	auto expand = ScaleBy::create(1.5f, 1.3f);
	auto shrink = expand->reverse();
	auto vibe = Sequence::create(expand, shrink, NULL);
	auto rotate = RotateBy::create(3.0f, 180);
	auto impulse = Spawn::create(vibe, rotate, NULL);

	act_extra = RepeatForever::create(impulse);
	*/
	CCLOG("Action making sucess");
	
	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuResult::getInstance()->getHeroTarget(), act, 0);
}


//** 플레이 등급 표시, 확인 버튼 페이드인
void Menu_Result_Grading_0::setEvent()
{
	//** 액션0 - 해당 스테이지 결과 등급을 보여주는 액션
	auto expand = ScaleBy::create(3.0f, 3.0f);
	auto fade_in = FadeIn::create(2.6f);
	auto resulting = Spawn::create(expand, fade_in, NULL);

	//** 액션1 - 확인 버튼을 띄워주는 액션
	auto delay_confirm = DelayTime::create(2.5f);
	auto fade_in_confirm = FadeTo::create(2.0f, 200);
	auto act_confirm = Sequence::create(delay_confirm, fade_in_confirm, NULL);

	//** 이벤트 레이어의 각 타겟에 연결
	setTarget(MenuResult::getInstance()->getGradeTarget(), resulting, 0);
	setTarget(MenuResult::getInstance()->getConfirmTarget(), act_confirm, 1);
}


//** 왼쪽 위로 걸어나감. 4.8초 소모
void Menu_Result_Outro_0::setEvent()
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/for_events/walk_left/walk_left.plist");
	animation = Animation::create();
	animation->setDelayPerUnit(0.2f);

	for (int i = 0; i < 6; i++)
	{
		auto frame = SpriteFrameCache::getInstance()
			->getSpriteFrameByName(StringUtils::format("walk_left_%d.png", i));
		animation->addSpriteFrame(frame);
	}

	animate = Animate::create(animation);
	auto walk = Repeat::create(animate, 4);
	///왼쪽 걷는 모션 4번 반복. 총 4.8초
	auto move_1st = MoveBy::create(0.8f, Point(-winSize().width*0.15f, 0));
	auto move_2nd = MoveBy::create(3.0f, Point(-winSize().width*0.55f, winSize().height*0.3f));
	auto move_3rd = MoveBy::create(1.0f, Point(-winSize().width*0.2f, 0));
	auto move_total = Sequence::create(move_1st, move_2nd, move_3rd, NULL);

	//** 액션 0 - 걸어서 언덕 올라가는 액션. 총 4.8초
	auto moving_walk = Spawn::create(walk, move_total, NULL);

	setTarget(MenuResult::getInstance()->getHeroTarget(), moving_walk, 0);
}
