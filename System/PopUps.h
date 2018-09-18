#ifndef __POPUPS__H_
#define __POPUPS__H_

#include "cocos2d.h"
USING_NS_CC;
using namespace std;


//모든 팝업창의 베이스가 되는 클래스
class PopUpLayer : public LayerColor
{
	const Size screenSize;
protected:

	vector<Sprite*> iconTarget = {};
	vector<int> iconTouchID = {};
	EventListenerTouchOneByOne* listenerPopUp;


public:
	PopUpLayer(float width, float height) : screenSize(Size(width, height)) {};
	virtual ~PopUpLayer() {
		listenerPopUp->release();
	};

	virtual bool onTouchBegan_PopUp(Touch* touch, Event *unused_event) = 0;
	virtual void onTouchEnded_PopUp(Touch* touch, Event *unused_event) = 0;
	virtual bool init();
	virtual void initUI() = 0;
};




class PopUpSetting : public PopUpLayer
{
	enum IconName {
		MUSIC_TOGGLE, SOUND_TOGGLE, VIBRATION_TOGGLE,
		CLOSE_BUTTON
	};
protected:

	bool isMusicOn = false;
	bool isSoundOn = false;
	bool isVibrationOn = false;

public:


	PopUpSetting(float width, float height) : PopUpLayer(width, height)
	{
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("layout/PopUp_Setting.plist");
		iconTarget.resize(4);
		iconTouchID.resize(4);
		assert(init());
	};
	~PopUpSetting() {
		iconTarget.clear();
		iconTouchID.clear();
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("layout/PopUp_Setting.plist");
	};

	virtual bool init();
	virtual void initUI();
	virtual void onEnter();
	virtual void onExit();

	virtual bool onTouchBegan_PopUp(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_PopUp(Touch* touch, Event *unused_event);

	inline void toggleMusic(bool set_on) {
		if (set_on) {
			iconTarget[MUSIC_TOGGLE]->setDisplayFrame(SpriteFrameCache::getInstance()
				->getSpriteFrameByName("ToggleOn.png"));
		}
		else {
			iconTarget[MUSIC_TOGGLE]->setDisplayFrame(SpriteFrameCache::getInstance()
				->getSpriteFrameByName("ToggleOff.png"));
		}
		isMusicOn = set_on;
	};
	inline void toggleSound(bool set_on) {
		if (set_on) {
			iconTarget[SOUND_TOGGLE]->setDisplayFrame(SpriteFrameCache::getInstance()
				->getSpriteFrameByName("ToggleOn.png"));
		}
		else {
			iconTarget[SOUND_TOGGLE]->setDisplayFrame(SpriteFrameCache::getInstance()
				->getSpriteFrameByName("ToggleOff.png"));
		}
		isSoundOn = set_on;
	};
	inline void toggleVibration(bool set_on) {
		if (set_on) {
			iconTarget[VIBRATION_TOGGLE]->setDisplayFrame(SpriteFrameCache::getInstance()
				->getSpriteFrameByName("ToggleOn.png"));
		}
		else {
			iconTarget[VIBRATION_TOGGLE]->setDisplayFrame(SpriteFrameCache::getInstance()
				->getSpriteFrameByName("ToggleOff.png"));
		}
		isVibrationOn = set_on;
	};
};



class PopUpPause : public PopUpLayer
{
	enum IconName {
		RESUME, QUIT, SETTING
	};

public:

	PopUpPause(float width, float height) : PopUpLayer(width, height)
	{
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("layout/PopUp_Pause.plist");
		iconTarget.resize(3);
		iconTouchID.resize(3);
		assert(init());
	}

	~PopUpPause()
	{
		iconTarget.clear();
		iconTouchID.clear();
		SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("layout/PopUp_Pause.plist");
	};


	virtual bool init();
	virtual void initUI();
	virtual void onEnter();
	virtual void onExit();

	virtual bool onTouchBegan_PopUp(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_PopUp(Touch* touch, Event *unused_event);
};




//
//class PopUpPause : public Layer
//{
//private:
//	PopUpPause() {};
//	static PopUpPause* PopUpPause_instance;
//
//	Layer *layer_pause;
//	//Size winSize;
//	float DivisionForScreen_vertical;
//	float DivisionForScreen_horizontal;
//
//public:
//	static PopUpPause* getInstance();
//
//	void loadPopUp();
//	Layer* getLayer() { return layer_pause; }
//
//	Sprite* spr_icon_resume;
//	Sprite* spr_icon_quit;
//	Sprite* spr_icon_setting;
//};
//



#endif // ! __PopUps__H_

