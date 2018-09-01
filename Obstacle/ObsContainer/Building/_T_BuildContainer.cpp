#include "_T_BuildContainer.h"
#include "Manager/GameManager.h"
#include "Manager/EffectManager.h"
//#include "Obstacle/ObsTexBatcher.h"


BuildContainer::BuildContainer(BatchElement& batch_elem, Character& main_char, ParticlePool& ruins_pool)
	: ObsContainer(batch_elem, main_char, ruins_pool)
{
	rigidFactor = 1.0f;
	curVeloY = 0.0f;
	curChunkID = 0;
	chunkingFinishFlag = false;
	chunkingPhaseFlag = true;
	chunkingCnt = 0;
	frameDamage = 0;
	brokenCnt = 0;
};


void BuildContainer::onEnterTransitionDidFinish()
{
	if (!featuredFlag) {
		buildBlocks(initPos);
	}
	isAliveFlag = true;		//해당 시점부터 update
}




void BuildContainer::setMold(float scale_factor, int row_num, int col_num)
{
	scaleFactor = scale_factor;
	numRow = row_num;
	numCol = col_num;

	//Row * Col에 맞춰 Building Texture에 crop해야 할 크기 계산
	cropBlkSize.width = texBatcher.frameSize.width / numCol;
	cropBlkSize.height = texBatcher.frameSize.height / numRow;

	//유닛 Sprite 정사각형 scaling
	unitScaleX = scaleFactor * (winSize().width / texBatcher.frameSize.width);
	unitLength = scaleFactor * winSize().width / numCol;
}


void BuildContainer::setSpec(float delta_g, float max_dt, float min_dt, int unit_stren)
{
	curGravity = delta_g;
	maxVeloY = max_dt;
	minVeloY = min_dt;
	unitStrength = unit_stren;
};



void BuildContainer::initBlkFrames()
{
	blkArray = new BlockUnit[numRow * numCol];

	for (int i = 0; i < 4; i++)
	{
		//해당 강도의 건물프레임 기준좌표 (좌측 상단)
		auto unit_frame = texBatcher.frames.at(i);
		Point datum_pos = Point(unit_frame->getRect().getMinX(), unit_frame->getRect().getMinY());
	
		for (int j = 0; j != numRow * numCol; j++)
		{
			//정해진 unit size에 맞게 텍스쳐로부터 이미지를 crop
			//해당 좌표의 빌딩타일을 crop 해서 저장
			blkArray[j].sprUnitFrames.pushBack(
				SpriteFrame::createWithTexture(texBatcher.texture,	
					Rect(datum_pos.x + cropBlkSize.width * get2DIndex(j).second,
						datum_pos.y + cropBlkSize.height * get2DIndex(j).first,
						cropBlkSize.width, cropBlkSize.height)));
		}
	}

	bfsMainStack = new vector<int>();
	bfsSubStack = new vector<int>();
}

void BuildContainer::buildBlocks(Point& init_pos)
{
	for (int i = 0; i < numRow * numCol; i++)
	{
		int row_idx = get2DIndex(i).first;
		int col_idx = get2DIndex(i).second;

		blkArray[i].sprUnit = Sprite::create();	
		
		//각 블럭 스프라이트를 정사각형으로 강제 조절
		blkArray[i].sprUnit->setScaleX(unitScaleX);
		blkArray[i].sprUnit->setScaleY(unitScaleX * cropBlkSize.width / cropBlkSize.height);
		///width_scaler * (cropBlkWidth / cropBlkHeight)

		blkArray[i].sprUnit->setSpriteFrame(blkArray[i].sprUnitFrames.at(0));
		blkArray[i].sprUnit->setAnchorPoint(Point(0, 1));
		blkArray[i].sprUnit->setPosition(init_pos.x + unitLength * col_idx, init_pos.y - unitLength * row_idx);
	
		//Batch Element의 batch_node 아래로 unit sprite 삽입
		texBatcher.batchNode->addChild(blkArray[i].sprUnit);	///해당 시점부터 렌더링

		blkArray[i].isDefensibleFlag = true;
		blkArray[i].maxStrength = this->unitStrength;
		blkArray[i].curStrength = this->unitStrength;

		blkArray[i].unitIdx = i;
		blkArray[i].mainChar = &mainChar;
		blkArray[i].container = this;


		//상, 하, 좌, 우 Block과 연결 (기본 Chunk 설정)
		static int row_num[] = { 1, 0, -1, 0 };
		static int col_num[] = { 0, 1, 0, -1 };
		for (int j = 0; j < 4; j++)
		{
			int row_joint = row_idx + row_num[j];
			int col_joint = col_idx + col_num[j];

			//CCLOG("joint %d [%d][%d]", i, row_joint, col_joint);
			if (0 <= row_joint && row_joint < numRow
				&& 0 <= col_joint && col_joint < numCol)
			{
				blkArray[i].linkedUnit.insert(&blkArray[get1DIndex(row_joint, col_joint)]);
			}
		}
	}
	featuredFlag = true;
}



