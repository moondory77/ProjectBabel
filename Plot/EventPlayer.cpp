#include "Plot\EventPlayer.h"
#include "Plot\EventBox.h"
#include "Manager/GameManager.h"
#include <string>

using std::string;
USING_NS_CC;

//*************************** #Scene - Lobby	***************************************// 
MenuLobby* MenuLobby::MenuStart_instance = NULL;

MenuLobby* MenuLobby::getInstance() {
	if (!MenuStart_instance)
	{
		MenuStart_instance = new MenuLobby();
	}
	return MenuStart_instance;
}
void MenuLobby::initMenuLobby() {

	//** 아이콘 layer 생성
	layer_menu_lobby = LayerColor::create(Color4B::WHITE);
	layer_menu_lobby->setAnchorPoint(Point::ZERO);
	layer_menu_lobby->setOpacity(0);

	//** Title 아이콘 스프라이트 생성 배치
	spr_icon_title = Sprite::create("layout/main/title.png");
	spr_icon_title->setScale(DivForHorizontal(spr_icon_title)*0.9f);
	spr_icon_title->setPosition(Point(winSize().width*0.5f, winSize().height*0.85f));
	spr_icon_title->setOpacity(0);
	layer_menu_lobby->addChild(spr_icon_title);

	//** Start 아이콘 스프라이트 생성 배치
	spr_icon_start = Sprite::create("layout/main/start.png");
	spr_icon_start->setScale(DivForHorizontal(spr_icon_start)*0.7f);
	spr_icon_start->setAnchorPoint(Point(0.5, 0));
	spr_icon_start->setPosition(Point(winSize().width*0.5f, 0));
	spr_icon_start->setOpacity(0);
	layer_menu_lobby->addChild(spr_icon_start);

	//** Record 아이콘 스프라이트 생성 배치
	spr_icon_record = Sprite::create("layout/main/record.png");
	spr_icon_record->setScale(DivForHorizontal(spr_icon_record)*0.25f);
	spr_icon_record->setAnchorPoint(Point(0, 0.5));
	spr_icon_record->setPosition(Point(0, winSize().height*0.25f));
	spr_icon_record->setOpacity(0);
	layer_menu_lobby->addChild(spr_icon_record);

	//** Character 아이콘 스프라이트 생성 배치
	spr_icon_character = Sprite::create("layout/main/character.png");
	spr_icon_character->setScale(DivForHorizontal(spr_icon_character)*0.25f);
	spr_icon_character->setAnchorPoint(Point(0, 0.5));
	spr_icon_character->setPosition(Point(0, winSize().height*0.4f));
	spr_icon_character->setOpacity(0);
	layer_menu_lobby->addChild(spr_icon_character);

	//** Setting 아이콘 스프라이트 생성 배치
	spr_icon_setting = Sprite::create("layout/main/setting.png");
	spr_icon_setting->setScale(DivForHorizontal(spr_icon_setting)*0.25f);
	spr_icon_setting->setAnchorPoint(Point(1, 0.5));
	spr_icon_setting->setPosition(Point(winSize().width, winSize().height*0.3f));
	spr_icon_setting->setOpacity(0);
	layer_menu_lobby->addChild(spr_icon_setting);


	spr_icon_hero = Sprite::create("default_back.png");
	spr_icon_hero->setScale(DivForHorizontal(spr_icon_hero)*0.3f);
	spr_icon_hero->setAnchorPoint(Point(0.5, 0));
	spr_icon_hero->setPosition(Point(winSize().width*0.5f, -winSize().height*0.2));
	layer_menu_lobby->addChild(spr_icon_hero);


	////** Icon_hero 스프라이트 생성 배치
	////** Case 1) 스테이지 클리어 후 귀환이라면,
	//if (EventManager::getInstance()->getFromResult())
	//{
	//	//** hero 아이콘 스프라이트 생성 배치
	//	spr_icon_hero = Sprite::create("default_back.png");
	//	spr_icon_hero->setScale(DivForHorizontal(spr_icon_hero)*0.3f);
	//	spr_icon_hero->setAnchorPoint(Point(0.5, 0));
	//	spr_icon_hero->setPosition(Point(winSize().width*0.5f, -winSize().height*0.2));
	//	layer_menu_lobby->addChild(spr_icon_hero);
	//}
	////** Case 2) 그 밖의 다른 씬에서의 귀환이라면,
	//else
	//{
	//	//** hero 아이콘 스프라이트 생성 배치
	//	spr_icon_hero = Sprite::create("default_back.png");
	//	spr_icon_hero->setScale(DivForHorizontal(spr_icon_hero)*0.3f);
	//	spr_icon_hero->setAnchorPoint(Point(0.5, 0));
	//	spr_icon_hero->setPosition(Point(winSize().width*0.5f, winSize().height*0.5f));
	//	layer_menu_lobby->addChild(spr_icon_hero);
	//}

	list_menu_lobby = new EventList();

	list_menu_lobby->pushBox(new Menu_Lobby_Intro_0(NULL, NULL));
	list_menu_lobby->pushBox(new Menu_Lobby_Intro_1(NULL, NULL));
	list_menu_lobby->pushBox(new Menu_Lobby_Outro_0(NULL, NULL));
	list_menu_lobby->pushBox(new Menu_Lobby_Outro_1(NULL, NULL));
}



