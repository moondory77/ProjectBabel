#define FRAME_DELAY 0.034	//60프레임 기준 1프레임은 0.017초 
#include "Character.h"
#include <String.h>
#include "Manager/GameManager.h"
#include "Manager/BGManager.h"
#include "Manager/EffectManager.h"


Character::Character() {
	state = (charStateType)NONE;
}

Character::Character(Point initPos, float gravity, float jumpVelocity, int powerSP, Layer* canvas)
{

	//character state 초기화
	state = (charStateType)NONE;
	jumpVelo = jumpVelocity;
	curVeloY = 0;
	curGravity = gravity;
	outerVeloY = 0;


	landHeight = initPos.y;
	powerSP = 40;
	powerNormal = 220;	//기본데미지 임시 셋팅


	curCharAction = actNone;
	curDefPoint = maxDefPoint;
	this->canvas = canvas;


	sprChar = Sprite::create("motions/default_land/1.png");
	sprChar->setName("Body");
	bodyScale = 0.108f;
	sprChar->setScale(bodyScale * DivForHorizontal(sprChar));
	sprChar->setPosition(initPos);
	//sprChar->setAnchorPoint(Point(0.5f, 0));
	canvas->addChild(sprChar, 2);


	//weapon sprite가 올라갈 컨테이너
	sprCircle = Sprite::create("motions/circle.png");
	sprCircle->setName("Circle");
	sprCircle->setPosition(sprChar->getContentSize().width / 2, sprChar->getContentSize().height*0.36);
	sprCircle->setOpacity(0);
	sprCircle->setRotation(41);
	sprChar->addChild(sprCircle);


	sprWeapon = Sprite::create("weapon/sword.png");
	sprWeapon->setName("weapon");
	defWpScale = 0.05f;
	curWpScale = defWpScale;
	sprWeapon->setRotation(4);
	sprWeapon->setAnchorPoint(Point(0.5f, 0.06f));
	sprWeapon->setPosition(sprCircle->getContentSize().width*0.74f, sprCircle->getContentSize().height / 2);
	sprCircle->addChild(sprWeapon);	//더미용 원 => 위에 검 올림


	colliderChar = LayerColor::create();
	colliderChar->setColor(Color3B::BLUE);
	colliderChar->ignoreAnchorPointForPosition(false);
	float collider_scale = bodyScale * 0.5f;
	colliderChar->setScaleX(collider_scale);
	colliderChar->setScaleY(sqrtf(3.0) * collider_scale / winAspectRatio());
	colliderChar->setPosition(initPos);	//sprChar와 함께 이동
	colliderChar->setAnchorPoint(Point(0.5f, 0.6f));
	colliderChar->setOpacity(0);
	canvas->addChild(colliderChar, 3);


	//공격범위 체크를 위한 더미레이어
	attackRadar = LayerColor::create();
	attackRadar->setColor(Color3B::RED);
	attackRadar->ignoreAnchorPointForPosition(false);	//레이어의 앵커포인트 조절을 위해 필수
	attackRadar->setAnchorPoint(Point(0.5, 0));
	attackRadar->setPosition(sprChar->getContentSize().width / 2, sprChar->getContentSize().height*0.36);
	attackRadar->setOpacity(0);
	sprChar->addChild(attackRadar, 3);


	shieldRadar = LayerColor::create();
	shieldRadar->setColor(Color3B::GREEN);
	shieldRadar->ignoreAnchorPointForPosition(false);
	shieldRadar->setAnchorPoint(Point(0.5, 0));
	shieldRadar->setPosition(sprChar->getContentSize().width / 2, sprChar->getContentSize().height*0.36);
	float shield_radar_scale = bodyScale*3.0f;
	shieldRadar->setScaleX(shield_radar_scale / sprChar->getScale() * 1.5f);
	shieldRadar->setScaleY(shield_radar_scale / sprChar->getScale() / winAspectRatio() * 0.5f);
	//shieldRadar->setScale(0.9f, 0.2f);
	shieldRadar->setOpacity(0);
	sprChar->addChild(shieldRadar, 6);


	//필살기범위를 지정해주는 더미레이더
	specialRadar = LayerColor::create();
	specialRadar->setColor(Color3B::BLUE);
	specialRadar->ignoreAnchorPointForPosition(false);
	specialRadar->setAnchorPoint(Point(0.5, 0));
	specialRadar->setPosition(initPos);
	//specialRadar->setScale(0.05f, 1.0f);
	//specialRadar->setScale(DivForVertical(sprWeapon)*0.028f*weaponScale, 1.5f);
	specialRadar->setOpacity(0);
	//canvas->addChild(specialRadar, 5);
	setWpScale(curWpScale);
	initMotionFrames();
}


Character::~Character() {
	//cocos2d::Director::getInstance()->getScheduler()->unschedule("CharacterUpdate", this);
}


//캐릭터 액션 애니메이션에 필요한 프레임 삽입
void Character::initMotionFrames() {

	//actNone 
	SpriteFrameCache::getInstance()->removeSpriteFrames();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/Motion_Else.plist");

	auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Default.png"));
	motionFrames[actNone].pushBack(frame);

	//actJumpUpReady
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_JumpUp_Ready_%d.png", i + 1));
		motionFrames[actJumpReady].pushBack(frame);
	}
	//actJumpUp
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_JumpUp_%d.png", i + 1));
		motionFrames[actJumpUp].pushBack(frame);
	}
	//actJumpDown
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_JumpDown_%d.png", i + 1));
		motionFrames[actJumpDown].pushBack(frame);
	}
	//actDefLand
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Defense_Land_%d.png", i + 1));
		motionFrames[actDefLand].pushBack(frame);
	}
	//actDefLandEnd
	for (int i = 4; i > 0; i--) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Defense_Land_%d.png", i));
		motionFrames[actDefLandEnd].pushBack(frame);
	}
	//actDefJump
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Defense_Jump_%d.png", i + 1));
		motionFrames[actDefJump].pushBack(frame);
	}
	//actDefJumpEnd
	for (int i = 4; i > 0; i--) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Defense_Jump_%d.png", i));
		motionFrames[actDefJumpEnd].pushBack(frame);
	}
	//actSPCLand
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Special_Land_%d.png", i + 1));
		motionFrames[actSPCLand].pushBack(frame);
	}
	//actSPCLandEnd
	for (int i = 4; i > 0; i--) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Special_Land_%d.png", i));
		motionFrames[actSPCLandEnd].pushBack(frame);
	}
	//actSPCJump
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Special_Jump_%d.png", i + 1));
		motionFrames[actSPCJump].pushBack(frame);
	}
	//actSPCJumpEnd
	for (int i = 4; i > 0; i--) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_Special_Jump_%d.png", i));
		motionFrames[actSPCJumpEnd].pushBack(frame);
	}
	//actMoveLeft
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_MoveLeft_%d.png", i + 1));
		motionFrames[actMoveLeft].pushBack(frame);
	}
	//actMoveLeftEnd
	for (int i = 3; i > 0; i--) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_MoveLeft_%d.png", i));
		motionFrames[actMoveLeftEnd].pushBack(frame);
	}
	//actMoveRight
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_MoveRight_%d.png", i + 1));
		motionFrames[actMoveRight].pushBack(frame);
	}
	//actMoveRightEnd
	for (int i = 3; i > 0; i--) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Else_MoveRight_%d.png", i));
		motionFrames[actMoveRightEnd].pushBack(frame);
	}


	//actAtkLandReady
	SpriteFrameCache::getInstance()->removeSpriteFrames();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("motions/Motion_Attack.plist");
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Attack_Land_Ready_%d.png", i + 1));
		motionFrames[actAtkLandReady].pushBack(frame);
	}
	//actAtkLandHit
	for (int i = 0; i < 9; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Attack_Land_Hit_%d.png", i + 1));
		motionFrames[actAtkLandHit].pushBack(frame);
	}
	//actAtkLandBack
	for (int i = 0; i < 6; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Attack_Land_Back_%d.png", i + 1));
		motionFrames[actAtkLandBack].pushBack(frame);
	}
	//actAtkJumpReady
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Attack_Jump_Ready_%d.png", i + 1));
		motionFrames[actAtkJumpReady].pushBack(frame);
	}
	//actAtkJumpHit
	for (int i = 0; i < 9; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Attack_Jump_Hit_%d.png", i + 1));
		motionFrames[actAtkJumpHit].pushBack(frame);
	}
	//actAtkJumpBack
	for (int i = 0; i < 6; i++) {
		auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Attack_Jump_Back_%d.png", i + 1));
		motionFrames[actAtkJumpBack].pushBack(frame);
	}

	//initFinishFrame
	SpriteFrameCache::getInstance()->removeSpriteFrames();
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


