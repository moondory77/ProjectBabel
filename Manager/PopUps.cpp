#include <stddef.h>
#include "Manager/PopUps.h"
#include "Manager/GameManager.h"
#include "Scene/_Scene_Lobby.h"


PopUpSetting* PopUpSetting::PopUpSetting_instance = NULL;

PopUpSetting* PopUpSetting::getInstance() 
{
	if (!PopUpSetting_instance)
	{
		PopUpSetting_instance = new PopUpSetting();
		//PopUpSetting_instance->loadPopUp();
	}
	return PopUpSetting_instance;
}

//** �˾� ���� ���̾���� �ʱ�ȭ��Ű��, �ε� �غ� 
void PopUpSetting::loadPopUp()
{
		winSize = Director::getInstance()->getWinSize();

		//**** #1 Setting �˾��� ������� ����� ���̾� �غ�
		layer_setting = LayerColor::create(Color4B::WHITE);
		layer_setting->setOpacity(0);
		layer_setting->setAnchorPoint(Point::ZERO);
		layer_setting->setPosition(Point::ZERO);

		//**** #1-2 �⺻ ��潺������Ʈ(����) �����ؼ� ������ ���� �� ����
		auto spr_setting_back = Sprite::create("layout/popup_setting/bg_black.png");
		DivisionForScreen_vertical = winSize.width / (spr_setting_back->getContentSize().width);
		DivisionForScreen_horizontal = winSize.height / (spr_setting_back->getContentSize().height);
		spr_setting_back->setScale(DivisionForScreen_vertical, DivisionForScreen_horizontal);
		spr_setting_back->setAnchorPoint(Point::ZERO);
		spr_setting_back->setPosition(Point::ZERO);
		spr_setting_back->setOpacity(100);
		layer_setting->addChild(spr_setting_back);

		///** �׸��� ���̾� �����Ͽ� ���� (z-order 2��)
		//layer_setting->addChild(GridLayer::getInstance()->getLayer(), 6);


		//**** #2  Setting �ڽ� ��������Ʈ ����, ������ ���� �� ����
		auto spr_setting_box = Sprite::create("layout/popup_setting/setting_window.png");		
		DivisionForScreen_vertical = winSize.width / (spr_setting_box->getContentSize().width);
		DivisionForScreen_horizontal = winSize.height / (spr_setting_box->getContentSize().height);
		spr_setting_box->setScale(DivisionForScreen_vertical*0.75f, DivisionForScreen_horizontal*0.65f);
		spr_setting_box->setPosition(Point(winSize.width*0.5f, winSize.height*0.6f));

		layer_setting->addChild(spr_setting_box);

		
		//**** #3 Setting �ڽ� ��������Ʈ �ȿ� �����ܵ��� �����ؼ� ������ ��ġ
		//** �� �������� ���ùڽ��� child�� �ȴ�.

		///** #3-1 ��������� on/off�� ���� �˸´� ��������Ʈ �����ؼ� ��ġ
		if (is_bgm_on)
			spr_icon_bgm = Sprite::create("layout/popup_setting/on.png");
		else
			spr_icon_bgm = Sprite::create("layout/popup_setting/off.png");

		DivisionForScreen_vertical = winSize.width / spr_icon_bgm->getContentSize().width;
		DivisionForScreen_horizontal = winSize.height / spr_icon_bgm->getContentSize().height;
		spr_icon_bgm->setScale(DivisionForScreen_vertical*0.35f, DivisionForScreen_horizontal*0.05f);
		spr_icon_bgm->setPosition(winSize.width*0.63f, winSize.height*0.615f);
		layer_setting->addChild(spr_icon_bgm);



		///** #3-2 ȿ������ on/off�� ���� �˸´� ��������Ʈ �����ؼ� ��ġ
		if (is_effect_on)
			spr_icon_effect = Sprite::create("layout/popup_setting/on.png");
		else
			spr_icon_effect = Sprite::create("layout/popup_setting/off.png");
		spr_icon_effect->setScale(DivisionForScreen_vertical*0.35f, DivisionForScreen_horizontal*0.05f);
		spr_icon_effect->setPosition(winSize.width*0.63f, winSize.height*0.535f);
		layer_setting->addChild(spr_icon_effect);


		///** #3-3 ������ on/off�� ���� �˸´� ��������Ʈ �����ؼ� ��ġ
		if (is_vibrate_on)
			spr_icon_vibrate = Sprite::create("layout/popup_setting/on.png");
		else
			spr_icon_vibrate = Sprite::create("layout/popup_setting/off.png");
		spr_icon_vibrate->setScale(DivisionForScreen_vertical*0.35f, DivisionForScreen_horizontal*0.05f);
		spr_icon_vibrate->setPosition(winSize.width*0.63f, winSize.height*0.455f);
		layer_setting->addChild(spr_icon_vibrate);




		spr_icon_close = Sprite::create("layout/popup_setting/close.png");
		DivisionForScreen_vertical = winSize.width / spr_icon_close->getContentSize().width;
		DivisionForScreen_horizontal = winSize.height / spr_icon_close->getContentSize().height;

		spr_icon_close->setScale(DivisionForScreen_vertical*0.23f, DivisionForScreen_horizontal*0.1f);
		spr_icon_close->setPosition(winSize.width *0.5f, winSize.height*0.36f);
		layer_setting->addChild(spr_icon_close);
		
	
}
void PopUpSetting::bgmToggle()
{
	is_bgm_on = !is_bgm_on;
	///bgm ��۹�ư�� ����

	layer_setting->removeChild(spr_icon_bgm);
	///���� ��۹�ư�� �����

	///** #3-1 ��������� on/off�� ���� �˸´� ��������Ʈ �����ؼ� ��ġ
	if (is_bgm_on)
		spr_icon_bgm = Sprite::create("layout/popup_setting/on.png");
	else
		spr_icon_bgm = Sprite::create("layout/popup_setting/off.png");
	
	DivisionForScreen_vertical = winSize.width / spr_icon_bgm->getContentSize().width;
	DivisionForScreen_horizontal = winSize.height / spr_icon_bgm->getContentSize().height;
	spr_icon_bgm->setScale(DivisionForScreen_vertical*0.35f, DivisionForScreen_horizontal*0.05f);
	spr_icon_bgm->setPosition(winSize.width*0.63f, winSize.height*0.615f);
	layer_setting->addChild(spr_icon_bgm);

}
void PopUpSetting::effectToggle()
{
	is_effect_on = !is_effect_on;

	layer_setting->removeChild(spr_icon_effect);

	///** #3-2 ȿ������ on/off�� ���� �˸´� ��������Ʈ �����ؼ� ��ġ
	if (is_effect_on)
		spr_icon_effect = Sprite::create("layout/popup_setting/on.png");
	else
		spr_icon_effect = Sprite::create("layout/popup_setting/off.png");

	DivisionForScreen_vertical = winSize.width / spr_icon_effect->getContentSize().width;
	DivisionForScreen_horizontal = winSize.height / spr_icon_effect->getContentSize().height;

	spr_icon_effect->setScale(DivisionForScreen_vertical*0.35f, DivisionForScreen_horizontal*0.05f);
	spr_icon_effect->setPosition(winSize.width*0.63f, winSize.height*0.535f);
	layer_setting->addChild(spr_icon_effect);

}
void PopUpSetting::vibrateToggle()
{
	is_vibrate_on = !is_vibrate_on;
	layer_setting->removeChild(spr_icon_vibrate);

	///** #3-3 ������ on/off�� ���� �˸´� ��������Ʈ �����ؼ� ��ġ
	if (is_vibrate_on)
		spr_icon_vibrate = Sprite::create("layout/popup_setting/on.png");
	else
		spr_icon_vibrate = Sprite::create("layout/popup_setting/off.png");

	DivisionForScreen_vertical = winSize.width / spr_icon_vibrate->getContentSize().width;
	DivisionForScreen_horizontal = winSize.height / spr_icon_vibrate->getContentSize().height;

	spr_icon_vibrate->setScale(DivisionForScreen_vertical*0.35f, DivisionForScreen_horizontal*0.05f);
	spr_icon_vibrate->setPosition(winSize.width*0.63f, winSize.height*0.455f);
	layer_setting->addChild(spr_icon_vibrate);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PopUpPause* PopUpPause::PopUpPause_instance = NULL;

PopUpPause* PopUpPause::getInstance()
{
	if (!PopUpPause_instance)
	{
		PopUpPause_instance = new PopUpPause();
	}
	return PopUpPause_instance;
}

void PopUpPause::loadPopUp()
{
	winSize = Director::getInstance()->getWinSize();

	//**** #1 Pause �˾��� ������� ����� ���̾� �غ�
	layer_pause = LayerColor::create(Color4B::BLACK);
	layer_pause->setOpacity(150);
	layer_pause->setAnchorPoint(Point::ZERO);
	layer_pause->setPosition(Point::ZERO);

	//**** #1-2 �⺻ ��潺������Ʈ(����) �����ؼ� ������ ���� �� ����
	auto spr_pause_back = Sprite::create("layout/popup_pause/bg_black.png");
	DivisionForScreen_vertical = winSize.width / (spr_pause_back->getContentSize().width);
	DivisionForScreen_horizontal = winSize.height / (spr_pause_back->getContentSize().height);
	spr_pause_back->setScale(DivisionForScreen_vertical, DivisionForScreen_horizontal);
	spr_pause_back->setAnchorPoint(Point::ZERO);
	spr_pause_back->setPosition(Point::ZERO);
	spr_pause_back->setOpacity(150);
	layer_pause->addChild(spr_pause_back);

	///** �׸��� ���̾� �����Ͽ� ���� (z-order 2��)
	//layer_pause->addChild(GridLayer::getInstance()->getLayer(), 6);

	//**** #1-3 Pause �ΰ� ��������Ʈ�� �����ؼ� ��ġ
	auto spr_pause_logo = Sprite::create("layout/popup_pause/pause_logo.png");
	DivisionForScreen_vertical = winSize.width / (spr_pause_logo->getContentSize().width);
	DivisionForScreen_horizontal = winSize.height / (spr_pause_logo->getContentSize().height);
	spr_pause_logo->setScale(DivisionForScreen_vertical*0.5f);
	spr_pause_logo->setPosition(Point(winSize.width*0.5f, winSize.height*0.75f));
	//spr_pause_logo->setOpacity(100);
	layer_pause->addChild(spr_pause_logo);


	//**** #2 Pause �˾��� �� ������ ��������Ʈ ���� �� ��ġ

	//**** #2-1 ���� �簳 ��ư ��������Ʈ
	spr_icon_resume = Sprite::create("layout/popup_pause/resume.png");
	DivisionForScreen_vertical = winSize.width / (spr_icon_resume->getContentSize().width);
	//DivisionForScreen_horizontal = winSize.height / (spr_icon_resume->getContentSize().height);
	spr_icon_resume->setScale(DivisionForScreen_vertical*0.9f);
	spr_icon_resume->setPosition(Point(winSize.width*0.5f, winSize.height*0.6f));
	layer_pause->addChild(spr_icon_resume);

	//**** #2-2 ���� ���� ��ư ��������Ʈ
	spr_icon_quit = Sprite::create("layout/popup_pause/quit.png");
	DivisionForScreen_vertical = winSize.width / (spr_icon_quit->getContentSize().width);
	//DivisionForScreen_horizontal = winSize.height / (spr_icon_resume->getContentSize().height);
	spr_icon_quit->setScale(DivisionForScreen_vertical*0.9f);
	spr_icon_quit->setPosition(Point(winSize.width*0.5f, winSize.height*0.5f));
	layer_pause->addChild(spr_icon_quit);

	//**** #2-3 ���� ��ư ��������Ʈ
	spr_icon_setting = Sprite::create("layout/popup_pause/setting.png");
	DivisionForScreen_vertical = winSize.width / (spr_icon_setting->getContentSize().width);
	//DivisionForScreen_horizontal = winSize.height / (spr_icon_resume->getContentSize().height);
	spr_icon_setting->setScale(DivisionForScreen_vertical*0.9f);
	spr_icon_setting->setPosition(Point(winSize.width*0.5f, winSize.height*0.4f));
	layer_pause->addChild(spr_icon_setting);

}
