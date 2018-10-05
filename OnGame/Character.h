#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "cocos2d.h"
#include "OnGame/CollisionDetector.h"
#include "OnGame/CustomAction.h"

#define MOTION_ACTION 1001
#define	WEAPON_ROT(a) (a+90.0f)

USING_NS_CC;
using namespace std;


//캐릭터 FSM의 Base State
enum CharStateType
{
	NONE = 1, MOVE = 2,
	ATTACK = 7, DEFENSE = 11,
	SPC_ATK = 13,
	JUMP = 17, CRASH = 29,
	DEAD = 31, PRESSED = 37
};


class Character : public Sprite
{
	//MotionLock에 대한, 각 Atomic Action의 bit 위치
	enum MotionElement
	{
		BODY = 0x01,
		CIRCLE = 0x02,
		WEAPON = 0x04,
	};

	//Animation 형태의 Motion Action 
	enum MotionActionType {
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


	//** 충돌에 의한, 다수의 position 갱신을 optimizing 하기 위한 클래스
	class BodyCollider : public LayerColor
	{
	private:
		float bodyAngle;	//캐릭터 콜라이더(AABB)의 가로-세로 사잇각

	public:
		BodyCollider(float x_scale, float y_scale);
		~BodyCollider() {};

		bool isCrashed[4];	// top - right - bottom - left

		float getBodyAngle() { return bodyAngle; }
		inline void clearCollider() { for (int i = 0; i != 4; i++) { isCrashed[i] = false; } };
		inline Size getSize() { return this->getBoundingBox().size; }
		inline float getWidth() { return getSize().width; }
		inline float getHeight() { return getSize().height; }

		inline float getTopY() { return this->getBoundingBox().getMaxY(); }
		inline float getMidY() { return this->getBoundingBox().getMidY(); }
		inline float getBottomY() { return this->getBoundingBox().getMinY(); }
		inline float getRightX() { return this->getBoundingBox().getMaxX(); }
		inline float getMidX() { return this->getBoundingBox().getMidX(); }
		inline float getLeftX() { return this->getBoundingBox().getMinX(); }

		inline Vec2 getTopPostion() { return Point(getMidX(), getTopY()); }
		inline Vec2 getRightPosition() { return Point(getRightX(), getMidY()); }
		inline Vec2 getBottomPosition() { return Point(getMidX(), getBottomY()); }
		inline Vec2 getLeftPosition() { return Point(getLeftX(), getMidY()); }
		inline Vec2 getPosition() { return Point(getMidX(), getMidY()); }
	};

	//** 캐릭터 모션을 형성하는 3가지 (atomic action)을 하나로 바인딩
	class AtomicMotionAction : public Node
	{
	private:
		Character* Host;	//외부클래스의 비정적 멤버참조를 위해

	public:
		AtomicMotionAction() {};
		~AtomicMotionAction() {};

		CustomSequence* actionBody;
		CustomSequence* actionCircle;
		CustomSequence* actionWeapon;

		MotionActionType curMotion;
		unsigned char motionLockBit = 0;
		bool isMotionPlaying = false;		//전체 모션액션 진행 플래그

		void setHost(Character* host) { Host = host; };
		Character* getHost() { return this->Host; };

		void getMotionLock(MotionActionType input_action);
		void releaseMotionLock(MotionActionType input_action, MotionElement target);

		//factor에 의해 진행속도 왜곡
		inline void skewMotionSpeed(float skew_factor)
		{
			if (skew_factor < 0.05f) skew_factor = 0.05f;
			if (skew_factor > 3.0f) skew_factor = 3.0f;

			CCLOG("skew factor is %.3f", skew_factor);

			actionBody->setSkewFactor(skew_factor);
			actionCircle->setSkewFactor(skew_factor);
			actionWeapon->setSkewFactor(skew_factor);
		};