//현재 play되고 있는 charAction을 멈추고, 모션락을 획득
void Character::getMotionLock(charActionType input_action)
{

	//다른 액션 진행 중, MotionLock을 획득하려는 액션에 대해, 선택적인 콜백 후처리
	if (isMotionPlaying)
	{
		this->sprChar->stopActionByTag(MOTION_ACTION);
		this->sprCircle->stopActionByTag(MOTION_ACTION);
		this->sprWeapon->stopActionByTag(MOTION_ACTION);

		switch (curCharAction)
		{
		case actAtkLandHit:
			delState(ATTACK);
			break;
		case actAtkJumpHit:
			delState(ATTACK);
			break;
		default:
			break;
		}
	}

	//현재 진행 액션을 갱신
	curCharAction = input_action;

	//모션락 비트를 모두 on
	motionLockBit |= ANIMATION; motionLockBit |= CIRCLE; motionLockBit |= WEAPON;
	//모션락 플래그 획득
	isMotionPlaying = true;
};
void Character::releaseMotionLock(charActionType input_action, charMotionElement target)
{
	//target 액션에 대하여, 락을 해제한다.
	switch (target)
	{
	case ANIMATION:
		motionLockBit &= ~ANIMATION;
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

	//전체 모션락이 해제 가능하면, 해제
	if (motionLockBit == 0)
	{
		isMotionPlaying = false;

		//action 후 콜백처리 여기서
		switch (input_action)
		{
		case actJumpCancel:
		{
			if (isDefense())
				setCharFrame(actDefLand);
			else if (getOnStateTrigger(ATTACK, TOUCH))
			{
				if (getOnStateTrigger(ATTACK, SWIPE))
					setCharFrame(actAtkLandHit);
				else
					setCharFrame(actAtkLandReady);
			}
			else if (isMove())
			{
				if (getOnStateInput(MOVE, LEFT) && !getOnStateInput(MOVE, RIGHT))
					setCharFrame(actMoveLeft);
				else if (!getOnStateInput(MOVE, LEFT) && getOnStateInput(MOVE, RIGHT))
					setCharFrame(actMoveRight);
			}
			else {
				if (isJump()) {
					if (getOnStateTrigger(JUMP, DOWN))
						setCharFrame(actJumpDown);
					else
						setCharFrame(actJumpUp);
				}
				else
					setCharFrame(actNone);
			}
		}
		break;
		case actAtkLandHit:
		{
			delState(ATTACK);
		}
		break;
		case actAtkLandCancel:
		{
			setCharFrame(actNone);
		}
		break;
		case actAtkJumpHit:
		{
			delState(ATTACK);
		}
		break;
		case actAtkJumpCancel:
		{
			if (getOnStateInput(JUMP, DOWN))
				setCharFrame(actJumpDown);
			else
				setCharFrame(actJumpUp);
		}
		break;
		case actDefLandEnd:
		{
			if (getOnStateInput(MOVE, LEFT) && !getOnStateInput(MOVE, RIGHT))
				setCharFrame(actMoveLeft);
			else if (!getOnStateInput(MOVE, LEFT) && getOnStateInput(MOVE, RIGHT))
				setCharFrame(actMoveRight);
			else
				setCharFrame(actNone);
		}
		break;
		case actDefJumpEnd:
		{
			if (getOnStateTrigger(JUMP, DOWN))
				setCharFrame(actJumpDown);
			else
				setCharFrame(actJumpUp);
		}
		break;
		case actMoveLeftEnd:
		{
			if (isJump()) {
				if (getOnStateTrigger(JUMP, DOWN))
					setCharFrame(actJumpDown);
				else
					setCharFrame(actJumpUp);
			}
			else
				setCharFrame(actNone);
		}
		break;
		case actMoveRightEnd:
		{
			if (isJump()) {
				if (getOnStateTrigger(JUMP, DOWN))
					setCharFrame(actJumpDown);
				else
					setCharFrame(actJumpUp);
			}
			else
				setCharFrame(actNone);
		}
		break;
		default:
			break;
		}
	}
};


int Character::chkAttckRadar(const Sprite& attackable_unit)
{
	auto unit_bound_box = attackable_unit.getBoundingBox();
	Point world_origin = attackable_unit.getParent()->convertToWorldSpace(unit_bound_box.origin);
	Point trans_origin = sprChar->convertToNodeSpace(world_origin);

	//bound_box를 attack radar의 좌표계로 transformatioin
	unit_bound_box.origin = trans_origin;
	unit_bound_box.size.operator/ (sprChar->getScale());


	//프레임 드랍을 막기위해, Radar하는 Bounding 영역에 들어왔을 때만, 세부 데미지 계산
	if (attackRadar->getBoundingBox().intersectsRect(unit_bound_box))
	{
		Point trans_pos = Point(unit_bound_box.getMidX(), unit_bound_box.getMidY());

		//유효데미지 체크
		Vec2 cur_attack_vector = trans_pos - sprCircle->getPosition();
		float cur_dist = cur_attack_vector.getLength();
		float cur_angle = ccpToAngle(cur_attack_vector);


		//현재 공격이 유효타격 범위 안에 있는지를 검사 하여 데미지 계산
		if (cur_dist < getWpHeight())
		{
			//유효 타격 범위 구함
			float unit_scope = (atkScopeAngle.second - atkScopeAngle.first) / 3;
			float min_bound = 2 * unit_scope * lapsedAtkTick;

			if (atkScopeAngle.first + min_bound < cur_angle && cur_angle < atkScopeAngle.first + unit_scope + min_bound)
			{
				float damage_factor = cur_dist / getWpHeight();
				return powerNormal * (1 - damage_factor*damage_factor);
			}
		}
	}
	return 0;
};
int Character::chkDefenseRadar(const Sprite& defensable)
{
	auto bound_box = defensable.getBoundingBox();
	Point world_origin = defensable.getParent()->convertToWorldSpace(bound_box.origin);
	Point trans_origin = sprChar->convertToNodeSpace(world_origin);

	bound_box.origin = trans_origin;
	bound_box.size.operator/ (sprChar->getScale());

	//shield radar에 들어와 있는 block인지 체크
	if (shieldRadar->getBoundingBox().intersectsRect(bound_box))
	{
		return 5;
	}
	else
		return -1;
}



//
//
////호출된 frame 시점에, blk이 받는 damage를 계산
//int Character::getAttackDamage(Block* blk)
//{
//	if (isAttack() && attackID != blk->getAttackID())
//	{
//		//(block) node 좌표 -> world 좌표 -> (character) node 좌표
//		Point world_blk_pos = blk->sprBlk->getParent()->convertToWorldSpace(blk->getPosition());
//		Point trans_blk_pos = sprChar->convertToNodeSpace(world_blk_pos);
//
//		//프레임 드랍을 막기위해, Radar하는 Bounding 영역에 들어왔을 때만, 세부 데미지 계산
//		if (attackRadar->getBoundingBox().intersectsCircle(trans_blk_pos, blk->getBlkWidth() * 0.5f / sprChar->getScale()))
//		{
//			//CCLOG("radar size %.2f Block Size %.2f", attackRadar->getBoundingBox().size.width,
//				//blk->getBlkHeight()/sprChar->getScale());
//
//			//attack 중심 -> 블록 중심 Vec2 => length와 angle로 판단
//			Vec2 cur_attack_vector = trans_blk_pos - sprCircle->getPosition();
//			float cur_dist = cur_attack_vector.getLength();
//			float cur_angle = ccpToAngle(cur_attack_vector);
//
//			//유효 타격 범위 구함
//			float angle_scope = (attackAngle.second - attackAngle.first) / 3;
//			float cur_angle_delta = 2 * angle_scope * lapsedAttackTick;
//
//			//현재 공격이 유효타격 범위 안에 있는지를 검사 하여 데미지 계산
//			if (cur_dist < getWpHeight()
//				&& attackAngle.first + cur_angle_delta < cur_angle
//				&& cur_angle < attackAngle.first + angle_scope + cur_angle_delta) {
//
//				float damage_factor = cur_dist / getWpHeight();
//				return normalDamage * (1 - damage_factor*damage_factor);
//			}
//		}
//	}
//	return 0;
//
//
//	//Point blk_pos = sprCircle->convertToNodeSpace(blk->getPosition());
//	////공격 중심 -> block 까지의 거리 계산
//	//Vec2 relative_vec = blk->getPosition() - sprCircle->convertToWorldSpace(sprCircle->getPosition());
//	//float dist = 0;
//	//float angle = ccpToAngle(relative_vec);
//
//	//if (attackAngle.first < angle && angle < attackAngle.second)
//	//{
//	//	if (attackRadius.first < dist && dist < attackRadius.second)
//	//	{
//	//		return normalDamage * dist / attackRadius.second;
//	//	}
//	//}
//	//return 0;
//}
//
//



//charAction에 대한 Animation 액션을 실행
void Character::playCharAction(charActionType targetAction) {

	//실행할 액션의 모션락을 획득한다
	getMotionLock(targetAction);

	switch (targetAction)
	{
	case actJumpReady:
	{
		//case문 안에서 생성 구문 있을 때, {} 바인딩을 해줘야 하는듯.. 안하면 에러
		auto delay = DelayTime::create(FRAME_DELAY*1.5f);
		auto animation = Animation::createWithSpriteFrames(motionFrames[actJumpReady], FRAME_DELAY*1.5f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actJumpReady, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		//원 rotate액션 정의 - 애니메이션의 각 frame에 대해 위치 해야할 각도 지정
		auto circle_rot_1 = RotateTo::create(0.0f, 38.0f);
		auto circle_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 38.0f), nullptr);
		auto circle_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 38.0f), nullptr);
		auto circleAction = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actJumpReady, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		//검 rotate액션 
		auto wp_rot_1 = RotateTo::create(0.0f, 18.0f);
		auto wp_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 18.0f), nullptr);
		auto wp_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 18.0f), nullptr);
		auto wpAction = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actJumpReady, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actJumpCancel:
	{
		//case문 안에서 초기화 시, {} 바인딩을 해줘야 하는듯.. 안하면 에러
		auto delay = DelayTime::create(FRAME_DELAY*1.5f);
		auto animation = Animation::createWithSpriteFrames(motionFrames[actJumpReady], FRAME_DELAY*1.5f);

		auto animAction = Sequence::create(Animate::create(animation)->reverse(),
			CallFunc::create([&]() {releaseMotionLock(actJumpCancel, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		//원 rotate액션 정의 - 애니메이션의 각 frame에 대해 위치 해야할 각도 지정
		auto circle_rot_1 = RotateTo::create(0.0f, 38.0f);
		auto circle_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 38.0f), nullptr);
		auto circle_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 38.0f), nullptr);
		auto circleAction = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actJumpCancel, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		//검 rotate액션 
		auto wp_rot_1 = RotateTo::create(0.0f, 18.0f);
		auto wp_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 18.0f), nullptr);
		auto wp_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 18.0f), nullptr);
		auto wpAction = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actJumpCancel, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actJumpUp:
	{
		//case문 안에서 초기화 시, {} 바인딩을 해줘야 하는듯.. 안하면 에러
		auto delay = DelayTime::create(FRAME_DELAY*1.5f);
		auto animation = Animation::createWithSpriteFrames(motionFrames[actJumpUp], FRAME_DELAY*1.5f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actJumpUp, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		//원 rotate액션 정의 - 애니메이션의 각 frame에 대해 위치 해야할 각도 지정
		auto circle_rot_1 = RotateTo::create(0.0f, 38.0f);
		auto circle_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 38.0f), nullptr);
		auto circle_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 38.0f), nullptr);
		auto circleAction = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actJumpUp, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		//검 rotate액션 
		auto wp_rot_1 = RotateTo::create(0.0f, 18.0f);
		auto wp_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 18.0f), nullptr);
		auto wp_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 16.0f), nullptr);
		auto wpAction = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actJumpUp, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actJumpDown:
	{
		auto delay = DelayTime::create(FRAME_DELAY*1.5f);
		auto animation = Animation::createWithSpriteFrames(motionFrames[actJumpDown], FRAME_DELAY*1.5f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actJumpDown, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		//원 rotate액션 정의
		auto circle_rot_1 = RotateTo::create(0.0f, 38.0f);
		auto circle_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 30.0f), nullptr);
		auto circle_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 30.0f), nullptr);
		auto circle_rot_4 = Sequence::create(delay, RotateTo::create(0.0f, -20.0f), nullptr);
		auto circle_rot_5 = Sequence::create(delay, RotateTo::create(0.0f, -20.0f), nullptr);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3, circle_rot_4, circle_rot_5, nullptr);
		auto circleAction = Sequence::create(circle_seq,
			CallFunc::create([&]() {releaseMotionLock(actJumpDown, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		//검 rotate 액션 정의
		auto wp_rot_1 = RotateTo::create(0.0f, 18.0f);
		auto wp_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 18.0f), nullptr);
		auto wp_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 18.0f), nullptr);
		auto wp_rot_4 = Sequence::create(delay, RotateTo::create(0.0f, 40.0f), nullptr);
		auto wp_rot_5 = Sequence::create(delay, RotateTo::create(0.0f, 40.0f), nullptr);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3, wp_rot_4, wp_rot_5, nullptr);
		auto wpAction = Sequence::create(wp_seq,
			CallFunc::create([&]() {releaseMotionLock(actJumpDown, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actAtkLandReady:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkLandReady], FRAME_DELAY*0.8f*0.8f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actAtkLandReady, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		//원 rotate액션 정의
		auto circle_rot_1 = RotateTo::create(0, 50.0f);	//시작 시, 원 각도 약간 젖히기
		auto circle_rot_2 = RotateBy::create(FRAME_DELAY * 0.8f * 4.5f, -95.0f);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, nullptr);
		auto circleAction = Sequence::create(circle_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkLandReady, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		//무기 rotate 액션 정의
		auto wp_rot_1 = RotateTo::create(0, 6.0f);
		auto wp_rot_2 = RotateBy::create(FRAME_DELAY * 0.8f * 4.5f, 74.0f);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, nullptr);
		auto wpAction = Sequence::create(wp_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkLandReady, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actAtkLandHit:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkLandHit], 1.5f * FRAME_DELAY*0.6f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actAtkLandHit, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		//원 rotate액션 정의
		auto circle_rot_1 = RotateTo::create(0, -45.0f);	//시작 시, 원 각도 약간 젖히기
		auto circle_rot_2 = RotateBy::create(1.5f * FRAME_DELAY * 0.6f * 5, -223.0f);
		auto circle_rot_3 = DelayTime::create(1.5f * FRAME_DELAY * 0.6f * 3);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3, nullptr);
		auto circleAction = Sequence::create(circle_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkLandHit, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		//무기 rotate 액션 정의
		auto wp_rot_1 = RotateTo::create(0, 80.0f);
		auto wp_rot_2 = RotateBy::create(1.5f * FRAME_DELAY * 0.6f * 5, 120.0f);
		auto wp_rot_3 = DelayTime::create(1.5f * FRAME_DELAY * 0.6f * 3);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3, nullptr);
		auto wpAction = Sequence::create(wp_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkLandHit, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		//attack dist, angle 증가시키는 tick 스케쥴 실행
		lapsedAtkTick = 0.0f;
		Director::getInstance()->getScheduler()
			->schedule(schedule_selector(Character::callback_tick_AtkScope), this, 1 / 60.f, false);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actAtkLandBack:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkLandBack], FRAME_DELAY*0.5f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actAtkLandBack, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		//원 rotate액션 정의
		auto circle_rot_1 = RotateTo::create(0, finishRotation[actAtkLandHit].first);
		auto circle_rot_2 = RotateBy::create(FRAME_DELAY * 0.5f * 5, 223.0f);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, nullptr);
		auto circleAction = Sequence::create(circle_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkLandBack, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		//무기 rotate 액션 정의
		auto wp_rot_1 = RotateTo::create(0, finishRotation[actAtkLandHit].second);
		auto wp_rot_2 = RotateBy::create(FRAME_DELAY * 0.5f * 5, -120.0f);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, nullptr);
		auto wpAction = Sequence::create(wp_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkLandBack, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actAtkLandCancel:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkLandReady], FRAME_DELAY*0.6f*0.8f);
		auto animAction = Sequence::create(Animate::create(animation)->reverse(),
			CallFunc::create([&]() {releaseMotionLock(actAtkLandCancel, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		//원 rotate액션 정의
		auto circle_rot_1 = RotateTo::create(0, finishRotation[actAtkLandReady].first);
		auto circle_rot_2 = RotateBy::create(FRAME_DELAY * 0.6f * 4.5f, 86.0f);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, nullptr);
		auto circleAction = Sequence::create(circle_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkLandCancel, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);


		//무기 rotate 액션 정의
		auto wp_rot_1 = RotateTo::create(0, finishRotation[actAtkLandReady].second);
		auto wp_rot_2 = RotateBy::create(FRAME_DELAY * 0.6f * 4.5f, -76.0f);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, nullptr);
		auto wpAction = Sequence::create(wp_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkLandCancel, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);


		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actAtkJumpReady:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkJumpReady], FRAME_DELAY*0.8f * 0.8f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actAtkJumpReady, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);


		auto circle_rot_1 = RotateTo::create(0, 56.0f);
		auto circle_rot_2 = RotateBy::create(FRAME_DELAY*0.8f * 4.5f, -98.0f);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, nullptr);
		auto circleAction = Sequence::create(circle_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkJumpReady, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);


		auto wp_rot_1 = RotateTo::create(0, 10.0f);
		auto wp_rot_2 = RotateBy::create(FRAME_DELAY*0.8f * 4.5f, 73.0f);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, nullptr);
		auto wpAction = Sequence::create(wp_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkJumpReady, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);


		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actAtkJumpHit:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkJumpHit], FRAME_DELAY*0.6f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actAtkJumpHit, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		auto circle_rot_1 = RotateTo::create(0, -42.0f);
		auto circle_rot_2 = RotateBy::create(FRAME_DELAY*0.6f * 5, -226.0f);
		auto circle_rot_3 = DelayTime::create(FRAME_DELAY*0.6f * 3);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3, nullptr);
		auto circleAction = Sequence::create(circle_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkJumpHit, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		auto wp_rot_1 = RotateTo::create(0, 83.0f);
		auto wp_rot_2 = RotateBy::create(FRAME_DELAY*0.6f * 5, 117.0f);
		auto wp_rot_3 = DelayTime::create(FRAME_DELAY*0.6f * 3);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3, nullptr);
		auto wpAction = Sequence::create(wp_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkJumpHit, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);


		lapsedAtkTick = 0.0f;
		Director::getInstance()->getScheduler()
			->schedule(schedule_selector(Character::callback_tick_AtkScope), this, 1 / 60.f, false);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actAtkJumpBack:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkJumpBack], FRAME_DELAY*0.5f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actAtkJumpBack, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		//원 rotate액션 정의
		auto circle_rot_1 = RotateTo::create(0, finishRotation[actAtkJumpHit].first);
		auto circle_rot_2 = RotateBy::create(FRAME_DELAY * 0.5f * 5, 226.0f);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, nullptr);
		auto circleAction = Sequence::create(circle_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkJumpBack, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		//무기 rotate 액션 정의
		auto wp_rot_1 = RotateTo::create(0, finishRotation[actAtkJumpHit].second);
		auto wp_rot_2 = RotateBy::create(FRAME_DELAY * 0.5f * 5, -117.0f);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, nullptr);
		auto wpAction = Sequence::create(wp_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkJumpBack, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actAtkJumpCancel:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actAtkJumpReady], FRAME_DELAY*0.6f*0.8f);
		auto animAction = Sequence::create(Animate::create(animation)->reverse(),
			CallFunc::create([&]() {releaseMotionLock(actAtkJumpCancel, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		//원 rotate액션 정의
		auto circle_rot_1 = RotateTo::create(0, finishRotation[actAtkJumpReady].first);
		auto circle_rot_2 = RotateBy::create(FRAME_DELAY * 0.6f * 4.5f, 83.0f);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, nullptr);
		auto circleAction = Sequence::create(circle_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkJumpCancel, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		//무기 rotate 액션 정의
		auto wp_rot_1 = RotateTo::create(0, finishRotation[actAtkJumpReady].second);
		auto wp_rot_2 = RotateBy::create(FRAME_DELAY * 0.6f * 4.5f, -79.0f);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, nullptr);
		auto wpAction = Sequence::create(wp_seq,
			CallFunc::create([&]() {releaseMotionLock(actAtkJumpCancel, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actDefLand:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actDefLand], FRAME_DELAY*0.8f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actDefLand, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		//원 rotate액션 정의
		auto circle_rot_1 = RotateTo::create(0.0f, 35.0f);
		auto circleAction = Sequence::create(circle_rot_1,
			CallFunc::create([&]() {releaseMotionLock(actDefLand, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		//검 rotate 액션 정의
		auto wp_rot_1 = RotateTo::create(0.0f, 27.0f);
		auto wpAction = Sequence::create(wp_rot_1,
			CallFunc::create([&]() {releaseMotionLock(actDefLand, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actDefLandEnd:
	{
		auto delay = DelayTime::create(FRAME_DELAY*0.8f);
		auto animation = Animation::createWithSpriteFrames(motionFrames[actDefLandEnd], FRAME_DELAY*0.8f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actDefLandEnd, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);


		//원 rotate액션 정의
		auto circle_rot_1 = Spawn::create(RotateTo::create(0.0f, 35.0f), delay, nullptr);
		auto circle_rot_2 = Spawn::create(RotateTo::create(0.0f, 35.0f), delay, nullptr);
		auto circle_rot_3 = Spawn::create(RotateTo::create(0.0f, 35.0f), delay, nullptr);
		auto circleAction = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actDefLandEnd, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);


		//검 rotate액션 
		auto wp_rot_1 = Spawn::create(RotateTo::create(0.0f, 27.0f), delay, nullptr);
		auto wp_rot_2 = Spawn::create(RotateTo::create(0.0f, 27.0f), delay, nullptr);
		auto wp_rot_3 = Spawn::create(RotateTo::create(0.0f, 27.0f), delay, nullptr);
		auto wpAction = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actDefLandEnd, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);


		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actDefJump:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actDefJump], FRAME_DELAY*0.8f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actDefJump, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		//원 rotate액션 정의
		auto circle_rot_1 = RotateTo::create(0.0f, 53.0f);
		auto circleAction = Sequence::create(circle_rot_1,
			CallFunc::create([&]() {releaseMotionLock(actDefJump, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		auto wp_rot_1 = RotateTo::create(0.0f, 27.0f);
		auto wpAction = Sequence::create(wp_rot_1,
			CallFunc::create([&]() {releaseMotionLock(actDefJump, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actDefJumpEnd:
	{
		auto delay = DelayTime::create(FRAME_DELAY*0.8f);
		auto animation = Animation::createWithSpriteFrames(motionFrames[actDefJumpEnd], FRAME_DELAY*0.8f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actDefJumpEnd, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);


		//원 rotate액션 정의
		auto circle_rot_1 = Spawn::create(RotateTo::create(0.0f, 53.0f), delay, nullptr);
		auto circle_rot_2 = Spawn::create(RotateTo::create(0.0f, 53.0f), delay, nullptr);
		auto circle_rot_3 = Spawn::create(RotateTo::create(0.0f, 53.0f), delay, nullptr);
		auto circleAction = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actDefJumpEnd, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);


		//검 rotate액션 
		auto wp_rot_1 = Spawn::create(RotateTo::create(0.0f, 27.0f), delay, nullptr);
		auto wp_rot_2 = Spawn::create(RotateTo::create(0.0f, 27.0f), delay, nullptr);
		auto wp_rot_3 = Spawn::create(RotateTo::create(0.0f, 27.0f), delay, nullptr);
		auto wpAction = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actDefJumpEnd, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actSPCLand:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actSPCLand], FRAME_DELAY*12.0f);
		auto finish_callback = CallFuncN::create(CC_CALLBACK_1(Character::callback_FinishSpcIntro, this));
		auto animAction = Sequence::create(Animate::create(animation), finish_callback,
			CallFunc::create([&]() {releaseMotionLock(actSPCLand, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);


		//원 rotate액션 정의
		//1~5프레임 -106,  6~10프레임 -125
		auto circle_rot_1 = RotateTo::create(0, 64.0f);	//시작 시, 검 각도 홱 젖히기
		auto circle_rot_2 = RotateBy::create(FRAME_DELAY * 0.6f * 5, -110.0f);
		auto circle_rot_3 = RotateBy::create(FRAME_DELAY * 0.6f * 5, -142.0f);
		auto circle_seq = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3, nullptr);
		auto circleAction = Sequence::create(circle_seq,
			CallFunc::create([&]() {releaseMotionLock(actSPCLand, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);


		auto wp_rot_1 = RotateTo::create(0, 20.0f);
		auto wp_rot_2 = RotateBy::create(FRAME_DELAY*0.6f * 5, 55.0f);
		auto wp_rot_3 = RotateBy::create(FRAME_DELAY*0.6f * 5, 45.0f);
		auto wp_seq = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3, nullptr);
		auto wpAction = Sequence::create(wp_seq,
			CallFunc::create([&]() {releaseMotionLock(actSPCLand, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		//sprCircle->runAction(circleAction);
		//sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
		//EffectManager::getInstance()->runSpecialEffect(this, canvas, FRAME_DELAY*12.0f * 4);
	}
	break;
	case actSPCLandEnd:
	{
		auto animation = Animation::createWithSpriteFrames(motionFrames[actSPCLandEnd], FRAME_DELAY*5.0f);
		auto finish_callback = CallFuncN::create(CC_CALLBACK_1(Character::callback_DelSpc, this));
		auto animAction = Sequence::create(Animate::create(animation), finish_callback,
			CallFunc::create([&]() {releaseMotionLock(actSPCLandEnd, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		sprChar->runAction(animAction);
	}
	break;
	case actSPCJump:
		break;
	case actSPCJumpEnd:
		break;
	case actMoveLeft:
	{
		auto delay = DelayTime::create(FRAME_DELAY * 1.5f);
		auto animation = Animation::createWithSpriteFrames(motionFrames[actMoveLeft], FRAME_DELAY * 1.5f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actMoveLeft, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);


		auto circle_rot_1 = RotateTo::create(0.0f, 28.0f);
		auto circle_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 28.0f), nullptr);
		auto circle_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 28.0f), nullptr);
		auto circleAction = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actMoveLeft, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);


		auto wp_rot_1 = RotateTo::create(0.0f, 10.0f);
		auto wp_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 10.0f), nullptr);
		auto wp_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 13.0f), nullptr);
		auto wpAction = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actMoveLeft, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actMoveLeftEnd:
	{
		auto delay = DelayTime::create(FRAME_DELAY * 1.5f);
		auto animation = Animation::createWithSpriteFrames(motionFrames[actMoveLeftEnd], FRAME_DELAY * 1.5f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actMoveLeftEnd, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		auto circle_rot_1 = RotateTo::create(0.0f, 28.0f);
		auto circle_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 28.0f), nullptr);
		auto circle_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 28.0f), nullptr);
		auto circleAction = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actMoveLeftEnd, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		auto wp_rot_1 = RotateTo::create(0.0f, 13.0f);
		auto wp_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 13.0f), nullptr);
		auto wp_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 10.0f), nullptr);
		auto wpAction = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actMoveLeftEnd, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);

		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actMoveRight:
	{
		auto delay = DelayTime::create(FRAME_DELAY * 1.5f);
		auto animation = Animation::createWithSpriteFrames(motionFrames[actMoveRight], FRAME_DELAY * 1.5f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actMoveRight, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);

		auto circle_rot_1 = RotateTo::create(0.0f, 151.0f);
		auto circle_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 151.0f), nullptr);
		auto circle_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 151.0f), nullptr);
		auto circleAction = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actMoveRight, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);

		auto wp_rot_1 = RotateTo::create(0.0f, 161.0f);
		auto wp_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 161.0f), nullptr);
		auto wp_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 170.0f), nullptr);
		auto wpAction = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actMoveRight, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);


		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	case actMoveRightEnd:
	{
		auto delay = DelayTime::create(FRAME_DELAY * 1.5f);
		auto animation = Animation::createWithSpriteFrames(motionFrames[actMoveRightEnd], FRAME_DELAY * 1.5f);
		auto animAction = Sequence::create(Animate::create(animation),
			CallFunc::create([&]() {releaseMotionLock(actMoveRightEnd, ANIMATION); }), nullptr);
		animAction->setTag(MOTION_ACTION);


		auto circle_rot_1 = RotateTo::create(0.0f, 151.0f);
		auto circle_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 151.0f), nullptr);
		auto circle_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 151.0f), nullptr);
		auto circleAction = Sequence::create(circle_rot_1, circle_rot_2, circle_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actMoveRightEnd, CIRCLE); }), nullptr);
		circleAction->setTag(MOTION_ACTION);


		auto wp_rot_1 = RotateTo::create(0.0f, 170.0f);
		auto wp_rot_2 = Sequence::create(delay, RotateTo::create(0.0f, 170.0f), nullptr);
		auto wp_rot_3 = Sequence::create(delay, RotateTo::create(0.0f, 161.0f), nullptr);
		auto wpAction = Sequence::create(wp_rot_1, wp_rot_2, wp_rot_3,
			CallFunc::create([&]() {releaseMotionLock(actMoveRightEnd, WEAPON); }), nullptr);
		wpAction->setTag(MOTION_ACTION);


		sprCircle->runAction(circleAction);
		sprWeapon->runAction(wpAction);
		sprChar->runAction(animAction);
	}
	break;
	default:
		break;
	}
}
//charAction에 대한 고정frame으로 교체 (각 Action이 끝났을 때)
void Character::setCharFrame(charActionType targetAction)
{
	switch (targetAction)
	{
	case actNone:
		sprChar->setSpriteFrame(finishFrame[actNone]);
		sprCircle->setRotation(finishRotation[actNone].first);
		sprWeapon->setRotation(finishRotation[actNone].second);
		break;
	case actJumpReady:
		sprChar->setSpriteFrame(finishFrame[actAtkJumpHit]);
		sprCircle->setRotation(finishRotation[actAtkJumpHit].first);
		sprWeapon->setRotation(finishRotation[actAtkJumpHit].second);
		break;
	case actJumpUp:
		sprChar->setSpriteFrame(finishFrame[actJumpUp]);
		sprCircle->setRotation(finishRotation[actJumpUp].first);
		sprWeapon->setRotation(finishRotation[actJumpUp].second);
		break;
	case actJumpDown:
		sprChar->setSpriteFrame(finishFrame[actJumpDown]);
		sprCircle->setRotation(finishRotation[actJumpDown].first);
		sprWeapon->setRotation(finishRotation[actJumpDown].second);
		break;
	case actAtkLandReady:
		sprChar->setSpriteFrame(finishFrame[actAtkLandReady]);
		sprCircle->setRotation(finishRotation[actAtkLandReady].first);
		sprWeapon->setRotation(finishRotation[actAtkLandReady].second);
		break;
	case actAtkLandHit:
		sprChar->setSpriteFrame(finishFrame[actAtkLandHit]);
		sprCircle->setRotation(finishRotation[actAtkLandHit].first);
		sprWeapon->setRotation(finishRotation[actAtkLandHit].second);
		break;
	case actAtkJumpReady:
		sprChar->setSpriteFrame(finishFrame[actAtkJumpReady]);
		sprCircle->setRotation(finishRotation[actAtkJumpReady].first);
		sprWeapon->setRotation(finishRotation[actAtkJumpReady].second);
		break;
	case actAtkJumpHit:
		sprChar->setSpriteFrame(finishFrame[actAtkJumpHit]);
		sprCircle->setRotation(finishRotation[actAtkJumpHit].first);
		sprWeapon->setRotation(finishRotation[actAtkJumpHit].second);
		break;
	case actDefLand:
		sprChar->setSpriteFrame(finishFrame[actDefLand]);
		sprCircle->setRotation(finishRotation[actDefLand].first);
		sprWeapon->setRotation(finishRotation[actDefLand].second);
		break;
	case actDefJump:
		sprChar->setSpriteFrame(finishFrame[actDefJump]);
		sprCircle->setRotation(finishRotation[actDefJump].first);
		sprWeapon->setRotation(finishRotation[actDefJump].second);
		break;
	case actMoveLeft:
		sprChar->setSpriteFrame(finishFrame[actMoveLeft]);
		sprCircle->setRotation(finishRotation[actMoveLeft].first);
		sprWeapon->setRotation(finishRotation[actMoveLeft].second);
		break;
	case actMoveRight:
		sprChar->setSpriteFrame(finishFrame[actMoveRight]);
		sprCircle->setRotation(finishRotation[actMoveRight].first);
		sprWeapon->setRotation(finishRotation[actMoveRight].second);
		break;
	default:
		break;
	}
}

