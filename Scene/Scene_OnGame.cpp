#define PI 3.14159265358979323846

#include "SimpleAudioEngine.h"
#include <string.h>
#include <array>
#include "base/CCEventDispatcher.h"
#include "Manager/GameManager.h"
#include "Manager/BGManager.h"
#include "Manager/PopUps.h"
#include "Manager/EffectManager.h"
#include "OnGame/Character.h"
#include "Obstacle/ObsManager.h"
#include "Scene_OnGame.h"

USING_NS_CC;
using namespace CocosDenshion;
using namespace std;

/** 남은 스페셜공격량을 저장하기 위한 변수 */
float timer;
int NowSpecialDamage;
bool isSpecial = true;
bool isShakeFinish = true;
//bool isSpecialStart = false;
bool isJumpTouched = false;
bool isCinematic = false;
//bool packFlag = false;	//빌딩의 묶음 번호 부여를 위한 플래그



Scene* OnGame::createScene()
{
	//프리로딩
	//CCSpriteFrameCache *frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	//frameCache->addSpriteFramesWithFile("character/basic/basic.plist");
	//SimpleAudioEngine::sharedEngine()->preloadEffect("special_sound.wav");
	//SimpleAudioEngine::sharedEngine()->preloadEffect("hitting_building.wav");
	//SimpleAudioEngine::sharedEngine()->preloadEffect("swinging_sword.wav");
	//SimpleAudioEngine::sharedEngine()->preloadEffect("floor_clear.wav");
	auto scene = Scene::create();
	auto layer = OnGame::create();

	scene->addChild(layer);
	return scene;
}


void OnGame::initJoyController()
{
	joyController = JoyController::create();
	joyController->setMainChar(mainChar);
	joyController->setCanvas(movingLayer);
	joyController->setCameraTarget(cameraTarget);
	joyController->setLimitScreen(true);

	fixedLayer->addChild(joyController);	


	listener_OnGame = EventListenerTouchOneByOne::create();
	listener_OnGame->onTouchBegan = CC_CALLBACK_2(OnGame::onTouchBegan_OnGame, this);
	listener_OnGame->onTouchEnded = CC_CALLBACK_2(OnGame::onTouchEnded_OnGame, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener_OnGame, 1);
	this->setKeypadEnabled(true); //device의 home, back button enable 해줌.

}



//**************************OnGame씬 시작**********************************//
bool OnGame::init()
{
	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 0))) {
		return false;
	}

	for (int i = 0; i < 10; i++) {
		diagfragment[i] = NULL;
	}

	ruinCount = 0;
	deadCount = 0;
	trialCount = 0;
	gameTime = 0;
	newscale = 1;
	on_score = 0;

	increase_count = 0;
	is_special_effect = false;
	blocked = false;
	land = winSize().height*0.25f;

	_eventDispatcher->removeAllEventListeners();//현재 세팅한 모든 이벤트 리스너를 해제해줌
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	movingLayer = Layer::create();
	fixedLayer = Layer::create();	

	initBG();
	initIcon();
	is_boss = false;

	initCharacter();
	initJoyController();
	initGauge();


	initCamera();
	building_num = 0;
	this->addChild(movingLayer);
	this->addChild(fixedLayer);


	obsBatcher = new ObsTexBatcher();
	movingLayer->addChild(obsBatcher, 5);
	obsBatcher->initTex(APARTMENT);
	obsBatcher->initTex(COMPANY);

	particleMgr = new ParticleManager();
	this->addChild(particleMgr);
	particleMgr->initPool(RUIN_COMPANY, "Particle_Ruin", 3, 40);

	EffectManager::getInstance()->initEffectInfo(BLADE_SLASH, "Effect_0726", 1);
	EffectManager::getInstance()->initEffectInfo(BLADE_WIND_1ST, "Effect_0726", 16);
	EffectManager::getInstance()->initEffectInfo(ATTACK_CHARGE, "AttackCharge", 30);

	//SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/stage_1.wav", true); //stage1 노래 스타트
	//ParticleSystem *ParticleSpecial = ParticleSystemQuad::create("particles/particle_special.plist");
	//ParticleSpecial->setDuration(-1);
	//ParticleSpecial->setVisible(false);
	//movingLayer->addChild(ParticleSpecial, 10, "ParticleSpecial");
	//BGManager::getInstance()->moveBG(2500);
	//EffectManager::getInstance()->runSpecialEffect(mainChar, movingLayer);
	//Director::getInstance()->schedule([&]() {}, 1.0f);
	tracer = LineTracer::create();
	movingLayer->addChild(tracer, 11);


	obsMgr = new ObsManager(3, activeBuildList);
	auto seed = new DefaultBuilding(obsBatcher->getTexBatcher(APARTMENT), *mainChar, *(particleMgr->Pools[RUIN_COMPANY]));
	seed->setMold(0.5f, 20, 12);
	seed->setSpec(8, 6.0f, -13.0f, 210);
	obsMgr->setProtoType(0, *seed);

	seed = new DefaultBuilding(obsBatcher->getTexBatcher(COMPANY), *mainChar, *(particleMgr->Pools[RUIN_COMPANY]));
	seed->setMold(0.7f, 25, 16);
	seed->setSpec(8, 6.0f, -15.0f, 210);
	obsMgr->setProtoType(1, *seed);

	seed = new DefaultBuilding(obsBatcher->getTexBatcher(APARTMENT), *mainChar, *(particleMgr->Pools[RUIN_COMPANY]));
	seed->setMold(0.3f, 14, 8);
	seed->setSpec(9, 6.0f, -12.0f, 210);
	obsMgr->setProtoType(2, *seed);

	obsMgr->generateMutant(obsMgr->getProtoType(0), 1);
	obsMgr->generateMutant(obsMgr->getProtoType(0), 1);
	obsMgr->generateMutant(obsMgr->getProtoType(0), 1);
	obsMgr->generateMutant(obsMgr->getProtoType(1), 0);
	obsMgr->generateMutant(obsMgr->getProtoType(1), 0);
	obsMgr->generateMutant(obsMgr->getProtoType(1), 0);
	obsMgr->generateMutant(obsMgr->getProtoType(2), 1);
	obsMgr->generateMutant(obsMgr->getProtoType(2), 0);
	obsMgr->generateMutant(obsMgr->getProtoType(2), 1);
	obsMgr->generateMutant(obsMgr->getProtoType(2), 1);
	this->addChild(obsMgr);		
	
	// MainScheduler 시작. 디폴트 프레임 시간 간격 호출
	this->schedule(schedule_selector(OnGame::MainScheduler));

	return true;
}



