#ifndef __CHARACTER_H__
#define __CHARACTER_H__
#include "cocos2d.h"

#define MOTION_ACTION 1001

USING_NS_CC;
using namespace std;

//캐릭터 FSM의 Base State
enum charStateType {
	NONE = 1, MOVE = 2,
	ATTACK = 7, DEFENSE = 11,
	SPC_ATK = 13,
	JUMP = 17, CRASH = 29,
	DEAD = 31, PRESSED = 37
};

//모션교체 State-Machine에서, animation 형태의 Action 
enum charActionType {
	actNone,

	actJumpReady, actJumpCancel,
	actJumpUp, actJumpDown,

	actAtkLandReady, actAtkLandHit, actAtkLandBack, actAtkLandCancel,
	actAtkJumpReady, actAtkJumpHit, actAtkJumpBack, actAtkJumpCancel,

	actDefLand, actDefLandEnd,
	actDefJump, actDefJumpEnd,

	actSPCLand, actSPCLandEnd,
	actSPCJump, actSPCJumpEnd,

	actMoveLeft, actMoveLeftEnd,
	actMoveRight, actMoveRightEnd
};



//MotionLock에 대한, 각 Atomic Action의 bit 위치
enum charMotionElement
{
	ANIMATION = 1,
	CIRCLE = 2,
	WEAPON = 4,
};



class Character : public CCNode
{
private:

	int state;				//캐릭터의 상태 스택
	Point prevPos;			//운동 변화 추적을 위한 (이전 프레임의) 위치

	///# 모든 Sprite의 Scale은, winSize의 width에 대한 비율을 기준으로 한다
	float bodyScale;

	float defWpScale;			//현재 status에 대한, 디폴트 스케일
	float maxWpScale = 0.2f;
	float curWpScale;

	float limitHeight;		//점프 상승 한계 height
	float landHeight;	//지면 위치 (y축)

	float jumpVelo;				//점프 시작 속도  (y축)
	float curVeloY;				//현재 속도(y축)
	float outerVeloY;			//충돌 시, 외부로부터 받는 delta velocity (y축)
	float curGravity;

	int powerSP;
	int powerNormal;

	//1회 공격&방어 보장을 위한 유니크 아이디
	int attackID = 0;
	int defenseID = 0;
	int specialID = 0;

	unsigned char motionLockBit = 0;
	//구성액션 플래그들로부터 전체 모션 액션이 진행중인지를 판단해주는 플래그
	bool isMotionPlaying = false;
	bool isSpcIntroPlaying = false;	//스페셜 어택 모션 플레이 중 플래그
	SpriteBatchNode* batchNode;


	//Controller에서 들어오는 Input
	map<charStateType, unsigned char> inputState = {
		{ NONE , 0 },
		{ MOVE, 0 },
		{ ATTACK , 0 },{ DEFENSE , 0 },
		{ SPC_ATK , 0 },{ JUMP , 0 },
		{ CRASH , 0 },{ PRESSED , 0 },
		{ DEAD , 0 }
	};

	map<charActionType, pair<float, float>> finishRotation = {
		{ actNone			, { 41.0f, 4.0f}},
		{ actJumpReady		, { 38.0f, 18.0f }},
		{ actJumpUp			, { 38.0f, 16.0f }},
		{ actJumpDown		, { -20.0f, 40.0f }},
		{ actAtkLandReady	, { -45.0f, 80.0f }},
		{ actAtkLandHit		, { -268.0f, 200.0f }},
		{ actAtkJumpReady	, { -42.0f, 83.0f }},
		{ actAtkJumpHit		, { -268.0f, 200.0f }},
		{ actDefLand		, { 35.0f, 27.0f }},
		{ actDefJump		, { 53.0f, 27.0f }},
		{ actMoveLeft		, { 28.0f, 10.0f }},
		{ actMoveRight		, { 151.0f, 170.0f }}
	};

