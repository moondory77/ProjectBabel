#define FRAME_DELAY 0.034	//60프레임 기준 1프레임은 0.017초 
#include "EffectManager.h"

EffectManager* EffectManager::EffectManager_instance = NULL;

EffectManager* EffectManager::getInstance() {

	if (!EffectManager_instance) {
		EffectManager_instance = new EffectManager();
	}
	return EffectManager_instance;
}


//SprEffectInfo 초기화
void EffectManager::initEffect(EffectType spr_type, const char* tex_name, int frame_num)
{
	if (EffectTable.find(spr_type) == EffectTable.end())
	{
		string join_tex_name = string(tex_name);
		Texture2D* tex_in_cache = texPool->joinTexLink("effect/" + join_tex_name + ".png");

		EffectTable.insert({ spr_type , new SprEffectInfo(*tex_in_cache, spr_type, join_tex_name, frame_num)});
	}
}



//labelEffectInfo 초기화
void EffectManager::initEffect(EffectType label_type, const char* tex_name)
{
	if (EffectTable.find(label_type) == EffectTable.end())
	{
		string join_tex_name = string(tex_name);
		Texture2D* tex_in_cache = texPool->joinTexLink("effect/" + join_tex_name + ".png");

		EffectTable.insert({ label_type , new LabelEffectInfo(*tex_in_cache, label_type, join_tex_name) });
	}
}


void EffectManager::removeEffect(EffectType effect_name)
{
	if (EffectTable.find(effect_name) != EffectTable.end())
	{
		string detach_tex_address = "effect/" + EffectTable[effect_name]->texName + ".png";
		EffectTable[effect_name]->release();
		EffectTable.erase(effect_name);

		//texture Pool에서 링크 해제
		texPool->detachTexLink(detach_tex_address);
	}
}



void EffectManager::runBladeWind(Point position, Vec2 direction, float size, bool clockwise, Layer& target_canvas)
{
	if (EffectTable[BLADE_WIND_1ST])
	{
		SprEffectInfo& casted_effect = *static_cast<SprEffectInfo*>(EffectTable[BLADE_WIND_1ST]);

		casted_effect.sprEffect = Sprite::create();
		target_canvas.addChild(casted_effect.sprEffect, 10);

		auto delay = DelayTime::create(FRAME_DELAY*0.3f);
		auto animation = Animation::createWithSpriteFrames(casted_effect.frames, FRAME_DELAY * 0.3f);
		auto animAction = Animate::create(animation);

		casted_effect.sprEffect->setOpacity(245);
		casted_effect.sprEffect->setRotation(-20.0f);
		casted_effect.sprEffect->setScale(size*1.1f);
		casted_effect.sprEffect->setPosition(position);

		auto fadeout = FadeOut::create(0.2f);
		auto move_out = MoveBy::create(0.2f, direction);
		auto waving_out = Spawn::create(fadeout, move_out, nullptr);
		auto remove_self = CallFunc::create([&]()
		{
			casted_effect.sprEffect->removeFromParentAndCleanup(true);
			casted_effect.sprEffect = NULL;
		});

		if (clockwise)
			casted_effect.sprEffect->runAction(Sequence::create(
				Spawn::create(animAction->reverse(), waving_out, nullptr), remove_self, nullptr));
		else
			casted_effect.sprEffect->runAction(Sequence::create(
				Spawn::create(animAction, waving_out, nullptr), remove_self, nullptr));
	}
}
void EffectManager::runBladeSlash(Point position, Vec2 direction, float size, Layer& target_canvas)
{
	if (EffectTable[BLADE_SLASH])
	{
		SprEffectInfo& casted_effect = *static_cast<SprEffectInfo*>(EffectTable[BLADE_SLASH]);
		casted_effect.sprEffect = Sprite::create();
		target_canvas.addChild(casted_effect.sprEffect, 10);

		casted_effect.sprEffect->setSpriteFrame(casted_effect.frames.at(0));
		casted_effect.sprEffect->setOpacity(245);
		casted_effect.sprEffect->setRotation(-20.0f);
		casted_effect.sprEffect->setScale(size*1.5f);
		casted_effect.sprEffect->setPosition(position);

		auto fadeout = FadeOut::create(0.3f);
		auto scale_up = ScaleBy::create(0.4f, 1.7f);
		auto waving_out = Spawn::create(fadeout, scale_up, nullptr);
		auto remove_self = CallFunc::create([&]() {
			casted_effect.sprEffect->removeFromParentAndCleanup(true);
			casted_effect.sprEffect = NULL;
		});
		casted_effect.sprEffect->runAction(Sequence::create(waving_out, remove_self, nullptr));
	}
}