		//sprite에 setting된 모션 액션을 play
		inline void runMotion(Sprite& body, Sprite& circle, Sprite& weapon) {
			actionBody->setTag(MOTION_ACTION);
			actionCircle->setTag(MOTION_ACTION);
			actionWeapon->setTag(MOTION_ACTION);

			body.runAction(actionBody);
			circle.runAction(actionCircle);
			weapon.runAction(actionWeapon);
		};
	};


private:

	Layer* UICanvas;
	Point prevPos;				//(이전 프레임의) 위치 (운동 추적 위해..)

	int stateStack;				//캐릭터의 상태 스택

	float bodyScale;			///# 모든 Sprite의 Scale은, winSize의 width에 대한 비율을 기준으로 한다
	float curAltitude;			//현재 지면 위치 (y축)
	Action* shakeCanvas;

	bool isSpcIntroPlaying = false;
	AtomicMotionAction motionAction;
	CollisionDetector* weaponDetector;


	/* 캐릭터 몸체를 구성하는 각 Sprite Part */
	struct {
		Sprite *Body;
		Sprite *Circle;
		Sprite *WpHandle;
		Sprite *WpBlade;
	}BodyPart;


	/* 인게임 내 각종 능력치 */
	struct {
		int powerSP;
		int powerNormal;
		float ceilingHeight;	///점프 상승 한계 height
	}AbilityStatus;

	/* 운동상태 변화에 영향을 미치는 value */
	struct {
		float jumpVelo;				///점프 시작 속도  (y축)
		float curVeloY;				///현재 속도(y축)
		float outerVeloY;			///충돌 시, 외부로부터 받는 delta velocity (y축)
		float curGravity;
	}Momentum;

	/* onGame에서 유동적으로 변하는 무기의 scale */
	struct {
		float min;
		float max = 0.2f;
		float cur;
	}WeaponScale;

	/* 1회 공격&방어 보장을 위한 유니크 아이디 */
	struct {
		int attackID = 0;
		int defenseID = 0;
		int specialID = 0;
	}CommandID;

	/* 장착 weapon에 대해, 유효 타격 min~max 범위 */
	struct {
		float min;
		float max;
		//float extraAngle = 0.0f;
	}atkScopeRadius;


	struct {
		float limitAngle = 5.0f;
		float curAngle = 0;
	}ExtraCircleRot;

	struct {
		float limitAngle = 9.0f;
		float curAngle = 0;
	}ExtraWeaponRot;


	inline float OnCircleRot(float origin_wp_rot) { return origin_wp_rot + 90.0f; }

	//Controller로부터 들어오는 Input
	map<CharStateType, unsigned char> inputState =
	{
		{ NONE , 0x00 },
		{ MOVE, 0x00 },
		{ ATTACK , 0x00 },{ DEFENSE , 0x00 },
		{ SPC_ATK , 0x00 },{ JUMP , 0x00 },
		{ CRASH , 0x00 },{ PRESSED , 0x00 },
		{ DEAD , 0x00 }
	};

	//Circle Rot + Weapon Rot
	map<MotionActionType, pair<float, float>> finishRotation =
	{
		{ actNone			,{ 28.0f, OnCircleRot(-80.0f) } },
		{ actJumpReady		,{ 40.0f, OnCircleRot(-96.0f) } },
		{ actJumpUp			,{ 48.0f, OnCircleRot(-74.0f) } },
		{ actJumpDown		,{ -28.0f, OnCircleRot(-30.0f) } },

		{ actAtkLandReady	,{ -50.0f, OnCircleRot(-1.0f) } },
		{ actAtkLandHit		,{ -270.0f, OnCircleRot(104.0f) } },
		{ actAtkJumpReady	,{ -50.0f, OnCircleRot(-1.0f) } },
		{ actAtkJumpHit		,{ -270.0f, OnCircleRot(104.0f) } },

		{ actDefLand		,{ 35.0f, OnCircleRot(27.0f) } },
		{ actDefJump		,{ 53.0f, OnCircleRot(27.0f) } },

		{ actMoveLeft		,{ 28.0f, OnCircleRot(-74.0f) } },
		{ actMoveRight		,{ 160.0f, OnCircleRot(62.0f) } }
	};