	map<charActionType, Vector<SpriteFrame*>> motionFrames = {
		{ actNone , {}},
		{ actJumpReady ,{} },		{ actJumpCancel ,{} },
		{ actJumpUp ,{} },			{ actJumpDown ,{} },

		{ actAtkLandReady ,{} },	{ actAtkLandHit ,{} },
		{ actAtkLandBack ,{} },		{ actAtkJumpReady ,{} },
		{ actAtkJumpHit ,{} },		{ actAtkJumpBack ,{} },

		{ actDefLand ,{} },			{ actDefLandEnd ,{} },
		{ actDefJump ,{} },			{ actDefJumpEnd ,{} },

		{ actSPCLand ,{} },			{ actSPCLandEnd ,{} },
		{ actSPCJump ,{} },			{ actSPCJumpEnd ,{} },

		{ actMoveLeft ,{} },		{ actMoveLeftEnd ,{} },
		{ actMoveRight ,{} },		{ actMoveRightEnd ,{} },
	};

	map<charActionType, SpriteFrame*> finishFrame = {
		{ actNone , NULL },
		{ actAtkLandReady ,NULL }, { actAtkLandHit ,NULL },
		{ actAtkJumpReady ,NULL }, { actAtkJumpHit ,NULL },

		{ actJumpReady ,NULL },	{ actJumpUp ,NULL },
		{ actJumpDown ,NULL },

		{ actDefLand ,NULL }, { actDefJump ,NULL },
		{ actSPCLand ,NULL }, { actSPCJump ,NULL },

		{ actMoveLeft ,NULL }, { actMoveRight ,NULL },
	};

public:

	Character();
	Character(Point initPos, float gravity, float jumpVelocity, int specialDamage, Layer* canvas);
	~Character();

	Action* shakeCanvas;
	Layer* canvas;

	void addState(charStateType aState);
	void delState(charStateType dState);

	void positionUpdate(float deltaTime);
	void stateUpdate(float deltaTime);
	void stateTransitionUpdate();


	/* 캐릭터 모션/State 관련 함수 */
	void initMotionFrames();							//캐릭터 액션 애니메이션에 필요한 프레임 삽입
	void playCharAction(charActionType targetAction);	//charAction에 대해 Animation 액션을 실행
	void setCharFrame(charActionType targetAction);		//charAction에 대해 Frame으로 교체 (각 Action이 끝났을 때)
	void changeCharMotion(bool isAdd, charStateType targetState); //State의 Add/Delete 에 맞게, 모션(Animation/Frame) 교체(true - addState, false - delState)

	charActionType curCharAction;

	float curDefPoint = 0;		//현재 방어 게이지
	float curSpecPoint = 0;	//현재 필살기 게이지
	float maxDefPoint = 5000;
	float maxSpecPoint;


	Layer* colliderChar;
	Sprite *sprChar, *sprCircle, *sprWeapon;
	Layer* attackRadar;
	Layer* specialRadar;
	Layer* shieldRadar;


	//int getAttackDamage(Block* blk);
	//대상을 attack radar 체크해서 유효데미지 반환
	int chkAttckRadar(const Sprite& attackable_unit);
	int chkDefenseRadar(const Sprite& defensable_unit);


	//현재 장착 weapon에 대해, 유효 타격 min~max 범위
	pair<float, float> atkScopeRadius;
	pair<float, float> atkScopeAngle = { -45.0f, 178.0f };
	float lapsedAtkTick;			//공격 애니메이션이 진행되는 시간 지표 (0 ~ 1 까지 변화)

	int getState() { return state; };
	void getMotionLock(charActionType input_action);
	void releaseMotionLock(charActionType input_action, charMotionElement target);

	bool getSpcIntroPlaying() { return isSpcIntroPlaying; }
	void setSpcIntroPlaying(bool isSpcIntroPlaying) { this->isSpcIntroPlaying = isSpcIntroPlaying; }
	bool getMotionPlaying() { return isMotionPlaying; }
	void setMotionPlaying(bool isMotionPlaying) { this->isMotionPlaying = isMotionPlaying; }


