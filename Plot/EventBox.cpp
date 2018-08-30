#include "Scene/Scene_Lobby.h"
#include "Plot/EventBox.h"
#include "Plot/EventPlayer.h"
#include "Manager/GameManager.h"

using std::string;
USING_NS_CC;

//*************************** #Scene - Lobby	***************************************// 

// �κ�ȭ���� �� ������ ���̵��� (1.5�� �Ҹ�)
void Menu_Lobby_Intro_0::setEvent()
{
	//** �׼�0 - Ÿ��Ʋ �ΰ�(extra_target)�� ��Ÿ���� �׼�
	auto act_extra = FadeIn::create(1.5f);
	//** �׼�1 - start ������ ��Ÿ���� �׼�
	auto act_icon_start = FadeIn::create(1.5f);
	//** �׼�2 - record ������ ��Ÿ���� �׼�
	auto act_icon_record = FadeIn::create(1.5f);
	//** �׼�3 - character ������ ��Ÿ���� �׼�
	auto act_icon_character = FadeIn::create(1.5f);
	//** �׼�4 - setting ������ ��Ÿ���� �׼�
	auto act_icon_setting = FadeIn::create(1.5f);

	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuLobby::getInstance()->getTitleTarget(), act_extra, 0);
	setTarget(MenuLobby::getInstance()->getStartTarget(), act_icon_start, 1);
	setTarget(MenuLobby::getInstance()->getRecordTarget(), act_icon_record, 2);
	setTarget(MenuLobby::getInstance()->getCharacterTarget(), act_icon_character, 3);
	setTarget(MenuLobby::getInstance()->getSettingTarget(), act_icon_setting, 4);
}

// ���ΰ� ���� �ɾ�ö󰡴� (3.3�� �Ҹ�)
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
	/// �ݺ��ϴ� �ȱ� �ִϸ��̼�
	auto up = MoveBy::create(3.3f, Point(0, winSize().height*0.7));
	/// ���� �̵��ϴ� �׼�																
	auto walk_up = Spawn::create(walk, up, NULL);
	///�ɾ ���� �ö󰡴� �׼�
	animation = Animation::create();
	animation->setDelayPerUnit(0.1f);
	animation->addSpriteFrameWithFile("default_back.png");
	animate = Animate::create(animation);
	auto stand_back = Repeat::create(animate, 1);
	///������ ��, �̹��� ��������Ʈ�� ��ü �׼�

	//** �׼�0 - ���ΰ�(target)�� ���� �ö󰡴� �׼�. 3.3��
	auto comeback = Sequence::create(walk_up, stand_back, NULL);
	//** �̺�Ʈ ���̾� Ÿ�ٿ� ����
	setTarget(MenuLobby::getInstance()->getHeroTarget(), comeback, 0);
}

//�κ�ȭ�� icon & logo ���̵� �ƿ�
void Menu_Lobby_Outro_0::setEvent()
{
	//** �׼�0 - Ÿ��Ʋ �ΰ�(extra_target)�� �� ������ ������� �׼�
	auto act_extra = FadeOut::create(1.5f);
	//** �׼�1 - start ������ ������� �׼�
	auto act_icon_start = FadeOut::create(3.5f);
	//** �׼�2 - record ������ ������� �׼�
	auto act_icon_record = FadeOut::create(3.5f);
	//** �׼�3 - character ������ ������� �׼�
	auto act_icon_character = FadeOut::create(3.5f);
	//** �׼�4 - setting ������ ������� �׼�
	auto act_icon_setting = FadeOut::create(3.5f);

	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuLobby::getInstance()->getTitleTarget(), act_extra, 0);
	setTarget(MenuLobby::getInstance()->getStartTarget(), act_icon_start, 1);
	setTarget(MenuLobby::getInstance()->getRecordTarget(), act_icon_record, 2);
	setTarget(MenuLobby::getInstance()->getCharacterTarget(), act_icon_character, 3);
	setTarget(MenuLobby::getInstance()->getSettingTarget(), act_icon_setting, 4);
}

