#include "_T_BlockUnit.h"
#include "Manager/GameManager.h"
#include "OnGame/JoyController.h"
#include "Obstacle/ObsContainer/Building/_T_BuildContainer.h"


BlockUnit::BlockUnit()
{
	isDefensibleFlag = true;
	attackID = 0;
	//defenseID = 0;
	specialID = 0;
	curRepulsion = 0.0f;
	isAliveFlag = true;
	chunkID = 0;
}


//deltaTime�� ���� position Update
void BlockUnit::positionUpdate(float deltaTime)
{
	float repulsion_factor = getUnitChunk().size();
	curRepulsion = repulsion_factor * repulsion_factor  * 0.008f;

	if (curRepulsion < -5.0f)
		curRepulsion = -5.0f;
	if (curRepulsion > 5.0f)
		curRepulsion = 5.0f;

	//������Ʈ ��, position ���۸�
	Point cur_pos = getPosition();
	Point updated_pos = cur_pos;

	//Building ��ü�� ���ӷ� + Block ������ �ݹ߷�(Repulsion)
	auto blk_delta_velo = container->getVeloY() + curRepulsion;

	if (container->rigidFactor == 1.0f)
		updated_pos.y += blk_delta_velo * container->rigidFactor;

	//position �ϰ� ������Ʈ 
	prevPos = cur_pos;
	setPosition(updated_pos);
	//CCLOG("curpos: %.2f, updatedPos : %.2f", cur_pos.y, updated_pos.y);

	//�浹 �߻� -> (Block - Character) X-Y ��ȿ�� ������ ����, Bounce Buffer�� ����
	if (isCrashed(*mainChar))
	{
		//crash ���� (Block <- ĳ���� �ν��Ͻ�) ��� ����
		prevRelativePos = this->prevPos - mainChar->getPrevPos();
		//crash ���� (Block <- ĳ���� collider) ��� ����
		curRelativePos = updated_pos - mainChar->getColliderPosition();

		//Delta ��� ���� (��������Ʈ ��ģ��ŭ�� ����)
		crashVec = curRelativePos - prevRelativePos;		
		///�浹 ������ Block�� ĳ������ delta vector ���� -> � �浹(x��, y��)�� �� ���͸�
		crashAngle = CC_RADIANS_TO_DEGREES(ccpToAngle(mainChar->getPrevPos() - prevPos));

	
		//x�� ��ȿ bounce
		if ((120.0f < crashAngle && crashAngle <= 180.0f)
			|| (-180.0f <= crashAngle && crashAngle < -120.0f)
			|| (-60.0f < crashAngle && crashAngle < 60.0f))
		{
			container->bufferCrashX.push_back(unitIdx);
		}

		//y�� ��ȿ�� bounce
		if ((60.0f < crashAngle && crashAngle < 120.0f)
			|| (-120.0f < crashAngle && crashAngle < -60.0f))
		{
			//CCLOG("[%d] bloock crashVector is [%.2f, %.2f]", unitIdx, crashVec.x, crashVec.y);
			mainChar->addState(CRASH);
			mainChar->setOuterVeloY(blk_delta_velo);
			container->bufferCrashY.push_back(unitIdx);
		}
	}
	else
	{
		crashVec = Vec2::ZERO;
		crashAngle = 0.0f;
	}

}
void BlockUnit::stateUpdate(float deltaTime)
{
	//���ݿ� ���� update
	if ((curStrength > 0)
		&& (mainChar->getAttackID() != attackID)
		&& mainChar->isAttack())
	{
		int damage = 0;

		if (damage = mainChar->chkAttckRadar(*sprUnit))
		{
			//CCLOG("[%d][%d] was damaged by %d", rowIdx, colIdx, damage);
			attackID = mainChar->getAttackID();

			//�������� Block�� ���� ü�� �̻��� ���
			if (curStrength < damage)
				damage = curStrength;

			curStrength -= damage;
			container->frameDamage += damage;
			//container->setDeltaVelocity(container->getDeltaVelocity() + 0.07f);

			auto tint_in = TintTo::create(0.1f, Color3B::RED);
			auto tint_out = TintTo::create(0.15f, Color3B::WHITE);
			sprUnit->runAction(Sequence::create(tint_in, tint_out, nullptr));

			if (curStrength <= 0)
			{
				isAliveFlag = false;
				container->setBrokenCnt(container->getBrokenCnt() + 1);

				////////////////////��ǥ�� ���� �ʿ�//////////////////
				//breakPos.x = sprUnit->getParent()->convertToWorldSpace(getPosition()).x;
				//breakPos.y = getPosition().y;
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



	//��� üũ
	if (curStrength > 0
		&& mainChar->isDefense() && mainChar->isCrash()) {

		if ((sprUnit != NULL) && mainChar->chkDefenseRadar(*sprUnit) > 0) {
			updateForDefense();
		}
	}

	//�����߶� ��� �ı�
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




//Dead ó����(remove ������ ����)�� ������ ����
void BlockUnit::breakSelf()
{
	///�� ��ġ�� �̷��� �����ؾ� ����????? �Ф�
	Point breaking_pos = Point(sprUnit->getParent()->convertToWorldSpace(getPosition()).x, getPositionY());
	container->ruinsPool.playParticleEffect(breaking_pos);

	sprUnit->removeFromParentAndCleanup(true);	//���⼭ ��ġ��忡�� ����
	sprUnit = NULL;
};


bool BlockUnit::isCrashed(Character& mainChar) {
	return mainChar.colliderChar->getBoundingBox().intersectsCircle(getPosition(), getHeight() / 2);
}


void BlockUnit::updateForDefense() {

	if (defenseID != mainChar->getDefenseID())
	{
		//mainChar->setOuterVelocity(container->getDeltaVelocity());
		//�����̳��� ���ӵ��� ���� ���� (���� �����Ӻ��� ����)
		container->setVeloY(8.0f);

		//container->bufferCrashDefense.pushBack(this);
		defenseID = mainChar->getDefenseID();
		mainChar->setOffStateInput(DEFENSE, TOUCH);
		//CCLOG("Defense really effected!!");
	}
}

//���� ������ ƨ�ܳ������� ���� / �Ÿ� ��ȯ
float BlockUnit::isDefensibleRange() {

	float radar_ht = mainChar->shieldRadar->getBoundingBox().size.height;
	float radar_wd = mainChar->shieldRadar->getBoundingBox().size.width / 2;
	float max_range = sqrt(radar_ht*radar_ht + radar_wd*radar_wd);

	Vec2 defensible_vector = getPosition() - mainChar->sprChar->getPosition();
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

		//���� 0 ������ �������� �ı� �ݹ�
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
	float limit_x = mainChar->specialRadar->getBoundingBox().getMaxX() - mainChar->specialRadar->getBoundingBox().getMidX();
	float dist_x = mainChar->specialRadar->getBoundingBox().getMidX() - getPositionX();

	float special_factor = 1.0f - (dist_x * dist_x) / (limit_x * limit_x);


	//special ����� ���� �̻��� ����, ������ ����
	if (special_factor > 0.05f)
		return mainChar->getPowerSP() * special_factor;
	else
		return 0;
}


void BlockUnit::setPosition(Point pos)
{
	sprUnit->setPosition(pos + Vec2(-getWidth() / 2, getHeight() / 2));
}
void BlockUnit::setPositionX(float newX)
{
	sprUnit->setPositionX(newX - getWidth() / 2);
}
void BlockUnit::setPositionY(float newY)
{
	sprUnit->setPositionY(newY + getHeight() / 2);
}


//���� �湮 �� ������� �˻�
bool BlockUnit::isVisitable()
{
	//��� Phase�� ���� Ű, Ȥ�� �� �ݴ��� ��� ���� �湮 ���� �� �� ����
	if ((container->getChunkingPhase() && (chunkID <= 0))
		|| (!container->getChunkingPhase() && (chunkID >= 0)))
		return true;
	else
		return false;
}



bool BlockUnit::isSearchable(int chunk_id, const Point& pos)
{
	if (isAlive() && isVisitable()){
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
//		//���� ���� ��, y������ �ణ ���
//		sprBlk->setPositionY(sprBlk->getPositionY() + 4.5f);
//
//		//���� 0 ������ �������� �ı� �ݹ�
//		if (curStrength <= 0)
//		{
//			//auto tmp_ruin = ParticleManager::getInstance()->getParticle(RUIN_1ST);
//			//if (tmp_ruin != NULL) {
//			//	tmp_ruin->setPosition(sprBlk->getBoundingBox().origin);
//			//	tmp_ruin->resetSystem();
//			//}
//
//			//���� ���ۿ� ����
//			container->bufferRemove.pushBack(this);
//
//			//��ƼŬ�����̷� ����, �ణ ������ �� ��������Ʈ ����
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
//		//������ �߽��� (ĳ���� �߽�-��ġ�κ�...?)
//		Point attack_center = mainChar->sprChar->getPosition() + Vec2(0, -mainChar->getBodyHeight()*0.17f);
//		//������->����߽� ������ ����
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