//단위시간에 대한, onGame씬 전체 스케쥴 관리
//매 frame 체크되어야 할 state는 모두 여기서 관리하도록
void OnGame::MainScheduler(float deltaTime) {


	//게임이 진행 중 일 때,
	if (!mainChar->isDead()
		&& !mainChar->getSpcIntroPlaying())
	{

		// 1 - 캐릭터 position 업데이트
		mainChar->positionUpdate(deltaTime);

		// 2 - Building Position/State 업데이트
		vector<BuildContainer*>::iterator iter = activeBuildList.begin();
		while (iter != activeBuildList.end())
		{
			(*iter)->update(deltaTime);

			if (!(*iter)->isAlive())
			{
				(*iter)->removeFromParent();
				(*iter)->release();
				iter = activeBuildList.erase(iter);
			}
			else
				iter++;
		}

		// 3 - 캐릭터 state 업데이트
		mainChar->stateUpdate(deltaTime);

		// 4 - 컨트롤러 state 업데이트
		joyController->update(deltaTime);

		setCameraPosition();

		//if (activeBuilding->getAttackDamage() > 0)
		//{
		//	//EffectManager::getInstance()->runAttackEffect(mainChar, movingLayer, mainChar->getAttackID());
		//	//EffectManager::getInstance()->runDamageEffect(fixedLayer, activeBuilding->getAttackDamage(), mainChar->getAttackID());
		//	auto move_left = MoveBy::create(0.1f, Vec2(5, 0));
		//	auto move_right = move_left->reverse();
		//	auto sequence = Sequence::create(move_left, move_right, nullptr);
		//	this->runAction(Repeat::create(sequence, 2));
		//	//무기 크기 갱신
		//	float tmp_scale = mainChar->getWeaponScale();	
		//	if (tmp_scale <= 6.5f)
		//	{
		//		tmp_scale += 0.015f;
		//		CCLOG("now scale is %f", tmp_scale);
		//		mainChar->setWeaponScale(tmp_scale);
		//	}
		//	
		//	activeBuilding->initAttackDamage();	//다음 frame 에서의 데미지 재측정을 위해, 초기화
		//}

		//누르기 플래그 갱신하면서 쌓임 발생
		//if (mainChar->isPressed() && !pressedFlag)
		//{	
		//	auto tremble_right = MoveBy::create(0.05f, Vec2(70, 0));
		//	auto tremble_left = tremble_right->reverse();
		//	auto tremble_seq = Repeat::create(Sequence::create(tremble_right, tremble_left, nullptr), 20);
		//	auto shakeEarth = Sequence::create(tremble_seq, CallFuncN::create(CC_CALLBACK_1(OnGame::shakeEarthFinish, this)), nullptr);
		//	
		//	BGManager::getInstance()->moveBG(600);
		//	movingLayer->runAction(shakeEarth);
		//	pressedFlag = true;
		//}


		//gaugeUpdate(deltaTime);	
		//DiagFragmentScheduler(deltaTime);


		////현재 등장 빌딩이 다 부서진 후, 다음 빌딩으로 스케쥴 진행
		//if (!activeBuilding->isAlive()) {
		//	CCLOG("new building appears!!");
		//	movingLayer->removeChild(activeBuilding);
		//	delete(activeBuilding);
		//	
		//	Building_Scheduller();
		//}


		//gettimeofday(&timeValue, NULL);
		//	//스페셜 어택 조작
		//	if (newscale >= 3 && !is_special_effect)
		//	{
		//		is_special_effect = true;
		//		particleSpecialEffect = ParticleSystemQuad::create("particles/special_effect_2.plist");
		//		particleSpecialEffect->setScale(2);
		//		particleSpecialEffect->setName("particleSpecialEffect");
		//		mainChar->sprWeapon->addChild(particleSpecialEffect, 100, "particleSpecialEffect");
		//	}
	}

}


void OnGame::initGauge() {
	DefenseTime = 16.f;
	NowDefenseGauge = 0.0;

	auto def_gauge_back = Sprite::create("gauge/defense_gauge_bar.png");
	def_gauge_back->setAnchorPoint(Point(0.5, 0));
	def_gauge_back->setPosition(Point(joyController->getBtnDefense()->getPositionX(), joyController->getBtnDefense()->getPositionY() + joyController->getBtnDefense()->getBoundingBox().getMaxY() / 2));

	def_gauge_back->setOpacity(160);
	fixedLayer->addChild(def_gauge_back);

	auto def_gauge = Sprite::create("gauge/defense_gauge.png");
	//GaugeDefense->setScale(1.4f, 1.1f);
	Point defense_gauge_pos = def_gauge_back->getPosition() + Vec2(0, def_gauge_back->getContentSize().height / 2);
	//GaugeDefense->setPosition(gauge_loc);
	GaugeDefense = CCProgressTimer::create(def_gauge);
	GaugeDefense->setScale(1.4f, 1.1f);
	GaugeDefense->setPosition(defense_gauge_pos);
	GaugeDefense->setType(kCCProgressTimerTypeBar);
	GaugeDefense->setMidpoint(ccp(0.5f, 0));	//중심점은 맨 끝
	GaugeDefense->setBarChangeRate(ccp(0, 1));	//게이지는 y축 방향으로 변화
	//GaugeDefense->setPercentage(100.0f);
	fixedLayer->addChild(GaugeDefense);

	MaxGaugeSize = def_gauge->getContentSize().height;
	CCLOG("MaxGauge Size : %f", MaxGaugeSize);
	//fixedLayer->addChild(GaugeDefense);
}


