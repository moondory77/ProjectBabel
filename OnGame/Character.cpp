#define FRAME_DELAY 0.034	//60프레임 기준 1프레임은 0.017초 

#include "Character.h"
#include "System/GameManager.h"
#include "System/BGManager.h"
#include "VFX/EffectManager.h"

Character::BodyCollider::BodyCollider(float x_scale, float y_scale) {
	assert(init());
	this->setScale(x_scale, y_scale / winAspectRatio());
	this->setColor(Color3B::BLUE);
	this->setOpacity(150);
	this->ignoreAnchorPointForPosition(false);
	this->setAnchorPoint(Vec2(0.5f, 0.5f));
	bodyAngle = CC_RADIANS_TO_DEGREES(ccpToAngle(Vec2(getWidth(), getHeight())));
};

Character::Character() {
	stateStack = (CharStateType)NONE;
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("RoyalWeapon.plist");
}

Character::Character(Point initPos, float gravity, float jumpVelocity, int powerSP) : Character()
{
	//본체 sprite 
	BodyPart.Body = this;

	///sprChar = Sprite::create("motions/default_land/1.png");
	assert(BodyPart.Body->initWithFile("motions/default_land/1.png"));
	bodyScale = 0.108f;
	BodyPart.Body->setScale(bodyScale * DivForHorizontal(this));
	BodyPart.Body->setAnchorPoint(Vec2(0.5f, 0.4f));	//콜라이더와의 위치 차 보정
	BodyPart.Body->setPosition(initPos);

	// sprCircle 
	BodyPart.Circle = Sprite::create("HeroCircle.png");
	BodyPart.Circle->setPosition(BodyPart.Body->getContentSize().width / 2, BodyPart.Body->getContentSize().height*0.38);
	BodyPart.Circle->setOpacity(0);
	BodyPart.Circle->setRotation(28);
	BodyPart.Body->addChild(BodyPart.Circle);

	// sprHandle
	BodyPart.WpHandle = Sprite::create("DefaultHandle.png");
	BodyPart.WpHandle->setRotation(OnCircleRot(-80.0f));
	BodyPart.WpHandle->setAnchorPoint(Vec2(0.5f, 0.7f));
	BodyPart.WpHandle->setScale(0.25f);
	BodyPart.WpHandle->setPosition(BodyPart.Circle->getContentSize().width*0.77f, BodyPart.Circle->getContentSize().height / 2);
	BodyPart.Circle->addChild(BodyPart.WpHandle);

	// sprBlade
	BodyPart.WpBlade = Sprite::create("DefaultBlade.png");
	BodyPart.WpBlade->setAnchorPoint(Vec2(0.5f, 0.0f));
	BodyPart.WpBlade->setPosition(BodyPart.WpHandle->getContentSize().width*0.5f, BodyPart.WpHandle->getContentSize().height * 0.99f);
	BodyPart.WpHandle->addChild(BodyPart.WpBlade);
	WeaponScale.min = 0.08f;
	WeaponScale.cur = WeaponScale.min;


	// Attack Radar (Attack-Range의 Rough 체크를 위한)
	Radar.attack = LayerColor::create();
	Radar.attack->setColor(Color3B::YELLOW);
	Radar.attack->ignoreAnchorPointForPosition(false);	///레이어의 앵커포인트 조절을 위해 필수
	Radar.attack->setAnchorPoint(Vec2(0.5, 0));
	Radar.attack->setPosition(BodyPart.Body->getContentSize().width / 2, BodyPart.Body->getContentSize().height*0.36);
	Radar.attack->setOpacity(0);
	BodyPart.Body->addChild(Radar.attack);


	// Defense Radar (Shield-Range의 체크를 위한)
	Radar.defense = LayerColor::create();
	Radar.defense->setColor(Color3B::GREEN);
	Radar.defense->ignoreAnchorPointForPosition(false);
	Radar.defense->setAnchorPoint(Vec2(0.5, 0));
	Radar.defense->setPosition(BodyPart.Body->getContentSize().width / 2, BodyPart.Body->getContentSize().height*0.36);
	float shield_radar_scale = bodyScale * 3.0f;
	Radar.defense->setScaleX(shield_radar_scale / BodyPart.Body->getScale() * 1.5f);
	Radar.defense->setScaleY(shield_radar_scale / BodyPart.Body->getScale() / winAspectRatio() * 0.5f);
	Radar.defense->setOpacity(0);
	BodyPart.Body->addChild(Radar.defense);


	weaponDetector = new CollisionDetector("DefaultBlade.png", BodyPart.WpBlade, 1.0f, 100.0f);
	BodyPart.WpBlade->addChild(weaponDetector);

	bodyCollider = new BodyCollider(bodyScale * 0.4f, bodyScale * 0.4f * sqrtf(3.0));
	motionAction.setHost(this);

	Momentum.jumpVelo = jumpVelocity;
	Momentum.curGravity = gravity;
	Momentum.curVeloY = 0;
	Momentum.outerVeloY = 0;
	curAltitude = initPos.y;

	AbilityStatus.powerSP = 40;
	AbilityStatus.powerNormal = 220;	//기본데미지 임시 셋팅
	motionAction.curMotion = actNone;
	curDefPoint = maxDefPoint;

	////필살기범위를 지정해주는 더미레이더
	//specialRadar = LayerColor::create();
	//specialRadar->setColor(Color3B::BLUE);
	//specialRadar->ignoreAnchorPointForPosition(false);
	//specialRadar->setAnchorPoint(Point(0.5, 0));
	//specialRadar->setPosition(initPos);
	////specialRadar->setScale(0.05f, 1.0f);
	////specialRadar->setScale(DivForVertical(sprWeapon)*0.028f*weaponScale, 1.5f);
	//specialRadar->setOpacity(0);
	////canvas->addChild(specialRadar, 5);

	setWpScale(WeaponScale.cur);
	initMotionFrames();
}

Character::~Character() {
	bodyCollider->release();
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("motions/Motion_Else.plist");
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("motions/Motion_Attack.plist");
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("motions/Motion_Finish.plist");
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("RoyalWeapon.plist");
}

//캐릭터 액션 애니메이션에 필요한 프레임 구분 삽입
void Character::initMotionFrames()
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/Motion_Else.plist");
	auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Default.png"));
	motionFrames[actNone].pushBack(frame);

	//actJumpUpReady
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_JumpUp_Ready_%d.png", i));
		motionFrames[actJumpReady].pushBack(frame);
	}
	//actJumpUp
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_JumpUp_%d.png", i));
		motionFrames[actJumpUp].pushBack(frame);
	}
	//actJumpDown
	for (int i = 0; i < 7; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_JumpDown_%d.png", i));
		motionFrames[actJumpDown].pushBack(frame);
	}
	//actDefLand
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Defense_Land_%d.png", i));
		motionFrames[actDefLand].pushBack(frame);
	}
	//actDefLandEnd
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Defense_Land_%d.png", 3 - i));
		motionFrames[actDefLandEnd].pushBack(frame);
	}
	//actDefJump
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Defense_Jump_%d.png", i));
		motionFrames[actDefJump].pushBack(frame);
	}
	//actDefJumpEnd
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Defense_Jump_%d.png", 3 - i));
		motionFrames[actDefJumpEnd].pushBack(frame);
	}
	//actSPCLand
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Special_Land_%d.png", i));
		motionFrames[actSPCLand].pushBack(frame);
	}
	//actSPCLandEnd
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Special_Land_%d.png", 4 - i));
		motionFrames[actSPCLandEnd].pushBack(frame);
	}
	//actSPCJump
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Special_Jump_%d.png", i));
		motionFrames[actSPCJump].pushBack(frame);
	}
	//actSPCJumpEnd
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Special_Jump_%d.png", 4 - i));
		motionFrames[actSPCJumpEnd].pushBack(frame);
	}
	//actMoveLeft
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_MoveLeft_%d.png", i));
		motionFrames[actMoveLeft].pushBack(frame);
	}
	//actMoveLeftEnd
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_MoveLeft_%d.png", 2 - i));
		motionFrames[actMoveLeftEnd].pushBack(frame);
	}
	//actMoveRight
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_MoveRight_%d.png", i));
		motionFrames[actMoveRight].pushBack(frame);
	}
	//actMoveRightEnd
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_MoveRight_%d.png", 2 - i));
		motionFrames[actMoveRightEnd].pushBack(frame);
	}


	//actAtkLandReady	
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/Motion_Attack.plist");
	for (int i = 0; i < 6; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Attack_Land_Ready_%d.png", i));
		motionFrames[actAtkLandReady].pushBack(frame);
	}
	//actAtkLandHit
	for (int i = 0; i < 13; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Attack_Land_Hit_%d.png", i));
		motionFrames[actAtkLandHit].pushBack(frame);
	}
	//actAtkLandBack
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Attack_Land_Back_%d.png", i));
		motionFrames[actAtkLandBack].pushBack(frame);
	}
	//actAtkJumpReady
	for (int i = 0; i < 6; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Attack_Jump_Ready_%d.png", i));
		motionFrames[actAtkJumpReady].pushBack(frame);
	}
	//actAtkJumpHit
	for (int i = 0; i < 13; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Attack_Jump_Hit_%d.png", i));
		motionFrames[actAtkJumpHit].pushBack(frame);
	}
	//actAtkJumpBack
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Attack_Jump_Back_%d.png", i));
		motionFrames[actAtkJumpBack].pushBack(frame);
	}

	//FinishFrame
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/Motion_Finish.plist");
	finishFrame[actNone] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Default_Land.png");
	finishFrame[actAtkLandReady] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Attack_Land_Ready.png");
	finishFrame[actAtkLandHit] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Attack_Land_Hit.png");
	finishFrame[actAtkJumpReady] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Attack_Jump_Ready.png");
	finishFrame[actAtkJumpHit] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Attack_Jump_Hit.png");
	finishFrame[actJumpReady] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Jump_Ready.png");
	finishFrame[actJumpUp] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Jump_Up.png");
	finishFrame[actJumpDown] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Jump_Down.png");
	finishFrame[actDefLand] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Defense_Land.png");
	finishFrame[actDefJump] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Defense_Jump.png");
	finishFrame[actSPCLand] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Special_Land.png");
	finishFrame[actSPCJump] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Special_Jump.png");
	finishFrame[actMoveLeft] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Move_Left.png");
	finishFrame[actMoveRight] = SpriteFrameCache::getInstance()->getSpriteFrameByName("Move_Right.png");

}