//*************************** #Scene - CharacterInfo	**************************************// 
MenuCharacter* MenuCharacter::MenuCha_instance = NULL;

MenuCharacter* MenuCharacter::getInstance() {
	if (!MenuCha_instance)
	{
		MenuCha_instance = new MenuCharacter;
	}
	return MenuCha_instance;
}
void MenuCharacter::initMenuCharacter() {
	
		//**캐릭터 메뉴를 눌렀을 때, 액션 이벤트를 표현하기 위한 레이어
		layer_menu_cha = LayerColor::create(Color4B::WHITE);
		layer_menu_cha->setOpacity(0);
		layer_menu_cha->setAnchorPoint(Point::ZERO);
		layer_menu_cha->setPosition(Point::ZERO);

		//** 주인공 실루엣
		spr_icon_hero = Sprite::create("motions/for_events/menu_cha/menu_character_1.png");
		spr_icon_hero->setScale(DivForHorizontal(spr_icon_hero)*0.3f);
		spr_icon_hero->setAnchorPoint(Point(0.5, 0));
		spr_icon_hero->setPosition(Point(winSize().width*0.5f, winSize().height*0.5f));
		layer_menu_cha->addChild(spr_icon_hero);
		//** 현재무기 탭 스프라이트
		spr_icon_weapon = Sprite::create("layout/character_info/weapon.png");
		spr_icon_weapon->setScale(DivForHorizontal(spr_icon_weapon)*0.8f, DivForVertical(spr_icon_weapon)*0.1f);
		spr_icon_weapon->setAnchorPoint(Point::ZERO);
		spr_icon_weapon->setPosition(-winSize().width*0.8f, winSize().height*0.8f);
		layer_menu_cha->addChild(spr_icon_weapon);
		//** 뒤로가기 스프라이트
		spr_icon_back = Sprite::create("layout/character_info/back.png");
		spr_icon_back->setScale(DivForHorizontal(spr_icon_back)*0.23f, DivForVertical(spr_icon_back)*0.08f);
		spr_icon_back->setPosition(winSize().width*0.5f, winSize().height*0.15f);
		spr_icon_back->setOpacity(0);
		layer_menu_cha->addChild(spr_icon_back);

		//** 현재무기 탭 클릭 했을 시, (일단 Opacity 0으로 해놓음)
		//** #1 sword 이미지
		spr_icon_sword = Sprite::create("weapon/sword.png");
		spr_icon_sword->setScale(DivForHorizontal(spr_icon_sword)*0.18f);
		spr_icon_sword->setAnchorPoint(Point(0.5, 0));
		spr_icon_sword->setPosition(Point(winSize().width*0.32f, winSize().height*0.22f));
		spr_icon_sword->setOpacity(0);
		layer_menu_cha->addChild(spr_icon_sword);
		//** #2 bat 이미지
		spr_icon_bat = Sprite::create("weapon/bat.png");
		spr_icon_bat->setScale(DivForHorizontal(spr_icon_bat)*0.2f, DivForVertical(spr_icon_bat)*0.26f);
		spr_icon_bat->setAnchorPoint(Point(0.5, 0));
		spr_icon_bat->setPosition(Point(winSize().width*0.68f, winSize().height*0.22f));
		spr_icon_bat->setOpacity(0);
		layer_menu_cha->addChild(spr_icon_bat);

		list_menu_cha = new EventList();

		list_menu_cha->pushBox(new Menu_Character_Intro_0(NULL, NULL));
		list_menu_cha->pushBox(new Menu_Character_Intro_1(NULL, NULL));
		list_menu_cha->pushBox(new Menu_Character_WTab_0(NULL, NULL));
		list_menu_cha->pushBox(new Menu_Character_WTab_1(NULL, NULL));
		list_menu_cha->pushBox(new Menu_Character_Outro_0(NULL, NULL));
		list_menu_cha->pushBox(new Menu_Character_Outro_1(NULL, NULL));
}