void OnGame::gaugeUpdate(float deltaTime) {
	//tmp += 0.1f;
	float cur_def_percentage = mainChar->curDefPoint / mainChar->maxDefPoint * 100.0f;
	GaugeDefense->setPercentage(cur_def_percentage);
}


void OnGame::initCamera() {

	cameraTarget = Sprite::create();
	cameraTarget->setPosition(winSize().width / 2, winSize().height / 2);
	cameraTarget->retain();

	movingLayer->addChild(cameraTarget);
	//BGManager::getInstance()->getParallax()->addChild(cameraTarget);
	followCenter = CustomFollow::create(cameraTarget, Rect::ZERO);

	movingLayer->runAction(followCenter);
}

void OnGame::setCameraPosition() {

	//y축 follow
	if (mainChar->getSprPositionY() >= winSize().height / 4)
		cameraTarget->setPositionY(mainChar->getSprPosition().y + winSize().height / 4);
	else
		cameraTarget->setPositionY(winSize().height / 2);





	//x축 follow
	if (mainChar->getSprPositionX() <= 0)
	{
		cameraTarget->setPositionX(0);
	}
	else if (mainChar->getSprPositionX() >= winSize().width)
	{
		cameraTarget->setPositionX(winSize().width);
	}
	else
		cameraTarget->setPositionX(mainChar->getSprPositionX());



	//mainChar->setPositionY(mainChar->getCharPosY());
	//moveLayer->getChildByName("ParticleSpecial")->setPosition(mainChar->getPositionX(), mainChar->getCharPosY() + mainChar->getBoundingBox().size.height);
	//building->setPositionY(NowBuildPosY); //이부분 없어도됌

	//#####주석 취소
	//GaugeDefense->setTextureRect(Rect(0.f, MaxGaugeSize - NowDefenseGauge, GaugeDefense->getContentSize().width, NowDefenseGauge));
}






// 조작 (Touch 관련)
inline bool CheckSlide(const Vec2 &start, const Vec2 &touch) {
	float temp = abs((touch - start).getAngle());
	return temp >= 2.0 || temp < 0.7;
}
inline bool CheckSpecial(const Vec2 &start, const Vec2 &touch) {
	float temp = (touch - start).getAngle();
	return temp > 1.3 && temp < 1.8;
}



//아이콘 클릭 받으면 Item.cpp로 넘어가기
bool OnGame::onTouchBegan_OnGame(Touch* touches, Event *unused_event) {
	Point touchPoint = touches->getLocation();
	bool is_jump_icon_touch, is_popup_icon_touch, is_def_icon_touch;

	/*
	is_jump_icon_touch = jump_icon->getBoundingBox().containsPoint(touchPoint);
	is_popup_icon_touch = popup_setting_icon->getBoundingBox().containsPoint(touchPoint);
	is_def_icon_touch = def_icon->getBoundingBox().containsPoint(touchPoint);
	log("touch OnGame");
	*/
	//joyButton->onTouchBegan_JoyButton(touches, unused_event);
	//테스트하는 동안만 주석처링
	//if (HelloWorld::OnGame_touch_on == false)
	//	return false;
	//if (joystick->getBoundingBox().containsPoint(touchPoint))
	//	return false;



	if (popup_setting_icon->getBoundingBox().containsPoint(touchPoint))
		return true;


	//if (!is_popup_on) {
	//log("not popup");
	//팝업창 없을 때(게임중일때)


	//// 스페셜이나 좌우이동, 죽을시 먼지효과 중에는 어떠한 인터페이스도 사용불가
	//if (!mainChar->isSpcAttack() && isShakeFinish)
	//{
	//	log("not spc && shake finish");

	//	if (is_jump_icon_touch) 
	//	{ // 점프 아이콘이 눌렸을 때(테스팅용으로 임의로 만듬)
	//		log("jump touched");
	//		if (!mainChar->isDead() 
	//			&& mainChar->isAtLand() 
	//			&& mainChar->getSizeBasicChar() < building->getPositionY()
	//			&& !isJumpTouched) 
	//		{
	//			if (mainChar->isAtLand()) {
	//				log("actually jump!");
	//				isJumpTouched = true;
	//				jump_icon->setOpacity(100);
	//				mainChar->addState((charStateType)JUMP);
	//				return true;
	//			}
	//			else {
	//				if (NowDefenseGauge / MaxGaugeSize * 100 <= 30.f) {
	//					return false;
	//				}
	//				else {
	//					mainChar->setVelocityChar(mainChar->getVelocityChar() - 40);
	//					NowDefenseGauge -= MaxGaugeSize*0.3;
	//					return true;
	//				}
	//			}
	//		}
	//		return false;
	//	}
	//	else if (is_def_icon_touch) {// 방어(Defense Bar조건 추가)
	//								 //	SimpleAudioEngine::getInstance()->playEffect("sound/defense.wav");//방어 효과음
	//		if (NowDefenseGauge / MaxGaugeSize * 100 <= 30.f) {
	//			return false;
	//		}
	//		else {
	//			def_icon->setOpacity(100);
	//			mainChar->addState((charStateType)DEFENSE);
	//			//	statement = "defense";
	//			log("touch_def OnGame");
	//			return true;
	//		}
	//	}
	//	//** #moondorr
	//	else if (popup_setting_icon->getBoundingBox().containsPoint(touchPoint))
	//		return true;
	//	else {//그 외엔 다 공격
	//		log("attack touched");
	//		is_attack_touch = true;
	//		attackcheck = true;
	//		return false;
	//	}
	//}



	return true;
}


void OnGame::onTouchMoved_OnGame(Touch* touches, Event *unuesd_event)
{
	//Point touchPoint = touches->getLocation();
};



