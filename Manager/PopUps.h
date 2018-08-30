#ifndef __PopUps__H_
#define __PopUps__H__

#include "cocos2d.h"
USING_NS_CC;


class PopUpSetting :public Layer
{
private:
	PopUpSetting() {};
	static PopUpSetting* PopUpSetting_instance;

	Layer *layer_setting;

	Size winSize;
	float DivisionForScreen_vertical;
	float DivisionForScreen_horizontal;

	//** ���� �� ������ ������ -> �� ����
	bool is_bgm_on = true;
	bool is_effect_on = true;
	bool is_vibrate_on = true;

public:
	static PopUpSetting* getInstance();

	void loadPopUp();	//�˾� ���� ���̾���� �ٷ� �ҷ� �� �� �ְ� �ϰ������� �ε��Ų��.
	Layer* getLayer() { return layer_setting; }

	void bgmToggle();		//bgm ��ư�� on/off ��Ű�� ���
	void effectToggle();	//effect ��ư�� on/off ��Ű�� ���
	void vibrateToggle();	//vibrate ��ư�� on/off ��Ű�� ���

	Sprite* spr_icon_bgm;
	Sprite* spr_icon_effect;
	Sprite* spr_icon_vibrate;
	Sprite* spr_icon_close;
};

class PopUpPause : public Layer
{
private:
	PopUpPause() {};
	static PopUpPause* PopUpPause_instance;

	Layer *layer_pause;
	Size winSize;
	float DivisionForScreen_vertical;
	float DivisionForScreen_horizontal;

public:
	static PopUpPause* getInstance();

	void loadPopUp();
	Layer* getLayer() { return layer_pause; }

	Sprite* spr_icon_resume;
	Sprite* spr_icon_quit;
	Sprite* spr_icon_setting;
};




#endif // ! __PopUps__H_