//State의 Add/Delete 에 맞게, 모션(Animation/Frame) 교체(true - addState, false - delState)
void Character::changeCharMotion(bool isAdd, charStateType targetState)
{
	//1) Add State 의 경우
	if (isAdd)
	{
		switch (targetState)
		{
		case MOVE:
		{
			if (getOnStateInput(MOVE, LEFT) && !getOnStateInput(MOVE, RIGHT))
				playCharAction(actMoveLeft);
			else if (!getOnStateInput(MOVE, LEFT) && getOnStateInput(MOVE, RIGHT))
				playCharAction(actMoveRight);
		}
		break;
		case ATTACK:
		{
			if (isJump())
				playCharAction(actAtkJumpHit);
			else
				playCharAction(actAtkLandHit);
		}
		break;
		case DEFENSE:
		{
			if (isJump())
				playCharAction(actDefJump);
			else
				playCharAction(actDefLand);
		}
		break;
		case SPC_ATK:
			playCharAction(actSPCLand);
			break;
		case JUMP:
		{
			//공격 인풋 들어와 있을 때,
			if (getOnStateInput(ATTACK, TOUCH))
			{
				if (getOnStateTrigger(ATTACK, SWIPE))
					setCharFrame(actAtkJumpHit);
				else
					setCharFrame(actAtkJumpReady);
			}
			//방어 상태일 때,
			else if (isDefense())
				setCharFrame(actDefJump);
			else
				playCharAction(actJumpUp);
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
				playCharAction(actDefJumpEnd);
			else
				playCharAction(actDefLandEnd);

			initStateBit(DEFENSE);
			break;

		case SPC_ATK:

			if (!isJump() && getDirtyInputTrigger(MOVE, true))
			{
				if (getOnStateInput(MOVE, LEFT) && !getOnStateInput(MOVE, RIGHT))
					playCharAction(actMoveLeft);
				else if (!getOnStateInput(MOVE, LEFT) && getOnStateInput(MOVE, RIGHT))
					playCharAction(actMoveRight);
			}
			else
			{
				if (isJump()) {
					if (getOnStateTrigger(JUMP, DOWN))
						setCharFrame(actJumpDown);
					else
						setCharFrame(actJumpUp);
				}
				else
					setCharFrame(actNone);
			}
			break;

		case JUMP:

			if (isDefense())
				setCharFrame(actDefLand);
			else if (getOnStateTrigger(ATTACK, TOUCH))
			{
				if (getOnStateTrigger(ATTACK, SWIPE))
					setCharFrame(actAtkLandHit);
				else
					setCharFrame(actAtkLandReady);
			}
			else if (isMove())
			{
				if (getOnStateInput(MOVE, LEFT) && !getOnStateInput(MOVE, RIGHT))
					setCharFrame(actMoveLeft);
				else if (!getOnStateInput(MOVE, LEFT) && getOnStateInput(MOVE, RIGHT))
					setCharFrame(actMoveRight);
			}
			else if (isMotionPlaying) {
				playCharAction(actJumpCancel);
			}
			else {
				setCharFrame(actNone);
			}
			initStateBit(JUMP);
			break;
		case MOVE:
			if (isDefense())
			{
				setCharFrame(actDefLand);
			}
			else if (getOnStateTrigger(ATTACK, TOUCH))
			{
				if (getOnStateTrigger(ATTACK, SWIPE))
					setCharFrame(actAtkLandHit);
				else
					setCharFrame(actAtkLandReady);
			}
			else if (getOnStateTrigger(MOVE, LEFT) && !getOnStateTrigger(MOVE, RIGHT))
			{
				playCharAction(actMoveLeftEnd);
			}
			else if (!getOnStateTrigger(MOVE, LEFT) && getOnStateTrigger(MOVE, RIGHT))
			{
				playCharAction(actMoveRightEnd);
			}

			initStateBit(MOVE);
			break;
		default:
			break;
		}
	}
}


void Character::addState(charStateType aState)
{
	//새로운 state 삽입 시도에만, 삽입
	if (this->state % aState != 0)
	{
		//CCLOG("new state come in!");
		int prev_state = this->state;	//삽입 전, state 백업
		this->state *= aState;	//실제로 새로운 state 삽입 완료

		//add되는 State에 대해, 다른 처리가 필요한 경우에 처리
		switch (aState)
		{
		case ATTACK:
		{
			updateAttackID();



			////Attak Charge 중이었으면, Charge 해제와 동시에 Release 시작
			//if (Director::getInstance()->getScheduler()->isScheduled(schedule_selector(Character::callback_tick_AtkCharge), this))
			//{
			//	Director::getInstance()->getScheduler()->unschedule(schedule_selector(Character::callback_tick_AtkCharge), this);
			//	Director::getInstance()->getScheduler()->schedule(schedule_selector(Character::callback_tick_AtkRelease), this, 1 / 30.f, false);
			//}



			//방어 input이 있다면, 초기화
			if (getDirtyInputTrigger(DEFENSE, true)) {
				delState(DEFENSE);
				initStateBit(DEFENSE);
			}
		}
		break;
		case DEFENSE:
		{
			//공격 관련 state는 모두 초기화
			if (getDirtyInputTrigger(ATTACK, true)) {
				delState(ATTACK);
				initStateBit(ATTACK);
			}
		}
		break;
		case JUMP:
		{
			curVeloY = jumpVelo;
		}
		break;
		case SPC_ATK:
		{
			isSpcIntroPlaying = true;
		}
		break;
		case CRASH:
		{
			//crash 지속시간 0.15초 
			//auto crash_finish = Sequence::create(DelayTime::create(0.6f),
			//	CallFuncN::create(CC_CALLBACK_1(Character::callback_DelCrash, this)), nullptr);
			//sprChar->runAction(crash_finish);

			//부딪힌 건물 속도로부터 자신의 충돌 후 속도 계산
			if (curVeloY*0.008f >= 0)
				setVeloY(-curVeloY*0.008f + outerVeloY);
			else {
				setVeloY(outerVeloY);
			}
			return;
		}
		break;
		case PRESSED:
		{
			auto press_finish = Sequence::create(DelayTime::create(1.2f),
				CallFuncN::create(CC_CALLBACK_1(Character::callback_DelPress, this)), nullptr);
			CCLOG("press start!");
			attackRadar->runAction(press_finish);

			auto move_left = MoveBy::create(0.05f, Vec2(winSize().width / 25, 0));
			auto move_right = move_left->reverse();
			auto sequence = Sequence::create(move_left, move_right, nullptr);
			shakeCanvas = Repeat::create(sequence, 14);
			canvas->runAction(shakeCanvas);
			BGManager::getInstance()->moveBG(5);
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

void Character::delState(charStateType dState)
{
	//존재하는 state의 삭제 명령에만 삭제
	if (this->state % dState == 0)
	{
		int prev_state = this->state; //삭제 전, state 백업
		this->state /= dState;

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

void Character::positionUpdate(float deltaTime) {

	if (!getSpcIntroPlaying())
	{
		//위치 갱신 예정 포인트(이 시점의 sprPos == colliderPos)
		Point updated_pos = getSprPosition();
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

		//이전 frame pos 갱신
		prevPos = getSprPosition();
		//우선 collider pos만 임시 갱신
		setColliderPosition(updated_pos);
	}

}


void Character::stateUpdate(float deltaTime)
{
	//(collider의 위치)에 본체(sprChar)를 calibration 
	setSprPosition(getColliderPosition());

	//지면에서 충돌시, Press 상태
	if (isCrash() && isAtLand())
		addState(PRESSED);

	//Input Crash가 사라지면, Delete Crash 타이머 발동
	if (!getOnStateInput(CRASH, 0) && isCrash()) {
		//crash 지속시간 0.15초 
		auto crash_finish = Sequence::create(DelayTime::create(0.15f),
			CallFuncN::create(CC_CALLBACK_1(Character::callback_DelCrash, this)), nullptr);
		attackRadar->runAction(crash_finish);
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

	stateTransitionUpdate();


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

void Character::stateTransitionUpdate()
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

	//#2 ATTCK transition 발생
	if (!getEqualInputTrigger(ATTACK))
	{
		if (getOnStateInput(ATTACK, TOUCH))
		{
			if (!isMotionPlaying)
			{
				//Attack Ready (00 | 01)
				if (!getOnStateTrigger(ATTACK, TOUCH))
				{
					setOnStateTrigger(ATTACK, TOUCH);
					if (isJump())
						playCharAction(actAtkJumpReady);
					else
						playCharAction(actAtkLandReady);
					/*
					if (!Director::getInstance()->getScheduler()->isScheduled(schedule_selector(Character::callback_tick_AtkCharge), this)) {
							Director::getInstance()->getScheduler()->schedule(schedule_selector(Character::callback_tick_AtkCharge), this, 1 / 30.f, false);
						}
						*/
				}
				//Attack Hit (01 | 11)
				else if (!getOnStateTrigger(ATTACK, SWIPE) && (getOnStateInput(ATTACK, SWIPE)))
				{
					setOnStateTrigger(ATTACK, SWIPE);
					addState(ATTACK);

					EffectManager::getInstance()->runBladeWind(sprChar->getPosition() + Vec2(0, getWpHeight()*0.15f), Vec2(-130, 160), 1.3f, false, *canvas);
				}

				//Attack Back (11 | 01)
				else if (getOnStateTrigger(ATTACK, SWIPE) && !getOnStateInput(ATTACK, SWIPE))
				{
					setOffStateTrigger(ATTACK, SWIPE);
					if (isJump())
						playCharAction(actAtkJumpBack);
					else
						playCharAction(actAtkLandBack);
					EffectManager::getInstance()->runBladeWind(sprChar->getPosition() + Vec2(0, getWpHeight()*0.15f), Vec2(-10, 80), 1.3f, true, *canvas);
				}
			}
		}
		else if (!isMotionPlaying)
		{
			if (isJump())
				playCharAction(actAtkJumpCancel);
			else if (isAtLand())
				playCharAction(actAtkLandCancel);

			initStateBit(ATTACK);
		}
	}

	//#3 JUMP transition 필요
	if (!getEqualInputTrigger(JUMP))
	{
		//점프 touch input 인식
		if (getOnStateInput(JUMP, TOUCH))
		{
			if (!isMotionPlaying && !isPressed())
			{
				//-> Jump ready (000 | 001)
				if (!getOnStateTrigger(JUMP, TOUCH))
				{
					setOnStateTrigger(JUMP, TOUCH);
					playCharAction(actJumpReady);
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
						playCharAction(actJumpDown);
					}
				}
			}
			//(공중 -> 지상) 복귀
			if (getOnStateTrigger(JUMP, UP) && !getOnStateInput(JUMP, UP) && (getVeloY() < 0))
			{
				setPositionY(landHeight);
				delState(JUMP);
				setVeloY(0);
			}
		}
		else if (!isMotionPlaying && getOnStateTrigger(JUMP, TOUCH)) {
			initStateBit(JUMP);
			playCharAction(actJumpCancel);
		}
	}


	//#4 MOVE transition
	if (!getEqualInputTrigger(MOVE))
	{
		if (!isMotionPlaying)
		{
			//( dirty / dirty ) 좌-우 모션만 교체
			if (getDirtyInputTrigger(MOVE, true) && getDirtyInputTrigger(MOVE, false))
			{
				if (getOnStateInput(MOVE, LEFT) && !getOnStateInput(MOVE, RIGHT)) {
					setOnStateTrigger(MOVE, LEFT);
					setOffStateTrigger(MOVE, RIGHT);
					playCharAction(actMoveLeft);
				}
				else if (!getOnStateInput(MOVE, LEFT) && getOnStateInput(MOVE, RIGHT)) {
					setOnStateTrigger(MOVE, RIGHT);
					setOffStateTrigger(MOVE, LEFT);
					playCharAction(actMoveRight);
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


//무기 Sprite와 그에따른 레이더의 크기 함께 Scale 
void Character::setWpScale(float input_wp_scale) {

	curWpScale = input_wp_scale;
	//CCLOG("wp scale set : %.2f", curWpScale);
	sprWeapon->setScale(curWpScale * DivForHorizontal(sprWeapon) / sprChar->getScale());

	//(wp_scale + body_scale)의 기준 스케일을 구해, 그 비율로 레이더 크기 증가
	float radar_x_scale = bodyScale / 2 + getWpLengthScale() * 0.8f;
	float radar_y_scale = getWpLengthScale();

	attackRadar->setScaleX(radar_x_scale / sprChar->getScale() * 1.75f);
	attackRadar->setScaleY(radar_y_scale / sprChar->getScale() / winAspectRatio());
}


//공격(검 휘두름)에 시간에 따른 데미지 범위 변화 스케쥴 콜백
void Character::callback_tick_AtkScope(float deltaTime)
{
	lapsedAtkTick += deltaTime / (FRAME_DELAY*1.5f * 7);

	//tick time이 모두 지나면 스케쥴 해제
	if (lapsedAtkTick > 1) {
		lapsedAtkTick = 1.0f;
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(Character::callback_tick_AtkScope), this);
	}
};


void Character::callback_FinishSpcIntro(Ref* sender) {

	//스페셜공격 인트로 모션 끝내고, 여기서 메인스케쥴러 재작동
	isSpcIntroPlaying = false;

	//초당 간격으로 19번 스페셜 데미지 계산 스케쥴러 작동
	Director::getInstance()->getScheduler()
		->schedule(std::bind(&Character::updateSpecialID, this, std::placeholders::_1),
			this, 0.15f, 19, 0.15f, false, "updateSpecialID");
}
void Character::callback_DelSpc(Ref* sender) {

	//CCLOG("Delete Special State!!!!");
	this->delState(SPC_ATK);
}
void Character::callback_DelCrash(Ref* sender) {

	//CCLOG("Delete Crash State!!!!");
	delState(CRASH);
}
void Character::callback_DelPress(Ref* sender) {

	//CCLOG("Delete Press State!!!!");
	this->delState(PRESSED);

	if (isCrash() && isAtLand())
		this->addState(PRESSED);
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


void Character::callback_tick_AtkCharge(float deltaTime)
{
	if (curWpScale < maxWpScale)
	{
		auto updated_scale = curWpScale + 0.05f * deltaTime / 1.0f;
		setWpScale(updated_scale);
	}
}
void Character::callback_tick_AtkRelease(float deltaTime)
{
	if (curWpScale > defWpScale)
	{
		auto updated_scale = curWpScale - 0.2f * deltaTime / 1.0f;
		setWpScale(updated_scale);
	}
	else {
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(Character::callback_tick_AtkRelease), this);
	}
}


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