void EffectManager::runAttackCharge(float max_size, Sprite& target_canvas)
{
	if (EffectTable[ATTACK_CHARGE])
	{
		SprEffectInfo& casted_effect = *static_cast<SprEffectInfo*>(EffectTable[ATTACK_CHARGE]);
		casted_effect.sprEffect = Sprite::create();
		target_canvas.addChild(casted_effect.sprEffect);
		auto animation = Animation::createWithSpriteFrames(casted_effect.frames, FRAME_DELAY * 1.2f);
		auto animAction = Animate::create(animation);

		casted_effect.sprEffect->setOpacity(230);
		casted_effect.sprEffect->setScale(1.5f);
		casted_effect.sprEffect->setPosition(Point(target_canvas.getContentSize().width / 2, target_canvas.getContentSize().height *0.7f));
		casted_effect.sprEffect->runAction(RepeatForever::create(animAction));
	}
}
void EffectManager::stopAttackCharge()
{
	SprEffectInfo& casted_effect = *static_cast<SprEffectInfo*>(EffectTable[ATTACK_CHARGE]);

	if (casted_effect.sprEffect != NULL)
	{
		auto fade_out = FadeOut::create(0.5f);
		casted_effect.sprEffect->runAction(Sequence::create(fade_out, RemoveSelf::create(true), nullptr));
		casted_effect.sprEffect = NULL;
	}
};


void EffectManager::displayAtkScore(int score, Point position, float size, Layer& target_canvas)
{
	if (EffectTable[SCORE_NORMAL])
	{
		LabelEffectInfo& casted_effect = *static_cast<LabelEffectInfo*>(EffectTable[SCORE_NORMAL]);
		string casted_score = to_string(score);

		if (casted_effect.labelEffect == NULL) {
			//casted_effect.labelEffect = Label::createWithCharMap("effect/combo/damage.png", 258, 298, '0');
			casted_effect.labelEffect = Label::createWithCharMap(&casted_effect.texture, 258, 298, '0');
			casted_effect.labelEffect->setPosition(winSize().width / 2, winSize().height *0.7f);
			casted_effect.labelEffect->setScale(1.0f);
			target_canvas.addChild(casted_effect.labelEffect);
		}
		else {
			//현재 running 중이라면, action만 취소
			casted_effect.labelEffect->stopAllActions();
			casted_effect.labelEffect->setOpacity(255);
		}

		casted_effect.labelEffect->setString(casted_score);
		casted_effect.labelEffect->updateContent();

		auto tremble_left = MoveBy::create(0.04f, Point(30, 0));
		auto tremble_once = Sequence::create(tremble_left, tremble_left->reverse(), nullptr);
		
		auto tramble_seq = Repeat::create(tremble_once, 2);
		auto delay = DelayTime::create(0.8f);
		auto fade_out = FadeOut::create(0.3f);

		casted_effect.labelEffect
			->runAction(Sequence::create(delay, fade_out, 
				CallFunc::create([&]() {
			casted_effect.labelEffect->removeFromParentAndCleanup(true);
			casted_effect.labelEffect = NULL;
		}), nullptr));


		//	//curAttackID = input_id; //attack ID 갱신 (1회 출력 보장)
		//	auto act = FadeOut::create(0.4f);
		//	auto tremble = Sequence::create(MoveBy::create(0.04f, Point(30, 0)), MoveBy::create(0.04f, Point(-30, 0)), nullptr);
		//	auto tremble_seq = Repeat::create(tremble, 2);
		//	auto shaking = Spawn::create(act, tremble_seq, nullptr);
		//
		//	label->runAction(Sequence::create(shaking, RemoveSelf::create(true), nullptr));
		//	//}

	}
}



//void EffectManager::callBack_AttackCharge(float deltaTime)
//{
//	if (Effects[ATTACK_CHARGE]->sprEffect->getScale() < 4.0f)
//		Effects[ATTACK_CHARGE]->sprEffect->setScale(Effects[ATTACK_CHARGE]->sprEffect->getScale() * 1.002f);
//};