void OnGame::onTouchEnded_OnGame(Touch* touches, Event *unuesd_event) {

	Point startPoint = touches->getStartLocation();
	Point touchPoint = touches->getLocation();
	//joyButton->onTouchEnded_JoyButton(touches, unuesd_event);

	//테스트용
	//if (isJumpTouched) {
	//	jump_icon->setOpacity(255);
	//	isJumpTouched = false;
	//}

	//if (tracer->getLineType() == LINE_TEMP)
	//{
	//	CCLOG("line touch end");
	//	tracer->setPivot(touchPoint);
	//	tracer->setLineLength(ccpDistance(startPoint, touchPoint));
	//	tracer->setLineType(LINE_NONE);
	//}



	//PopUp 스프라이트가 눌릴 때, 새로운 이벤트 리스너 등록
	if (popup_setting_icon->getBoundingBox().containsPoint(touchPoint))
	{
		Director::sharedDirector()->pause();
		//this->setTouchEnabled(true);

		listener_pause = EventListenerTouchOneByOne::create();
		listener_pause->setSwallowTouches(true);
		listener_pause->onTouchBegan = CC_CALLBACK_2(OnGame::onTouchBegan_Pause, this);
		listener_pause->onTouchEnded = CC_CALLBACK_2(OnGame::onTouchEnded_Pause, this);

		Director::getInstance()->getEventDispatcher()
			->addEventListenerWithFixedPriority(listener_pause, 2);
		PopUpPause::getInstance()->loadPopUp();
		this->addChild(PopUpPause::getInstance()->getLayer(), 3);
	}
	else
	{
		//BGManager::getInstance()->moveBG(10);
		//BGManager::getInstance()->moveBG(40);
	}



	if (startPoint.getDistance(touchPoint) >= winSize().height / 3
		&& CheckSpecial(startPoint, touchPoint)
		&& !mainChar->isDead())
	{
		//if() 필살기 게이지 꽉 찼는지 check해야함 //else이면 sound 나게 함.

		if (is_special_effect)
		{
			//activeBuilding->setVelocity(0.f);
			//isSpecialStart = true;

			movingLayer->getChildByName("BG")->setColor(Color3B(0, 0, 0));

			ParticleSystem *particleSpecialEffect = ParticleSystemQuad::create("particles/special_effect_2.plist");
			particleSpecialEffect->setScale(2);
			particleSpecialEffect->setName("particleSpecialEffect");
			fixedLayer->addChild(particleSpecialEffect, 1000, "particleSpecialEffect");

			//	SimpleAudioEngine::getInstance()->playEffect("sound/special_sound.wav");
		/*	auto rotate = RotateBy::create(0.29, 145.0);
			auto animate = mainChar->getAction(actSPC);
			auto action = CallFunc::create(CC_CALLBACK_0(OnGame::SpecialAction, this));
			auto delay = DelayTime::create(0.6f);
			auto spcFinish = CallFunc::create(CC_CALLBACK_0(OnGame::SpecialFinish, this));
			auto sequence = Sequence::create(animate, delay, spcFinish, NULL);


			spc_effect = Sprite::create("effect/skill.png");
			spc_effect->setAnchorPoint(Point(0.5, 0));
			spc_effect->setPosition(mainChar->sprChar->getContentSize().width / 2, 0);
			spc_effect->setScale(2 / mainChar->defaultScale);
			spc_effect->setVisible(false);
			mainChar->sprChar->addChild(spc_effect, 100);

			mainChar->sprWeapon->setOpacity(200);
			mainChar->sprCircle->setRotation(-145.0);
			auto seq_wp = Sequence::create(rotate, action, NULL);
			mainChar->sprCircle->runAction(seq_wp);
			mainChar->sprChar->runAction(sequence);
			*/
		}
		else
		{

		}
	}
}


bool OnGame::onTouchBegan_Pause(Touch* touch, Event *unused_event)
{
	return true;
	/*
	if (PopUpPause::getInstance()->spr_icon_resume->getBoundingBox().containsPoint(touchPoint))
	{
	return true;
	}
	else if (PopUpPause::getInstance()->spr_icon_quit->getBoundingBox().containsPoint(touchPoint))
	return true;
	else if (PopUpPause::getInstance()->spr_icon_setting->getBoundingBox().containsPoint(touchPoint))
	return true;
	else
	*/

}
void OnGame::onTouchEnded_Pause(Touch* touch, Event *unused_event)
{
	Point touchPoint = touch->getLocation();


	if (PopUpPause::getInstance()->spr_icon_resume->getBoundingBox().containsPoint(touchPoint))
	{
		Director::sharedDirector()->resume();
		//this->schedule(schedule_selector(OnGame::MainScheduler));

		_eventDispatcher->removeEventListener(listener_pause);
		this->removeChild(PopUpPause::getInstance()->getLayer());
	}
	else if (PopUpPause::getInstance()->spr_icon_quit->getBoundingBox().containsPoint(touchPoint))
	{

		//HelloWorld::OnGame_touch_on = false;
		//HelloWorld::Lobby_touch_on = true;
		Director::sharedDirector()->resume();
		is_popup_on = false;
		//auto pScene = Lobby::createScene();
		//Director::getInstance()->replaceScene(pScene);
		//log("touch5 OnGame");
		//return true;

		_eventDispatcher->removeAllEventListeners();
		this->runAction(EventManager::getInstance()->sceneTransferAction(0.1f, LOBBY));
		//Director::getInstance()->replaceScene(Lobby::createScene());
	}
	else if (PopUpPause::getInstance()->spr_icon_setting->getBoundingBox().containsPoint(touchPoint))
	{
		//** 셋팅 팝업 레이어에 대한 리스너를 생성하고 콜백설정까지
		listener_setting = EventListenerTouchOneByOne::create();
		listener_setting->setSwallowTouches(true);
		listener_setting->onTouchBegan = CC_CALLBACK_2(OnGame::onTouchBegan_Setting, this);
		listener_setting->onTouchEnded = CC_CALLBACK_2(OnGame::onTouchEnded_Setting, this);
		listener_setting->setSwallowTouches(true);


		Director::getInstance()->getEventDispatcher()
			->addEventListenerWithFixedPriority(listener_setting, 1);

		PopUpSetting::getInstance()->loadPopUp();
		this->addChild(PopUpSetting::getInstance()->getLayer(), 5);
	}
	else
	{
	}
}