	//캐릭터 sprChar만 이동
	void setSprPosition(Point newPos) { sprChar->setPosition(newPos); };
	void setSprPositionX(float newX) { sprChar->setPositionX(newX); };
	void setSprPositionY(float newY) { sprChar->setPositionY(newY); };

	Point getSprPosition() { return sprChar->getPosition(); }
	float getSprPositionX() { return sprChar->getPositionX(); }
	float getSprPositionY() { return sprChar->getPositionY(); }

	//캐릭터 collider만 이동
	void setColliderPosition(Point newPos) { colliderChar->setPosition(newPos); };
	void setColliderPositionX(float newX) { colliderChar->setPositionX(newX); };
	void setColliderPositionY(float newY) { colliderChar->setPositionY(newY); };

	Point getColliderPosition() { return colliderChar->getPosition(); }
	float getColliderPositionX() { return colliderChar->getPositionX(); }
	float getColliderPositionY() { return colliderChar->getPositionY(); }


	//캐릭터 객체 전체를 이동
	void setPosition(Point newPos) { setSprPosition(newPos); setColliderPosition(newPos); };
	void setPositionX(float newX) { setSprPositionX(newX); setColliderPositionX(newX); };
	void setPositionY(float newY) { setSprPositionY(newY); setColliderPositionY(newY); };


	//Vec2 getMomentVec() { return getColliderPosition() - prevPos; }
	Point getPrevPos() { return prevPos; }
	float getLandY() { return landHeight; }


	float getTopY() { return colliderChar->getBoundingBox().getMaxY(); }
	float getBottomY() { return colliderChar->getBoundingBox().getMinY(); }
	float getRightX() { return colliderChar->getBoundingBox().getMaxX(); }
	float getLeftX() { return colliderChar->getBoundingBox().getMinX(); }




	float getLimitHeight() { return limitHeight; }
	float getJumpVelo() { return jumpVelo; }
	float getVeloY() { return curVeloY; }
	void setVeloY(float v) { curVeloY = v; }
	float getGravity() { return curGravity; }

	void setOuterVeloY(float v) {
		outerVeloY = v;
		/*	if (v <= 0)
		outerVelocity = v;
		else
		outerVelocity = -1.0f;*/
	};
	Point getCharPosition() { return sprChar->getPosition(); }



	Size getBodySize() { return colliderChar->getBoundingBox().size; }
	float getBodyWidth() { return getBodySize().width; }
	float getBodyHeight() { return getBodySize().height; }

	//sprChar의 스케일이 적용된 사이즈
	Size getWpSize() { return sprWeapon->getBoundingBox().size; }	//무기 Sprite와 그에따른 레이더의 크기 함께 Scale 
	float getWpWidth() { return getWpSize().width; };
	float getWpHeight() { return getWpSize().height; }

	float getWpLengthScale() { return curWpScale * sprWeapon->getContentSize().height / sprWeapon->getContentSize().width; }
	float getWpScale() { return curWpScale; }
	float getDefWpScale() { return defWpScale; }
	float getMaxWpScale() { return maxWpScale; }

	void setWpScale(float wp_scale);
	float getBodyScale() { return bodyScale; }




	//공격, 방어 공격 횟수 체크를 위한 ID 함수
	void updateAttackID() {
		if (attackID > 5)
			attackID = 0;
		else
			attackID++;
		CCLOG("attack ID is %d", attackID);
	}
	void updateDefenseID() {
		if (defenseID > 5)
			defenseID = 0;
		else
			defenseID++;
	}
	void updateSpecialID(float dt) {
		//if (specialID > 10)
		//	specialID = 0;
		//else
		specialID++;
		CCLOG("Special Id updated!! : %d", this->specialID);
	}

	int getSpecialID() { return specialID; }
	int getAttackID() { return attackID; }
	int getDefenseID() { return defenseID; }


	int getSpecialPoint(int combo);
	int getPowerSP() { return powerSP; };
	int getPowerNormal() { return powerNormal; };



	//디펜스 게이지 함수
	void decreaseDefPoint(float deltaTime);
	void increaseDefPoint(float deltaTime);


