#include "OnGame/JoyController.h"
#include "Manager/GameManager.h"
#include "Manager/EffectManager.h"

JoyController::~JoyController()
{}

//(터치)입력이 move에 유효한 입력인지
bool JoyController::chkMoveBound(Point touch) {
	return touch.y <= winSize().height*0.14f;
}


bool JoyController::init()
{
	if (!Layer::init())
		return false;

	velocity = 0.6f;
	isControllerShow = false;
	isLimitScreen = false;

	//move 컨트롤러와 jump버튼을 배치할 Center지점 지정
	centerPos = Point(winSize().width / 2, winSize().height*0.1f);
	//Move 컨트롤러 배치
	sprMoveLeft = Sprite::create("ui/slide_limit_left.png");
	sprMoveLeft->setAnchorPoint(Point(1, 0.5f));
	//sprBackLeft->setPosition(centerPos - Vec2(winSize().width*0.03f, 0));
	sprMoveLeft->setPosition(centerPos);
	sprMoveLeft->setScale(3.1f, 1.4f);
	this->addChild(sprMoveLeft);
	sprMoveRight = Sprite::create("ui/slide_limit_right.png");
	sprMoveRight->setAnchorPoint(Point(0, 0.5f));
	//sprBackRight->setPosition(centerPos + Vec2(winSize().width*0.03f, 0));
	sprMoveRight->setPosition(centerPos);
	sprMoveRight->setScale(3.1f, 1.4f);
	this->addChild(sprMoveRight);

	//Button 컨트롤러 배치
	sprBtnJump = Sprite::create("ui/jump.png");
	sprBtnJump->setScale(1.35f);
	sprBtnJump->setPosition(centerPos);
	this->addChild(sprBtnJump);

	//공격버튼. 임시로 이미지만 슬라이드
	sprBtnAttack = Sprite::create("ui/slide.png");
	sprBtnAttack->setScale(1.1f);
	sprBtnAttack->setPosition(Point(winSize().width / 15 * 2, winSize().height*0.22f));
	this->addChild(sprBtnAttack);

	sprBtnDefense = Sprite::create("ui/defense.png"); //방어 버튼(위치는 임의로 왼쪽 아래로 잡음)
	sprBtnDefense->setScale(1.1f);
	sprBtnDefense->setPosition(Point(winSize().width / 15 * 13, winSize().height*0.22f));
	this->addChild(sprBtnDefense);

	sprBtnSpc = Sprite::create("ui/heart.png");
	sprBtnSpc->setScale(1.5f);
	sprBtnSpc->setPosition(Point(winSize().width / 2, winSize().height*0.9f));
	this->addChild(sprBtnSpc);

	initAttackPos = sprBtnAttack->getPosition();

	//시험용 그리드 삽입
	//this->addChild(GridLayer::getInstance()->getLayer());
	//jumpBtnActivated = true;
	//defBtnActivated = true;


	listener_Move = EventListenerTouchOneByOne::create();
	//listener_Move->setSwallowTouches(true);
	listener_Move->onTouchBegan = CC_CALLBACK_2(JoyController::onTouchBegan_Move, this);
	listener_Move->onTouchMoved = CC_CALLBACK_2(JoyController::onTouchMoved_Move, this);
	listener_Move->onTouchEnded = CC_CALLBACK_2(JoyController::onTouchEnded_Move, this);


	listener_Jump = EventListenerTouchOneByOne::create();
	//listener_Jump->setSwallowTouches(true);
	listener_Jump->onTouchBegan = CC_CALLBACK_2(JoyController::onTouchBegan_Jump, this);
	listener_Jump->onTouchMoved = CC_CALLBACK_2(JoyController::onTouchMoved_Jump, this);
	listener_Jump->onTouchEnded = CC_CALLBACK_2(JoyController::onTouchEnded_Jump, this);
	//listener_Jump->onTouchCancelled = CC_CALLBACK_2(JoyController::onTouchCancelled_Jump, this);


	//listener_JoyButton = EventListenerTouchOneByOne::create();
	//listener_JoyButton->setSwallowTouches(true);
	//listener_JoyButton->onTouchBegan = CC_CALLBACK_2(JoyController::onTouchBegan_JoyButton, this);
	////listener_JoyButton->onTouchMoved = CC_CALLBACK_2(JoyController::onTouchMoved_JoyButton, this);
	//listener_JoyButton->onTouchEnded = CC_CALLBACK_2(JoyController::onTouchEnded_JoyButton, this);

	listener_Attack = EventListenerTouchOneByOne::create();
	//listener_Attack->setSwallowTouches(true);
	listener_Attack->onTouchBegan = CC_CALLBACK_2(JoyController::onTouchBegan_Attack, this);
	listener_Attack->onTouchMoved = CC_CALLBACK_2(JoyController::onTouchMoved_Attack, this);
	listener_Attack->onTouchEnded = CC_CALLBACK_2(JoyController::onTouchEnded_Attack, this);
	//listener_Attack->onTouchCancelled = CC_CALLBACK_2(JoyController::onTouchCancelled_Attack, this);

	listener_Defense = EventListenerTouchOneByOne::create();
	//listener_Defense->setSwallowTouches(true);
	listener_Defense->onTouchBegan = CC_CALLBACK_2(JoyController::onTouchBegan_Defense, this);
	listener_Defense->onTouchMoved = CC_CALLBACK_2(JoyController::onTouchMoved_Defense, this);
	listener_Defense->onTouchEnded = CC_CALLBACK_2(JoyController::onTouchEnded_Defense, this);
	//listener_Defense->onTouchCancelled = CC_CALLBACK_2(JoyController::onTouchCancelled_Defense, this);

	return true;
}
void JoyController::onEnter()
{
	Layer::onEnter();
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener_Move, 2);
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener_Jump, 3);
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener_Defense, 4);
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener_Attack, 5);
	listener_Move->setSwallowTouches(true);
	listener_Jump->setSwallowTouches(true);
	listener_Defense->setSwallowTouches(true);
	listener_Attack->setSwallowTouches(true);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener_Move, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener_Attack, sprBtnAttack);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener_Jump, sprBtnJump);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener_Defense, sprBtnDefense);

}