void Character::addState(CharStateType aState)
{
	//새로운 state 삽입 시도에만, 삽입
	if (this->stateStack % aState != 0)
	{
		//CCLOG("new state come in!");
		int prev_state = this->stateStack;	//삽입 전, state 백업
		this->stateStack *= aState;	//실제로 새로운 state 삽입 완료

									//add되는 State에 대해, 다른 처리가 필요한 경우에 처리
		switch (aState)
		{
		case ATTACK: {
			updateAttackID();
			//방어 input이 있다면, 초기화
			if (getDirtyInputTrigger(DEFENSE, true)) {
				delState(DEFENSE);
				initStateBit(DEFENSE);
			}
		}
					 break;
		case DEFENSE: {
			//공격 관련 state는 모두 초기화
			if (getDirtyInputTrigger(ATTACK, true)) {
				delState(ATTACK);
				initStateBit(ATTACK);
			}
		}
					  break;
		case JUMP: {
			Momentum.curVeloY = Momentum.jumpVelo;
		}
				   break;
		case SPC_ATK: {
			isSpcIntroPlaying = true;
		}
					  break;
		case CRASH: {
			//crash 지속시간 0.15초 
			//auto crash_finish = Sequence::create(DelayTime::create(0.6f),
			//	CallFuncN::create(CC_CALLBACK_1(Character::callback_DelCrash, this)), nullptr);
			//sprChar->runAction(crash_finish);

			//부딪힌 건물 속도로부터 자신의 충돌 후 속도 계산
			if (Momentum.curVeloY * 0.008f >= 0)
				setVeloY(Momentum.curVeloY * -0.008f + Momentum.outerVeloY);
			else {
				setVeloY(Momentum.outerVeloY);
			}
			return;
		}
					break;
		case PRESSED:
		{
			CCLOG("press start!");
			auto press_finish = Sequence::create(DelayTime::create(1.2f),
				CallFunc::create([&]()
			{
				this->delState(PRESSED);
				if (isCrash() && isAtLand()) this->addState(PRESSED);
			}), nullptr);
			Radar.attack->runAction(press_finish);

			auto move_left = MoveBy::create(0.05f, Vec2(winSize().width / 25, 0));
			auto move_right = move_left->reverse();
			auto sequence = Sequence::create(move_left, move_right, nullptr);
			shakeCanvas = Repeat::create(sequence, 14);

			this->getParent()->runAction(shakeCanvas);
			BGManager::getInstance()->moveBG(8);
			return;
		}
		break;
		default:
			break;
		}
		//새롭게 add 된 state 에 대한 모션 전환(필요할 경우만)
		if (aState != CRASH && aState != PRESSED && aState != DEAD)
			changeCharMotion(true, aState);
	}
}
void Character::delState(CharStateType dState)
{
	//존재하는 state의 삭제 명령에만 삭제
	if (this->stateStack % dState == 0)
	{
		int prev_state = this->stateStack; //삭제 전, state 백업
		this->stateStack /= dState;

		//Delete 되는 State에 대해, 다른 처리가 필요한 경우에 처리
		switch (dState)
		{
		case PRESSED:
			//canvas->stopAction(shakeCanvas);
			break;
		default:
			break;
		}
		changeCharMotion(false, dState);
	}
}

void Character::updatePosition(float deltaTime)
{
	if (!getSpcIntroPlaying())
	{
		Point updated_pos = getPosition();
		float moveX = 0;

		if (getOnStateInput(MOVE, LEFT) && !getOnStateInput(MOVE, RIGHT))
			moveX = -45;
		else if (!getOnStateInput(MOVE, LEFT) && getOnStateInput(MOVE, RIGHT))
			moveX = 45;

		//x축 방향 이동 갱신
		if (updated_pos.x + moveX < -(winSize().width) / 2) {
			updated_pos.x = -(winSize().width) / 2;
		}
		else if (updated_pos.x + moveX > winSize().width / 2 * 3) {
			updated_pos.x = winSize().width / 2 * 3;
		}
		else
			updated_pos.x += moveX * 0.4f;


		if (isJump()/*|| !isAtLand()*/)
		{
			//캐릭터의 y축 위치 갱신
			float cur_velocity = getVeloY();
			float gravity;

			//중력 임의조절
			if (cur_velocity < 40)
				gravity = getGravity()*0.9f;
			else if (cur_velocity < 20)
				gravity = getGravity()*0.8f;
			//else if (cur_velocity < 65)
			//	gravity = getGravity()*0.5f;
			//else if (cur_velocity < 55)
			//	gravity = getGravityChar()*0.45f;
			//else if (cur_velocity < 45)
			//	gravity = getGravityChar()*0.35f;
			//else if (cur_velocity < 35)
			//	gravity = getGravityChar()*0.25f;
			//else if (cur_velocity < 20)
			//	gravity = getGravityChar()*0.2f;
			//else if (cur_velocity < 10)
			//	gravity = getGravityChar();
			else
				gravity = getGravity();

			float delta_velocity = -gravity * gravity * gravity *0.25f * deltaTime + cur_velocity;
			setVeloY(delta_velocity);
			updated_pos.y += delta_velocity;

			//# 프레임은 60 유지하지만, 떨림현상 발생-> 이유 분석 필요(deltaTime에 대한 연산값이 너무 예민해서인가)
			//float updated_velocity = getVelocityChar() - gravity*gravity*0.5f;
			//setVelocityChar(updated_velocity);
			////float deltaVelocity = (curVelocity - gravity*gravity*0.5f)*deltaTime;
			////setVelocityChar(deltaVelocity);
			//setPositionY(getPositionY() + updated_velocity*deltaTime);
		}

		//업데이트 전, 현재의 위치를 previous로 백업
		prevPos = getPosition();
		//우선 collider pos만 임시 갱신
		bodyCollider->setPosition(updated_pos);
	}

}