bool OnGame::onTouchBegan_Setting(Touch* touch, Event *unused_event)
{

	Point touchPoint = touch->getLocation();

	/*
	if (PopUpSetting::getInstance()->
	spr_icon_bgm->getBoundingBox().containsPoint(touchPoint))
	return true;
	else if (PopUpSetting::getInstance()->
	spr_icon_effect->getBoundingBox().containsPoint(touchPoint))
	return true;
	else if (PopUpSetting::getInstance()->
	spr_icon_vibrate->getBoundingBox().containsPoint(touchPoint))
	return true;
	else if (PopUpSetting::getInstance()->
	spr_icon_close->getBoundingBox().containsPoint(touchPoint))
	return true;
	else
	*/
	return true;

}
void OnGame::onTouchEnded_Setting(Touch* touch, Event *unused_event)
{
	Point touchPoint = touch->getLocation();


	if (PopUpSetting::getInstance()->spr_icon_bgm->getBoundingBox().containsPoint(touchPoint))
	{
		PopUpSetting::getInstance()->bgmToggle();
	}
	else if (PopUpSetting::getInstance()->spr_icon_effect->getBoundingBox().containsPoint(touchPoint))
	{
		PopUpSetting::getInstance()->effectToggle();
	}

	else if (PopUpSetting::getInstance()->spr_icon_vibrate->getBoundingBox().containsPoint(touchPoint))
	{
		PopUpSetting::getInstance()->vibrateToggle();
	}
	else if (PopUpSetting::getInstance()->spr_icon_close->getBoundingBox().containsPoint(touchPoint))
	{
		this->removeChild(PopUpSetting::getInstance()->getLayer());
		_eventDispatcher->removeEventListener(listener_setting);
	}
}





void OnGame::DefenseCheck(float deltaTime) {
	if (mainChar->isDefense()) {
		if (NowDefenseGauge > 0.f)
			NowDefenseGauge -= deltaTime * MaxGaugeSize / 3;
		else
			NowDefenseGauge = 0.f;

		// Defense 게이지 부족시 막기 취소
		if (NowDefenseGauge / MaxGaugeSize * 100 <= 0.5f) {
			mainChar->delState((charStateType)DEFENSE);
			//mainChar->motionChange(actNONE);
			//def_icon->setOpacity(255);
			joyController->getBtnDefense()->setOpacity(255);
		}
	}
	else {
		if (NowDefenseGauge < MaxGaugeSize)
			NowDefenseGauge += deltaTime * MaxGaugeSize / DefenseTime;
		else
			NowDefenseGauge = MaxGaugeSize;
	}
}


void OnGame::initBG() {

	//BGManager::getInstance()->initBG(); 
	BGManager::getInstance()->loadBG();
	movingLayer->addChild(BGManager::getInstance()->getParallax(), 1, "BG");


	sprLand = Sprite::create("hill.png");
	sprLand->setScale(DivForHorizontal(sprLand), DivForVertical(sprLand)*0.35f);
	sprLand->setAnchorPoint(Point::ZERO);
	movingLayer->addChild(sprLand, 1);

	auto sprLand_left = Sprite::create("hill.png");
	sprLand_left->setScale(DivForHorizontal(sprLand), DivForVertical(sprLand)*0.35f);
	sprLand_left->setAnchorPoint(Point::ZERO);
	sprLand_left->setPositionX(-winSize().width / 3 * 2);
	movingLayer->addChild(sprLand_left, 1);

	auto sprLand_mid = Sprite::create("hill.png");
	sprLand_mid->setScale(DivForHorizontal(sprLand), DivForVertical(sprLand)*0.35f);
	sprLand_mid->setAnchorPoint(Point::ZERO);
	sprLand_mid->setPositionX(winSize().width / 2);
	movingLayer->addChild(sprLand_mid, 1);


	auto sprLand_right = Sprite::create("hill.png");
	sprLand_right->setScale(DivForHorizontal(sprLand), DivForVertical(sprLand)*0.35f);
	sprLand_right->setAnchorPoint(Point::ZERO);
	sprLand_right->setPositionX(winSize().width / 4 * 3);
	movingLayer->addChild(sprLand_right, 1);



}
void OnGame::initCharacter() {
	//initPos,  gravity,  jumpVelocity,  specialDamage, Canvas
	mainChar
		= new Character(
			Point(winSize().width / 2, land),
			6.5f,
			90.0f,
			40,
			movingLayer);

	//movingLayer->addChild(mainChar, 5);
}




void OnGame::initIcon() {

	//설정 버튼(이것은 팝업 창을 띄우고 위치는 오른쪽 위로 잡음)
	popup_setting_icon = Sprite::create("ui/pause.png");
	popup_setting_icon->setAnchorPoint(Point(1, 1));
	//popup_setting_icon->setPosition(Point(winSize.width / 8 * 7, winSize.height / 14 * 12));
	popup_setting_icon->setPosition(Point(winSize().width - 20, winSize().height - 20));
	fixedLayer->addChild(popup_setting_icon, 10, "setting");
}


