#include "PopUps.h"
#include "System/GameManager.h"

//PopUp Layer의 전체 초기화 과정
bool PopUpLayer::init()
{
	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 0), screenSize.width, screenSize.height)) {
		return false;
	}

	initUI();

	for (int i = 0; i < iconTouchID.size(); i++) {
		iconTouchID[i] = -1;
	}
	listenerPopUp = EventListenerTouchOneByOne::create();
	listenerPopUp->retain();
	listenerPopUp->setSwallowTouches(true);
	listenerPopUp->setEnabled(false);
	return true;
}

//별다른 추가처리 없으면 그냥 포워딩
bool PopUpSetting::init()
{
	if (!PopUpLayer::init())
		return false;

	listenerPopUp->onTouchBegan = CC_CALLBACK_2(PopUpSetting::onTouchBegan_PopUp, this);
	listenerPopUp->onTouchEnded = CC_CALLBACK_2(PopUpSetting::onTouchEnded_PopUp, this);

	return true;
}
void PopUpSetting::initUI()
{
	//this->setOpacity(200);
	//this->setColor(Color3B::BLUE);
	this->ignoreAnchorPointForPosition(false);
	this->setAnchorPoint(Point::ANCHOR_MIDDLE);
	this->setPosition(Point(winSize().width / 2, winSize().height*0.6f));

	//셋팅박스 스프라이트 삽입
	auto spr_box = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("SettingBox.png"));
	spr_box->setScale(1);
	spr_box->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	this->addChild(spr_box);

	///** 그리드 레이어 생성하여 삽입 (z-order 2로)
	//layer_setting->addChild(GridLayer::getInstance()->getLayer(), 6);
	iconTarget[MUSIC_TOGGLE] = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("ToggleOff.png"));
	iconTarget[MUSIC_TOGGLE]->setScale(1.1f, 1.0f);
	iconTarget[MUSIC_TOGGLE]->setPosition(this->getContentSize().width * 0.7f, this->getContentSize().height * 0.52f);
	this->addChild(iconTarget[MUSIC_TOGGLE]);


	iconTarget[SOUND_TOGGLE] = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("ToggleOff.png"));
	iconTarget[SOUND_TOGGLE]->setScale(1.1f, 1.0f);
	iconTarget[SOUND_TOGGLE]->setPosition(this->getContentSize().width * 0.7f, this->getContentSize().height * 0.41f);
	this->addChild(iconTarget[SOUND_TOGGLE]);


	iconTarget[VIBRATION_TOGGLE] = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("ToggleOff.png"));
	iconTarget[VIBRATION_TOGGLE]->setScale(1.1f, 1.0f);
	iconTarget[VIBRATION_TOGGLE]->setPosition(this->getContentSize().width * 0.7f, this->getContentSize().height * 0.298f);
	this->addChild(iconTarget[VIBRATION_TOGGLE]);


	iconTarget[CLOSE_BUTTON] = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("ButtonClose.png"));
	iconTarget[CLOSE_BUTTON]->setScale(1.4f);
	iconTarget[CLOSE_BUTTON]->setPosition(this->getContentSize().width * 0.5f, this->getContentSize().height * 0.13f);
	this->addChild(iconTarget[CLOSE_BUTTON]);
}

void PopUpSetting::onEnter()
{
	getParent()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerPopUp, this->getParent());
	listenerPopUp->setEnabled(true);

	toggleMusic(isMusicOn);
	toggleSound(isSoundOn);
	toggleVibration(isVibrationOn);
}
void PopUpSetting::onExit()
{
	listenerPopUp->setEnabled(false);
	getParent()->getEventDispatcher()->removeEventListener(listenerPopUp);
}