void Character::updateState(float deltaTime)
{
	weaponDetector->drawOutline();

	//Collider와 본체의 위치를 맞춰줌으로써, 이동 완료 
	setPosition(bodyCollider->getPosition());

	//지면에서 충돌시, Press 상태
	if (isCrash() && isAtLand())
		addState(PRESSED);

	//Input Crash가 사라지면, Delete Crash 타이머 발동
	if (!getOnStateInput(CRASH, 0) && isCrash()) {
		auto crash_finish = Sequence::create(
			DelayTime::create(0.15f),
			CallFunc::create([&]() {delState(CRASH); }),
			nullptr);
		Radar.attack->runAction(crash_finish);
	}

	//Jump State 갱신 
	if (isJump() && (getVeloY() < 0)) {
		//(상승 -> 하강)
		if (!getOnStateInput(JUMP, DOWN)) {
			setOnStateInput(JUMP, DOWN);
		}
		//(공중 -> 지상 복귀) 
		if (isAtLand()) {
			setOffStateInput(JUMP, UP);
		}
	}

	updateStateTransition();
	bodyCollider->clearCollider();

	//방어 게이지 업데이트
	if (isDefense()) {
		if (curDefPoint > 0)
			curDefPoint -= 30.0f;
		else
			curDefPoint = 0.0f;
	}
	else {
		if (curDefPoint < maxDefPoint)
			curDefPoint += 15.0f;
		else
			curDefPoint = maxDefPoint;
	}

	//#6 Special Input
	//if (!isMotionPlaying && inputSpecial && !isSpcAttack()) {
	//	inputSpecial = false;	//들어온 스페셜공격 인풋 초기화
	//	//isSpcMotionPlaying = true;	//스페셜 모션 시작 -> 여기서 스케쥴러 일시 정지
	//	addState(SPC_ATK);	//스페셜 공격 상태 입력, 모션 시작
	//	return;
	//}
	//CCLOG("character state is %d", state);
}
void Character::updateStateTransition()
{
	//#1 DEFENSE transition
	if (!getEqualInputTrigger(DEFENSE))
	{
		//DEFENSE 상태 시작 (0 | 1)
		if (getOnStateInput(DEFENSE, TOUCH) && !getOnStateTrigger(DEFENSE, TOUCH)) {
			addState(DEFENSE);
			setOnStateTrigger(DEFENSE, TOUCH);
		}
		//DEFENSE 상태 제거 (1 | 0) 
		else if (!getOnStateInput(DEFENSE, TOUCH) && getOnStateTrigger(DEFENSE, TOUCH)) {
			delState(DEFENSE);
			//setOffStateTrigger(DEFENSE, TOUCH);
		}
	}

	//#2 ATTCK transition
	if (!getEqualInputTrigger(ATTACK))
	{
		if (getOnStateInput(ATTACK, TOUCH))
		{
			if (!motionAction.isMotionPlaying)
			{
				//Attack Ready (00 | 01)
				if (!getOnStateTrigger(ATTACK, TOUCH))
				{
					setOnStateTrigger(ATTACK, TOUCH);
					if (isJump())
						runMotionAction(actAtkJumpReady);
					else
						runMotionAction(actAtkLandReady);
					/*if (!Director::getInstance()->getScheduler()->isScheduled(schedule_selector(Character::callback_tick_AtkCharge), this)) {
					Director::getInstance()->getScheduler()->schedule(schedule_selector(Character::callback_tick_AtkCharge), this, 1 / 30.f, false);
					}*/
				}
				//Attack Hit (01 | 11)
				else if (!getOnStateTrigger(ATTACK, SWIPE) && (getOnStateInput(ATTACK, SWIPE)))
				{
					setOnStateTrigger(ATTACK, SWIPE);
					addState(ATTACK);
					EffectManager::getInstance()->runBladeWind(getPosition() + Vec2(0, getWpHeight()*0.15f), Vec2(-130, 160), 1.3f, false, *static_cast<Layer*>(getParent()));
				}

				//Attack Back (11 | 01)
				else if (getOnStateTrigger(ATTACK, SWIPE) && !getOnStateInput(ATTACK, SWIPE))
				{
					setOffStateTrigger(ATTACK, SWIPE);
					if (isJump())
						runMotionAction(actAtkJumpBack);
					else
						runMotionAction(actAtkLandBack);
					EffectManager::getInstance()->runBladeWind(getPosition() + Vec2(0, getWpHeight()*0.15f), Vec2(-10, 80), 1.3f, true, *static_cast<Layer*>(getParent()));
				}
			}
		}
		else if (!motionAction.isMotionPlaying)
		{
			if (isJump())
				runMotionAction(actAtkJumpCancel);
			else if (isAtLand())
				runMotionAction(actAtkLandCancel);

			initStateBit(ATTACK);
		}
	}

	//#3 JUMP transition
	if (!getEqualInputTrigger(JUMP))
	{
		//점프 touch input 인식
		if (getOnStateInput(JUMP, TOUCH))
		{
			if (!motionAction.isMotionPlaying && !isPressed())
			{
				//-> Jump ready (000 | 001)
				if (!getOnStateTrigger(JUMP, TOUCH))
				{
					setOnStateTrigger(JUMP, TOUCH);
					runMotionAction(actJumpReady);
				}
				//-> Jump Up (001 | 01*)
				else if (!getOnStateTrigger(JUMP, UP) && getOnStateInput(JUMP, UP))
				{
					setOnStateTrigger(JUMP, UP);
					addState(JUMP);
				}
				//-> Jump Down (011 | 11*)
				else if (!getOnStateTrigger(JUMP, DOWN) && getOnStateInput(JUMP, DOWN))
				{
					//다른 액션이 진행중 상태인지 확인
					if (!isDefense() && !getDirtyInputTrigger(ATTACK, true)) {
						setOnStateTrigger(JUMP, DOWN);
						runMotionAction(actJumpDown);
					}
				}
			}
			//(공중 -> 지상) 복귀
			if (getOnStateTrigger(JUMP, UP) && !getOnStateInput(JUMP, UP) && (getVeloY() < 0))
			{
				setPositionY(curAltitude);
				delState(JUMP);
				setVeloY(0);
			}
		}
		else if (!motionAction.isMotionPlaying && getOnStateTrigger(JUMP, TOUCH)) {
			initStateBit(JUMP);
			runMotionAction(actJumpCancel);
		}
	}

	//#4 MOVE transition
	if (!getEqualInputTrigger(MOVE))
	{
		if (!motionAction.isMotionPlaying)
		{
			//( dirty / dirty ) 좌-우 모션만 교체
			if (getDirtyInputTrigger(MOVE, true) && getDirtyInputTrigger(MOVE, false))
			{
				if (getOnStateInput(MOVE, LEFT) && !getOnStateInput(MOVE, RIGHT)) {
					setOnStateTrigger(MOVE, LEFT);
					setOffStateTrigger(MOVE, RIGHT);
					runMotionAction(actMoveLeft);
				}
				else if (!getOnStateInput(MOVE, LEFT) && getOnStateInput(MOVE, RIGHT)) {
					setOnStateTrigger(MOVE, RIGHT);
					setOffStateTrigger(MOVE, LEFT);
					runMotionAction(actMoveRight);
				}
			}
			//( 00 / dirty ) move 시작
			else if (!getDirtyInputTrigger(MOVE, false))
			{
				if (getOnStateInput(MOVE, LEFT) && !getOnStateInput(MOVE, RIGHT))
				{
					addState(MOVE);
					setOnStateTrigger(MOVE, LEFT);
				}
				else if (!getOnStateInput(MOVE, LEFT) && getOnStateInput(MOVE, RIGHT))
				{
					addState(MOVE);
					setOnStateTrigger(MOVE, RIGHT);
				}
			}
			//( dirty / 00 ) move 끝 
			else if (!getDirtyInputTrigger(MOVE, true)) {
				delState(MOVE);
			}
		}
	}
}


int Character::chkAttackRadar(Rect& attackable_unit)
{
	Rect trans_rect = attackable_unit;
	trans_rect.origin = BodyPart.WpBlade->convertToNodeSpace(attackable_unit.origin);
	trans_rect.size.width /= (this->getScale() * BodyPart.WpHandle->getScale());
	trans_rect.size.height /= (this->getScale() * BodyPart.WpHandle->getScale());

	/*
	1) (공격 가용 범위) AABB 형태 Radar(Layer)로 1차 탐지(rough),
	2) Radar 감지 후, 디테일한 boundary로 2차 탐지
	*/
	if (Radar.attack->getBoundingBox().intersectsRect(trans_rect))
	{
		Point world_crash_pos = Point(attackable_unit.getMidX(), attackable_unit.getMidY());

		///input 시, 좌표 & 스케일은 항상 world 기준
		if (weaponDetector->detectCollision(world_crash_pos, attackable_unit.size.width / 2))
		{
			float atk_dist = (Point(trans_rect.getMidX(), trans_rect.getMidY()) - BodyPart.Circle->getPosition()).getLengthSq();
			float max_dist = getWpHeight() * getWpHeight();

			if (tickDamageHit > 10) {
				EffectPool.spark->playParticleEffect(world_crash_pos);
			}
			return AbilityStatus.powerNormal * 0.5f * (1 + atk_dist / max_dist);
		}
	}
	return 0;
};