void OnGame::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event) {
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE || keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE) {
		//Director::sharedDirector()->popScene(); //이 부분에서 다른 씬으로 넘어가면, 다음씬에서 그 다음 씬으로 넘어갈 때 오류남. 알아보겠음. (석원)
		//HelloWorld::OnGame_touch_on = false;
		//HelloWorld::Lobby_touch_on = true;
		auto pScene = Lobby::createScene();
		Director::getInstance()->replaceScene(pScene);
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_HOME) {
		//HelloWorld::OnGame_touch_on = false;
		//HelloWorld::Lobby_touch_on = true;
		auto pScene = Lobby::createScene();
		Director::getInstance()->replaceScene(pScene);
	}
}
void OnGame::score_presentation(int score) { //점수 화면에 뜨게 하는 함수
	for (unsigned i = 0; i < str_length; i++) //현재 까지 있는 점수 이미지 삭제
		fixedLayer->removeChildByTag(i + 100000);

	std::string str = IntToString(score); //숫자를 받아서 String로 바꿔줌
	str_length = str.length();
	for (unsigned i = 0; i < str_length; i++)
	{
		char buf[20] = { 0 };
		sprintf(buf, "numbers/%d.png", (int)str.at(i) - '0');

		auto score_number = Sprite::create(buf);
		auto DivisionForScreen_vertical = winSize().width / (score_number->getContentSize().width);
		auto DivisionForScreen_horizontal = winSize().height / (score_number->getContentSize().height);
		score_number->setScale((DivisionForScreen_vertical / 11), (DivisionForScreen_horizontal / 11));
		score_number->setAnchorPoint(Point(1, 1));
		score_number->setPosition(Point(popup_setting_icon->getPositionX() - popup_setting_icon->getBoundingBox().size.width - (str_length - i - 1) * 40, popup_setting_icon->getPositionY() - 20)); //위치는 그냥 왼쪽 위로 임의로 잡음. 위치 수정 해야함.
		score_number->setTag(i + 100000); //태그 번호 임의로 잡음
		fixedLayer->addChild(score_number, 4);
	}
}
void OnGame::combo_presentation(int combo) { //점수 화면에 뜨게 하는 함수
	for (unsigned i = 0; i <= combo_str_length; i++) //현재 까지 있는 점수 이미지 삭제
		fixedLayer->removeChildByTag(i + 100010);

	std::string str = IntToString(combo); //숫자를 받아서 String로 바꿔줌
	combo_str_length = str.length();

	float temp;
	for (int i = 0; i <= combo_str_length; ++i)
	{
		if (i == combo_str_length) {
			auto score_number = Sprite::create("numbers/combo.png");

			//auto DivisionForScreen_vertical = winSize().width / (score_number->getContentSize().width);
			//auto DivisionForScreen_horizontal = winSize().height / (score_number->getContentSize().height);

			score_number->setScale(DivForHorizontal(score_number) / 4, DivForVertical(score_number) / 4);

			score_number->setAnchorPoint(Point(1, 0));
			score_number->setPosition(Point(winSize().width / 2, winSize().height * 0.6 + temp / 3));
			score_number->setTag(i + 100010);
			fixedLayer->addChild(score_number, 5);

			auto fadeOut = FadeOut::create(0.5f); //스코어 fade in
			score_number->runAction(fadeOut);
		}
		else {
			char buf[20] = { 0 };
			sprintf(buf, "numbers/combo_%d.png", (int)str.at(i) - '0');
			auto score_number = Sprite::create(buf);

			score_number->setScale(DivForHorizontal(score_number) / 7, DivForVertical(score_number) / 7);
			temp = DivForVertical(score_number) / 7;

			score_number->setAnchorPoint(Point(0.5, 0));
			//score_number->setPosition(Point(250 + (i * 70), winSize.height / 7 * 5)); //위치는 그냥 중간 위로 임의로 잡음. 위치 수정 해야함.
			score_number->setPosition(Point(winSize().width / 2 + i * score_number->getBoundingBox().size.width * 0.8, winSize().height * 0.6)); // 위치 수정본
			score_number->setTag(i + 100010); //태그 번호 임의로 잡음
			fixedLayer->addChild(score_number, 4);

			auto fadeOut = FadeOut::create(0.5f); //스코어 fade in
			score_number->runAction(fadeOut);
		}
	}
}






std::string OnGame::IntToString(int number) //숫자를 인자로 넘겨주면 String 을 반환하는 함수
{
	std::stringstream conversion;
	conversion << number;
	return conversion.str();
}

void OnGame::cinematicEffect(bool didSpecial) {
	if (!didSpecial) {
		// 소리 추가 필수
		auto FadeSpecial = Sprite::create("stage/white.png");
		FadeSpecial->setScale(winSize().width, winSize().height);
		movingLayer->getChildByName("BG")->setColor(Color3B(255, 255, 255));
		auto Fadeout = FadeOut::create(3.f);
		fixedLayer->addChild(FadeSpecial);
		FadeSpecial->runAction(Fadeout);
	}
	// 캐릭터와 건물 위치 리셋 + Trigger작동
	mainChar->setPosition(Point(mainChar->getPosition().x, 0.f));
	//mainChar->setPositionY(0.f);
	//activeBuilding->setPositionY(winSize().height / 3);
	NowBuildPosY = winSize().height / 3;
	cameraTarget->setPositionY(winSize().height / 2);

	isCinematic = true;

	ParticleSystem* breakBuild = ParticleSystemQuad::create("particles/break_building_2.plist");
	breakBuild->setScale(0.6);
	breakBuild->setAnchorPoint(Vec2(0.5, 0));
	breakBuild->setEmissionRate(20);
	breakBuild->setPosition(Vec2(winSize().width / 2, NowBuildPosY));
	breakBuild->setDuration(-1);
	movingLayer->addChild(breakBuild, 100, "breakBuild");
}



