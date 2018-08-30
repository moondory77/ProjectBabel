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

	//** 설정 값 저장할 변수들 -> 값 보존
	bool is_bgm_on = true;
	bool is_effect_on = true;
	bool is_vibrate_on = true;

public:
	static PopUpSetting* getInstance();

	void loadPopUp();	//팝업 관련 레이어들을 바로 불러 올 수 있게 일괄적으로 로드시킨다.
	Layer* getLayer() { return layer_setting; }

	void bgmToggle();		//bgm 버튼을 on/off 시키는 토글
	void effectToggle();	//effect 버튼을 on/off 시키는 토글
	void vibrateToggle();	//vibrate 버튼을 on/off 시키는 토글

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