//*************************** #Scene - OnReady	***************************************// 

MenuOnReady* MenuOnReady::MenuOnReady_instance = NULL;

MenuOnReady* MenuOnReady::getInstance()
{
	if (!MenuOnReady_instance)
	{
		MenuOnReady_instance = new MenuOnReady();
	}
	return MenuOnReady_instance;
}
void MenuOnReady::initMenuOnReady()
{

	//** #1 - onReady 레이어 생성
	layer_menu_onReady = LayerColor::create(Color4B::BLACK);
	layer_menu_onReady->setAnchorPoint(Point::ZERO);
	layer_menu_onReady->setPosition(Point::ZERO);
	layer_menu_onReady->setOpacity(0);
	
	//** #2 - icon_logo 스프라이트 생성
	spr_icon_logo = Sprite::create("layout/on_ready/item_logo.png");
	spr_icon_logo->setScale(DivForHorizontal(spr_icon_logo)*0.8f, DivForVertical(spr_icon_logo)*0.2f);
	spr_icon_logo->setPosition(winSize().width*0.5f, winSize().height*0.85f);
	//layer_menu_onReady->addChild(spr_icon_logo);

	//** #3 - icon_start(출격) 스프라이트 생성
	spr_icon_start = Sprite::create("layout/on_ready/start.png");
	//spr_icon_start->setAnchorPoint(Point(1, 0));
	spr_icon_start->setScale(DivForHorizontal(spr_icon_start)*0.25f, DivForVertical(spr_icon_start)*0.085f);
	spr_icon_start->setPosition(winSize().width*0.5f, winSize().height*0.1f);
	spr_icon_start->setOpacity(200);
	layer_menu_onReady->addChild(spr_icon_start);


	//** #4 - icon_trial(시련) 스프라이트 생성
	spr_icon_trial = Sprite::create("layout/on_ready/trial.png");
	spr_icon_trial->setScale(DivForHorizontal(spr_icon_trial)*0.63f);
	spr_icon_trial->setPosition(winSize().width*0.85f, winSize().height*0.925f);
	layer_menu_onReady->addChild(spr_icon_trial);

	//** #5 - icon_buff(버프) 스프라이트 생성
	spr_icon_buff = Sprite::create("layout/on_ready/buff.png");
	spr_icon_buff->setScale(DivForHorizontal(spr_icon_buff)*0.3f);
	spr_icon_buff->setPosition(winSize().width*0.6f, winSize().height*0.4f);
	//layer_menu_onReady->addChild(spr_icon_buff);

	//** #6 - icon_hero 스프라이트 생성
	spr_icon_hero = Sprite::create("motions/for_events/walk_right/walk_right_5.png");
	spr_icon_hero->setScale(DivForHorizontal(spr_icon_hero)*0.2f);
	spr_icon_hero->setAnchorPoint(Point(0.5, 0));
	spr_icon_hero->setPosition(-winSize().width*0.1f, winSize().height*0.34f);
	layer_menu_onReady->addChild(spr_icon_hero);

	//layer_menu_onReady->addChild(GridLayer::getInstance()->getLayer());

	list_menu_onReady = new EventList();

	list_menu_onReady->pushBox(new Menu_OnReady_Intro_0(NULL, NULL));
	//list_menu_onReady->pushBox(new Menu_OnReady_Meteor_0(NULL, NULL));
	//list_menu_onReady->pushBox(new Menu_OnReady_Meteor_1(NULL, NULL));
	//list_menu_onReady->pushBox(new Menu_OnReady_Gear_0(NULL, NULL));
	//list_menu_onReady->pushBox(new Menu_OnReady_Gear_1(NULL, NULL));
	list_menu_onReady->pushBox(new Menu_OnReady_Outro_0(NULL, NULL));

}