	map<MotionActionType, Vector<SpriteFrame*>> motionFrames = {
		{ actNone ,{} },
		{ actJumpReady ,{} },{ actJumpCancel ,{} },
		{ actJumpUp ,{} },{ actJumpDown ,{} },

		{ actAtkLandReady ,{} },{ actAtkLandHit ,{} },
		{ actAtkLandBack ,{} },{ actAtkJumpReady ,{} },
		{ actAtkJumpHit ,{} },{ actAtkJumpBack ,{} },

		{ actDefLand ,{} },{ actDefLandEnd ,{} },
		{ actDefJump ,{} },{ actDefJumpEnd ,{} },

		{ actSPCLand ,{} },{ actSPCLandEnd ,{} },
		{ actSPCJump ,{} },{ actSPCJumpEnd ,{} },

		{ actMoveLeft ,{} },{ actMoveLeftEnd ,{} },
		{ actMoveRight ,{} },{ actMoveRightEnd ,{} },
	};

	map<MotionActionType, SpriteFrame*> finishFrame = {
		{ actNone , NULL },
		{ actAtkLandReady ,NULL },{ actAtkLandHit ,NULL },
		{ actAtkJumpReady ,NULL },{ actAtkJumpHit ,NULL },

		{ actJumpReady ,NULL },{ actJumpUp ,NULL },
		{ actJumpDown ,NULL },

		{ actDefLand ,NULL },{ actDefJump ,NULL },
		{ actSPCLand ,NULL },{ actSPCJump ,NULL },

		{ actMoveLeft ,NULL },{ actMoveRight ,NULL },
	};


public:

	Character();
	Character(Point initPos, float gravity, float jumpVelocity, int specialDamage);
	~Character();

	struct {	/* 사전 rough-tracking을 위한 AABB 형태의 레이더  */
		Layer* attack;
		Layer* special;
		Layer* defense;
	}Radar;

	BodyCollider* bodyCollider;


	float lapsedTimeHit = 0;	//Hit 액션의 시간지표(0 ~ 1 까지 변화) 
	int lapsedDamageHit = 0;
	int tickDamageHit = 0;



	float curDefPoint = 0;		///현재 방어 게이지
	float curSpecPoint = 0;		///현재 필살기 게이지
	float maxDefPoint = 5000;
	float maxSpecPoint;


	void updatePosition(float deltaTime);
	void updateState(float deltaTime);
	void updateStateTransition();

	void addState(CharStateType aState);
	void delState(CharStateType dState);

	int getState() { return this->stateStack; };
	bool getSpcIntroPlaying() { return isSpcIntroPlaying; }
	void setSpcIntroPlaying(bool isSpcIntroPlaying) { this->isSpcIntroPlaying = isSpcIntroPlaying; }
	bool getMotionPlaying() { return motionAction.isMotionPlaying; }
	void setMotionPlaying(bool isMotionPlaying) { motionAction.isMotionPlaying = isMotionPlaying; }

	void setUICanvas(Layer* ui_canvas) { UICanvas = ui_canvas; };
	Layer* getUICanvas() { return UICanvas; }

	/* 캐릭터 모션/State 관련 함수 */
	void initMotionFrames();							//캐릭터 액션 애니메이션에 필요한 프레임 삽입	
	void changeCharMotion(bool isAdd, CharStateType targetState); //State의 Add/Delete 에 맞게, 모션(Animation/Frame) 교체(true - addState, false - delState)	
	void runMotionAction(MotionActionType targetAction);	//charAction에 대해 Animation 액션을 실행
	void setMotionFrame(MotionActionType targetAction);		//charAction에 대해 Frame으로 교체 (각 Action이 끝났을 때)

	int chkAttackRadar(Rect& attackable_unit);
	int chkDefenseRadar(const Sprite& defensable_unit);

