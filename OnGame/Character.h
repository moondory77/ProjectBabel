#ifndef __CHARACTER_H__
#define __CHARACTER_H__
#include "cocos2d.h"
#include "OnGame/CollisionDetector.h"

#define MOTION_ACTION 1001
#define	WEAPON_ROT(a) (a+90.0f)

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

//Animation 형태의 Motion Action 
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
	BODY = 0x01,
	CIRCLE = 0x02,
	WEAPON = 0x04,
};


class Character : public Sprite
{
	CollisionDetector* weaponDetector;
	SpriteBatchNode* batchNode;
	Layer* UICanvas;

	Point prevPos;				//(운동 추적 위한) (이전 프레임의) 위치
	int stateStack;				//캐릭터의 상태 스택

	///# 모든 Sprite의 Scale은, winSize의 width에 대한 비율을 기준으로 한다
	float bodyScale;
	float curAltitude;		//현재 지면 위치 (y축)


	charActionType curCharAction;
	Action* shakeCanvas;

	struct {
		Sprite *Body;
		Sprite *Circle;
		Sprite *Weapon;
	}BodyPart;

	struct {	/* 인게임 내 각종 능력치 */
		int powerSP;
		int powerNormal;
		float ceilingHeight;	///점프 상승 한계 height
	}AbilityStatus;


	struct {	/* 운동상태 변화에 영향을 미치는 value */
		float jumpVelo;				///점프 시작 속도  (y축)
		float curVeloY;				///현재 속도(y축)
		float outerVeloY;			///충돌 시, 외부로부터 받는 delta velocity (y축)
		float curGravity;
	}Momentum;


	struct {	/* onGame에서 유동적으로 변하는 무기의 scale */
		float min;
		float max = 0.2f;
		float cur;
	}WeaponScale;


	struct {	/* 1회 공격&방어 보장을 위한 유니크 아이디 */
		int attackID = 0;
		int defenseID = 0;
		int specialID = 0;
	}CommandID;



	struct {	/* 장착 weapon에 대해, 유효 타격 min~max 범위 */
		float min;
		float max;
	}atkScopeRadius;

	struct {
		float min = CC_DEGREES_TO_RADIANS(30.0f);
		float max = CC_DEGREES_TO_RADIANS(180.0f);
	}atkScopeAngle;



	unsigned char motionLockBit = 0;

	//구성액션 플래그들로부터 전체 모션 액션이 진행중인지를 판단해주는 플래그
	bool isMotionPlaying = false;
	bool isSpcIntroPlaying = false;	//스페셜 어택 모션 플레이 중 플래그



	//Controller에서 들어오는 Input
	map<charStateType, unsigned char> inputState = {
		{ NONE , 0x00 },
		{ MOVE, 0x00 },
		{ ATTACK , 0x00 },{ DEFENSE , 0x00 },
		{ SPC_ATK , 0x00 },{ JUMP , 0x00 },
		{ CRASH , 0x00 },{ PRESSED , 0x00 },
		{ DEAD , 0x00 }
	};

