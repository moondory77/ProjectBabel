#include "_T_BlockUnit.h"
#include "Obstacle/Container/_T_Build/_T_BuildContainer.h"
#include "System/GameManager.h"
#include "OnGame/JoyController.h"
#include "OnGame/Character.h"


BlockUnit::BlockUnit()
{
	isDefensibleFlag = true;
	attackID = 0;
	//defenseID = 0;
	specialID = 0;
	curWeight = 0.0f;
	//isAliveFlag = true;
	chunkID = 0;
}

//캐릭터와의 접촉면적 발생 시, 리턴
bool BlockUnit::isCrashed(Character& main_char) {

	return sprUnit->getBoundingBox().intersectsRect(main_char.bodyCollider->getBoundingBox());

	//auto unit_boundary = sprUnit->getBoundingBox();
	//bool isCrashedFlag = false;
	//if (unit_boundary.containsPoint(main_char.bodyCollider->getTopPostion()))
	//	isCrashedFlag = main_char.bodyCollider->isCrashed[0] = true;
	//if (unit_boundary.containsPoint(main_char.bodyCollider->getRightPosition()))
	//	isCrashedFlag = main_char.bodyCollider->isCrashed[1] = true;
	//if (unit_boundary.containsPoint(main_char.bodyCollider->getBottomPosition()))
	//	isCrashedFlag = main_char.bodyCollider->isCrashed[2] = true;
	//if (unit_boundary.containsPoint(main_char.bodyCollider->getLeftPosition()))
	//	isCrashedFlag = main_char.bodyCollider->isCrashed[3] = true;
	//return isCrashedFlag;
}


//deltaTime에 대한 position Update
void BlockUnit::updatePosition(float deltaTime)
{
	float weight_factor = getUnitChunk().size();
	curWeight = pow(weight_factor, 2) * 0.008f;

	if (curWeight < -5.0f) curWeight = -5.0f;
	if (curWeight > 5.0f) curWeight = 5.0f;

	//업데이트 전, position 버퍼링
	Point cur_pos = getPosition();
	Point updated_pos = cur_pos;

	//Building 전체의 가속력 + Block 개별의 반발력(Repulsion)
	auto blk_delta_velo = container->getVeloY() + curWeight;

	if (container->rigidFactor == 1.0f)
		updated_pos.y += blk_delta_velo * container->rigidFactor;

	//블록의 update 전/후 위치를 갱신
	CrashInfo.prevPos = cur_pos;
	setPosition(updated_pos);

	//충돌 발생 -> (Block - Character) X-Y 유효성 Filtering 후, Bounce Buffer에 저장
	if (isCrashed(*mainChar))
	{
		//(충돌 발생 시) unit의 x/y축 boundary 좌표값 저장
		CrashInfo.boundX = { getLeftX(), getRightX() };
		CrashInfo.boundY = { getBottomY(), getTopY() };
	
		//인덱스를 crash buffer에 등록
		container->bufferCrash[0].insert(unitIdx);
	}

	auto world_blk_pos = sprUnit->getParent()->convertToWorldSpace(getPosition());

	if (mainChar->weaponDetector->detectCollision(world_blk_pos, getWidth()/2))
	{
		////(충돌 발생 시) unit의 x/y축 boundary 좌표값 저장
		CrashInfo.boundX = { getLeftX(), getRightX() };
		CrashInfo.boundY = { getBottomY(), getTopY() };

		container->bufferCrash[1].insert(unitIdx);

		///-> world space로 변환 후,
		//auto unit_boundary = sprUnit->getBoundingBox();
		//unit_boundary.origin = sprUnit->getParent()->convertToWorldSpace(unit_boundary.origin);
		//CrashInfo.boundX = { unit_boundary.getMinX(), unit_boundary.getMaxX()};
		//CrashInfo.boundY = { unit_boundary.getMinY(), unit_boundary.getMaxY()};

		//침투벡터의 방향
		//Vec2 prev_relative_pos = CrashInfo.prevPos - mainChar->getPrevPos();
		//Vec2 cur_relative_pos = CrashInfo.curPos - mainChar->bodyCollider->getPosition();
		//Vec2 penetrate_direction = cur_relative_pos - prev_relative_pos;
		//mainChar->weaponDetector->getPenetrateVec(penetrate_direction, unit_center, unit_boundary.size.width / 2);
		//mainChar->bodyCollider->setPosition(mainChar->bodyCollider->getPosition() + mainChar->weaponDetector->getPenetrateVec(penetrate_direction, unit_center, unit_boundary.size.width / 2));

		if (actionCrash == NULL)
		{
			auto tint_in = TintTo::create(0.2f, Color3B::BLUE);
			auto tint_out = TintTo::create(0.15f, Color3B::WHITE);
			//auto detect_act = Sequence::create(tint_in, tint_out, nullptr);
			actionCrash = Sequence::create(tint_in, tint_out, nullptr);
			//detect_act->setTag(111);
			sprUnit->runAction(actionCrash);
		}
	}

}