bool PopUpSetting::onTouchBegan_PopUp(Touch* touch, Event *unused_event)
{
	Point touchPos = this->convertToNodeSpace(getParent()->convertToWorldSpace(touch->getLocation()));

	//터치 시, 터치된 아이콘의 id 획득 후 이벤트 넘긴다
	if (iconTarget[MUSIC_TOGGLE]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[MUSIC_TOGGLE] = touch->getID();
		return true;
	}
	if (iconTarget[SOUND_TOGGLE]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[SOUND_TOGGLE] = touch->getID();
		return true;
	}
	if (iconTarget[VIBRATION_TOGGLE]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[VIBRATION_TOGGLE] = touch->getID();
		return true;
	}
	if (iconTarget[CLOSE_BUTTON]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[CLOSE_BUTTON] = touch->getID();
		return true;
	}
	return false;
}
void PopUpSetting::onTouchEnded_PopUp(Touch* touch, Event *unused_event)
{
	Point touchPos = this->convertToNodeSpace(getParent()->convertToWorldSpace(touch->getLocation()));


	if (iconTarget[MUSIC_TOGGLE]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[MUSIC_TOGGLE]))
	{
		toggleMusic(!isMusicOn);
	}
	else if (iconTarget[SOUND_TOGGLE]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[SOUND_TOGGLE]))
	{
		toggleSound(!isSoundOn);
	}
	else if (iconTarget[VIBRATION_TOGGLE]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[VIBRATION_TOGGLE]))
	{
		toggleVibration(!isVibrationOn);
	}
	else if (iconTarget[CLOSE_BUTTON]->getBoundingBox().containsPoint(touchPos)
		&& (touch->getID() == iconTouchID[CLOSE_BUTTON]))
	{
		this->removeFromParent();
		//설정값 저장하기
	}
}


bool PopUpPause::init()
{
	if (!PopUpLayer::init())
		return false;

	listenerPopUp->onTouchBegan = CC_CALLBACK_2(PopUpPause::onTouchBegan_PopUp, this);
	listenerPopUp->onTouchEnded = CC_CALLBACK_2(PopUpPause::onTouchEnded_PopUp, this);
	return true;
}
void PopUpPause::initUI()
{
	//this->setOpacity(200);
	//this->setColor(Color3B::BLUE);
	this->ignoreAnchorPointForPosition(false);
	this->setAnchorPoint(Point::ANCHOR_MIDDLE);
	this->setPosition(Point(winSize().width / 2, winSize().height*0.6f));

	//셋팅박스 스프라이트 삽입
	auto spr_logo = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("PauseLogo.png"));
	spr_logo->setScale(0.5f);
	spr_logo->setPosition(this->getContentSize().width * 0.5f, this->getContentSize().height * 0.75f);
	this->addChild(spr_logo);

	///** 그리드 레이어 생성하여 삽입 (z-order 2로)
	//layer_setting->addChild(GridLayer::getInstance()->getLayer(), 6);
	iconTarget[RESUME] = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("Resume.png"));
	iconTarget[RESUME]->setScale(0.9f);
	iconTarget[RESUME]->setPosition(this->getContentSize().width * 0.5f, this->getContentSize().height * 0.6f);
	this->addChild(iconTarget[RESUME]);

	iconTarget[QUIT] = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("Quit.png"));
	iconTarget[QUIT]->setScale(0.9f);
	iconTarget[QUIT]->setPosition(this->getContentSize().width * 0.5f, this->getContentSize().height * 0.5f);
	this->addChild(iconTarget[QUIT]);

	iconTarget[SETTING] = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("Setting.png"));
	iconTarget[SETTING]->setScale(1.1f, 1.0f);
	iconTarget[SETTING]->setPosition(this->getContentSize().width * 0.5f, this->getContentSize().height * 0.4f);
	this->addChild(iconTarget[SETTING]);

	//	//**** #1 Pause 팝업의 배경으로 사용할 레이어 준비
	//	layer_pause = LayerColor::create(Color4B::BLACK);
	//	layer_pause->setOpacity(150);
	//	layer_pause->setAnchorPoint(Point::ZERO);
	//	layer_pause->setPosition(Point::ZERO);

	//	//**** #1-2 기본 배경스프라이트(검은) 생성해서 사이즈 조정 후 삽입
	//	auto spr_pause_back = Sprite::create("layout/popup_pause/bg_black.png");
	//	DivisionForScreen_vertical = winSize.width / (spr_pause_back->getContentSize().width);
	//	DivisionForScreen_horizontal = winSize.height / (spr_pause_back->getContentSize().height);
	//	spr_pause_back->setScale(DivisionForScreen_vertical, DivisionForScreen_horizontal);
	//	spr_pause_back->setAnchorPoint(Point::ZERO);
	//	spr_pause_back->setPosition(Point::ZERO);
	//	spr_pause_back->setOpacity(150);
	//	layer_pause->addChild(spr_pause_back);

	//	///** 그리드 레이어 생성하여 삽입 (z-order 2로)
	//	//layer_pause->addChild(GridLayer::getInstance()->getLayer(), 6);

	//	//**** #1-3 Pause 로고 스프라이트를 생성해서 배치
	//	auto spr_pause_logo = Sprite::create("layout/popup_pause/pause_logo.png");
	//	DivisionForScreen_vertical = winSize.width / (spr_pause_logo->getContentSize().width);
	//	DivisionForScreen_horizontal = winSize.height / (spr_pause_logo->getContentSize().height);
	//	spr_pause_logo->setScale(DivisionForScreen_vertical*0.5f);
	//	spr_pause_logo->setPosition(Point(winSize.width*0.5f, winSize.height*0.75f));
	//	//spr_pause_logo->setOpacity(100);
	//	layer_pause->addChild(spr_pause_logo);

	//	//**** #2 Pause 팝업의 각 아이콘 스프라이트 생성 및 배치
	//
	//	//**** #2-1 게임 재개 버튼 스프라이트
	//	spr_icon_resume = Sprite::create("layout/popup_pause/resume.png");
	//	DivisionForScreen_vertical = winSize.width / (spr_icon_resume->getContentSize().width);
	//	//DivisionForScreen_horizontal = winSize.height / (spr_icon_resume->getContentSize().height);
	//	spr_icon_resume->setScale(DivisionForScreen_vertical*0.9f);
	//	spr_icon_resume->setPosition(Point(winSize.width*0.5f, winSize.height*0.6f));
	//	layer_pause->addChild(spr_icon_resume);
	//
	//	//**** #2-2 게임 종료 버튼 스프라이트
	//	spr_icon_quit = Sprite::create("layout/popup_pause/quit.png");
	//	DivisionForScreen_vertical = winSize.width / (spr_icon_quit->getContentSize().width);
	//	//DivisionForScreen_horizontal = winSize.height / (spr_icon_resume->getContentSize().height);
	//	spr_icon_quit->setScale(DivisionForScreen_vertical*0.9f);
	//	spr_icon_quit->setPosition(Point(winSize.width*0.5f, winSize.height*0.5f));
	//	layer_pause->addChild(spr_icon_quit);
	//
	//	//**** #2-3 셋팅 버튼 스프라이트
	//	spr_icon_setting = Sprite::create("layout/popup_pause/setting.png");
	//	DivisionForScreen_vertical = winSize.width / (spr_icon_setting->getContentSize().width);
	//	//DivisionForScreen_horizontal = winSize.height / (spr_icon_resume->getContentSize().height);
	//	spr_icon_setting->setScale(DivisionForScreen_vertical*0.9f);
	//	spr_icon_setting->setPosition(Point(winSize.width*0.5f, winSize.height*0.4f));
	//	layer_pause->addChild(spr_icon_setting);
	//
};