void JoyController::update(float deltaTime) {

	//Slide 입력에 의한, Jump
	if (isTouchedJump)
	{
		//잔상 이펙트
		auto spr_finger = Sprite::create("touch.png");
		spr_finger->setPosition(curJumpPos);
		this->addChild(spr_finger, 5);

		auto fade_out = FadeOut::create(0.6f);
		auto remove_self = CallFunc::create([spr_finger]() {
			spr_finger->removeFromParentAndCleanup(true); });
		spr_finger->runAction(Sequence::create(fade_out, remove_self, nullptr));


		//12시 방향. 실제 점프 시작
		if (curJumpPos.y - initJumpPos.y > winSize().width*0.05)
		{
			auto move_up = MoveBy::create(0.08f, Point(0, 100));
			auto sequence = Sequence::create(move_up, move_up->reverse(), nullptr);
			sprBtnJump->runAction(sequence);
			inActivateBtnJump();

			//jump up 비트를 on
			mainChar->setOnStateInput(JUMP, UP);

			isTouchedJump = false;
			if (isZoomIn) {
				auto zoom_out = ScaleTo::create(0.15f, 1.0f);
				canvas->runAction(zoom_out);
				isZoomIn = false;
			}
		}
	}


	//Swipe 입력에 의한, Attack
	if (isTouchedAttack)
	{	
		//Attack Ready 상태
		if (!mainChar->getOnStateInput(ATTACK, SWIPE) && !mainChar->getOnStateTrigger(ATTACK, SWIPE))
		{
			float between_angle = curAtkRot - marginAtkRot.first;
			if (90.0f < between_angle && between_angle < 180.0f){
				marginAtkRot.first = 0.0f;
				EffectManager::getInstance()->stopAttackCharge();
				mainChar->setOnStateInput(ATTACK, SWIPE);
			}
		}
		//Attack Back 상태
		else if (mainChar->getOnStateInput(ATTACK, SWIPE) && mainChar->getOnStateTrigger(ATTACK, SWIPE))
		{
			float between_angle = curAtkRot - marginAtkRot.second;
			if (-180.0f < between_angle && between_angle < -90.0f){
				marginAtkRot.second = 0.0f;
				EffectManager::getInstance()->runAttackCharge(4.0f, *sprBtnAttack);
				mainChar->setOffStateInput(ATTACK, SWIPE);
			}
		}
	}



	//if (!mainChar->isAttack()
	//	&& !isTouchedAttack)
	//{
	//	activateBtnAttack();
	//}
	//else
		//inActivateBtnAttack();


	if (!mainChar->getDirtyInputTrigger(JUMP, true)
		&& !isTouchedJump)
	{
		activateBtnJump();
	}


	if (!mainChar->isDefense()
		&& !isTouchedDefense)
	{
		activateBtnDefense();
	}
	else
		inActivateBtnDefense();


	//if (!isTouchedLeft && !isTouchedRight
	//	&& !mainChar->getInputLeft() && !mainChar->getInputRight())
	//{
	//	isActiveBtnMove = true;
	//}
	//else
	//	isActiveBtnMove = false;

}