	//Circle Rot + Weapon Rot
	map<charActionType, pair<float, float>> finishRotation = {
		{ actNone			, { 28.0f, OnCircleRot(-80.0f)}},		//Default 상태의 circle, weapon 각도

		{ actJumpReady		, { 40.0f, OnCircleRot(-96.0f) }},
		{ actJumpUp			, { 48.0f, OnCircleRot(-74.0f) }},
		{ actJumpDown		, { -28.0f, OnCircleRot(-30.0f)}},

		{ actAtkLandReady	, { -50.0f, OnCircleRot(-1.0f) }},
		{ actAtkLandHit		, { -270.0f, OnCircleRot(104.0f) }},
		{ actAtkJumpReady	, { -50.0f, OnCircleRot(-1.0f) }},
		{ actAtkJumpHit		, { -270.0f, OnCircleRot(104.0f) }},

		{ actDefLand		, { 35.0f, OnCircleRot(27.0f) }},
		{ actDefJump		, { 53.0f, OnCircleRot(27.0f) }},

		{ actMoveLeft		, { 28.0f, OnCircleRot(-74.0f) }},
		{ actMoveRight		, { 160.0f, OnCircleRot(62.0f) }}
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

	inline float OnCircleRot(float origin_wp_rot) { return origin_wp_rot + 90.0f; }

public:


	Character();
	Character(Point initPos, float gravity, float jumpVelocity, int specialDamage);
	~Character();

	float lapsedAtkTick;			//공격 애니메이션이 진행되는 시간 지표 (0 ~ 1 까지 변화)
	int lapsedAtkScore;

	struct {	/* rough 한 형태로 tracking을 위한 AABB 형태의 레이더  */
		Layer* attack;
		Layer* special;
		Layer* defense;
	}Radar;

	struct {
		Layer* body;
		bool isCrashed[4];	
	}Collider;

	inline void clearCollider() { for (int i = 0; i != 4; i++) { Collider.isCrashed[i] = false; } };


	float curDefPoint = 0;		///현재 방어 게이지
	float curSpecPoint = 0;		///현재 필살기 게이지
	float maxDefPoint = 5000;
	float maxSpecPoint;


	void addState(charStateType aState);
	void delState(charStateType dState);

	void positionUpdate(float deltaTime);
	void stateUpdate(float deltaTime);
	void stateTransitionUpdate();

	void setUICanvas(Layer* ui_canvas) { UICanvas = ui_canvas; };
	Layer* getUICanvas() { return UICanvas; }

	/* 캐릭터 모션/State 관련 함수 */
	void initMotionFrames();							//캐릭터 액션 애니메이션에 필요한 프레임 삽입
	void playCharAction(charActionType targetAction);	//charAction에 대해 Animation 액션을 실행
	void setCharFrame(charActionType targetAction);		//charAction에 대해 Frame으로 교체 (각 Action이 끝났을 때)
	void changeCharMotion(bool isAdd, charStateType targetState); //State의 Add/Delete 에 맞게, 모션(Animation/Frame) 교체(true - addState, false - delState)

	//대상을 attack radar 체크해서 유효데미지 반환
	int chkAttackRadar(Rect& attackable_unit);
	int chkDefenseRadar(const Sprite& defensable_unit);


	int getState() { return this->stateStack; };
	void getMotionLock(charActionType input_action);
	void releaseMotionLock(charActionType input_action, charMotionElement target);

	bool getSpcIntroPlaying() { return isSpcIntroPlaying; }
	void setSpcIntroPlaying(bool isSpcIntroPlaying) { this->isSpcIntroPlaying = isSpcIntroPlaying; }
	bool getMotionPlaying() { return isMotionPlaying; }
	void setMotionPlaying(bool isMotionPlaying) { this->isMotionPlaying = isMotionPlaying; }


	//void setPosition(Point newPos) { this->setPosition(newPos); };
	//void setPositionX(float newX) { sprChar->setPositionX(newX); };
	//void setPositionY(float newY) { sprChar->setPositionY(newY); };

	//Point getSprPosition() { return sprChar->getPosition(); }
	//float getSprPositionX() { return sprChar->getPositionX(); }
	//float getSprPositionY() { return sprChar->getPositionY(); }

	////캐릭터 객체 전체를 이동
	//void setPosition(Point newPos) { setSprPosition(newPos); setColliderPosition(newPos); };
	//void setPositionX(float newX) { setSprPositionX(newX); setColliderPositionX(newX); };
	//void setPositionY(float newY) { setSprPositionY(newY); setColliderPositionY(newY); };



	//캐릭터 collider만 이동
	void setColliderPosition(Point newPos) { Collider.body->setPosition(newPos); };
	void setColliderPositionX(float newX) { Collider.body->setPositionX(newX); };
	void setColliderPositionY(float newY) { Collider.body->setPositionY(newY); };

	Point getColliderPosition() { return Collider.body->getPosition(); }
	float getColliderPositionX() { return Collider.body->getPositionX(); }
	float getColliderPositionY() { return Collider.body->getPositionY(); }


	Point getPrevPos() { return this->prevPos; }
	float getAltitude() { return this->curAltitude; }

	inline float getTopY() { return Collider.body->getBoundingBox().getMaxY(); }
	inline float getMidY() { return Collider.body->getBoundingBox().getMidY(); }
	inline float getBottomY() { return Collider.body->getBoundingBox().getMinY(); }
	inline float getRightX() { return Collider.body->getBoundingBox().getMaxX(); }
	inline float getMidX() { return Collider.body->getBoundingBox().getMidX(); }
	inline float getLeftX() { return Collider.body->getBoundingBox().getMinX(); }
	
	inline Point getTopPoint() { return Point(getMidX(), getTopY()); }
	inline Point getRightPoint() { return Point(getRightX(), getMidY()); }
	inline Point getBottomPoint() { return Point(getMidX(), getBottomY()); }
	inline Point getLeftPoint() { return Point(getLeftX(), getMidY()); }


	float getLimitHeight() { return this->AbilityStatus.ceilingHeight; }
	float getJumpVelo() { return this->Momentum.jumpVelo; }
	float getGravity() { return this->Momentum.curGravity; }

	float getVeloY() { return this->Momentum.curVeloY; }
	void setVeloY(float v) { this->Momentum.curVeloY = v; }
	void setOuterVeloY(float v) { this->Momentum.outerVeloY = v; };



	Size getColliderSize() { return Collider.body->getBoundingBox().size; }
	float getColliderWidth() { return getColliderSize().width; }
	float getColliderHeight() { return getColliderSize().height; }

	Size getWpSize() { return BodyPart.Weapon ->getBoundingBox().size; }	
	float getWpWidth() { return getWpSize().width; };
	float getWpHeight() { return getWpSize().height; }

	float getWpLengthScale() {
		return WeaponScale.cur / BodyPart.Weapon->getContentSize().width * BodyPart.Weapon->getContentSize().height; 
	}
	float getWpScale() { return WeaponScale.cur; }
	//float getDefWpScale() { return defWpScale; }
	//float getMaxWpScale() { return maxWpScale; }

	void setWpScale(float wp_scale);
	float getBodyScale() { return bodyScale; }




	//공격, 방어 공격 횟수 체크를 위한 ID 함수
	void updateAttackID() {
		if (CommandID.attackID > 5)
			CommandID.attackID = 0;
		else
			CommandID.attackID++;
		CCLOG("attack ID is %d", CommandID.attackID);
	}
	void updateDefenseID() {
		if (CommandID.defenseID > 5)
			CommandID.defenseID = 0;
		else
			CommandID.defenseID++;
	}
	void updateSpecialID(float dt) {
		//if (specialID > 10)
		//	specialID = 0;
		//else
		CommandID.specialID++;
		CCLOG("Special Id updated!! : %d", CommandID.specialID);
	}

	int getSpecialID() { return CommandID.specialID; }
	int getAttackID() { return CommandID.attackID; }
	int getDefenseID() { return CommandID.defenseID; }

	int getSpecialPoint(int combo);
	int getPowerSP() { return AbilityStatus.powerSP; };
	int getPowerNormal() { return AbilityStatus.powerNormal; };


	//디펜스 게이지 함수
	void decreaseDefPoint(float deltaTime);
	void increaseDefPoint(float deltaTime);

	/* State Machine 관련 */
	void delAllState() { this->stateStack = 1; };

	bool isCritical() { return RandomHelper::random_int(1, 100) <= 30; };		//크리티컬 판정
	bool isAttack() { return (this->stateStack % ATTACK == 0); };
	bool isJump() { return (this->stateStack % JUMP == 0); };
	bool isPressed() { return (this->stateStack % PRESSED == 0); };
	bool isDefense() { return (this->stateStack % DEFENSE == 0); }
	bool isSpcAttack() { return (this->stateStack % SPC_ATK == 0); };
	bool isCrash() { return (this->stateStack % CRASH == 0); };
	bool isDead() { return (this->stateStack %DEAD == 0); };
	bool isMove() { return (this->stateStack % MOVE == 0); };
	bool isAtLand() { return (getPositionY() <= curAltitude); };
	//bool isCritical();		//크리티컬 판정
	//bool isSpcAttack() ;
	//bool isDead() ;
	//bool isMove() ;


	void callback_tick_AtkScore(float deltaTime);	//Hit액션 중, delta-time에 따른 공격범위 변화 스케쥴
	void callback_tick_AtkCharge(float deltaTime);
	void callback_tick_AtkRelease(float deltaTime);
	//void callback_tick_AtkConsume(float deltaTime);

	void callback_FinishSpcIntro(Ref* sender);
	void callback_DelSpc(Ref* sender);
	//void callback_DelCrash(Ref* sender);
	//void callback_DelPress(Ref* sender);

	//State Bit 전체 초기화
	inline void initStateBit(charStateType state) {
		this->inputState[state] = 0x00;
	};
	//State Bit(0 ~ 7 bit) n번째 비트를 set on 
	inline void setOnStateBit(charStateType state, unsigned char target_bit) {
		inputState[state] |= (0x01 << target_bit);
	};
	//State Bit(0 ~ 7 bit) n번째 비트를 set off
	inline void setOffStateBit(charStateType state, unsigned char target_bit) {
		inputState[state] &= ~(0x01 << target_bit);
	}
	//State Bit(0 ~ 7 bit) n번째 비트 on/off 여부 반환 
	inline bool getOnStateBit(charStateType state, unsigned char target_bit) {
		return inputState[state] & (0x01 << target_bit);
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
		unsigned char trigger = state_bit &= ~0x0f;		//input bit 부분은 전체 마스킹

		return (input == trigger);						//input 비트와 trigger 비트 동일 여부
	};

	//input - trigger dirty 여부 반환
	inline bool getDirtyInputTrigger(charStateType state, bool input_or_trigger) {
		unsigned char state_bit = inputState[state];
		if (input_or_trigger) {
			unsigned char input = state_bit << 4;
			return input != 0x00;
		}
		else {
			unsigned char trigger = state_bit &= ~0x0f;	//input_bit 부분 마스킹(off 처리)
			return trigger != 0x00;
		}
	};

};

#endif