void PopUpPause::onEnter()
{
	getParent()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerPopUp, this->getParent());
	listenerPopUp->setEnabled(true);
};
void PopUpPause::onExit()
{
	listenerPopUp->setEnabled(false);
	getParent()->getEventDispatcher()->removeEventListener(listenerPopUp);
};


bool PopUpPause::onTouchBegan_PopUp(Touch* touch, Event *unused_event)
{
	Point touchPos = this->convertToNodeSpace(getParent()->convertToWorldSpace(touch->getLocation()));

	//터치 시, 터치된 아이콘의 id 획득 후 이벤트 넘긴다
	if (iconTarget[RESUME]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[RESUME] = touch->getID();
		return true;
	}
	if (iconTarget[QUIT]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[QUIT] = touch->getID();
		return true;
	}
	if (iconTarget[SETTING]->getBoundingBox().containsPoint(touchPos)) {
		iconTouchID[SETTING] = touch->getID();
		return true;
	}
	return false;
};
void PopUpPause::onTouchEnded_PopUp(Touch* touch, Event *unused_event)
{
	Point touchPos = this->convertToNodeSpace(getParent()->convertToWorldSpace(touch->getLocation()));

	if (iconTarget[RESUME]->getBoundingBox().containsPoint(touchPos) && (touch->getID() == iconTouchID[RESUME]))
	{
	}
	else if (iconTarget[QUIT]->getBoundingBox().containsPoint(touchPos) && (touch->getID() == iconTouchID[QUIT]))
	{
	}
	else if (iconTarget[SETTING]->getBoundingBox().containsPoint(touchPos) && (touch->getID() == iconTouchID[SETTING]))
	{
	}
};

