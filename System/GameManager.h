#ifndef __GameManager__H__
#define __GameManager__H__
#define PI 3.14159265358979323846

#include "cocos2d.h"
#include "Scene/_Scene_Lobby.h"
#include "Scene/_Scene_OnReady.h"
#include "Scene/_Scene_Result.h"
#include "Scene/_Scene_PlayerInfo.h"
#include "Scene/_Scene_Record.h"
#include "OnGame/_Scene_OnGame.h"


USING_NS_CC;

enum sceneNum {
	LOBBY = 0,
	ON_READY = 1,
	ON_GAME = 2,
	RESULT = 3,
	PLAYER_INFO = 4,
	RECORD = 5,
};

static inline Size winSize() {
	return Director::getInstance()->getWinSize();
};

static inline float winAspectRatio() {
	return winSize().height / winSize().width;
};

//Sprite�� ����, ��ũ�� ��/Ⱦ ������
static inline float DivForVertical(Sprite* target){
	return winSize().height / (target->getContentSize().height);
};

static inline float DivForHorizontal(Sprite* target){
	return winSize().width / (target->getContentSize().width);
};

static inline float getTowardAngle(Vec2 toward_vec) 
{
	//float converted = CC_RADIANS_TO_DEGREES(ccpToAngle(toward_vec)) + 360.0f;

	//if (converted < 360.0f)
	//	return converted;
	//else
	//	return converted - 360.0f;


	return CC_RADIANS_TO_DEGREES(ccpToAngle(toward_vec));

	
}



//** ���� ���� ���� �̺�Ʈ �߻� ��, �� ������ �����ִ� Ŭ����
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
	///�ݹ��Լ�. �׼����� ����� ���� ����

	/*
	���ѹ�
	0 => Lobby ��
	1 => onReady ��
	2 => onGame ��
	3 => Result ��
	4 => CharacterInfo ��
	5 => Record ��
	*/

	// delay �ð� �Ŀ�, �� ��ȯ�ϴ� �׼� ��ȯ �ش� �ð� ����, ��� �����ʵ� ������.	 
	Action* sceneTransferAction(float delay, sceneNum scene_num);
	void callback_sceneTransfer(Ref *sender, sceneNum scene_num);

	// �ݹ��Լ�, �Ű������� �޴� object �����ʸ� enable ��Ŵ
	void callback_listenerAwake(Ref *sender, void *object);
	Action* getListenerSleep(float delay, EventListener *listener);
	/// delay �ð����� �ش� ������ ������Ű�� �׼� ��ȯ. eventOnToggle �ݹ� �̿�

};

//** ���߿� �׸��� ���̾�
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