int Character::chkDefenseRadar(const Sprite& defensable)
{
	auto bound_box = defensable.getBoundingBox();
	Point world_origin = defensable.getParent()->convertToWorldSpace(bound_box.origin);
	Point trans_origin = this->convertToNodeSpace(world_origin);

	bound_box.origin = trans_origin;
	bound_box.size.operator/ (this->getScale());

	//shield radar에 들어와 있는 block인지 체크
	if (Radar.defense->getBoundingBox().intersectsRect(bound_box))
	{
		return 5;
	}
	else
		return -1;
}


//charAction => Animation 액션을 실행
void Character::runMotionAction(MotionActionType targetAction) {

	//실행할 액션의 모션락을 획득한다
	motionAction.getMotionLock(targetAction);

	switch (targetAction)
	{
	case actJumpReady:
	{
		///case문 안에서 생성 구문 있을 때, {} 바인딩을 해줘야 하는듯.. 안하면 에러
		float frame_interval = FRAME_DELAY*1.5f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actJumpReady], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actJumpReady, BODY); }), nullptr);

		auto circle_rot_1 = RotateTo::create(0.0f, 28.0f);
		auto circle_rot_2 = RotateBy::create(frame_interval * 2, 12.0f);
		motionAction.actionCircle = CustomSequence::create(circle_rot_1, circle_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actJumpReady, CIRCLE); }), nullptr);

		auto wp_rot_1 = RotateTo::create(0.0f, OnCircleRot(-80.0f));
		auto wp_rot_2 = RotateBy::create(frame_interval * 2, OnCircleRot(-96.0f) - OnCircleRot(-80.0f));
		motionAction.actionWeapon = CustomSequence::create(wp_rot_1, wp_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actJumpReady, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actJumpCancel:
	{
		float frame_interval = FRAME_DELAY*1.5f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actJumpReady], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation)->reverse(), CallFunc::create([&]() {motionAction.releaseMotionLock(actJumpCancel, BODY); }), nullptr);

		auto circle_rot_1 = RotateTo::create(0.0f, 40.0f);
		auto circle_rot_2 = RotateBy::create(frame_interval * 2, -12.0f);
		motionAction.actionCircle = CustomSequence::create(circle_rot_1, circle_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actJumpCancel, CIRCLE); }), nullptr);

		auto wp_rot_1 = RotateTo::create(0.0f, OnCircleRot(-96.0f));
		auto wp_rot_2 = RotateBy::create(0.0f, OnCircleRot(-80.0f) - OnCircleRot(-96.0f));
		motionAction.actionWeapon = CustomSequence::create(wp_rot_1, wp_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actJumpCancel, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actJumpUp:
	{
		float frame_interval = FRAME_DELAY*1.5f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actJumpUp], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actJumpUp, BODY); }), nullptr);

		auto circle_rot_1 = RotateTo::create(0.0f, 40.0f);
		auto circle_rot_2 = RotateBy::create(frame_interval * 2, 8.0f);
		motionAction.actionCircle = CustomSequence::create(circle_rot_1, circle_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actJumpUp, CIRCLE); }), nullptr);

		auto wp_rot_1 = RotateTo::create(0.0f, OnCircleRot(-106.0f));
		auto wp_rot_2 = RotateBy::create(frame_interval * 2, OnCircleRot(-74.0f) - OnCircleRot(-106.0f));
		motionAction.actionWeapon = CustomSequence::create(wp_rot_1, wp_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actJumpUp, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actJumpDown:
	{
		float frame_interval = FRAME_DELAY*1.5f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actJumpDown], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actJumpDown, BODY); }), nullptr);

		auto circle_rot_1 = RotateTo::create(0.0f, 48.0f);
		auto circle_rot_2 = RotateBy::create(frame_interval * 2, -12.0f);
		auto circle_rot_3 = RotateBy::create(frame_interval * 3.8f, -64.0f);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3, nullptr);
		motionAction.actionCircle = CustomSequence::create(circle_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actJumpDown, CIRCLE); }), nullptr);

		auto wp_rot_1 = RotateTo::create(0.0f, OnCircleRot(-74.0f));
		auto wp_rot_2 = RotateBy::create(frame_interval * 2, OnCircleRot(-60.0f) - OnCircleRot(-74.0f));
		auto wp_rot_3 = RotateBy::create(frame_interval * 3.8f, OnCircleRot(-30.0f) - OnCircleRot(-60.0f));
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3, nullptr);
		motionAction.actionWeapon = CustomSequence::create(wp_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actJumpDown, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actAtkLandReady:
	{
		float frame_interval = FRAME_DELAY * 0.64f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkLandReady], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkLandReady, BODY); }), nullptr);

		float start_angle = finishRotation[actNone].first + 2.0f + ExtraCircleRot.curAngle;
		auto circle_rot_1 = RotateTo::create(0, start_angle);
		float finish_angle = finishRotation[actAtkLandReady].first + ExtraCircleRot.curAngle;
		auto circle_rot_2 = RotateBy::create(frame_interval * 5, finish_angle - start_angle);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, nullptr);
		motionAction.actionCircle = CustomSequence::create(circle_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkLandReady, CIRCLE); }), nullptr);


		start_angle = finishRotation[actNone].second + ExtraWeaponRot.curAngle;
		auto wp_rot_1 = RotateTo::create(0, start_angle);
		finish_angle = finishRotation[actAtkLandReady].second + ExtraWeaponRot.curAngle;
		auto wp_rot_2 = RotateBy::create(frame_interval * 5, finish_angle - start_angle);

		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, nullptr);
		motionAction.actionWeapon = CustomSequence::create(wp_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkLandReady, WEAPON); }), nullptr);
		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;

	case actAtkLandHit:
	{
		float frame_interval = FRAME_DELAY * 0.62f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkLandHit], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkLandHit, BODY); }), nullptr);

		float start_angle = finishRotation[actAtkLandReady].first + 3.0f + ExtraCircleRot.curAngle;
		auto circle_rot_1 = RotateTo::create(0, start_angle);
		float finish_angle = finishRotation[actAtkLandHit].first;
		auto circle_rot_2 = RotateBy::create(frame_interval * 8, finish_angle - start_angle);
		auto circle_rot_3 = DelayTime::create(frame_interval * 4);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3, nullptr);
		motionAction.actionCircle = CustomSequence::create(circle_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkLandHit, CIRCLE); }), nullptr);

		start_angle = finishRotation[actAtkLandReady].second + 5.0f + ExtraWeaponRot.curAngle;
		auto wp_rot_1 = RotateTo::create(0, start_angle);	///시작보다 약간 더 뒤로 젖힘		
		finish_angle = finishRotation[actAtkLandHit].second;
		auto wp_rot_2 = RotateBy::create(frame_interval * 8, finish_angle - start_angle);
		auto wp_rot_3 = DelayTime::create(frame_interval * 4);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3, nullptr);
		motionAction.actionWeapon = CustomSequence::create(wp_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkLandHit, WEAPON); }), nullptr);

		//attack dist, angle 증가시키는 tick 스케쥴 실행
		Director::getInstance()->getScheduler()->schedule(schedule_selector(Character::callback_tick_Hit), this, 1 / 30.f, false);
		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actAtkLandBack:
	{
		float frame_interval = FRAME_DELAY*0.5f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkLandBack], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkLandBack, BODY); }), nullptr);


		float start_angle = finishRotation[actAtkLandHit].first;
		auto circle_rot_1 = RotateTo::create(0, start_angle);
		float finish_angle = finishRotation[actAtkLandReady].first + ExtraCircleRot.curAngle;
		auto circle_rot_2 = RotateBy::create(frame_interval * 7, finish_angle - start_angle);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, nullptr);
		motionAction.actionCircle = CustomSequence::create(circle_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkLandBack, CIRCLE); }), nullptr);


		start_angle = finishRotation[actAtkLandHit].second;
		auto wp_rot_1 = RotateTo::create(0, start_angle);
		finish_angle = finishRotation[actAtkLandReady].second + ExtraWeaponRot.curAngle;
		auto wp_rot_2 = RotateBy::create(frame_interval * 7, finish_angle - start_angle);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, nullptr);
		motionAction.actionWeapon = CustomSequence::create(wp_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkLandBack, WEAPON); }), nullptr);

		//CCLOG("cur extra angle is %.2f", atkScopeRadius.extraAngle);
		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actAtkLandCancel:
	{
		float frame_interval = FRAME_DELAY * 0.64f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkLandReady], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation)->reverse(), CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkLandCancel, BODY); }), nullptr);

		float start_angle = finishRotation[actAtkLandReady].first + ExtraCircleRot.curAngle;
		auto circle_rot_1 = RotateTo::create(0, start_angle);
		float finish_angle = finishRotation[actNone].first;
		auto circle_rot_2 = RotateBy::create(frame_interval * 5, finish_angle - start_angle);

		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, nullptr);
		motionAction.actionCircle = CustomSequence::create(circle_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkLandCancel, CIRCLE); }), nullptr);

		start_angle = finishRotation[actAtkLandReady].second + ExtraWeaponRot.curAngle;
		auto wp_rot_1 = RotateTo::create(0, start_angle);
		finish_angle = finishRotation[actNone].second;
		auto wp_rot_2 = RotateBy::create(frame_interval * 5, finish_angle - start_angle);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, nullptr);
		motionAction.actionWeapon = CustomSequence::create(wp_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkLandCancel, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actAtkJumpReady:
	{
		float frame_interval = FRAME_DELAY * 0.64f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkJumpReady], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkJumpReady, BODY); }), nullptr);

		float start_angle = finishRotation[actNone].first + 2.0f + ExtraCircleRot.curAngle;
		auto circle_rot_1 = RotateTo::create(0, start_angle);
		float finish_angle = finishRotation[actAtkJumpReady].first + ExtraCircleRot.curAngle;
		auto circle_rot_2 = RotateBy::create(frame_interval * 5, finish_angle - start_angle);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, nullptr);
		motionAction.actionCircle = CustomSequence::create(circle_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkJumpReady, CIRCLE); }), nullptr);


		start_angle = finishRotation[actNone].second + ExtraWeaponRot.curAngle;
		auto wp_rot_1 = RotateTo::create(0, start_angle);
		finish_angle = finishRotation[actAtkJumpReady].second + ExtraWeaponRot.curAngle;
		auto wp_rot_2 = RotateBy::create(frame_interval * 5, finish_angle - start_angle);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, nullptr);
		motionAction.actionWeapon = CustomSequence::create(wp_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkJumpReady, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actAtkJumpHit:
	{
		float frame_interval = FRAME_DELAY * 0.62f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkJumpHit], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkJumpHit, BODY); }), nullptr);

		float start_angle = finishRotation[actAtkJumpReady].first + 3.0f + ExtraCircleRot.curAngle;
		auto circle_rot_1 = RotateTo::create(0, start_angle);
		float finish_angle = finishRotation[actAtkJumpHit].first;
		auto circle_rot_2 = RotateBy::create(frame_interval * 8, finish_angle - start_angle);
		auto circle_rot_3 = DelayTime::create(frame_interval * 4);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3, nullptr);
		motionAction.actionCircle = CustomSequence::create(circle_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkJumpHit, CIRCLE); }), nullptr);

		start_angle = finishRotation[actAtkJumpReady].second + 5.0f + ExtraWeaponRot.curAngle;
		auto wp_rot_1 = RotateTo::create(0, start_angle);
		finish_angle = finishRotation[actAtkJumpHit].second;
		auto wp_rot_2 = RotateBy::create(frame_interval * 8, finish_angle - start_angle);
		auto wp_rot_3 = DelayTime::create(frame_interval * 4);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3, nullptr);
		motionAction.actionWeapon = CustomSequence::create(wp_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkJumpHit, WEAPON); }), nullptr);

		Director::getInstance()->getScheduler()->schedule(schedule_selector(Character::callback_tick_Hit), this, 1 / 30.f, false);
		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actAtkJumpBack:
	{
		float frame_interval = FRAME_DELAY * 0.5f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkJumpBack], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkJumpBack, BODY); }), nullptr);

		float start_angle = finishRotation[actAtkJumpHit].first;
		auto circle_rot_1 = RotateTo::create(0, start_angle);
		float finish_angle = finishRotation[actAtkJumpReady].first + ExtraCircleRot.curAngle;
		auto circle_rot_2 = RotateBy::create(frame_interval * 7, finish_angle - start_angle);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, nullptr);
		motionAction.actionCircle = CustomSequence::create(circle_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkJumpBack, CIRCLE); }), nullptr);

		start_angle = finishRotation[actAtkJumpHit].second;
		auto wp_rot_1 = RotateTo::create(0, finishRotation[actAtkJumpHit].second);
		finish_angle = finishRotation[actAtkJumpReady].second + ExtraWeaponRot.curAngle;
		auto wp_rot_2 = RotateBy::create(frame_interval * 7, finish_angle - start_angle);

		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, nullptr);
		motionAction.actionWeapon = CustomSequence::create(wp_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkJumpBack, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actAtkJumpCancel:
	{
		float frame_interval = FRAME_DELAY * 0.64f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkJumpReady], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation)->reverse(), CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkJumpCancel, BODY); }), nullptr);

		float start_angle = finishRotation[actAtkJumpReady].first + ExtraCircleRot.curAngle;
		auto circle_rot_1 = RotateTo::create(0, start_angle);
		float finish_angle = finishRotation[actNone].first;
		auto circle_rot_2 = RotateBy::create(frame_interval * 5, finish_angle - start_angle);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, nullptr);
		motionAction.actionCircle = CustomSequence::create(circle_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkJumpCancel, CIRCLE); }), nullptr);

		start_angle = finishRotation[actAtkJumpReady].second + ExtraWeaponRot.curAngle;
		auto wp_rot_1 = RotateTo::create(0, finishRotation[actAtkJumpReady].second);
		finish_angle = finishRotation[actNone].second;
		auto wp_rot_2 = RotateBy::create(frame_interval * 5, finish_angle - start_angle);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, nullptr);
		motionAction.actionWeapon = CustomSequence::create(wp_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actAtkJumpCancel, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actDefLand:
	{
		float frame_interval = FRAME_DELAY * 0.8f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actDefLand], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actDefLand, BODY); }), nullptr);

		//원 rotate액션 정의
		auto circle_rot_1 = RotateTo::create(0.0f, 35.0f);
		motionAction.actionCircle = CustomSequence::create(circle_rot_1, CallFunc::create([&]() {motionAction.releaseMotionLock(actDefLand, CIRCLE); }), nullptr);

		//검 rotate 액션 정의
		auto wp_rot_1 = RotateTo::create(0.0f, OnCircleRot(27.0f));
		motionAction.actionWeapon = CustomSequence::create(wp_rot_1, CallFunc::create([&]() {motionAction.releaseMotionLock(actDefLand, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actDefLandEnd:
	{
		float frame_interval = FRAME_DELAY * 0.8f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actDefLandEnd], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actDefLandEnd, BODY); }), nullptr);

		auto delay_frame = DelayTime::create(3 * frame_interval);
		auto circle_rot = RotateTo::create(0.0f, 35.0f);
		motionAction.actionCircle = CustomSequence::create(circle_rot, delay_frame, CallFunc::create([&]() {motionAction.releaseMotionLock(actDefLandEnd, CIRCLE); }), nullptr);

		auto wp_rot = RotateTo::create(0.0f, OnCircleRot(27.0f));
		motionAction.actionWeapon = CustomSequence::create(wp_rot, delay_frame, CallFunc::create([&]() {motionAction.releaseMotionLock(actDefLandEnd, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actDefJump:
	{
		float frame_interval = FRAME_DELAY * 0.8f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actDefJump], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actDefJump, BODY); }), nullptr);

		auto circle_rot_1 = RotateTo::create(0.0f, 53.0f);
		motionAction.actionCircle = CustomSequence::create(circle_rot_1, CallFunc::create([&]() {motionAction.releaseMotionLock(actDefJump, CIRCLE); }), nullptr);

		auto wp_rot_1 = RotateTo::create(0.0f, OnCircleRot(27.0f));
		motionAction.actionWeapon = CustomSequence::create(wp_rot_1, CallFunc::create([&]() {motionAction.releaseMotionLock(actDefJump, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actDefJumpEnd:
	{
		float frame_interval = FRAME_DELAY * 0.8f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actDefJumpEnd], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actDefJumpEnd, BODY); }), nullptr);

		auto delay_frame = DelayTime::create(3 * frame_interval);
		auto circle_rot = RotateTo::create(0.0f, 53.0f);
		motionAction.actionCircle = CustomSequence::create(circle_rot, delay_frame, CallFunc::create([&]() {motionAction.releaseMotionLock(actDefJumpEnd, CIRCLE); }), nullptr);

		auto wp_rot = RotateTo::create(0.0f, OnCircleRot(27.0f));
		motionAction.actionWeapon = CustomSequence::create(wp_rot, delay_frame, delay_frame, delay_frame, CallFunc::create([&]() {motionAction.releaseMotionLock(actDefJumpEnd, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actSPCLand:
	{
		float frame_interval = FRAME_DELAY * 12.0f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actSPCLand], frame_interval);
		auto finish_callback = CallFuncN::create(CC_CALLBACK_1(Character::callback_FinishSpcIntro, this));
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), finish_callback, CallFunc::create([&]() {motionAction.releaseMotionLock(actSPCLand, BODY); }), nullptr);

		//원 rotate액션 정의
		//1~5프레임 -106,  6~10프레임 -125
		auto circle_rot_1 = RotateTo::create(0, 64.0f);	//시작 시, 검 각도 홱 젖히기
		auto circle_rot_2 = RotateBy::create(FRAME_DELAY * 0.6f * 5, -110.0f);
		auto circle_rot_3 = RotateBy::create(FRAME_DELAY * 0.6f * 5, -142.0f);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3, nullptr);
		motionAction.actionCircle = CustomSequence::create(circle_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actSPCLand, CIRCLE); }), nullptr);

		auto wp_rot_1 = RotateTo::create(0, OnCircleRot(20.0f));
		auto wp_rot_2 = RotateBy::create(FRAME_DELAY*0.6f * 5, 55.0f);
		auto wp_rot_3 = RotateBy::create(FRAME_DELAY*0.6f * 5, 45.0f);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3, nullptr);
		motionAction.actionWeapon = CustomSequence::create(wp_seq, CallFunc::create([&]() {motionAction.releaseMotionLock(actSPCLand, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
		//sprCircle->runAction(circleAction);
		//sprWeapon->runAction(wpAction);
		//BodyPart.Body->runAction(animAction);
		//EffectManager::getInstance()->runSpecialEffect(this, canvas, FRAME_DELAY*12.0f * 4);
	}
	break;
	case actSPCLandEnd:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actSPCLandEnd], FRAME_DELAY*5.0f);
		auto finish_callback = CallFuncN::create(CC_CALLBACK_1(Character::callback_DelSpc, this));
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), finish_callback, CallFunc::create([&]() {motionAction.releaseMotionLock(actSPCLandEnd, BODY); }), nullptr);
		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actSPCJump:
		break;
	case actSPCJumpEnd:
		break;
	case actMoveLeft:
	{
		float frame_interval = FRAME_DELAY * 1.5f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actMoveLeft], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actMoveLeft, BODY); }), nullptr);

		auto circle_rot_1 = RotateTo::create(0, 28.0f);
		auto circle_rot_2 = RotateTo::create(frame_interval * 2, 28.0f);
		motionAction.actionCircle = CustomSequence::create(circle_rot_1, circle_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actMoveLeft, CIRCLE); }), nullptr);

		auto wp_rot_1 = RotateTo::create(0.0f, OnCircleRot(-80.0f));
		auto wp_rot_2 = RotateTo::create(frame_interval * 2, OnCircleRot(-74.0f));
		motionAction.actionWeapon = CustomSequence::create(wp_rot_1, wp_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actMoveLeft, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actMoveLeftEnd:
	{
		float frame_interval = FRAME_DELAY * 1.5f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actMoveLeftEnd], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actMoveLeftEnd, BODY); }), nullptr);

		auto circle_rot_1 = RotateTo::create(0.0f, 28.0f);
		auto circle_rot_2 = RotateTo::create(frame_interval * 2, 28.0f);
		motionAction.actionCircle = CustomSequence::create(circle_rot_1, circle_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actMoveLeftEnd, CIRCLE); }), nullptr);

		auto wp_rot_1 = RotateTo::create(0.0f, OnCircleRot(-74.0f));
		auto wp_rot_2 = RotateTo::create(frame_interval * 2, OnCircleRot(-80.0f));
		motionAction.actionWeapon = CustomSequence::create(wp_rot_1, wp_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actMoveLeftEnd, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actMoveRight:
	{
		float frame_interval = FRAME_DELAY * 1.5f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actMoveRight], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actMoveRight, BODY); }), nullptr);

		auto circle_rot_1 = RotateTo::create(0.0f, 150.0f);
		auto circle_rot_2 = RotateBy::create(0, 10.0f);
		motionAction.actionCircle = CustomSequence::create(circle_rot_1, circle_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actMoveRight, CIRCLE); }), nullptr);

		auto wp_rot_1 = RotateTo::create(0.0f, OnCircleRot(78.0f));
		auto wp_rot_2 = RotateBy::create(frame_interval * 2, -16.0f);
		motionAction.actionWeapon = CustomSequence::create(wp_rot_1, wp_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actMoveRight, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	case actMoveRightEnd:
	{
		float frame_interval = FRAME_DELAY * 1.5f;
		auto animation = Animation::createWithSpriteFrames(motionFrames[actMoveRightEnd], frame_interval);
		motionAction.actionBody = CustomSequence::create(Animate::create(animation), CallFunc::create([&]() {motionAction.releaseMotionLock(actMoveRightEnd, BODY); }), nullptr);

		auto circle_rot_1 = RotateTo::create(0.0f, 160.0f);
		auto circle_rot_2 = RotateBy::create(0, -10.0f);
		motionAction.actionCircle = CustomSequence::create(circle_rot_1, circle_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actMoveRightEnd, CIRCLE); }), nullptr);

		auto wp_rot_1 = RotateTo::create(0.0f, OnCircleRot(62.0f));
		auto wp_rot_2 = RotateBy::create(frame_interval * 2, 16.0f);
		motionAction.actionWeapon = CustomSequence::create(wp_rot_1, wp_rot_2, CallFunc::create([&]() {motionAction.releaseMotionLock(actMoveRightEnd, WEAPON); }), nullptr);

		motionAction.runMotion(*BodyPart.Body, *BodyPart.Circle, *BodyPart.WpHandle);
	}
	break;
	default:
		break;
	}
}

