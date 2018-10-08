#include "DefaultBuilding.h";


DefaultBuilding::DefaultBuilding(ObsBatcher& obs_batcher, Character& main_char, ParticlePool& ruins_pool)
	: BuildContainer(obs_batcher, main_char, ruins_pool)
{
	prevSpecialID = 0;
	prevSpecialDamage = 0;
};

DefaultBuilding::DefaultBuilding(const BuildContainer& bld)
	: BuildContainer(bld.obsBatcher, bld.mainChar, bld.ruinsPool)
{
	prevSpecialID = 0;
	prevSpecialDamage = 0;
}


void DefaultBuilding::update(float deltaTime)
{
	frameDamage = 0;

	//청크 재분류 필요시, 실행
	if (!chunkingFinishFlag)
		chunkBlocks();

	//업데이트 전, 충돌 input 다시 초기화
	mainChar.setOffStateInput(CRASH, 0);
	//빌딩의 velocity를 업데이트
	updateVelocity(deltaTime);

	for (int i = 0; i < numRow*numCol; i++)
	{
		if (blkArray[i].isAlive())
		{
			blkArray[i].updatePosition(deltaTime);
			blkArray[i].updateState(deltaTime);
		}
	}

	//각 bouce을 계산하여, Collider Position Optimizing
	if (!bufferCrashX.empty() || !bufferCrashY.empty()) {
		dumpCrashBuffer();
	}

	//if (!bufferCrashDefense.empty())
	//{
	//	for (auto iter = bufferCrashDefense.begin(); iter != bufferCrashDefense.end(); iter++)
	//	{
	//		//CCLOG(" was back bounced" );
	//		//캐릭터 바운스와 반대방향으로..
	//		auto bounce_back = (*iter)->getPosition() - (*iter)->getCrashVec();
	//		(*iter)->setPosition(bounce_back);
	//	}
	//	bufferCrashDefense.clear();
	//}

	if (!bufferRemove.empty()) {
		dumpRemoveBuffer();
		//새로운 Chunk 분류 -> positive-negative Phase 갱신
		resetChunkingPhase();
	}

	if (brokenCnt >= numRow * numCol)
	{
		//delete[] blkArray;
		isAliveFlag = false;
		return;
	}


	if (frameDamage > 0)
	{
		//유효데미지에 의한 경직효과
		if (rigidFactor > 0.0f)
		{

			float rigid_time = frameDamage * 0.0013f;
			if (rigid_time > 0.16f)
				rigid_time = 0.16f;

			if (rigid_time > 0.01f)
			{
				//CCLOG("enter the rigid time");
				this->rigidFactor = 0.0f;
				auto delay = DelayTime::create(rigid_time);
				auto rigid_in_attack = Sequence::create(delay, CallFunc::create([&]() { this->rigidFactor = 1.0f; }), nullptr);
				this->runAction(rigid_in_attack);
			}
		}
		mainChar.tickDamageHit += frameDamage;
	}
};


void DefaultBuilding::updateVelocity(float deltaTime)
{
	float updated_delta_velo = 0.0f;
	///# curDeltaVelo도 dt 곱해줘야 하지만, frame 떨림현상 발생으로, 해당 계산으로 근사하기로
	if (curVeloY > 0)
		updated_delta_velo = curVeloY - curGravity * curGravity * 0.3f * deltaTime;
	else
		updated_delta_velo = curVeloY - curGravity * curGravity * 0.5f * deltaTime;

	if (updated_delta_velo < minVeloY)
		updated_delta_velo = minVeloY;

	//deltaVelo 값 갱신
	curVeloY = updated_delta_velo * rigidFactor;
};



BuildContainer& DefaultBuilding::spawnChild(BuildContainer& mate)
{
	//texture, row-col은 mate. 나머지 spec은 자신으로 새로운 개체 생성
	auto clone = new DefaultBuilding(mate.obsBatcher, mainChar, ruinsPool);
	clone->setMold(mate.getScaleFactor(), mate.getNumRow(), mate.getNumCol());
	clone->setSpec(curGravity, maxVeloY, minVeloY, unitStrength);
	return *clone;
};