void BlockUnit::updateState(float deltaTime)
{
	///-> world space로 변환 후,
	auto unit_boundary = sprUnit->getBoundingBox();
	unit_boundary.origin = sprUnit->getParent()->convertToWorldSpace(unit_boundary.origin);
	Vec2 unit_center = Vec2(unit_boundary.getMidX(), unit_boundary.getMidY());
	//Action* crash_act = NULL;



	//공격에 대한 update
	if ((curStrength > 0) && (mainChar->getAttackID() != attackID)
		&& mainChar->isAttack())
	{
		int damage = 0;

		///-> world space로 변환 후,
		auto unit_boundary = sprUnit->getBoundingBox();
		unit_boundary.origin = sprUnit->getParent()->convertToWorldSpace(unit_boundary.origin);
		Vec2 unit_center = Vec2(unit_boundary.getMidX(), unit_boundary.getMidY());


		if (damage = mainChar->chkAttackRadar(unit_boundary) > 0)
		{
			/*if (damage > mainChar->getPowerNormal() * 0.75f){
					container->ruinsPool.playParticleEffect(Point(unit_boundary.getMidX(), unit_boundary.getMidY()));
				}*/

			attackID = mainChar->getAttackID();

			//데미지가 Block의 현재 체력 이상일 경우
			if (curStrength < damage)
				damage = curStrength;

			curStrength -= damage;
			container->frameDamage += damage;

			//if (actionCrash != NULL && !actionCrash->isDone())
			//{
			sprUnit->stopAllActions();
			sprUnit->setColor(Color3B::RED);
			//}		
			auto tint_in = TintTo::create(0.15f, Color3B::RED);
			auto tint_out = TintTo::create(0.2f, Color3B::WHITE);
			actionCrash = Sequence::create(tint_in, tint_out, nullptr);
			sprUnit->runAction(actionCrash);

			if (curStrength <= 0)
			{
				isAliveFlag = false;
				container->setBrokenCnt(container->getBrokenCnt() + 1);
				container->bufferRemove.push_back(unitIdx);
			}
			else if (curStrength < maxStrength * 0.15f) {
				sprUnit->setSpriteFrame(sprUnitFrames.at(3));
			}
			else if (curStrength < maxStrength * 0.4f) {
				sprUnit->setSpriteFrame(sprUnitFrames.at(2));
			}
			else if (curStrength < maxStrength * 0.7f) {
				sprUnit->setSpriteFrame(sprUnitFrames.at(1));
			}
		}
	}


	/*else if (mainChar->isSpcAttack()
			&& mainChar->specialRadar->getBoundingBox().containsPoint(this->getPosition())) {
			updateForSpecial();
	}*/


	//방어 체크
	if (curStrength > 0
		&& mainChar->isDefense() && mainChar->isCrash()) {

		if ((sprUnit != NULL) && mainChar->chkDefenseRadar(*sprUnit) > 0) {
			updateForDefense();
		}
	}

	//지상추락 블록 파괴
	if (curStrength > 0
		&& sprUnit->getPositionY() <= winSize().height * 0.03f)
	{
		this->isAliveFlag = false;
		container->setBrokenCnt(container->getBrokenCnt() + 1);
		sprUnit->removeFromParentAndCleanup(true);
		sprUnit = NULL;
	}

};


vector<int>& BlockUnit::getUnitChunk()
{
	if (chunkID >= 0)
		return container->getBufferChunk().at(chunkID - 1);
	else
		return container->getBufferChunk().at(-(chunkID + 1));
}



//Dead 처리된(remove 버퍼의 유닛)을 실제로 제거
void BlockUnit::breakSelf()
{
	//Point breaking_pos = sprUnit->getParent()->convertToWorldSpace(getPosition());
	//container->ruinsPool.playParticleEffect(breaking_pos);
	sprUnit->removeFromParentAndCleanup(true);	//여기서 배치노드에서 삭제
	sprUnit = NULL;
};