//
//void EffectManager::runDamageEffect(Layer* canvas, int damage, int input_id)
//{
//	//if (input_id != curAttackID)
//	//{
//	std::string str_damage = std::to_string(damage);
//	auto label = Label::createWithCharMap("effect/combo/damage.png", 258, 298, '0');
//	//label->setBMFontSize(800);
//	//label->setTextColor(Color3B(100,100,100));
//	label->setString(str_damage);
//	label->setPosition(winSize().width / 2, winSize().height *0.7f);
//	label->setScale(1.35f);
//	canvas->addChild(label);
//
//	//curAttackID = input_id; //attack ID 갱신 (1회 출력 보장)
//	auto act = FadeOut::create(0.4f);
//	auto tremble = Sequence::create(MoveBy::create(0.04f, Point(30, 0)), MoveBy::create(0.04f, Point(-30, 0)), nullptr);
//	auto tremble_seq = Repeat::create(tremble, 2);
//	auto shaking = Spawn::create(act, tremble_seq, nullptr);
//
//	label->runAction(Sequence::create(shaking, RemoveSelf::create(true), nullptr));
//	//}
//}
//
//void EffectManager::runSpecialEffect(Character* mainChar, Layer* canvas, float start_delay)
//{
//
//	auto init_delay = DelayTime::create(start_delay);
//	auto spr_skill = Sprite::create("effect/skill.png");
//	//spr_skill->setScale(DivForHorizontal(spr_skill), DivForVertical(spr_skill));
//
//	spr_skill->setAnchorPoint(Point(0.5, 0.04));
//	//spr_skill->setPosition(Point(winSize().width / 2, 0));
//	//spr_skill->setScale(0.0f, DivForHorizontal(spr_skill));
//	spr_skill->setScale(mainChar->specialRadar->getContentSize().width / winSize().width * 2.0f, 1.0f);
//	spr_skill->setPosition(Point(mainChar->specialRadar->getContentSize().width*0.41f, 0));
//	spr_skill->setOpacity(0);
//
//	mainChar->specialRadar->addChild(spr_skill);
//
//	auto fade = FadeIn::create(0.0f);
//	auto fade_to_zero = FadeTo::create(0.5f, 245);
//	auto fade_to_origin = FadeTo::create(0.5f, 255);
//	auto fade_seq = RepeatForever::create(Sequence::create(fade_to_zero, fade_to_origin, nullptr));
//
//	//auto expand = ScaleBy::create(0.06f, 1.04f, 1.0f);
//	//auto shrink = expand->reverse();
//	//auto scale_seq = RepeatForever::create(Sequence::create(expand, shrink, nullptr));
//	auto tremble_right = MoveBy::create(0.05f, Vec2(50, 0));
//	auto tremble_left = tremble_right->reverse();
//	auto tremble_seq = Repeat::create(Sequence::create(tremble_right, tremble_left, nullptr), 50);
//
//	//auto tt = Spawn::createWithTwoActions(fade_seq, tremble_seq);
//
//	//일단 페이드효과
//	//spr_skill->runAction(Sequence::create(init_delay, fade, tt, nullptr));
//	spr_skill->runAction(Sequence::create(init_delay, fade, tremble_seq, nullptr));
//	//auto expand = ScaleTo::create(3.0f, mainChar->specialRadar->getScaleX()*3, 1.0f);
//
//
//	auto expand = ScaleTo::create(0.8f, mainChar->specialRadar->getContentSize().width / winSize().width*4.5f, 1.0f);
//	auto delay_1 = DelayTime::create(2.5f);
//	//auto delay_2 = DelayTime::create(1.0f);
//	auto call_back = CallFuncN::create(CC_CALLBACK_1(EffectManager::callBackSpcEnd, this, mainChar));
//	auto shrink = ScaleTo::create(0.6f, mainChar->specialRadar->getContentSize().width / winSize().width, 1.0f);
//	auto remove = RemoveSelf::create();
//	spr_skill->runAction(Sequence::create(init_delay, expand, delay_1, call_back, shrink, remove, nullptr));
//
//	//spr_skill->runAction(Spawn::create(sk, tremble_seq, nullptr));
//	//spr_skill->runAction(tremble_seq);
//	//spr_skill->runAction(sk);
//	//spr_skill->runAction(tremble_seq);
//	//spr_skill->runAction(ScaleTo::create(3.5f, 3.0f, 1.0f));
//	//spr_skill->runAction(TintTo::create(2.0f, Color3B::RED));
//}
//
//
//void EffectManager::callBackSpcEnd(Ref* sender, Character* mainChar)
//{
//	mainChar->playCharAction(actSPCLandEnd);
//	//mainChar->delState(SPC_ATK);
//}
//
//void EffectManager::runDefenseEffect(Character* mainChar, Layer* canvas)
//{
//	Sprite* sprDefense = Sprite::create("effect/defense.png");
//	sprDefense->setOpacity(200);
//	sprDefense->setScale(2.0f);
//	sprDefense->setPosition(mainChar->getPosition() + Vec2(0, mainChar->getBodyHeight()*0.6f));
//
//	auto fadeout = FadeOut::create(0.6f);
//	auto remove = RemoveSelf::create(true);
//	auto sequence = Sequence::create(fadeout, remove, nullptr);
//
//	canvas->addChild(sprDefense, 10);
//	sprDefense->runAction(sequence);
//}
//
//







