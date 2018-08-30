#define FRAME_DELAY 0.034	//60프레임 기준 1프레임은 0.017초 
#include "Manager/EffectManager.h"
USING_NS_CC;

EffectManager* EffectManager::EffectManager_instance = NULL;

EffectManager* EffectManager::getInstance() {

	if (!EffectManager_instance) {
		EffectManager_instance = new EffectManager();
	}
	return EffectManager_instance;
}


void EffectManager::initEffectInfo(EffectName effect_name, const char* plist_texture_name, int frame_num)
{
	if (Effects[effect_name] != NULL)
		removeEffectInfo(effect_name);

	//address_name을 key로 맵에 저장하기 위해, string으로 convert
	string casted_name = string(plist_texture_name);

	//아직 캐쉬에 로드 안된 텍스쳐 주소일 경우,
	if (AddressTable.find(casted_name) == AddressTable.end())
	{
		AddressTable.insert(casted_name);
		auto tex_in_cache = TextureCache::getInstance()->addImage("effect/" + casted_name + ".png");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("effect/" + casted_name + ".plist", tex_in_cache);

		///사용 이미지를, Texture/SpriteFrame 캐쉬에 추가 (텍스쳐 모두 로드 후, 프레임캐쉬 추가를 위해 sync 함수 사용)
		//TextureCache::getInstance()->addImageAsync("effect/" + casted_name + ".png", [&](Ref* sender)
		//{
		//	auto tex_in_cache = static_cast<Texture2D*>(sender);
		//	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("effect/" + casted_name + ".plist", tex_in_cache);
		//	Effects[effect_name]->frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Effect_%d_1.png")));
		//});
	}
	Effects[effect_name] = new EffectInfo(casted_name);

	for (int i = 0; i < frame_num; i++) {
		Effects[effect_name]->frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Effect_%d_%i.png", (int)effect_name, i + 1)));
	}
}
void EffectManager::removeEffectInfo(EffectName effect_name)
{
	if (Effects[effect_name] != NULL)
	{
		Effects[effect_name]->frames.clear();
		Effects[effect_name]->sprEffect->removeFromParentAndCleanup(true);
		delete(Effects[effect_name]);
		SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();

		if (SpriteFrameCache::getInstance()->isSpriteFramesWithFileLoaded("effect/" + Effects[effect_name]->address + ".plist") == false)
		{
			TextureCache::getInstance()->removeTextureForKey("effect/" + Effects[effect_name]->address + ".png");
			AddressTable.erase(Effects[effect_name]->address);
		}

	}
}