	Point getPrevPos() { return this->prevPos; }
	float getAltitude() { return this->curAltitude; }

	float getLimitHeight() { return this->AbilityStatus.ceilingHeight; }
	float getJumpVelo() { return this->Momentum.jumpVelo; }
	float getGravity() { return this->Momentum.curGravity; }

	float getVeloY() { return this->Momentum.curVeloY; }
	void setVeloY(float v) { this->Momentum.curVeloY = v; }
	void setOuterVeloY(float v) { this->Momentum.outerVeloY = v; };

	Size getWpSize() { return BodyPart.WpBlade->getBoundingBox().size; }
	float getWpWidth() { return getWpSize().width * BodyPart.WpHandle->getScale(); };
	float getWpHeight() { return getWpSize().height * BodyPart.WpHandle->getScale(); };

	float getWpLengthScale() {
		return WeaponScale.cur / BodyPart.WpBlade->getContentSize().width * BodyPart.WpBlade->getContentSize().height;
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
		//CCLOG("attack ID is %d", CommandID.attackID);
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
		//CCLOG("Special Id updated!! : %d", CommandID.specialID);
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


	void callback_tick_Hit(float deltaTime);	//Hit액션 중, delta-time에 따른 공격범위 변화 스케쥴
	void callback_tick_AtkCharge(float deltaTime);
	void callback_tick_AtkRelease(float deltaTime);
	//void callback_tick_AtkConsume(float deltaTime);

	void callback_FinishSpcIntro(Ref* sender);
	void callback_DelSpc(Ref* sender);
	//void callback_DelCrash(Ref* sender);
	//void callback_DelPress(Ref* sender);

	//State Bit 전체 초기화
	inline void initStateBit(CharStateType state) {
		this->inputState[state] = 0x00;
	};
	//State Bit(0 ~ 7 bit) n번째 비트를 set on 
	inline void setOnStateBit(CharStateType state, unsigned char target_bit) {
		inputState[state] |= (0x01 << target_bit);
	};
	//State Bit(0 ~ 7 bit) n번째 비트를 set off
	inline void setOffStateBit(CharStateType state, unsigned char target_bit) {
		inputState[state] &= ~(0x01 << target_bit);
	}
	//State Bit(0 ~ 7 bit) n번째 비트 on/off 여부 반환 
	inline bool getOnStateBit(CharStateType state, unsigned char target_bit) {
		return inputState[state] & (0x01 << target_bit);
	}

	//input bit(4 bit)에 대한 on/off 
	inline void setOnStateInput(CharStateType state, unsigned char target_bit) {
		setOnStateBit(state, target_bit % 4);
	}
	inline void setOffStateInput(CharStateType state, unsigned char target_bit) {
		setOffStateBit(state, target_bit % 4);
	}
	inline bool getOnStateInput(CharStateType state, unsigned char target_bit) {
		return getOnStateBit(state, target_bit % 4);
	}

	//trigger(4 bit)에 대한 on/off
	inline void setOnStateTrigger(CharStateType state, unsigned char target_bit) {
		setOnStateBit(state, (target_bit % 4) + 4);
	};
	inline void setOffStateTrigger(CharStateType state, unsigned char target_bit) {
		setOffStateBit(state, (target_bit % 4) + 4);
	};
	inline bool getOnStateTrigger(CharStateType state, unsigned char target_bit) {
		return getOnStateBit(state, (target_bit % 4) + 4);
	}

	//input - trigger 비교 반환
	inline bool getEqualInputTrigger(CharStateType state) {

		unsigned char state_bit = inputState[state];
		unsigned char input = state_bit << 4;
		unsigned char trigger = state_bit &= ~0x0f;		//input bit 부분은 전체 마스킹

		return (input == trigger);						//input 비트와 trigger 비트 동일 여부
	};
	//input - trigger dirty 여부 반환
	inline bool getDirtyInputTrigger(CharStateType state, bool input_or_trigger) {
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