//charAction => 고정 frame으로 교체 (각 Action이 끝났을 때, 프레임 맞추는 용)
void Character::setMotionFrame(MotionActionType targetAction)
{
	switch (targetAction)
	{
	case actNone:
		BodyPart.Body->setSpriteFrame(finishFrame[actNone]);
		BodyPart.Circle->setRotation(finishRotation[actNone].first);
		BodyPart.WpHandle->setRotation(finishRotation[actNone].second);
		break;
	case actJumpReady:
		BodyPart.Body->setSpriteFrame(finishFrame[actAtkJumpHit]);
		BodyPart.Circle->setRotation(finishRotation[actAtkJumpHit].first);
		BodyPart.WpHandle->setRotation(finishRotation[actAtkJumpHit].second);
		break;
	case actJumpUp:
		BodyPart.Body->setSpriteFrame(finishFrame[actJumpUp]);
		BodyPart.Circle->setRotation(finishRotation[actJumpUp].first);
		BodyPart.WpHandle->setRotation(finishRotation[actJumpUp].second);
		break;
	case actJumpDown:
		BodyPart.Body->setSpriteFrame(finishFrame[actJumpDown]);
		BodyPart.Circle->setRotation(finishRotation[actJumpDown].first);
		BodyPart.WpHandle->setRotation(finishRotation[actJumpDown].second);
		break;
	case actAtkLandReady:
		BodyPart.Body->setSpriteFrame(finishFrame[actAtkLandReady]);
		BodyPart.Circle->setRotation(finishRotation[actAtkLandReady].first);
		BodyPart.WpHandle->setRotation(finishRotation[actAtkLandReady].second);
		break;
	case actAtkLandHit:
		BodyPart.Body->setSpriteFrame(finishFrame[actAtkLandHit]);
		BodyPart.Circle->setRotation(finishRotation[actAtkLandHit].first);
		BodyPart.WpHandle->setRotation(finishRotation[actAtkLandHit].second);
		break;
	case actAtkJumpReady:
		BodyPart.Body->setSpriteFrame(finishFrame[actAtkJumpReady]);
		BodyPart.Circle->setRotation(finishRotation[actAtkJumpReady].first);
		BodyPart.WpHandle->setRotation(finishRotation[actAtkJumpReady].second);
		break;
	case actAtkJumpHit:
		BodyPart.Body->setSpriteFrame(finishFrame[actAtkJumpHit]);
		BodyPart.Circle->setRotation(finishRotation[actAtkJumpHit].first);
		BodyPart.WpHandle->setRotation(finishRotation[actAtkJumpHit].second);
		break;
	case actDefLand:
		BodyPart.Body->setSpriteFrame(finishFrame[actDefLand]);
		BodyPart.Circle->setRotation(finishRotation[actDefLand].first);
		BodyPart.WpHandle->setRotation(finishRotation[actDefLand].second);
		break;
	case actDefJump:
		BodyPart.Body->setSpriteFrame(finishFrame[actDefJump]);
		BodyPart.Circle->setRotation(finishRotation[actDefJump].first);
		BodyPart.WpHandle->setRotation(finishRotation[actDefJump].second);
		break;
	case actMoveLeft:
		BodyPart.Body->setSpriteFrame(finishFrame[actMoveLeft]);
		BodyPart.Circle->setRotation(finishRotation[actMoveLeft].first);
		BodyPart.WpHandle->setRotation(finishRotation[actMoveLeft].second);
		break;
	case actMoveRight:
		BodyPart.Body->setSpriteFrame(finishFrame[actMoveRight]);
		BodyPart.Circle->setRotation(finishRotation[actMoveRight].first);
		BodyPart.WpHandle->setRotation(finishRotation[actMoveRight].second);
		break;
	default:
		break;
	}
}

