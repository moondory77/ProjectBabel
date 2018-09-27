#include "DefaultBuilding.h";


DefaultBuilding::DefaultBuilding(const ObsBatchUnit& batch_elem, Character& main_char, ParticlePool& ruins_pool)
	: BuildContainer(batch_elem, main_char, ruins_pool)
{
	prevSpecialID = 0;
	prevSpecialDamage = 0;
};

DefaultBuilding::DefaultBuilding(const BuildContainer& bld)
	: BuildContainer(bld.texBatchUnit, bld.mainChar, bld.ruinsPool)
{
	prevSpecialID = 0;
	prevSpecialDamage = 0;
}


void DefaultBuilding::update(float deltaTime)
{
	frameDamage = 0;
	
	//ûũ ��з� �ʿ��, ����
	if (!chunkingFinishFlag)
		chunkBlocks();

	//������Ʈ ��, �浹 input �ٽ� �ʱ�ȭ
	mainChar.setOffStateInput(CRASH, 0);
	//������ velocity�� ������Ʈ
	updateVelocity(deltaTime);

	for (int i = 0; i < numRow*numCol; i++)
	{
		if (blkArray[i].isAlive())
		{
			blkArray[i].positionUpdate(deltaTime);
			blkArray[i].stateUpdate(deltaTime);
		}
	}

	//�� bouce�� ����Ͽ�, Collider Position Optimizing
	if (!bufferCrashX.empty() || !bufferCrashY.empty()) {
		dumpCrashBuffer();
	}

	//if (!bufferCrashDefense.empty())
	//{
	//	for (auto iter = bufferCrashDefense.begin(); iter != bufferCrashDefense.end(); iter++)
	//	{
	//		//CCLOG(" was back bounced" );
	//		//ĳ���� �ٿ�� �ݴ��������..
	//		auto bounce_back = (*iter)->getPosition() - (*iter)->getCrashVec();
	//		(*iter)->setPosition(bounce_back);
	//	}
	//	bufferCrashDefense.clear();
	//}

	if (!bufferRemove.empty()) {
		dumpRemoveBuffer();
		//���ο� Chunk �з� -> positive-negative Phase ����
		resetChunkingPhase();
	}

	if (brokenCnt >= numRow * numCol)
	{
		//delete[] blkArray;
		isAliveFlag = false;
		return;
	}


	//��ȿ�������� ���� ����ȿ��
	if ((frameDamage > 0) && (rigidFactor > 0.0f)) 
	{
		float rigid_time = frameDamage*0.001f;
		if (rigid_time > 0.16f)
			rigid_time = 0.16f;

		if (rigid_time > 0.09f)
		{
			CCLOG("start rigid");
			rigidFactor = 0.0f;

			auto delay = DelayTime::create(rigid_time);
			auto rigid_in_attack
				= Sequence::create(delay, 
					CallFunc::create([&](){ rigidFactor = 1.0f; }), nullptr);
			this->runAction(rigid_in_attack);
		}
		/*if ((frameDamage > 100)
			&& (mainChar.getWpScale() < 0.4f))
		{
			mainChar.setWpScale(mainChar.getWpScale() * 1.3f);
		}*/
	}

	mainChar.lapsedAtkScore += frameDamage;
};


void DefaultBuilding::updateVelocity(float deltaTime)
{
	float updated_delta_velo = 0.0f;
	///# curDeltaVelo�� dt ������� ������, frame �������� �߻�����, �ش� ������� �ٻ��ϱ��
	if (curVeloY > 0)
		updated_delta_velo = curVeloY - curGravity * curGravity * 0.3f * deltaTime;
	else
		updated_delta_velo = curVeloY - curGravity * curGravity * 0.5f * deltaTime;

	if (updated_delta_velo < minVeloY)
		updated_delta_velo = minVeloY;

	//deltaVelo �� ����
	curVeloY = updated_delta_velo * rigidFactor;
};



BuildContainer& DefaultBuilding::spawnChild(BuildContainer& mate)
{
	auto clone = new DefaultBuilding(mate.texBatchUnit, mainChar, ruinsPool);
	clone->setMold(mate.getScaleFactor(), mate.getNumRow(), mate.getNumCol());
	clone->setSpec(curGravity, maxVeloY, minVeloY, unitStrength);
	return *clone;
};
