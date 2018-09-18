#include "PopUps.h"
#include "System/GameManager.h"

//PopUp Layer�� ��ü �ʱ�ȭ ����
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

//���ٸ� �߰�ó�� ������ �׳� ������
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

	//���ùڽ� ��������Ʈ ����
	auto spr_box = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("SettingBox.png"));
	spr_box->setScale(1);
	spr_box->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	this->addChild(spr_box);

	///** �׸��� ���̾� �����Ͽ� ���� (z-order 2��)
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

	//��ġ ��, ��ġ�� �������� id ȹ�� �� �̺�Ʈ �ѱ��
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
		//������ �����ϱ�
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

	//���ùڽ� ��������Ʈ ����
	auto spr_logo = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("PauseLogo.png"));
	spr_logo->setScale(0.5f);
	spr_logo->setPosition(this->getContentSize().width * 0.5f, this->getContentSize().height * 0.75f);
	this->addChild(spr_logo);

	///** �׸��� ���̾� �����Ͽ� ���� (z-order 2��)
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

	//	//**** #1 Pause �˾��� ������� ����� ���̾� �غ�
	//	layer_pause = LayerColor::create(Color4B::BLACK);
	//	layer_pause->setOpacity(150);
	//	layer_pause->setAnchorPoint(Point::ZERO);
	//	layer_pause->setPosition(Point::ZERO);

	//	//**** #1-2 �⺻ ��潺������Ʈ(����) �����ؼ� ������ ���� �� ����
	//	auto spr_pause_back = Sprite::create("layout/popup_pause/bg_black.png");
	//	DivisionForScreen_vertical = winSize.width / (spr_pause_back->getContentSize().width);
	//	DivisionForScreen_horizontal = winSize.height / (spr_pause_back->getContentSize().height);
	//	spr_pause_back->setScale(DivisionForScreen_vertical, DivisionForScreen_horizontal);
	//	spr_pause_back->setAnchorPoint(Point::ZERO);
	//	spr_pause_back->setPosition(Point::ZERO);
	//	spr_pause_back->setOpacity(150);
	//	layer_pause->addChild(spr_pause_back);

	//	///** �׸��� ���̾� �����Ͽ� ���� (z-order 2��)
	//	//layer_pause->addChild(GridLayer::getInstance()->getLayer(), 6);

	//	//**** #1-3 Pause �ΰ� ��������Ʈ�� �����ؼ� ��ġ
	//	auto spr_pause_logo = Sprite::create("layout/popup_pause/pause_logo.png");
	//	DivisionForScreen_vertical = winSize.width / (spr_pause_logo->getContentSize().width);
	//	DivisionForScreen_horizontal = winSize.height / (spr_pause_logo->getContentSize().height);
	//	spr_pause_logo->setScale(DivisionForScreen_vertical*0.5f);
	//	spr_pause_logo->setPosition(Point(winSize.width*0.5f, winSize.height*0.75f));
	//	//spr_pause_logo->setOpacity(100);
	//	layer_pause->addChild(spr_pause_logo);

	//	//**** #2 Pause �˾��� �� ������ ��������Ʈ ���� �� ��ġ
	//
	//	//**** #2-1 ���� �簳 ��ư ��������Ʈ
	//	spr_icon_resume = Sprite::create("layout/popup_pause/resume.png");
	//	DivisionForScreen_vertical = winSize.width / (spr_icon_resume->getContentSize().width);
	//	//DivisionForScreen_horizontal = winSize.height / (spr_icon_resume->getContentSize().height);
	//	spr_icon_resume->setScale(DivisionForScreen_vertical*0.9f);
	//	spr_icon_resume->setPosition(Point(winSize.width*0.5f, winSize.height*0.6f));
	//	layer_pause->addChild(spr_icon_resume);
	//
	//	//**** #2-2 ���� ���� ��ư ��������Ʈ
	//	spr_icon_quit = Sprite::create("layout/popup_pause/quit.png");
	//	DivisionForScreen_vertical = winSize.width / (spr_icon_quit->getContentSize().width);
	//	//DivisionForScreen_horizontal = winSize.height / (spr_icon_resume->getContentSize().height);
	//	spr_icon_quit->setScale(DivisionForScreen_vertical*0.9f);
	//	spr_icon_quit->setPosition(Point(winSize.width*0.5f, winSize.height*0.5f));
	//	layer_pause->addChild(spr_icon_quit);
	//
	//	//**** #2-3 ���� ��ư ��������Ʈ
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

	//��ġ ��, ��ġ�� �������� id ȹ�� �� �̺�Ʈ �ѱ��
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

