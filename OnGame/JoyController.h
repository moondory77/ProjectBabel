#ifndef __JOY_CONTROLLER_H__
#define __JOY_CONTROLLER_H__

#include "cocos2d.h"
#include "OnGame/Character.h"

USING_NS_CC;

enum InputBitType {
	TOUCH = 0,
	SWIPE = 1,
	RIGHT = 0,
	LEFT = 1,
	UP = 1,
	DOWN = 2
};


class JoyController : public Layer
{
private:

	Sprite* sprMoveLeft;
	Sprite* sprMoveRight;
	Sprite* sprBtnAttack;
	Sprite* sprBtnJump;
	Sprite* sprBtnDefense;
	Sprite* sprBtnSpc;

	//Point initTouchPos;
	//Point curTouchPos;

	bool isActiveBtnMove = true;
	bool isActiveBtnJump = true;
	bool isActiveBtnDefense = true;
	bool isActiveBtnAttack = true;
	bool isActiveBtnSpc = true;

	bool isTouchedLeft = false;
	bool isTouchedRight = false;
	bool isTouchedAttack = false;
	bool isTouchedDefense = false;
	bool isTouchedJump = false;
	bool isTouchedSpc = false;

	bool isZoomIn = false;
	
	float velocity;
	bool isControllerShow;
	bool isLimitScreen;

	bool isSpecial = true;
	bool isShakeFinish = true;
	bool isSpecialStart = false;
	bool isCinematic = false;

	Point initAttackPos;
	Point initDefensePos;
	Point initJumpPos;
	Point initMovePos;
	
	//min, max
	pair<float, float> marginAtkRot = {0, 0};	
	float curAtkRot;

	Point curDefensePos;
	Point curJumpPos;
	Point curMovePos;


	Character* mainChar;
	Layer* canvas;
	Sprite *cameraTarget;
	Vec2 centerPos;	//원래 있어야 할 버튼 자리
	Vec2 buttonPos;


	DrawNode* draw_tmp;
	//double defaultTimeScale;
public:

	JoyController() {};
	~JoyController() ;

	virtual bool init();
	virtual void onEnter();

	void update(float dt);




	void setCanvas(Layer* canvas) { this->canvas = canvas; }
	Layer* getCanvas() { return this->canvas; }
	void setCameraTarget(Sprite* cameraTarget) { this->cameraTarget = cameraTarget; }
	Sprite* getCameraTarget() { return this->cameraTarget; }
	void setMainChar(Character* ch) { this->mainChar = ch; }

	void setVelocity(float v) { this->velocity = v; }
	void setControllerShow(bool check) { this->isControllerShow = check; }
	void setLimitScreen(bool check) { this->isLimitScreen = check; }
	bool chkLimitScreen() { return this->isLimitScreen; }
	bool chkMoveBound(Point touch);

	Sprite* getBtnJump() { return this->sprBtnJump; }
	Sprite* getBtnDefense() { return this->sprBtnDefense; }

	//각 버튼을 활성화, 비활성화 시켜주는 함수. (flag 전환 + 버튼 이미지 전환)
	//void activateBtnLeft();
	//void activateBtnRight();
	void activateBtnJump();
	void activateBtnAttack();
	void activateBtnDefense();
	void activateBtnSpc();


	//void inActivateBtnLeft();
	//void inActivateBtnRight();
	void inActivateBtnJump();
	void inActivateBtnAttack();
	void inActivateBtnDefense();
	void inActivateBtnSpc();


	bool isListenerTargetTouched(Touch* touch, Event* unused_event);

	EventListenerTouchOneByOne * listener_Move;
	virtual bool onTouchBegan_Move(Touch* touch, Event* unused_event);
	virtual void onTouchMoved_Move(Touch* touch, Event* unused_event);
	virtual void onTouchEnded_Move(Touch* touch, Event* unused_event);


	EventListenerTouchOneByOne * listener_Jump;
	virtual bool onTouchBegan_Jump(Touch* touch, Event* unused_event);
	virtual void onTouchMoved_Jump(Touch* touch, Event* unused_event);
	virtual void onTouchEnded_Jump(Touch* touch, Event* unused_event);
	virtual void onTouchCancelled_Jump(Touch* touch, Event* unused_event);


	EventListenerTouchOneByOne * listener_Attack;
	virtual bool onTouchBegan_Attack(Touch* touch, Event* unused_event);
	virtual void onTouchMoved_Attack(Touch* touch, Event* unused_event);
	virtual void onTouchEnded_Attack(Touch* touch, Event* unused_event);
	virtual void onTouchCancelled_Attack(Touch* touch, Event* unused_event);


	EventListenerTouchOneByOne * listener_Defense;
	virtual bool onTouchBegan_Defense(Touch* touch, Event* unused_event);
	virtual void onTouchMoved_Defense(Touch* touch, Event* unused_event);
	virtual void onTouchEnded_Defense(Touch* touch, Event* unused_event);
	virtual void onTouchCancelled_Defense(Touch* touch, Event* unused_event);




	CREATE_FUNC(JoyController);
};

#endif // !__JOYCONTROLLER_H__