//Add/Delete State에 따라 => 모션(Animation/Frame) 교체(true - addState, false - delState)
void Character::changeCharMotion(bool isAdd, CharStateType targetState)
{
	//1) Add State 의 경우
	if (isAdd)
	{
		switch (targetState)
		{
		case MOVE:
		{
			if (getOnStateInput(MOVE, LEFT) && !getOnStateInput(MOVE, RIGHT))
				runMotionAction(actMoveLeft);
			else if (!getOnStateInput(MOVE, LEFT) && getOnStateInput(MOVE, RIGHT))
				runMotionAction(actMoveRight);
		}
		break;
		case ATTACK:
		{
			if (isJump())
				runMotionAction(actAtkJumpHit);
			else
				runMotionAction(actAtkLandHit);
		}
		break;
		case DEFENSE:
		{
			if (isJump())
				runMotionAction(actDefJump);
			else
				runMotionAction(actDefLand);
		}
		break;
		case SPC_ATK:
			runMotionAction(actSPCLand);
			break;
		case JUMP:
		{
			//공격 인풋 들어와 있을 때,
			if (getOnStateInput(ATTACK, TOUCH))
			{
				if (getOnStateTrigger(ATTACK, SWIPE))
					setMotionFrame(actAtkJumpHit);
				else
					setMotionFrame(actAtkJumpReady);
			}
			//방어 상태일 때,
			else if (isDefense())
				setMotionFrame(actDefJump);
			else
				runMotionAction(actJumpUp);
		}
		break;
		default:
			break;
		}
	}
	//2) Delete state 의 경우
	else
	{
		switch (targetState)
		{
		case DEFENSE:
			if (isJump())
				runMotionAction(actDefJumpEnd);
			else
				runMotionAction(actDefLandEnd);

			initStateBit(DEFENSE);
			break;

		case SPC_ATK:

			if (!isJump() && getDirtyInputTrigger(MOVE, true))
			{
				if (getOnStateInput(MOVE, LEFT) && !getOnStateInput(MOVE, RIGHT))
					runMotionAction(actMoveLeft);
				else if (!getOnStateInput(MOVE, LEFT) && getOnStateInput(MOVE, RIGHT))
					runMotionAction(actMoveRight);
			}
			else
			{
				if (isJump()) {
					if (getOnStateTrigger(JUMP, DOWN))
						setMotionFrame(actJumpDown);
					else
						setMotionFrame(actJumpUp);
				}
				else
					setMotionFrame(actNone);
			}
			break;

		case JUMP:

			if (isDefense())
				setMotionFrame(actDefLand);
			else if (getOnStateTrigger(ATTACK, TOUCH))
			{
				if (getOnStateTrigger(ATTACK, SWIPE))
					setMotionFrame(actAtkLandHit);
				else
					setMotionFrame(actAtkLandReady);
			}
			else if (isMove())
			{
				if (getOnStateInput(MOVE, LEFT) && !getOnStateInput(MOVE, RIGHT))
					setMotionFrame(actMoveLeft);
				else if (!getOnStateInput(MOVE, LEFT) && getOnStateInput(MOVE, RIGHT))
					setMotionFrame(actMoveRight);
			}
			else if (motionAction.isMotionPlaying) {
				runMotionAction(actJumpCancel);
			}
			else {
				setMotionFrame(actNone);
			}
			initStateBit(JUMP);
			break;
		case MOVE:
			if (isDefense())
			{
				setMotionFrame(actDefLand);
			}
			else if (getOnStateTrigger(ATTACK, TOUCH))
			{
				if (getOnStateTrigger(ATTACK, SWIPE))
					setMotionFrame(actAtkLandHit);
				else
					setMotionFrame(actAtkLandReady);
			}
			else if (getOnStateTrigger(MOVE, LEFT) && !getOnStateTrigger(MOVE, RIGHT))
			{
				runMotionAction(actMoveLeftEnd);
			}
			else if (!getOnStateTrigger(MOVE, LEFT) && getOnStateTrigger(MOVE, RIGHT))
			{
				runMotionAction(actMoveRightEnd);
			}

			initStateBit(MOVE);
			break;
		default:
			break;
		}
	}
}