//�Ʒ��� �ɾ������ ĳ���;׼� �̺�Ʈ (3.3�� �Ҹ�)
void Menu_Lobby_Outro_1::setEvent()
{
	//** ���� ĳ�����̹��� ��õ���, �����ִ� �ִϸ��̼��� ���� ����
	//** ������ 1������ �̷����
	animation = Animation::create();
	animation->setDelayPerUnit(0.01);
	animation->addSpriteFrameWithFile("motions/for_events/walk_down/walk_down_0.png");
	animate = Animate::create(animation);

	auto stand = Repeat::create(animate, 1);
	///sub action 1) - stand
	//** �ɾ�� ĳ���� ��������Ʈ ������ĳ�� ����
	//** walk_down �ִϸ��̼��� 5���� ���������� �̷������
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

	auto walk = Repeat::create(animate, 6);	/// �ݺ��ϴ� �ȱ� �ִϸ��̼�
	auto down = MoveBy::create(3.3f, Point(0, -winSize().height*0.7));	/// �Ʒ��� �̵��ϴ� �׼�
	auto walk_down = Spawn::create(walk, down, NULL);
	auto delay = DelayTime::create(0.5f);

	//** �׼�0 - ���ΰ�(target)�� �ɾ� �������� �׼�. ������ ���� 3.8��  
	auto act = Sequence::create(stand, delay, walk_down, NULL);
	/// total act = stand + delay + walk_down�� �������� ����

	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuLobby::getInstance()->getHeroTarget(), act, 0);
}



//*************************** #Scene - CharacterInfo	**************************************// 
// ���ΰ� Į �ֵθ� (2.22�� �Ҹ�)
void Menu_Character_Intro_0::setEvent()
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/for_events/menu_cha/menu_character.plist");
	animation = Animation::create();
	animation->setDelayPerUnit(0.09);
	
	//** �� �ֵθ��� ��� �����ӵ� animation ��ü�� ����
	for (int i = 0; i < 8; i++)
	{
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName
		(StringUtils::format("menu_character_%d.png", i+1));
		animation->addSpriteFrame(frame);
	}
	animate = Animate::create(animation);	
	
	//** �׼� 0 - ���� ������ ��, �� �ֵθ��� �׼� ���� (icon_hero Ÿ��)
	auto delay = DelayTime::create(1.5f);
	auto play = Repeat::create(animate, 1);
	auto act = Sequence::create(delay, play, NULL);

	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuCharacter::getInstance()->getHeroTarget(), act, 0);
}

// ����, �ڷΰ��� �� ���̵���
void Menu_Character_Intro_1::setEvent()
{
	//** �׼� 1 - (���繫�� ��) �޴� �ٸ� ��������  '�����ִ�' �׼� ����(spr_icon_weapon Ÿ��)
	auto unfold_menu = MoveBy::create(2.0f, Point(winSize().width*0.8f, 0));
	//** �׼� 2
	auto act_back = FadeIn::create(2.0f);

	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuCharacter::getInstance()->getWeaponTarget(), unfold_menu, 0);
	setTarget(MenuCharacter::getInstance()->getBackTarget(), act_back, 1);
}

// ���� �� Ŭ�� ��. 1.2�� �ҿ�
void Menu_Character_WTab_0::setEvent()
{
	//** (���繫�� ��) �޴� �� �ణ�������� '���ߴ�' �׼�(spr_icon_weapon)
	auto fold_menu = MoveBy::create(1.0f, Point(-winSize().width*0.6f, 0));
	auto act_sword = FadeIn::create(1.2f);
	auto act_bat = FadeIn::create(1.2f);
	auto act_back = FadeOut::create(1.2f);	///�ڷΰ��� ��ư ��������..

										
	setTarget(MenuCharacter::getInstance()->getWeaponTarget(), fold_menu, 0);
	setTarget(MenuCharacter::getInstance()->getSwordTarget(), act_sword, 1);
	setTarget(MenuCharacter::getInstance()->getBatTarget(), act_bat, 2);
	setTarget(MenuCharacter::getInstance()->getBackTarget(), act_back, 3);
}

// ���� �ٽ� ������ ��, ĳ����ȭ�� ���� (1.2�� �Ҹ�)
void Menu_Character_WTab_1::setEvent()
{
	//** (���繫�� ��) �޴� �ٸ� �ٽ� �������� '�����ִ�' Reverse �׼�(spr_icon_weapon)
	auto unfold_menu = MoveBy::create(1.0f, Point(winSize().width*0.6f, 0));
	auto act_sword = FadeOut::create(1.2f);
	auto act_bat = FadeOut::create(1.2f);
	auto act_back = FadeIn::create(1.2f);

	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuCharacter::getInstance()->getWeaponTarget(), unfold_menu, 0);
	setTarget(MenuCharacter::getInstance()->getSwordTarget(), act_sword, 1);
	setTarget(MenuCharacter::getInstance()->getBatTarget(), act_bat, 2);
	setTarget(MenuCharacter::getInstance()->getBackTarget(), act_back, 3);
}