void BlockUnit::updateForDefense() {

	if (defenseID != mainChar->getDefenseID())
	{
		//mainChar->setOuterVelocity(container->getDeltaVelocity());

		//컨테이너의 가속도를 직접 변경 (다음 프레임부터 적용)
		container->setVeloY(8.0f);
		defenseID = mainChar->getDefenseID();
		mainChar->setOffStateInput(DEFENSE, TOUCH);
	}
}


//범위 내에서 튕겨나가는지 여부 / 거리 반환
float BlockUnit::isDefensibleRange() {

	float radar_ht = mainChar->Radar.defense->getBoundingBox().size.height;
	float radar_wd = mainChar->Radar.defense->getBoundingBox().size.width / 2;
	float max_range = sqrt(radar_ht*radar_ht + radar_wd * radar_wd);

	Vec2 defensible_vector = getPosition() - mainChar->getPosition();
	float radian = ccpToAngle(defensible_vector);
	float distance = defensible_vector.getLength();

	float defense_factor = 1 - (distance*distance) / (max_range*max_range);
	//blk->veloWeight *= (distance*distance) / (max_range*max_range);

	if (0 <= radian && radian <= PI && defense_factor > 0)
		return max_range * defense_factor;
	else
		return 0;
}


void BlockUnit::updateForSpecial()
{
	float damage = 0;

	if (specialID != mainChar->getSpecialID() && (damage = isSpecialRange() > 0))
	{
		curStrength -= damage;
		specialID = mainChar->getSpecialID();

		//강도 0 밑으로 떨어지면 파괴 콜백
		if (curStrength <= 0)
		{
			//auto tmp_ruin = ParticleManager::getInstance()->getParticle(RUIN_1ST);
			auto delay = DelayTime::create(0.3f);

			/*	sprBlk->runAction(Sequence::create(delay, CallFunc::create([&]() {
					sprBlk->removeFromParentAndCleanup(true);
					sprBlk = NULL;
					this->isAliveBlk = false;
					container->setBrokenCnt(container->getBrokenCnt() + 1); }),
					nullptr));*/

					//if (tmp_ruin != NULL)
					//{
					//	tmp_ruin->setPosition(sprBlk->getBoundingBox().origin);
					//	tmp_ruin->resetSystem();

					//	auto delay = DelayTime::create(0.3f);
					//	sprBlk->runAction(Sequence::create(delay, CallFunc::create([&]() {
					//		sprBlk->removeFromParentAndCleanup(true);
					//		sprBlk = NULL;
					//		this->isAliveBlk = false;
					//		container->setBrokenCnt(container->getBrokenCnt() + 1); }),
					//		nullptr));
					//}
		}
		else if (curStrength < maxStrength * 0.15f)
		{
			//sprBlk->setSpriteFrame(sprFrames.at(0));
		}
		else if (curStrength < maxStrength * 0.4f)
		{
			//sprBlk->setSpriteFrame(sprFrames.at(1));
		}
		else if (curStrength < maxStrength * 0.7f)
		{
			//sprBlk->setSpriteFrame(sprFrames.at(2));
		}
	}
}

float BlockUnit::isSpecialRange()
{
	float limit_x = mainChar->Radar.special->getBoundingBox().getMaxX() - mainChar->Radar.special->getBoundingBox().getMidX();
	float dist_x = mainChar->Radar.special->getBoundingBox().getMidX() - getPositionX();

	float special_factor = 1.0f - (dist_x * dist_x) / (limit_x * limit_x);


	//special 계수가 일정 이상일 때만, 데미지 적용
	if (special_factor > 0.05f)
		return mainChar->getPowerSP() * special_factor;
	else
		return 0;
}


void BlockUnit::setPosition(Vec2 pos)
{
	sprUnit->setPosition(pos + Vec2(-getWidth() / 2, getHeight() / 2));
	CrashInfo.curPos = pos;
}
void BlockUnit::setPositionX(float newX)
{
	sprUnit->setPositionX(newX - getWidth() / 2);
	CrashInfo.curPos.x = newX;

}
void BlockUnit::setPositionY(float newY)
{
	sprUnit->setPositionY(newY + getHeight() / 2);
	CrashInfo.curPos.y = newY;
}