//무기 Sprite와 그에따른 레이더의 크기 함께 Scale 
void Character::setWpScale(float input_wp_scale) {

	WeaponScale.cur = input_wp_scale;
	BodyPart.WpBlade->setScale(WeaponScale.cur * DivForHorizontal(BodyPart.WpBlade) / (BodyPart.Body->getScale() * BodyPart.WpHandle->getScale()));

	//(wp_scale + body_scale)의 기준 스케일을 구해, 그 비율로 레이더 크기 증가
	float radar_x_scale = bodyScale / 2 + getWpLengthScale() * 1.8f;
	float radar_y_scale = bodyScale / 2 + getWpLengthScale();

	Radar.attack->setScaleX(radar_x_scale / BodyPart.Body->getScale());
	Radar.attack->setScaleY(radar_y_scale / BodyPart.Body->getScale() / winAspectRatio());
}


void Character::callback_tick_Hit(float deltaTime)
{
	motionAction.skewMotionSpeed(2.0f / (sqrtf(tickDamageHit) * 0.08f + 2.0f));
	lapsedDamageHit += tickDamageHit;
	tickDamageHit = 0;	//tick damage 전달 후, 초기화

	lapsedTimeHit += deltaTime / (FRAME_DELAY * 0.9f * 8);
	EffectManager::getInstance()->displayAtkScore(lapsedDamageHit, Point::ZERO, 2.0f, *UICanvas);

	//tick time이 모두 지나면 스케쥴 해제
	if (lapsedTimeHit > 1) {
		lapsedTimeHit = 0.0f;
		lapsedDamageHit = tickDamageHit = 0;
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(Character::callback_tick_Hit), this);
	}
};


void Character::callback_tick_AtkCharge(float deltaTime)
{
	if (WeaponScale.cur < WeaponScale.max) {
		auto updated_scale = WeaponScale.cur + 0.05f * deltaTime / 1.0f;
		setWpScale(updated_scale);
	}

	if (ExtraCircleRot.curAngle < ExtraCircleRot.limitAngle)
	{
		float delta_angle = 2.0f * deltaTime / 1.0f;
		ExtraCircleRot.curAngle += delta_angle;
		BodyPart.Circle->setRotation(BodyPart.Circle->getRotation() + delta_angle);
	}

	if (ExtraWeaponRot.curAngle < ExtraWeaponRot.limitAngle)
	{
		float delta_angle = 4.0f * deltaTime / 1.0f;
		ExtraWeaponRot.curAngle += delta_angle;
		BodyPart.WpHandle->setRotation(BodyPart.WpHandle->getRotation() + delta_angle);
	}

}