//*************************** #Scene - Result	***************************************// 
MenuResult* MenuResult::MenuResult_instance = NULL;

MenuResult* MenuResult::getInstance()
{
	if (!MenuResult_instance)
	{
		MenuResult_instance = new MenuResult();
	}
	return MenuResult_instance;
}


void MenuResult::initMenuResult()
{

	//** #1 - Result 레이어 생성/배치
	layer_menu_result = LayerColor::create(Color4B::BLACK);
	layer_menu_result->setAnchorPoint(Point::ZERO);
	layer_menu_result->setPosition(Point::ZERO);
	layer_menu_result->setOpacity(0);

	//** #2 - icon_logo 스프라이트 생성/배치
	spr_icon_logo = Sprite::create("layout/result/result_logo.png");
	spr_icon_logo->setScale(DivForHorizontal(spr_icon_logo)*0.6f, DivForVertical(spr_icon_logo)*0.13f);
	spr_icon_logo->setPosition(winSize().width*0.5f, winSize().height*0.8f);
	layer_menu_result->addChild(spr_icon_logo);

	//** #3 - icon_confirm 스프라이트 생성/배치
	spr_icon_confirm = Sprite::create("layout/result/confirm.png");
	spr_icon_confirm->setScale(DivForHorizontal(spr_icon_confirm)*0.3f);
	spr_icon_confirm->setAnchorPoint(Point(0.5, 0));
	spr_icon_confirm->setPosition(Point(winSize().width*0.5f, winSize().height*0.3f));
	spr_icon_confirm->setOpacity(0);
	layer_menu_result->addChild(spr_icon_confirm);

	//** #4 - icon_hero 스프라이트 생성/배치
	spr_icon_hero = Sprite::create("motions/for_events/walk_left/walk_left_5.png");
	spr_icon_hero->setScale(DivForHorizontal(spr_icon_hero)*0.2f);
	spr_icon_hero->setAnchorPoint(Point(0.5, 0));
	spr_icon_hero->setPosition(winSize().width*1.1f, winSize().height*0.04f);
	layer_menu_result->addChild(spr_icon_hero);
	

	//** #5 - icon_grade 스프라이트 상황에 맞게 설정
	spr_icon_grade = Sprite::create("layout/result/grade_s.png");
	spr_icon_grade->setScale(DivForHorizontal(spr_icon_grade)*0.25f);
	spr_icon_grade->setOpacity(0);
	spr_icon_grade->setPosition(Point(winSize().width*0.5f, winSize().height*0.55f));
	layer_menu_result->addChild(spr_icon_grade);

	//layer_menu_result->addChild(GridLayer::getInstance()->getLayer());
	///그리드 레이어 생성해서 배치

	//** 이벤트 리스트를 생성하고, 박스들을 삽입한다. 
	list_menu_result = new EventList();
	list_menu_result->pushBox(new Menu_Result_Intro_0(NULL, NULL));
	list_menu_result->pushBox(new Menu_Result_Grading_0(NULL, NULL));
	list_menu_result->pushBox(new Menu_Result_Outro_0(NULL, NULL));

}