void BuildContainer::dumpRemoveBuffer()
{
	for (auto iter = bufferRemove.begin(); iter != bufferRemove.end(); iter++)
	{
		blkArray[*iter].breakSelf();
		blkArray[*iter].linkedUnit.clear();
	}
	bufferRemove.clear();
}





void BuildContainer::dumpCrashBuffer()
{
	mainChar.setColliderPosition(mainChar.getColliderPosition() + getCrashBounceVec());

	if (!bufferCrashX.empty()) bufferCrashX.clear();
	if (!bufferCrashY.empty()) bufferCrashY.clear();
}



Vec2 BuildContainer::getCrashBounceVec()
{
	float bounce_x = 0.0f;
	float bounce_y = 0.0f;

	//CCLOG("now crash buffer size is %d", bufferCrashed.size());
	for (auto iter = bufferCrashX.begin(); iter != bufferCrashX.end(); iter++)
	{
		//더 변화량이 큰 x값 추출
		if (fabsf(bounce_x) < fabsf(blkArray[*iter].getCrashVec().x))
			bounce_x = blkArray[*iter].getCrashVec().x;
	}

	for (auto iter = bufferCrashY.begin(); iter != bufferCrashY.end(); iter++)
	{
		//더 변화량이 큰 y값 추출
		if (fabsf(bounce_y) < fabsf(blkArray[*iter].getCrashVec().y))
			bounce_y = blkArray[*iter].getCrashVec().y;
	}
	CCLOG("Bounce Vector (%.2f, %.2f)", bounce_x, bounce_y);
	return Vec2(bounce_x, bounce_y);
}



int BuildContainer::getNewChunkID()
{
	//chunk ID를 새로 부여함과 동시에, Block 포인터를 저장할 버퍼 생성
	if (chunkingPhaseFlag) {
		///+(chunkID - 1) -> 버퍼 내 인덱스
		curChunkID++;
		bufferChunkPositive.push_back({});
	}
	else {
		///-(chunkID + 1) => 버퍼내 index
		curChunkID--;
		bufferChunkNegative.push_back({});
	}
	//CCLOG("new Chunk ID [%d] is generated!", curChunkID);
	return curChunkID;
}



void BuildContainer::resetChunkingPhase()
{
	curChunkID = 0;
	chunkingCnt = 0;
	chunkingFinishFlag = false;
	chunkingPhaseFlag = !chunkingPhaseFlag;

	if (chunkingPhaseFlag)
		bufferChunkPositive.clear();
	else
		bufferChunkNegative.clear();

	return;
}




void BuildContainer::BFS(int chunk_id, int caller_idx)
{
	if (!bfsMainStack->empty())
		bfsMainStack->clear();

	bfsMainStack->push_back(caller_idx);

	while (!bfsMainStack->empty() && !getChunkingFinish()) {
		bfsMainStack = &breadthSearch(chunk_id, *bfsMainStack);
	}
	return;
}



vector<int>& BuildContainer::breadthSearch(int chunk_id, vector<int>& main_stack)
{
	while (!main_stack.empty())
	{
		BlockUnit& cursor = blkArray[main_stack.back()];

		//아직 방문안한 노드인지 검사
		if (cursor.isAlive() && cursor.isVisitable())
		{
			cursor.setChunkID(chunk_id);
			chunkingCnt++;

			if (getChunkingPhase())
				bufferChunkPositive.at(chunk_id - 1).push_back(cursor.unitIdx);
			else
				bufferChunkNegative.at(-(chunk_id + 1)).push_back(cursor.unitIdx);

			//묶음분류(chunking)에 성공 할 때마다, 카운트 업. 모든 분류 시 loop 탈출
			if (chunkingCnt >= getAliveUnitCnt()) {
				chunkingFinishFlag = true;
				break;
			}

			for (auto iter = cursor.linkedUnit.begin(); iter != cursor.linkedUnit.end(); iter++){
				if ((*iter)->isSearchable(chunk_id, cursor.getPosition()))
					bfsSubStack->push_back((*iter)->unitIdx);
			}
		}
		main_stack.pop_back();
	}

	if (!main_stack.empty())
		main_stack.clear();
	vector<int>& next_main_stack = *bfsSubStack;
	bfsSubStack = &main_stack;

	return next_main_stack;
}