void Character::callback_tick_AtkRelease(float deltaTime)
{
	if (WeaponScale.cur > WeaponScale.min) {
		auto updated_scale = WeaponScale.cur - 0.2f * deltaTime / 1.0f;
		setWpScale(updated_scale);

		ExtraWeaponRot.curAngle = 0.0f;
		ExtraCircleRot.curAngle = 0.0f;
	}
	else {
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(Character::callback_tick_AtkRelease), this);
	}
}
void Character::callback_FinishSpcIntro(Ref* sender) {

	//스페셜공격 인트로 모션 끝내고, 여기서 메인스케쥴러 재작동
	isSpcIntroPlaying = false;

	//초당 간격으로 19번 스페셜 데미지 계산 스케쥴러 작동
	Director::getInstance()->getScheduler()->schedule(std::bind(&Character::updateSpecialID, this, std::placeholders::_1), this, 0.15f, 19, 0.15f, false, "updateSpecialID");
}
void Character::callback_DelSpc(Ref* sender) {

	//CCLOG("Delete Special State!!!!");
	this->delState(SPC_ATK);
}


int Character::getSpecialPoint(int combo)
{
	if (combo >= 40)
		return 1000;
	else if (combo >= 30)
		return 800;
	else if (combo >= 20)
		return 500;
	else if (combo >= 10)
		return 100;
	else
		return 50;
}
//프레임(호출)당 1%씩 감소
void Character::decreaseDefPoint(float deltaTime) {

	float unitDec = maxDefPoint *0.002f;

	if (curDefPoint >= unitDec)
		curDefPoint -= unitDec;
	else
		curDefPoint = 0;

}
//프레임(호출)당 1%씩 증가
void Character::increaseDefPoint(float deltaTime) {

	float unitInc = maxDefPoint * 0.001f;

	if (curDefPoint <= maxDefPoint)
		curDefPoint += unitInc;
	else
		curDefPoint = maxDefPoint;
}



//모션락을 획득 (현재 play되고 있는 charAction이 있다면, 해제)
void Character::AtomicMotionAction::getMotionLock(MotionActionType input_action)
{
	//다른 액션 진행 중, MotionLock을 획득하려는 액션에 대해, 선택적인 콜백 후처리
	if (isMotionPlaying)
	{
		Host->BodyPart.Body->stopActionByTag(MOTION_ACTION);
		Host->BodyPart.Circle->stopActionByTag(MOTION_ACTION);
		Host->BodyPart.WpHandle->stopActionByTag(MOTION_ACTION);

		switch (curMotion)
		{
		case actAtkLandHit:
			Host->delState(ATTACK);
			break;
		case actAtkJumpHit:
			Host->delState(ATTACK);
			break;
		default:
			break;
		}
	}
	curMotion = input_action;

	//모션락 비트를 모두 on
	motionLockBit |= BODY; motionLockBit |= CIRCLE; motionLockBit |= WEAPON;
	//모션락 플래그 획득
	isMotionPlaying = true;
};

//모션락 해제 (콜백으로 호출)
void Character::AtomicMotionAction::releaseMotionLock(MotionActionType input_action, MotionElement target)
{
	//target 액션에 대하여, 입력받은 part의 락을 해제한다.
	switch (target)
	{
	case BODY:
		motionLockBit &= ~BODY;
		break;
	case CIRCLE:
		motionLockBit &= ~CIRCLE;
		break;
	case WEAPON:
		motionLockBit &= ~WEAPON;
		break;
	default:
		break;
	}

	//전체 bit 확인 후, Lock 해제
	if (motionLockBit == 0)
	{
		isMotionPlaying = false;

		//해당 Action 후, 필요한 콜백처리 
		switch (input_action)
		{
		case actJumpCancel:
			if (Host->isDefense())
			{
				Host->setMotionFrame(actDefLand);
			}
			else if (Host->getOnStateTrigger(ATTACK, TOUCH))
			{
				if (Host->getOnStateTrigger(ATTACK, SWIPE))
					Host->setMotionFrame(actAtkLandHit);
				else
					Host->setMotionFrame(actAtkLandReady);
			}
			else if (Host->isMove())
			{
				if (Host->getOnStateInput(MOVE, LEFT) && !Host->getOnStateInput(MOVE, RIGHT))
					Host->setMotionFrame(actMoveLeft);
				else if (!Host->getOnStateInput(MOVE, LEFT) && Host->getOnStateInput(MOVE, RIGHT))
					Host->setMotionFrame(actMoveRight);
			}
			else {
				if (Host->isJump())
				{
					if (Host->getOnStateTrigger(JUMP, DOWN))
						Host->setMotionFrame(actJumpDown);
					else
						Host->setMotionFrame(actJumpUp);
				}
				else
					Host->setMotionFrame(actNone);
			}
			break;
		case actAtkLandHit:
			Host->delState(ATTACK);
			break;
		case actAtkLandCancel:
			Host->setMotionFrame(actNone);
			break;
		case actAtkJumpHit:
			Host->delState(ATTACK);
			break;
		case actAtkJumpCancel:
			if (Host->getOnStateInput(JUMP, DOWN))
				Host->setMotionFrame(actJumpDown);
			else
				Host->setMotionFrame(actJumpUp);
			break;
		case actDefLandEnd:
			if (Host->getOnStateInput(MOVE, LEFT) && !Host->getOnStateInput(MOVE, RIGHT))
				Host->setMotionFrame(actMoveLeft);
			else if (!Host->getOnStateInput(MOVE, LEFT) && Host->getOnStateInput(MOVE, RIGHT))
				Host->setMotionFrame(actMoveRight);
			else
				Host->setMotionFrame(actNone);
			break;
		case actDefJumpEnd:
			if (Host->getOnStateTrigger(JUMP, DOWN))
				Host->setMotionFrame(actJumpDown);
			else
				Host->setMotionFrame(actJumpUp);
			break;
		case actMoveLeftEnd:
			if (Host->isJump()) {
				if (Host->getOnStateTrigger(JUMP, DOWN))
					Host->setMotionFrame(actJumpDown);
				else
					Host->setMotionFrame(actJumpUp);
			}
			else
				Host->setMotionFrame(actNone);
			break;
		case actMoveRightEnd:
			if (Host->isJump()) {
				if (Host->getOnStateTrigger(JUMP, DOWN))
					Host->setMotionFrame(actJumpDown);
				else
					Host->setMotionFrame(actJumpUp);
			}
			else
				Host->setMotionFrame(actNone);
			break;
		default:
			break;
		}
	}
};





//void Character::callback_tick_AtkConsume(float deltaTime)
//{
//	if (curWpScale > defWpScale)
//	{
//		auto updated_scale = getWpScale() - 0.15f * deltaTime / 1.0f;
//		setWpScale(updated_scale);
//	}
//	else {
//		Director::getInstance()->getScheduler()->unschedule(schedule_selector(Character::callback_tick_AtkRelease), this);
//	}
//}

//void Character::callback_tick_AtkCharge(float deltaTime)
//{
//	if (curWpScale < maxWpScale) {
//		auto updated_scale = getWpScale() + 0.05f * deltaTime / 1.0f;
//		setWpScale(updated_scale);
//	}
//	/*else {
//		Director::getInstance()->getScheduler()->unschedule(schedule_selector(Character::callback_tick_AtkCharge), this);
//	}*/
//}


//
////target 제외, 나머지 두 모션이 모두 종료되었을 때 락 풀어줌
//void Character::callback_chkMotionFinish(Ref* sender, charMotionElement target)
//{
//	switch (target)
//	{
//	case ANIMATION:
//		if (sprCircle->getNumberOfRunningActionsByTag(MOTION_ACTION) <= 0
//			&& sprWeapon->getNumberOfRunningActionsByTag(MOTION_ACTION) <= 0)
//		{
//			isMotionPlaying = false;
//			//CCLOG("Motion Lock UnLocked from [ANIMATION]!!!!!");
//		}
//		break;
//	case CIRCLE:
//		if (sprChar->getNumberOfRunningActionsByTag(MOTION_ACTION) <= 0
//			&& sprWeapon->getNumberOfRunningActionsByTag(MOTION_ACTION) <= 0)
//		{
//			isMotionPlaying = false;
//			//CCLOG("Motion Lock UnLocked from [CIRCLE]!!!!!");
//		}
//		break;
//	case WEAPON:
//		if (sprChar->getNumberOfRunningActionsByTag(MOTION_ACTION) <= 0
//			&& sprCircle->getNumberOfRunningActionsByTag(MOTION_ACTION) <= 0)
//		{
//			isMotionPlaying = false;
//			//CCLOG("Motion Lock UnLocked from [WEAPON]!!!!!");
//		}
//		break;
//	}
//
//	return;
//}