	/* State Machine 관련 */
	void delAllState() { this->state = 1; };
	
	bool isCritical() { return RandomHelper::random_int(1, 100) <= 30; };		//크리티컬 판정
	bool isAttack() { return (this->state % ATTACK == 0); };
	bool isJump() { return (this->state % JUMP == 0); };

	bool isPressed() { return (this->state % PRESSED == 0); };
	bool isDefense() { return (this->state % DEFENSE == 0); }
	bool isSpcAttack() { return (this->state % SPC_ATK == 0); };
	bool isCrash() { return (this->state % CRASH == 0); };
	bool isDead() { return (this->state %DEAD == 0); };
	bool isMove() { return (this->state % MOVE == 0); };
	bool isAtLand() { return (getSprPositionY() <= landHeight); };

	//bool isCritical();		//크리티컬 판정
	//bool isSpcAttack() ;
	//bool isDead() ;
	//bool isMove() ;

	void callback_tick_AtkScope(float deltaTime);
	void callback_tick_AtkCharge(float deltaTime);
	void callback_tick_AtkRelease(float deltaTime);
	//void callback_tick_AtkConsume(float deltaTime);

	void callback_FinishSpcIntro(Ref* sender);
	void callback_DelSpc(Ref* sender);
	void callback_DelCrash(Ref* sender);
	void callback_DelPress(Ref* sender);


	//State Bit 전체 초기화
	inline void initStateBit(charStateType state) { this->inputState[state] = 0; };
	//State Bit(0 ~ 7 bit) n번째 비트를 set on 
	inline void setOnStateBit(charStateType state, unsigned char target_bit) {
		inputState[state] |= (unsigned char)pow(2, target_bit);
	};
	//State Bit(0 ~ 7 bit) n번째 비트를 set off
	inline void setOffStateBit(charStateType state, unsigned char target_bit) {
		inputState[state] &= ~(unsigned char)pow(2, target_bit);
	}
	//State Bit(0 ~ 7 bit) n번째 비트 on/off 여부 반환 
	inline bool getOnStateBit(charStateType state, unsigned char target_bit) {
		return inputState[state] & (unsigned char)pow(2, target_bit);
	}


	//input bit(4 bit)에 대한 on/off 
	inline void setOnStateInput(charStateType state, unsigned char target_bit) {
		setOnStateBit(state, target_bit % 4);
	}
	inline void setOffStateInput(charStateType state, unsigned char target_bit) {
		setOffStateBit(state, target_bit % 4);
	}
	inline bool getOnStateInput(charStateType state, unsigned char target_bit) {
		return getOnStateBit(state, target_bit % 4);
	}

	//trigger(4 bit)에 대한 on/off
	inline void setOnStateTrigger(charStateType state, unsigned char target_bit) {
		setOnStateBit(state, (target_bit % 4) + 4);
	};
	inline void setOffStateTrigger(charStateType state, unsigned char target_bit) {
		setOffStateBit(state, (target_bit % 4) + 4);
	};
	inline bool getOnStateTrigger(charStateType state, unsigned char target_bit) {
		return getOnStateBit(state, (target_bit % 4) + 4);
	}

	//input - trigger 비교 반환
	inline bool getEqualInputTrigger(charStateType state) {

		unsigned char state_bit = inputState[state];
		unsigned char input = state_bit << 4;
		unsigned char trigger = state_bit &= ~15;		//input bit 부분은 전체 마스킹

		return (input == trigger);						//input 비트와 trigger 비트 동일 여부
	};
	//input - trigger dirty 여부 반환
	inline bool getDirtyInputTrigger(charStateType state, bool input_or_trigger) {

		unsigned char state_bit = inputState[state];

		if (input_or_trigger) {
			unsigned char input = state_bit << 4;
			return input != 0;
		}
		else {
			//input_bit 부분 마스킹(off 처리)
			unsigned char trigger = state_bit &= ~15;
			return trigger != 0;
		}
	};



};

#endif