bool JoyController::onTouchBegan_Move(Touch* touch, Event* unused_event)
{
	if (isActiveBtnMove)
	{
		//Right Move 입력,
		if (chkMoveBound(touch->getLocation()) && touch->getLocation().x > winSize().width / 5 * 3)
		{
			if (isTouchedLeft) {
				mainChar->setOffStateInput(MOVE, LEFT);
				sprMoveLeft->setColor(Color3B::WHITE);
				isTouchedLeft = false;
			}
			mainChar->setOnStateInput(MOVE, RIGHT);
			sprMoveRight->setColor(Color3B::RED);
			isTouchedRight = true;
			return true;
		}

		//Left Move 입력,
		if (chkMoveBound(touch->getLocation()) && touch->getLocation().x < winSize().width / 5 * 2)
		{
			if (isTouchedRight) {
				//left 인풋 비트 꺼줌
				mainChar->setOffStateInput(MOVE, RIGHT);
				sprMoveRight->setColor(Color3B::WHITE);
				isTouchedRight = false;
			}
			mainChar->setOnStateInput(MOVE, LEFT);
			sprMoveLeft->setColor(Color3B::RED);
			isTouchedLeft = true;
			return true;
		}
	}
};
void JoyController::onTouchMoved_Move(Touch* touch, Event* unused_event)
{
	//Right Move 입력,
	if (isActiveBtnMove && chkMoveBound(touch->getLocation())
		&& touch->getLocation().x > winSize().width / 5 * 3)
	{
		if (isTouchedLeft) {
			mainChar->setOffStateInput(MOVE, LEFT);
			sprMoveLeft->setColor(Color3B::WHITE);
			isTouchedLeft = false;
		}
		mainChar->setOnStateInput(MOVE, RIGHT);
		sprMoveRight->setColor(Color3B::RED);
		isTouchedRight = true;
		return;
	}
	else if (isActiveBtnMove && chkMoveBound(touch->getLocation())
		&& touch->getLocation().x < winSize().width / 5 * 2)
	{
		if (isTouchedRight) {
			mainChar->setOffStateInput(MOVE, RIGHT);
			sprMoveRight->setColor(Color3B::WHITE);
			isTouchedRight = false;
		}
		mainChar->setOnStateInput(MOVE, LEFT);
		sprMoveLeft->setColor(Color3B::RED);
		isTouchedLeft = true;
		return;
	}
	else
	{
		if (isTouchedLeft) {
			mainChar->setOffStateInput(MOVE, LEFT);
			sprMoveLeft->setColor(Color3B::WHITE);
			isTouchedLeft = false;
		}
		if (isTouchedRight) {
			mainChar->setOffStateInput(MOVE, RIGHT);
			sprMoveRight->setColor(Color3B::WHITE);
			isTouchedRight = false;
		}
	}
};
void JoyController::onTouchEnded_Move(Touch* touch, Event* unused_event)
{
	if (isTouchedLeft) {
		mainChar->setOffStateInput(MOVE, LEFT);
		sprMoveLeft->setColor(Color3B::WHITE);
		isTouchedLeft = false;
	}
	if (isTouchedRight) {
		mainChar->setOffStateInput(MOVE, RIGHT);
		sprMoveRight->setColor(Color3B::WHITE);
		isTouchedRight = false;
	}
};