//���ΰ� �ٽ� Į ����ִ� (1.7�� �Ҹ�)
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
//����, �ڷΰ��� �� ���̵� �ƿ�
void Menu_Character_Outro_1::setEvent()
{
	// �׼� 2 - (���繫�� ��) �޴��ٸ� �ٽ� �������� '���ߴ�' Reverse �׼� (spr_icon_weapon Ÿ��)
	auto fold_menu = MoveBy::create(1.7f, Point(-winSize().width*0.8f, 0));
	// �׼� 3
	auto act_back = FadeOut::create(1.7f);

	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuCharacter::getInstance()->getWeaponTarget(), fold_menu, 0);
	setTarget(MenuCharacter::getInstance()->getBackTarget(), act_back, 1);
}





//*************************** #Scene - OnReady	***************************************// 

//�� ���� ��, ĳ���� ���ʿ��� õõ�� �ɾ� ������ ��� (2.16�� �Ҹ�)
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
	
	//** spr_icon_hero�� Ÿ������ �ϴ� �׼�
	auto walk = Repeat::create(animate, 2);
	/// 3�� �ݺ�. ���������� �ȱ� ���� ���
	auto move_right = MoveBy::create(2.16f, Point(winSize().width*0.34f, 0));
	/// ���������� �̵��ϴ� �׼�
	auto act = Spawn::create(walk, move_right, NULL);
	/// �ɾ ���ʿ��� ������, (�����׼����� ���)

	/*
	//** spr_icon_trial�� Ÿ������ �ϴ� �׼� (ȸ��)
	auto expand = ScaleBy::create(1.0f, 1.3f);
	auto shrink = expand->reverse();
	auto vibe = Sequence::create(expand, shrink, NULL);
	auto rotate = RotateBy::create(2.0f, 180);
	auto tint_red = TintTo::create(2.0f, 255, 0, 0);
	auto red_impulse = Spawn::create(vibe, rotate, tint_red, NULL);
	act_extra = RepeatForever::create(red_impulse);
	*/
	CCLOG("Action making sucess");

	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuOnReady::getInstance()->getHeroTarget(), act, 0);
}



//��� �÷ÿ� ����, �׼��� ��������ִ� �̺�Ʈ 
void Menu_OnReady_Meteor_0::setEvent()
{

	//** spr_icon_trial�� Ÿ������ �ϴ� �׼� (ȸ��)
	auto expand = ScaleTo::create(1.0f, 3.3f);
	auto shrink = ScaleTo::create(1.0f, 2.8f);
	auto vibe = Sequence::create(expand, shrink, NULL);
	auto rotate = RotateBy::create(2.0f, 180);
	auto tint_red = TintTo::create(2.0f, 255, 0, 0);
	auto red_impulse = Spawn::create(vibe, rotate, tint_red, NULL);
	//auto repeat_red_impulse = RepeatForever::create(red_impulse);
	auto act = RepeatForever::create(red_impulse);

	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuOnReady::getInstance()->getTrialTarget(), act, 0);

}

//��� �÷ÿ� ����, Message ��� (�÷� ���� ����)
void Menu_OnReady_Meteor_1::setEvent()
{
	string* tmp 
		= new string("[=================]_  ��� �÷���_   �����ϼ̽��ϴ�._[=================]"); 
	setBoxMsg(tmp);

	setTarget(MenuOnReady::getInstance()->getLayer());
	
	Point a = Point(winSize().width/2, winSize().height/2);
	setPrintLocation(&a);
	

	//** ��� ���̾ �� �� ����
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



//��Ϲ����� �÷ÿ� ����, Action�� ��������ִ� �̺�Ʈ
void Menu_OnReady_Gear_0::setEvent()
{
	//** spr_icon_trial�� Ÿ������ �ϴ� �׼� (ȸ��)
	auto expand = ScaleTo::create(1.0f, 3.3f);
	auto shrink = ScaleTo::create(1.0f, 2.8f);
	auto vibe = Sequence::create(expand, shrink, NULL);
	auto rotate = RotateBy::create(2.0f, 180);
	auto tint_blue = TintTo::create(2.0f, 0, 0, 255);
	auto blue_impulse = Spawn::create(vibe, rotate, tint_blue, NULL);
	//auto repeat_blue_impulse = RepeatForever::create(blue_impulse);
	auto act = RepeatForever::create(blue_impulse);

	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuOnReady::getInstance()->getTrialTarget(), act, 0);
}