void BuildContainer::chunkBlocks()
{
	chunkingFinishFlag = false;
	//auto iter = blkArray.begin();

	int i = 0;
	//Alive 상태의 모든 Block을 순회 update
	do {
		if (blkArray[i].isAlive() && blkArray[i].isVisitable())
			BFS(getNewChunkID(), i);
		i++;
	} while (!chunkingFinishFlag);


	//CCLOG("=======================================================================");
	//CCLOG("Chunking count is %d", chunkingCnt);
	//int j = 0;
	//for (auto iter = getBufferChunk().begin(); iter != getBufferChunk().end(); iter++, j++)
	//{
	//	CCLOG("Chunk [%d]'s size %d, repulsion is %f", j, (*iter).size(), (*iter).size() * 0.005f);
	//}
	//CCLOG("=======================================================================");

}



void BuildContainer::getRigidTime(float frame_damage)
{
	float rigid_time = frame_damage*0.001f;

	if (rigid_time > 0.16f)
		rigid_time = 0.16f;

	if (rigid_time > 0.09f)
	{
		rigidFactor = 0.0f;

		auto delay = DelayTime::create(rigid_time);
		auto rigid_in_attack = Sequence::create(delay,
			CallFunc::create([&]() {
			CCLOG("rigidity finish!");
			rigidFactor = 1.0f;
		}), nullptr);

		//this->runAction(rigid_in_attack);
	}
}




//void Building::DFS(int chunk_id, int linked_idx)
//{
//	//상, 하, 좌, 우 Block과 연결 (기본 Chunk 설정을 위해)
//	static int row_num[] = { -1, 0, 0, 1 };
//	static int col_num[] = { 0, -1, 1, 0 };
//
//	//int linked_idx = numCol * row + col;
//	if (chunkingCnt < getAliveUnitCnt())
//	{
//
//		chunkingCnt++;
//		blkArray[linked_idx].chunkID = chunk_id;
//
//
//		//phase 분류하여 chunk 묶기
//		if (getChunkingPhase()) {
//
//			bufferChunkPositive.at(chunk_id - 1).pushBack(&blkArray[linked_idx]);
//			blkArray[linked_idx].ptrChunkPositive = &bufferChunkPositive.at(chunk_id - 1);
//		}
//		else {
//			bufferChunkNegative.at(-(chunk_id + 1)).pushBack(&blkArray[linked_idx]);
//			blkArray[linked_idx].ptrChunkNegative = &bufferChunkNegative.at(-(chunk_id + 1));
//		}
//	}
//	else {
//		//CCLOG("DFS Finished 11111111 now chunk count %d", chunkingCnt);
//		//CCLOG("Exposion!!!");
//
//		chunkingFinishFlag = true;
//		return;
//	}
//
//
//
//
//
//	for (int i = 0; i < 4; i++)
//	{
//		int row_joint = (linked_idx / numCol) + row_num[i];
//		int col_joint = (linked_idx % numCol) + col_num[i];
//
//		if (0 <= row_joint && row_joint < numRow
//			&& 0 <= col_joint && col_joint < numCol)
//		{
//			if (blkArray[numCol * row_joint + col_joint].isVisitable(chunk_id))
//			{
//				//if ((blkArray[numCol * row_joint + col_joint].sprUnit != NULL)
//					//&& blkArray[numCol * row_joint + col_joint].isSprConnected(blkArray[linked_idx].getPosition()))
//				return DFS(chunk_id, numCol * row_joint + col_joint);
//			}
//		}
//	}
//
//
//	if (chunkingCnt >= getAliveUnitCnt())
//	{
//		CCLOG("DFS Finished!2222222222 now chunk count %d", chunkingCnt);
//		chunkingFinishFlag = true;
//		return;
//	}
//
//
//};
//
//

