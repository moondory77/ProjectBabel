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
	short stage,						//��������ΰ�
		heart,							//����
		combo = 0,						//�޺�
		gauge_def,						//�������� ��ġ
		gauge_spc,						//�ʻ������ ��ġ
		max_combo,						//�ִ��޺�
		max_density;					//�ִ�ǹ�����
	
	int score = 1;							//����
	int special_gauge;
	Layer *movingLayer, *fixedLayer;
	bool attackcheck;
	bool pressedFlag = false;


//	ParticleSystem* particleSpecial;
	timeval timeValue, startedTime;
	bool blocked;
	int building_num;
	char* statement; // attck, ���, �ʻ�� �� ��Ȳ;
	int ruinCount;						//���ط� - �������
	int deadCount;						//���� Ƚ�� - �������
	int trialCount;						//�÷ÿ� ����(?) Ƚ�� - �������
	int gameTime;
	Sprite *boss;
	bool is_boss;


	//���Ŀ� �ǹ� �� ��ֹ�����
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
	void cinematicEffect(bool didSpecial);	//������ ��, ������ ���� ī�޶� Ʈ��ŷ ȿ��


	//virtual void onTouchesBegan(const vector<Touch*> &touches, Event* unused_event);
	//virtual void onTouchesEnded(const vector<Touch*> &touches, Event* unused_event);
	//virtual void onTouchesMoved(const vector<Touch*> &touches, Event* unused_event);
	
	EventListenerTouchOneByOne *listener_OnGame;
	virtual bool onTouchBegan_OnGame(Touch* touches, Event *unused_event);
	virtual void onTouchMoved_OnGame(Touch* touches, Event *unuesd_event);
	virtual void onTouchEnded_OnGame(Touch* touches, Event *unuesd_event);

	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event); // ��⿡ �ִ� back, home ��ư�� ���� ����, �ȵ���̵忡���� home ��ư Ȱ��ȭ �Ұ����ϴٰ� ��.
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
	Layer* popup_bg; //�˾�â�� ���� ����
	Layer* bg;//���� �˾�â�� ���� ���̾�

	bool is_popup_on = false; // �˾�â�� ��� �ִ��� �ƴ��� �˷��ִ� ����
	bool is_popup_on_setting = false; //���� �˾�â�� ��� �ִ� �ƴ��� �˷��ִ� ����
	bool is_attack_touch = false;


	// MainSchedule
	void MainScheduler(float dt);
	
	////��ȭ�� ��Ȳ�� ����, (�ǹ�, ĳ����) ��ġ�� ������Ʈ
	void setCameraPosition();	//�� ������ ī�޶� ��ġ ����
	void gaugeUpdate(float dt);


	

	int str_length = 0, combo_str_length = 0;
	std::string IntToString(int number);
	static cocos2d::Scene* createScene();




	CREATE_FUNC(OnGame);
};

#endif 

