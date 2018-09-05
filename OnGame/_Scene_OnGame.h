#ifndef __SCENE_ON_GAME__
#define __SCENE_ON_GAME__

#include "cocos2d.h"
#include "Scene/_Scene_Record.h"
#include "Scene/_Scene_Lobby.h"

#include "OnGame/CustomFollow.h"
#include "OnGame/Character.h"
#include "OnGame/JoyController.h"
#include "OnGame/ParticleManager.h"

#include "Util/LineTracer.h"

#include "Obstacle/ObsTexBatcher.h"
#include "Obstacle/ObsContainer/Ordeal/_T_OrdContainer.h"
#include "Obstacle/ObsContainer/Building/DefaultBuilding.h"

using namespace std;
USING_NS_CC;

class ObsManager;
class OnGame : public cocos2d::LayerColor
{
private:
	short stage,						//몇스테인지인가
		heart,							//생명
		combo = 0,						//콤보
		gauge_def,						//방어게이지 수치
		gauge_spc,						//필살게이지 수치
		max_combo,						//최대콤보
		max_density;					//최대건물세기
	
	int score = 1;							//점수
	int special_gauge;
	Layer *movingLayer, *fixedLayer;
	bool attackcheck;
	bool pressedFlag = false;


//	ParticleSystem* particleSpecial;
	timeval timeValue, startedTime;
	bool blocked;
	int building_num;
	char* statement; // attck, 방어, 필살기 등 상황;
	int ruinCount;						//잔해량 - 결과전달
	int deadCount;						//죽은 횟수 - 결과전달
	int trialCount;						//시련에 당한(?) 횟수 - 결과전달
	int gameTime;
	Sprite *boss;
	bool is_boss;


	//추후에 건물 및 장애물순서
	float land;
	Sprite* sprLand;
	Sprite* tmpBuild;


	float width_trans;
	float height_trans;

	int increase_count;
	int on_score;
	int standard = 2;
	float increase_degree = 1.05;
	float decrease_degree = 1.5;
	float newscale;
	bool is_special_effect;
	ParticleSystem *particleSpecialEffect;
	BuildContainer* activeBuilding = NULL;


public:
	LineTracer* tracer;

	ObsTexBatcher* obsBatcher;
	ObsManager* obsMgr;
	ParticleManager* particleMgr;

	vector<BuildContainer*> activeBuildList = {};
	//int prevSpecialID = 0;
	//float prevSpecialDamage = 0.0f;
	//bool flag = true;
	
	void shakeEarthFinish(Ref* sender) { this->pressedFlag = false; };
	EventListenerTouchOneByOne *listener_pause;
	virtual bool onTouchBegan_Pause(Touch* touch, Event *unused_event);
	//virtual void onTouchMoved_Lobby(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_Pause(Touch* touch, Event *unused_event);

	EventListenerTouchOneByOne *listener_setting;
	virtual bool onTouchBegan_Setting(Touch* touch, Event *unused_event);
	//virtual void onTouchMoved_Lobby(Touch* touch, Event *unused_event);
	virtual void onTouchEnded_Setting(Touch* touch, Event *unused_event);


	virtual bool init();
	
	OrdContainer* diagfragment[10];
	
	
	JoyController* joyController;
	//JoyButton* joyButton;
	void initJoyController();
	//void initJoyButton();

	void initBG();
	void initCamera();
	void initCharacter();


	void initGauge();
	void initIcon();
	void cinematicEffect(bool didSpecial);	//오프닝 시, 위에서 부터 카메라 트래킹 효과


	//virtual void onTouchesBegan(const vector<Touch*> &touches, Event* unused_event);
	//virtual void onTouchesEnded(const vector<Touch*> &touches, Event* unused_event);
	//virtual void onTouchesMoved(const vector<Touch*> &touches, Event* unused_event);
	
	EventListenerTouchOneByOne *listener_OnGame;
	virtual bool onTouchBegan_OnGame(Touch* touches, Event *unused_event);
	virtual void onTouchMoved_OnGame(Touch* touches, Event *unuesd_event);
	virtual void onTouchEnded_OnGame(Touch* touches, Event *unuesd_event);

	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event); // 기기에 있는 back, home 버튼을 위해 생성, 안드로이드에서는 home 버튼 활성화 불가능하다고 함.
	void score_presentation(int score);
	void combo_presentation(int combo);

	Character *mainChar;
	CCProgressTimer* GaugeDefense;
	float tmp = 0.0f;


	Sprite *GaugeSpecial;
	Sprite *spc_effect;
	Sprite *jump_effect;
	float DefenseTime;
	float MaxGaugeSize = 0.0;
	float NowSpecialGauge;
	float NowDefenseGauge;
	void DefenseCheck(float dt);



	float NowBuildPosY;
	Sprite *back_icon_OnGame, *jump_icon, *def_icon, *popup_setting_icon;


	Sprite *cameraTarget;
	CustomFollow *followCenter;
	Layer* popup_bg; //팝업창을 위해 생성
	Layer* bg;//설정 팝업창을 위한 레이어

	bool is_popup_on = false; // 팝업창이 띄어 있는지 아닌지 알려주는 변수
	bool is_popup_on_setting = false; //설정 팝업창이 띄어 있는 아닌지 알려주는 변수
	bool is_attack_touch = false;


	// MainSchedule
	void MainScheduler(float dt);
	
	////변화된 상황에 대해, (건물, 캐릭터) 위치를 업데이트
	void setCameraPosition();	//매 프레임 카메라 위치 갱신
	void gaugeUpdate(float dt);


	

	int str_length = 0, combo_str_length = 0;
	std::string IntToString(int number);
	static cocos2d::Scene* createScene();




	CREATE_FUNC(OnGame);
};

#endif 