// ��Ϲ��� �÷ÿ� ����, Message ������ִ� �̺�Ʈ (�÷� ���� ����)
void Menu_OnReady_Gear_1::setEvent()
{
	string* tmp = new string("[=================]_  �غ� ���Դϴ�._[=================]");
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


//��ݹ�ư�� ������ ��, ĳ���� ���������� ������ �ɾ���� ��� (2.88�� �Ҹ�)
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
	auto walk = Repeat::create(animate, 4);	///���������� �ȴ� ��� 5�� �ݺ�
	auto move_right = MoveBy::create(0.822f, Point(winSize().width*0.24f, 0));	///������ �������� �̵��ϴ� �׼�
	auto move_diag = MoveBy::create(2.058f, Point(winSize().width*0.62f, -winSize().height*0.3f));
	///�밢�� �Ʒ��� �̵��ϴ� �׼�
	auto move = Sequence::create(move_right, move_diag, NULL);
	auto act = Spawn::create(walk, move, NULL);
	///�ɾ ��� �������� ��Ǿ׼�(�����׼����� ���)
	/// 2.88�ʵ���

	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuOnReady::getInstance()->getHeroTarget(), act, 0);
}








//*************************** #Scene - Result	***************************************// 

//�� ���� ��, ĳ���� �����ʿ��� õõ�� �ɾ� ������ ��� (2.16�� �Ҹ�)
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


	//** �׼�0 - spr_icon_hero�� Ÿ������ �ϴ� �׼� 
	auto walk = Repeat::create(animate, 2);
	/// 3�� �ݺ�. ���������� �ȱ� ���� ���
	auto move_left = MoveBy::create(2.16f, Point(-winSize().width*0.28f, 0));
	/// ���������� �̵��ϴ� �׼�
	auto act = Spawn::create(walk, move_left, NULL);
	/// �ɾ ���ʿ��� ������, (�����׼����� ���)

	/*
	//** spr_icon_trial�� Ÿ������ �ϴ� �׼� (ȸ��)
	auto expand = ScaleBy::create(1.5f, 1.3f);
	auto shrink = expand->reverse();
	auto vibe = Sequence::create(expand, shrink, NULL);
	auto rotate = RotateBy::create(3.0f, 180);
	auto impulse = Spawn::create(vibe, rotate, NULL);

	act_extra = RepeatForever::create(impulse);
	*/
	CCLOG("Action making sucess");
	
	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuResult::getInstance()->getHeroTarget(), act, 0);
}


//** �÷��� ��� ǥ��, Ȯ�� ��ư ���̵���
void Menu_Result_Grading_0::setEvent()
{
	//** �׼�0 - �ش� �������� ��� ����� �����ִ� �׼�
	auto expand = ScaleBy::create(3.0f, 3.0f);
	auto fade_in = FadeIn::create(2.6f);
	auto resulting = Spawn::create(expand, fade_in, NULL);

	//** �׼�1 - Ȯ�� ��ư�� ����ִ� �׼�
	auto delay_confirm = DelayTime::create(2.5f);
	auto fade_in_confirm = FadeTo::create(2.0f, 200);
	auto act_confirm = Sequence::create(delay_confirm, fade_in_confirm, NULL);

	//** �̺�Ʈ ���̾��� �� Ÿ�ٿ� ����
	setTarget(MenuResult::getInstance()->getGradeTarget(), resulting, 0);
	setTarget(MenuResult::getInstance()->getConfirmTarget(), act_confirm, 1);
}


//** ���� ���� �ɾ��. 4.8�� �Ҹ�
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
	///���� �ȴ� ��� 4�� �ݺ�. �� 4.8��
	auto move_1st = MoveBy::create(0.8f, Point(-winSize().width*0.15f, 0));
	auto move_2nd = MoveBy::create(3.0f, Point(-winSize().width*0.55f, winSize().height*0.3f));
	auto move_3rd = MoveBy::create(1.0f, Point(-winSize().width*0.2f, 0));
	auto move_total = Sequence::create(move_1st, move_2nd, move_3rd, NULL);

	//** �׼� 0 - �ɾ ��� �ö󰡴� �׼�. �� 4.8��
	auto moving_walk = Spawn::create(walk, move_total, NULL);

	setTarget(MenuResult::getInstance()->getHeroTarget(), moving_walk, 0);
}