//
////building <-> hero 공격처리
//void OnGame::Collision_Attack()
//{
//	double ch_x = mainChar->getPosition().x;
//	double ch_y = mainChar->getPosition().y;
//	double blockwidth = building->getblockwidth();
//	double blockheight = building->getblockwidth();
//	double Low_x, Low_y, High_x, High_y;
//	char buf[100];
//	int N_strength;
//
//	Low_y = mainChar->getCharHeight() + ch_y;
//	High_y = Low_y + mainChar->getCharHeight();
//	Low_x = ch_x - mainChar->getWpHeight();
//	High_x = ch_x + mainChar->getWpHeight();
//	bool check = false;
//	int temp = 0;
//	int type = building->gettype();
//	int floor = building->getfloor();
//	if (!is_boss)
//	{
//		for (int i = 0; i < building->getNumRow(); i++)
//			for (int j = 0; j < building->getNumCol(); j++)
//			{
//				if (building->blocks[i][j].spr == NULL || building->blocks[i][j].strength == 0)
//					continue;
//
//				if (building->blocks[i][j].spr->getPositionX() + 4 * blockwidth / 5 >= Low_x
//					&& building->blocks[i][j].spr->getPositionX() + blockwidth / 5 < High_x
//					&& building->blocks[i][j].spr->getPositionY() + 5 <= High_y && building->blocks[i][j].spr->getPositionY() > ch_y)
//				{
//					check = true;
//					building->blocks[i][j].strength--;
//					temp++;
//					if (building->blocks[i][j].strength == 0)
//					{
//						sprintf(buf, "ruins/%d/%d.plist", building->gettype(), (i + j) % 3 + 1);
//						ParticleSystem* particle_ruin = ParticleSystemQuad::create(buf);
//						particle_ruin->setAnchorPoint(Vec2(0.5, 0.f));
//						particle_ruin->setScale(2.0f);
//						particle_ruin->setDuration(0.5f);
//						particle_ruin->setPosition(Vec2(mainChar->getPosition().x, building->blocks[i][j].spr->getPositionY() + 10));
//						movingLayer->addChild(particle_ruin, 10, "particle_ruin");
//
//						building->blocks[i][j].spr->removeFromParentAndCleanup(true);
//						//	building->blocks[i][j].spr->removeAllChildrenWithCleanup(true);
//						building->blocks[i][j].spr = NULL;
//						continue;
//					}
//
//					if (building->blocks[i][j].strength > 5)
//						N_strength = 5;
//					else
//						N_strength = building->blocks[i][j].strength;
//
//
//					if (type == 1 || type == 2)
//						sprintf(buf, "building/%d/%d/%d_%d %s-%d-%d.png", type, 5 - N_strength, type, 5 - N_strength, "[www.imagesplitter.net]", building->build_N[i][j], j);
//					else if ((type == 3 || type == 4) && floor == 20)
//						sprintf(buf, "building/%d/%d/%d/%d_%d %s_%d-%d-%d.png", type, 1, 5 - N_strength, type, 5 - N_strength, "[www.imagesplitter.net]", 1, building->build_N[i][j], j);
//					else if ((type == 3 || type == 4) && floor == 32)
//						sprintf(buf, "building/%d/%d/%d/%d_%d %s_%d-%d-%d.png", type, 2, 5 - N_strength, type, 5 - N_strength, "[www.imagesplitter.net]", 2, building->build_N[i][j], j);
//
//					//sprintf(buf, "%s/%d/%d%s%d-%d.png", "building", building->gettype(), N_strength, "/building [www.imagesplitter.net]-", building->build_N[i][j], j);
//					building->blocks[i][j].spr->setTexture(buf);
//
//				}
//			}
//	}
//	else
//	{
//		if (building->blocks[0][0].spr->getPositionY() + 5 <= High_y && building->blocks[0][0].strength > 0)
//		{
//
//			check = true;
//			temp += 10 * (increase_count + 1);
//		}
//		if (building->blocks[0][0].strength >= 80)
//		{
//			sprintf(buf, "boss/1.png");
//			building->blocks[0][0].spr->setTexture(buf);
//		}
//		else if (building->blocks[0][0].strength >= 60)
//		{
//			sprintf(buf, "boss/2.png");
//			building->blocks[0][0].spr->setTexture(buf);
//		}
//		else if (building->blocks[0][0].strength >= 40)
//		{
//			sprintf(buf, "boss/3.png");
//			building->blocks[0][0].spr->setTexture(buf);
//		}
//		else if (building->blocks[0][0].strength >= 20)
//		{
//			sprintf(buf, "boss/4.png");
//			building->blocks[0][0].spr->setTexture(buf);
//		}
//		else if (building->blocks[0][0].strength > 0)
//		{
//			sprintf(buf, "boss/5.png");
//			building->blocks[0][0].spr->setTexture(buf);
//		}
//		else
//		{
//			//**end하는 기능 추가해야함
//			building->blocks[0][0].spr->removeFromParentAndCleanup(true);
//			building->blocks[0][0].spr = NULL;
//		}
//	}
//
//
//	if (check)
//	{
//		SimpleAudioEngine::getInstance()->playEffect("sound/hitting_building.wav");
//		if (is_boss)
//			building->blocks[0][0].strength--;
//
//		auto FadeSpecial = Sprite::create("effect/attack.png");
//		FadeSpecial->setAnchorPoint(Point(0.5, 0));
//		FadeSpecial->setPosition(Vec2(ch_x, High_y - 30));
//		FadeSpecial->setScale(mainChar->sprWeapon->getScale());
//		auto Fadeout = FadeOut::create(0.2f);
//		movingLayer->addChild(FadeSpecial, 10);
//		FadeSpecial->runAction(Fadeout);
//
//		on_score += temp;
//		special_gauge += temp;
//		if (on_score > standard*(increase_count + 1))
//		{
//			newscale *= increase_degree;
//			if (newscale > 3.0)
//			{
//				newscale = 3.0;
//				return;
//			}
//			increase_count++;
//			on_score = 0;
//
//			//검크기 테스트
//			//auto scale = mainChar->sprWeapon->getScale();
//			log("weapon scale: %f", newscale);
//			mainChar->sprWeapon->setScale(newscale);
//			//
//		}
//
//	}
//}
//bool OnGame::Collision_defense()
//{
//	float blockwidth = building->getblockwidth();
//	float blockheight = building->getblockwidth();
//	float ch_x = mainChar->getPosition().x;
//	float ch_y = mainChar->getPosition().y;
//	double Low_x, Low_y, High_x, High_y;
//	Low_y = mainChar->getCharHeight() + ch_y;
//	High_y = Low_y + mainChar->getWpHeight();
//
//	for (int i = 0; i < building->getNumRow(); i++)
//		for (int j = 0; j < building->getNumCol(); j++)
//		{
//			if (building->blocks[i][j].spr == NULL || building->blocks[i][j].strength == 0)
//				continue;
//
//			if (building->blocks[i][j].spr->getPositionX() < ch_x && ch_x <= building->blocks[i][j].spr->getPositionX() + blockwidth
//				&& building->blocks[i][j].spr->getPositionY() <= ch_y + mainChar->getCharHeight() + 20)
//			{
//				auto FadeSpecial = Sprite::create("effect/defence.png");
//				FadeSpecial->setAnchorPoint(Point(0.5, 0));
//				FadeSpecial->setPosition(Vec2(ch_x, (High_y + Low_y) / 2));
//				auto Fadeout = FadeOut::create(0.2f);
//				movingLayer->addChild(FadeSpecial, 10);
//				FadeSpecial->runAction(Fadeout);
//				return true;
//
//			}
//		}
//	return false;
//}
//void OnGame::Collision_DiagFragment()
//{
//	for (int i = 0; i < 10; i++)
//	{
//		if (diagfragment[i] == NULL || diagfragment[i]->DiagF == NULL)
//			return;
//
//		float Diag_x = diagfragment[i]->DiagF->getPosition().x;
//		float Diag_y = diagfragment[i]->DiagF->getPosition().y;
//		float ch_x = mainChar->getPosition().x;
//		float ch_y = mainChar->getPosition().y;
//		float ch_width = mainChar->getCharWidth();
//		float ch_height = mainChar->getCharHeight();
//
//		float Low_x, Low_y, High_x, High_y;
//		Low_y = ch_height + ch_y;
//		High_y = Low_y + mainChar->getWpHeight();
//		Low_x = ch_x - mainChar->getWpHeight();
//		High_x = ch_x + mainChar->getWpHeight();
//
//
//		if (ch_x - ch_width / 2 < Diag_x && Diag_x <= ch_x + ch_width / 2 && ch_y < Diag_y && Diag_y <= ch_y + ch_height && blocked == true)
//			return;
//
//		if (mainChar->isAttack())
//		{
//			if (Diag_x > Low_x - 10 && Diag_x <= High_x + 10 && Diag_y > Low_y - 30 && Diag_y <= High_y)
//			{
//				diagfragment[i]->DiagF->removeFromParentAndCleanup(true);
//				diagfragment[i]->DiagF = NULL;
//				continue;
//			}
//		}
//
//		//충돌!
//		if (ch_x - ch_width / 2 < Diag_x && Diag_x <= ch_x + ch_width / 2 && ch_y < Diag_y && Diag_y <= ch_y + ch_height)
//		{
//			gettimeofday(&startedTime, NULL);
//
//			on_score = 0;
//			increase_count = 0;
//			newscale = 1;
//			mainChar->sprWeapon->setScale(newscale);
//			blocked = true;
//
//			trialCount++;
//			return;
//		}
//		if (Diag_y <= 0)
//		{
//			diagfragment[i]->DiagF->removeFromParentAndCleanup(true);
//			diagfragment[i]->DiagF = NULL;
//			ruinCount += 10; // 페널티 우선 10으로 줬음
//			continue;
//		}
//
//	}
//
//	blocked = false;
//}
//void OnGame::Collision_Special()
//{
//	float blockwidth = building->getblockwidth();
//	float blockheight = building->getblockwidth();
//	float ch_x = mainChar->getPosition().x;
//	float ch_y = mainChar->getPosition().y;
//	float low, high;
//	low = ch_x - spc_effect->getContentSize().width / 2;
//	high = ch_x + spc_effect->getContentSize().width / 2;
//
//	if (!is_boss)
//	{
//		for (int i = 0; i < building->getNumRow(); i++)
//			for (int j = 0; j < building->getNumCol(); j++)
//			{
//				if (building->blocks[i][j].spr == NULL || building->blocks[i][j].strength == 0)
//					continue;
//
//				if (NowSpecialDamage <= 0)
//				{
//					mainChar->motionChange(actNONE);
//					mainChar->delState(SPCATK);
//					spc_effect->removeFromParentAndCleanup(true);
//					particleSpecialEffect->removeFromParentAndCleanup(true);
//					special_gauge = 0;
//					is_special_effect = false;
//					mainChar->setVelocityChar(-30);
//
//					auto FadeSpecial = Sprite::create("stage/white.png");
//					FadeSpecial->setScale(winSize().width, winSize().height);
//					movingLayer->getChildByName("BG")->setColor(Color3B(255, 255, 255));
//					auto Fadeout = FadeOut::create(0.4f);
//					fixedLayer->addChild(FadeSpecial);
//					FadeSpecial->runAction(Fadeout);
//					NowSpecialDamage = 0;
//					log("000");
//					return;
//				}
//				if (building->blocks[i][j].spr->getPositionX() > low && building->blocks[i][j].spr->getPositionX() <= high)
//				{
//					NowSpecialDamage -= building->blocks[i][j].strength;
//					building->blocks[i][j].strength = 0;
//					building->blocks[i][j].spr->removeFromParentAndCleanup(true);
//					building->blocks[i][j].spr = NULL;
//				}
//			}
//		// damage가 0 되기전에 건물 다깨지거나 보스 다 깨지는 것 처리`
//		mainChar->delState(SPCATK);
//		spc_effect->removeFromParentAndCleanup(true);
//		particleSpecialEffect->removeFromParentAndCleanup(true);
//		special_gauge = 0;
//		is_special_effect = false;
//		mainChar->setVelocityChar(-30);
//		log("11111tot");
//		auto FadeSpecial = Sprite::create("stage/white.png");
//		FadeSpecial->setScale(winSize().width, winSize().height);
//		movingLayer->getChildByName("BG")->setColor(Color3B(255, 255, 255));
//		auto Fadeout = FadeOut::create(0.4f);
//		fixedLayer->addChild(FadeSpecial);
//		FadeSpecial->runAction(Fadeout);
//		NowSpecialDamage = 0;
//
//		return;
//	}
//	else
//	{
//		if (building->blocks[0][0].strength > NowSpecialDamage)
//			building->blocks[0][0].strength -= NowSpecialDamage;
//		else
//		{
//			building->blocks[0][0].strength = 0;
//			building->blocks[0][0].spr->removeFromParentAndCleanup(true);
//			building->blocks[0][0].spr = NULL;
//		}
//		mainChar->delState(SPCATK);
//		spc_effect->removeFromParentAndCleanup(true);
//		particleSpecialEffect->removeFromParentAndCleanup(true);
//		special_gauge = 0;
//		is_special_effect = false;
//		mainChar->setVelocityChar(-30);
//		auto FadeSpecial = Sprite::create("stage/white.png");
//		FadeSpecial->setScale(winSize().width, winSize().height);
//		movingLayer->getChildByName("BG")->setColor(Color3B(255, 255, 255));
//		auto Fadeout = FadeOut::create(0.4f);
//		fixedLayer->addChild(FadeSpecial);
//		FadeSpecial->runAction(Fadeout);
//		NowSpecialDamage = 0;
//		return;
//	}
//
//
//}
//
//