bool JoyController::onTouchBegan_Jump(Touch* touch, Event* unused_event) {

	if (isActiveBtnJump && isListenerTargetTouched(touch, unused_event))
	{
		//CCLOG("Touch Rect W: %.2f, H: %.2f", target_size.width, target_size.height);
		auto zoom_in = ScaleTo::create(0.15f, 1.13f);
		canvas->runAction(zoom_in);
		auto tint_in = TintTo::create(0.15f, Color3B(255, 160, 122));
		sprBtnJump->runAction(tint_in);

		//터치 위치 시작
		initJumpPos = touch->getLocation();
		curJumpPos = touch->getLocation();

		//터치 비트 on. 준비자세 돌입
		mainChar->setOnStateInput(JUMP, TOUCH);

		//점프-줌인 액션이함께 움직이지만, 구별의 여지를 위해 플래그를 분리함
		isZoomIn = true;
		isTouchedJump = true;
		return true;
	}
	return false;
};
void JoyController::onTouchMoved_Jump(Touch* touch, Event* unused_event)
{
	if (isTouchedJump)
		curJumpPos = touch->getLocation();
};
void JoyController::onTouchEnded_Jump(Touch* touch, Event* unused_event) {


	if (isZoomIn) {
		auto zoom_out = ScaleTo::create(0.15f, 1.0f);
		canvas->runAction(zoom_out);
		isZoomIn = false;
	}

	if (isTouchedJump)
	{
		auto tint_out = TintTo::create(0.15f, Color3B(140, 140, 140));
		sprBtnJump->runAction(tint_out);
		//sprBtnJump->setPosition(initJumpPos);

		mainChar->setOffStateInput(JUMP, TOUCH);
		//터치가 들어왔나만 확인하는 변수. 터치 끝나므로 원상복귀
		isTouchedJump = false;
	}


};
void JoyController::onTouchCancelled_Jump(Touch* touch, Event* unused_event) {

	onTouchEnded_Jump(touch, unused_event);
};



bool JoyController::onTouchBegan_Attack(Touch* touch, Event* unused_event)
{
	if (isActiveBtnAttack && isListenerTargetTouched(touch, unused_event))
	{
		isTouchedAttack = true;
		//CCLOG("Attack Btn Touched X: %f, Y: %f", initAttackPos.x, initAttackPos.y);
		
		auto touch_pos = touch->getLocation();	
		curAtkRot = marginAtkRot.first = CC_RADIANS_TO_DEGREES(ccpToAngle(touch_pos - initAttackPos));
		
		auto tint_in = TintTo::create(0.15f, Color3B(245, 222, 179));
		sprBtnAttack->runAction(tint_in);
		sprBtnAttack->setOpacity(100);
			
		draw_tmp = DrawNode::create();
		draw_tmp->drawLine(initAttackPos, touch->getLocation(), Color4F::GREEN);
		addChild(draw_tmp);


		EffectManager::getInstance()->runAttackCharge(4.0f, *sprBtnAttack);
		mainChar->setOnStateInput(ATTACK, TOUCH);	//터치 비트 on
		return true;
	}
	return false;
}
void JoyController::onTouchMoved_Attack(Touch* touch, Event* unused_event)
{
	if (isTouchedAttack)
	{
		auto touch_pos = touch->getLocation();
		sprBtnAttack->setPosition(touch_pos);
		curAtkRot = CC_RADIANS_TO_DEGREES(ccpToAngle(touch_pos - initAttackPos));
	
		if (mainChar->getOnStateTrigger(ATTACK, SWIPE))
			marginAtkRot.second = max(marginAtkRot.second, curAtkRot);
		else
			marginAtkRot.first = min(marginAtkRot.first, curAtkRot);


		draw_tmp->clear();
		draw_tmp->drawLine(initAttackPos, touch_pos, Color4F::GREEN);
	}
}
void JoyController::onTouchEnded_Attack(Touch* touch, Event* unused_event)
{
	if (isTouchedAttack)
	{
		auto tint_out = TintTo::create(0.15f, Color3B(255, 255, 255));
		sprBtnAttack->runAction(tint_out);
		sprBtnAttack->setOpacity(255);

		sprBtnAttack->setPosition(initAttackPos);
		mainChar->setOffStateInput(ATTACK, TOUCH);	//터치 비트 off
		curAtkRot = marginAtkRot.first = marginAtkRot.second = 0.0f;
		
		EffectManager::getInstance()->stopAttackCharge();
		isTouchedAttack = false;

		draw_tmp->removeFromParentAndCleanup(true);
		return;
	}
}
void JoyController::onTouchCancelled_Attack(Touch* touch, Event* unused_event)
{
	onTouchEnded_Attack(touch, unused_event);
}



