

//
//
////id를 통해 (아직 방문 안한) + (연결 된) 모든 block을 순회
//void ObsUnit::DFS(int chunk_id)
//{
//	//CCLOG("Block[%d][%d] calls chunk ID %d", rowIdx, colIdx, chunk_id);
//	int cur_chunk_cnt = container->getChunkingCnt();
//
//
//	//호출한 uinit을 라벨링 한다
//	if (cur_chunk_cnt < container->getAliveUnitCnt())
//	{
//		
//		container->setChunkingCnt(++cur_chunk_cnt);
//		chunkID = chunk_id;
//		
//		
//		if (container->getChunkingPhase()) {
//			container->bufferChunkPositive.at(chunk_id - 1).pushBack(this);
//			ptrChunk = &container->bufferChunkPositive.at(chunk_id - 1);
//		}
//		else {
//			container->bufferChunkNegative.at(-(chunk_id + 1)).pushBack(this);
//			ptrChunk = &container->bufferChunkNegative.at(-(chunk_id + 1));
//		}
//
//	}
//	else 
//	{
//
//		CCLOG("chunking finished!");
//
//		//라벨링 완료시, 플래그 갱신하고 호출스택 빠져나옴
//		container->setChunkingFinish(true);
//		return;
//
//	}
//
//
//	if (!linkedUnit.empty())
//	{
//		//연결되어 있는 모든 블록을 DFS 탐색하여 갱신
//		for (auto iter = linkedUnit.begin(); iter != linkedUnit.end(); iter++)
//		{
//			if ((*iter)->isVisitable(chunk_id))
//			/*	&& (*iter)->isSprConnected(*this))*/
//			{
//				//CCLOG("[%d][%d] called [%d][%d]", rowIdx, colIdx, (*iter)->rowIdx, (*iter)->colIdx);
//				return (*iter)->DFS(chunk_id);
//			}
//			//else
//			//{
//			//	//연결이 끊어진 unit 발생시, 해당 포인터를 linkedUnit 셋에서 지워주고, 
//			//	//chunkFinishFlag false로 바꿔줌
//			//	(*iter)->linkedUnit.erase(this);
//			//	linkedUnit.erase(*iter);
//			//	container->setChunkingFinish(false);
//			//}
//		}
//
//	}
//
//
//	//빠져나오기 전, 한번더 chunking의 완료여부 체크
//	if (container->getChunkingCnt() >= container->getAliveUnitCnt())
//	{
//		CCLOG("chunking finished!");
//
//		//CCLOG("[%d][%d] finished all chunking", rowIdx, colIdx);
//		container->setChunkingFinish(true);
//	}
//	return;
//}
//
//
//






