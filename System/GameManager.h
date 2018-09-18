#ifndef __GameManager__H__
#define __GameManager__H__

#include "cocos2d.h"
#include "Scene/_Lobby_Scene.h"
#include "Scene/_OnReady_Scene.h"
#include "Scene/_Result_Scene.h"
//#include "Scene/_PlayerInfo_Scene.h"
//#include "Scene/_Record_Scene.h"
#include "OnGame/_OnGame_Scene.h"
#include "System/PopUps.h"

#define PI 3.14159265358979323846

USING_NS_CC;
using namespace std;

enum sceneNum {
	LOBBY = 0,
	ON_READY = 1,
	ON_GAME = 2,
	//RESULT = 3,
	//PLAYER_INFO = 4,
	//RECORD = 5,
};


static inline Size winSize() {
	return Director::getInstance()->getWinSize();
};
static inline float winAspectRatio() {
	return winSize().height / winSize().width;
};

//Sprite에 대한, 스크린 종/횡 역비율
static inline float DivForVertical(Sprite* target){
	return winSize().height / (target->getContentSize().height);
};

static inline float DivForHorizontal(Sprite* target){
	return winSize().width / (target->getContentSize().width);
};

static inline float getTowardAngle(Vec2 toward_vec) {
	return CC_RADIANS_TO_DEGREES(ccpToAngle(toward_vec));
}



class PopUpManager : public CCNode
{
	enum PopUpType {
		SETTING, PAUSE
	};

	static PopUpManager* PopUp_instance;

	PopUpLayer* PopUps[2];

	bool isMusicOn = false;
	bool isSoundOn = false;
	bool isVibrateOn = false;

public:
	static PopUpManager* getInstance();
	PopUpLayer* getSetting();
	PopUpLayer* getPause();
};




//** 각종 게임 내의 이벤트 발생 시, 그 관리를 도와주는 클래스
class EventManager : public cocos2d::CCNode
{
	EventManager()
	{
		this->stage = 1;
		this->is_from_result = false;
		this->trial_num = 1;
	};
	static EventManager* EventManager_instance;
	bool is_from_result;
	int trial_num;

public:
	static EventManager* getInstance();

	int stage;
	void setTrialNum(int trial_num) { this->trial_num = trial_num; }
	int getTrialNum() { return this->trial_num; }

	void setFromResult(bool t) { this->is_from_result = t; }
	bool getFromResult() { return this->is_from_result; }
	///콜백함수. 액션으로 만들기 위해 생성

	/*
	씬넘버
	0 => Lobby 씬
	1 => onReady 씬
	2 => onGame 씬
	3 => Result 씬
	4 => CharacterInfo 씬
	5 => Record 씬
	*/

	// delay 시간 후에, 씬 전환하는 액션 반환 해당 시간 동안, 모든 리스너도 잠재운다.	 
	Action* sceneTransferAction(float delay, sceneNum scene_num);
	void callback_sceneTransfer(Ref *sender, sceneNum scene_num);

	// 콜백함수, 매개변수로 받는 object 리스너를 enable 시킴
	void callback_listenerAwake(Ref *sender, void *object);
	Action* getListenerSleep(float delay, EventListener *listener);
	/// delay 시간동안 해당 리스너 중지시키는 액션 반환. eventOnToggle 콜백 이용

};




//** 개발용 그리드 레이어
class GridLayer :public Layer
{
private:

	GridLayer() {};
	static GridLayer* GridLayer_instance;


	Layer *layer_grid;

public:
	static GridLayer* getInstance();
	Layer* getLayer();
};





#endif