bool JoyController::onTouchBegan_Defense(Touch* touch, Event* unused_event) {

	//방어 버튼 터치 시,
	if (isActiveBtnDefense && isListenerTargetTouched(touch, unused_event))
	{
		//defense의 터치 인풋
		mainChar->setOnStateInput(DEFENSE, TOUCH);
		isTouchedDefense = true;
		mainChar->updateDefenseID();	//방어 아이디 갱신.방어 ID는 무조건 터치에 의해서만 갱신

		CCLOG("Defense updated!! : %d", mainChar->getDefenseID());
		return true;
	}
	return false;
};
void JoyController::onTouchMoved_Defense(Touch* touch, Event* unused_event) {
	//버튼 범위 넘어가면
	if (!sprBtnDefense->getBoundingBox().containsPoint(touch->getLocation())
		&& isTouchedDefense)
	{
		//현재 디펜스 상태가 유지되고 있다면(건물 방어 안한 채로)
		if (mainChar->isDefense() && !mainChar->isCrash()) {
			mainChar->setOffStateInput(DEFENSE, TOUCH);
		}
		isTouchedDefense = false;
		return;
	}
};
void JoyController::onTouchEnded_Defense(Touch* touch, Event* unused_event) {
	if (isTouchedDefense)
	{
		//현재 디펜스 상태가 유지되고 있다면(건물 방어 안한 채로)
		if (mainChar->isDefense() && !mainChar->isCrash()) {
			mainChar->setOffStateInput(DEFENSE, TOUCH);
		}
		isTouchedDefense = false;
		return;
	}
};
void JoyController::onTouchCancelled_Defense(Touch* touch, Event* unused_event) {
	onTouchEnded_Defense(touch, unused_event);
};


//리스너의 타겟이 터치되었는지 여부 반환
bool JoyController::isListenerTargetTouched(Touch* touch, Event* unused_event)
{
	Sprite* target = static_cast<Sprite*>(unused_event->getCurrentTarget());
	Size target_size = target->getContentSize();
	Vec2 touch_pos = target->convertToNodeSpace(touch->getLocation());

	if (Rect(0, 0, target_size.width, target_size.height).containsPoint(touch_pos))
		return true;
	else
		return false;
}





void JoyController::activateBtnJump()
{
	sprBtnJump->setColor(Color3B::WHITE);
	isActiveBtnJump = true;
};
void JoyController::activateBtnAttack()
{
	sprBtnAttack->setOpacity(255);
	isActiveBtnAttack = true;
};
void JoyController::activateBtnDefense()
{
	sprBtnDefense->setOpacity(255);
	isActiveBtnDefense = true;
};
void JoyController::activateBtnSpc()
{
	sprBtnSpc->setOpacity(255);
	isActiveBtnSpc = true;
}


void JoyController::inActivateBtnJump() {

	sprBtnJump->setColor(Color3B(100, 100, 100));
	isActiveBtnJump = false;
};
void JoyController::inActivateBtnAttack() {
	sprBtnAttack->setOpacity(100);
	isActiveBtnAttack = false;
};
void JoyController::inActivateBtnDefense() {
	sprBtnDefense->setOpacity(50);
	isActiveBtnDefense = false;
};
void JoyController::inActivateBtnSpc() {
	sprBtnSpc->setOpacity(50);
	isActiveBtnSpc = false;
}