void EffectManager::runBladeWind(Point position, Vec2 direction, float size, bool clockwise, Layer& target_canvas)
{
	if (Effects[BLADE_WIND_1ST])
	{
		Effects[BLADE_WIND_1ST]->sprEffect = Sprite::create();
		target_canvas.addChild(Effects[BLADE_WIND_1ST]->sprEffect, 10);

		auto delay = DelayTime::create(FRAME_DELAY*0.3f);
		auto animation = Animation::createWithSpriteFrames(Effects[BLADE_WIND_1ST]->frames, FRAME_DELAY * 0.3f);
		auto animAction = Animate::create(animation);

		Effects[BLADE_WIND_1ST]->sprEffect->setOpacity(245);
		Effects[BLADE_WIND_1ST]->sprEffect->setRotation(-20.0f);
		Effects[BLADE_WIND_1ST]->sprEffect->setScale(size*1.1f);
		Effects[BLADE_WIND_1ST]->sprEffect->setPosition(position);

		auto fadeout = FadeOut::create(0.2f);
		auto move_out = MoveBy::create(0.2f, direction);
		
		auto waving_out = Spawn::create(fadeout, move_out, nullptr);
		auto remove_self = RemoveSelf::create(true);

		if (clockwise)
			Effects[BLADE_WIND_1ST]->sprEffect->runAction(Sequence::create(
				Spawn::create(animAction->reverse(), waving_out, nullptr), remove_self, nullptr));
		else
			Effects[BLADE_WIND_1ST]->sprEffect->runAction(Sequence::create(
				Spawn::create(animAction, waving_out, nullptr), remove_self, nullptr));
	}

}
void EffectManager::runBladeSlash(Point position, Vec2 direction, float size, Layer& target_canvas)
{
	if (Effects[BLADE_SLASH])
	{
		Effects[BLADE_SLASH]->sprEffect = Sprite::create();
		target_canvas.addChild(Effects[BLADE_SLASH]->sprEffect, 10);

		Effects[BLADE_SLASH]->sprEffect->setSpriteFrame(Effects[BLADE_SLASH]->frames.at(0));
		Effects[BLADE_SLASH]->sprEffect->setOpacity(245);
		Effects[BLADE_SLASH]->sprEffect->setRotation(-20.0f);
		Effects[BLADE_SLASH]->sprEffect->setScale(size*1.5f);
		Effects[BLADE_SLASH]->sprEffect->setPosition(position);

		auto fadeout = FadeOut::create(0.3f);
		auto scale_up = ScaleBy::create(0.4f, 1.7f);
		auto waving_out = Spawn::create(fadeout, scale_up, nullptr);
		auto remove_self = RemoveSelf::create(true);

		Effects[BLADE_SLASH]->sprEffect->runAction(Sequence::create(waving_out, remove_self));
	}
}


void EffectManager::runAttackCharge(float max_size, Sprite& target_canvas)
{
	if (Effects[ATTACK_CHARGE])
	{
		Effects[ATTACK_CHARGE]->sprEffect = Sprite::create();
		target_canvas.addChild(Effects[ATTACK_CHARGE]->sprEffect);

		auto animation = Animation::createWithSpriteFrames(Effects[ATTACK_CHARGE]->frames, FRAME_DELAY * 1.5f);
		auto animAction = Animate::create(animation);

		Effects[ATTACK_CHARGE]->sprEffect->setOpacity(230);
		Effects[ATTACK_CHARGE]->sprEffect->setScale(1.5f);
		Effects[ATTACK_CHARGE]->sprEffect->setPosition(Point(target_canvas.getContentSize().width / 2, target_canvas.getContentSize().height *0.7f));
		Effects[ATTACK_CHARGE]->sprEffect->runAction(RepeatForever::create(animAction));
		//Director::getInstance()->getScheduler()->schedule(schedule_selector(EffectManager::callBack_AttackCharge), this, 1 / 60.f, false);
	}
}
void EffectManager::stopAttackCharge()
{
	if (Effects[ATTACK_CHARGE]->sprEffect != NULL)
	{
		//Director::getInstance()->getScheduler()->unschedule(schedule_selector(EffectManager::callBack_AttackCharge), this);
		auto fade_out = FadeOut::create(0.5f);
		Effects[ATTACK_CHARGE]->sprEffect->runAction(Sequence::create(fade_out, RemoveSelf::create(true), nullptr));
		Effects[ATTACK_CHARGE]->sprEffect = NULL;

	}
};
void EffectManager::callBack_AttackCharge(float deltaTime)
{
	if (Effects[ATTACK_CHARGE]->sprEffect->getScale() < 4.0f)
		Effects[ATTACK_CHARGE]->sprEffect->setScale(Effects[ATTACK_CHARGE]->sprEffect->getScale() * 1.002f);
};



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



//
//void EffectManager::cleanParticle(ParticleSystem* particle)
//{
//	auto remove_act = Sequence::create(DelayTime::create(0.5f),
//		CallFuncN::create(CC_CALLBACK_1(EffectManager::resetParticle, this)),
//		nullptr);
//
//	particle->runAction(remove_act);
//}
//
//void EffectManager::resetParticle(Ref* sender)
//{
//	auto particle = (ParticleSystemQuad*)sender;
//	this->removeChild(particle);
//}
//