//아직 방문 전 블록인지 검사
bool BlockUnit::isVisitable()
{
	//양수 Phase에 음수 키, 혹은 그 반대일 경우 아직 방문 전을 알 수 있음
	if ((container->getChunkingPhase() && (chunkID <= 0))
		|| (!container->getChunkingPhase() && (chunkID >= 0)))
		return true;
	else
		return false;
}

bool BlockUnit::isSearchable(int chunk_id, const Point& pos)
{
	if (isAlive() && isVisitable()) {
		return (fabsf(pos.getDistance(getPosition())) < getWidth()*1.1f);
	}
	return false;
}



//
//
//void Block::updateForAttack() {
//
//	float damage = 0.0f;
//
//	if (attackID != mainChar->getAttackID()
//		&& (damage = isAttackableRange()) > 0)
//	{
//		//CCLOG("attack check Damage is %d", damage);
//		curStrength -= damage;
//		attackID = mainChar->getAttackID();
//
//		//공격 성공 시, y축으로 약간 상승
//		sprBlk->setPositionY(sprBlk->getPositionY() + 4.5f);
//
//		//강도 0 밑으로 떨어지면 파괴 콜백
//		if (curStrength <= 0)
//		{
//			//auto tmp_ruin = ParticleManager::getInstance()->getParticle(RUIN_1ST);
//			//if (tmp_ruin != NULL) {
//			//	tmp_ruin->setPosition(sprBlk->getBoundingBox().origin);
//			//	tmp_ruin->resetSystem();
//			//}
//
//			//삭제 버퍼에 넣음
//			container->bufferRemove.pushBack(this);
//
//			//파티클딜레이로 인해, 약간 딜레이 후 스프라이트 삭제
//			//auto delay = DelayTime::create(0.3f);
//			//this->sprBlk->runAction(Sequence::create(delay,
//			//CallFunc::create(CC_CALLBACK_0(Block::breakSelf, this)), nullptr));
//		}
//		else if (curStrength < maxStrength * 0.15f)
//		{
//			sprBlk->setSpriteFrame(sprFrames.at(1));
//		}
//		else if (curStrength < maxStrength * 0.4f)
//		{
//			sprBlk->setSpriteFrame(sprFrames.at(2));
//		}
//		else if (curStrength < maxStrength * 0.7f)
//		{
//			//CCLOG("this block is here %.2f", this->sprBlk->getPositionY());
//			sprBlk->setSpriteFrame(sprFrames.at(3));
//		}
//	}
//
//
//}
//
//


//
//
//float Block::isAttackableRange() {
//
//	if (sprBlk != NULL)
//	{
//		//공격의 중심점 (캐릭터 중심-명치부분...?)
//		Point attack_center = mainChar->sprChar->getPosition() + Vec2(0, -mainChar->getBodyHeight()*0.17f);
//		//공격점->블록중심 으로의 벡터
//		Vec2 attackable_vector = getPosition() - attack_center;
//
//		//CCLOG("character position is %.2f  %.2f", attack_center.x, attack_center.y);
//		//CCLOG("block position is %.2f  %.2f", getPosition().x, getPosition().y);
//		//CCLOG("normal damage is %d", mainChar->getNormalDamage());
//
//		float weapon_length = mainChar->getWpHeight()*0.9f;
//		float weapon_width = mainChar->getWpWidth()*0.5f;
//		float range_max = mainChar->getBodyWidth()*0.6f + sqrt(weapon_length*weapon_length + weapon_width*weapon_width);
//		float range_min = mainChar->getBodyWidth()*0.6f;
//
//		//radian test. 45 ~ 150
//		float radian = ccpToAngle(attackable_vector);
//		float distance = attackable_vector.getLength();
//		float attack_factor = 1 - (distance*distance*distance) / (range_max*range_max*range_max);
//		//float attack_factor = (distance*distance) / (range_max*range_max);
//
//
//		if (range_min < distance && distance < range_max
//			&&  PI / 4 < radian && radian < PI / 10 * 9)
//			//return mainChar->getNormalDamage() * attack_factor;
//			return mainChar->getNormalDamage();
//		else
//			return 0;
//		/*return range_min < distance && distance < range_max
//		&&  PI / 5 < radian && radian < PI / 6 * 5;*/
//
//	}
//	else
//		return 0;
//}
//


