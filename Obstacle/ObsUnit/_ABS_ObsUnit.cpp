

//
//
////id�� ���� (���� �湮 ����) + (���� ��) ��� block�� ��ȸ
//void ObsUnit::DFS(int chunk_id)
//{
//	//CCLOG("Block[%d][%d] calls chunk ID %d", rowIdx, colIdx, chunk_id);
//	int cur_chunk_cnt = container->getChunkingCnt();
//
//
//	//ȣ���� uinit�� �󺧸� �Ѵ�
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
//		//�󺧸� �Ϸ��, �÷��� �����ϰ� ȣ�⽺�� ��������
//		container->setChunkingFinish(true);
//		return;
//
//	}
//
//
//	if (!linkedUnit.empty())
//	{
//		//����Ǿ� �ִ� ��� ����� DFS Ž���Ͽ� ����
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
//			//	//������ ������ unit �߻���, �ش� �����͸� linkedUnit �¿��� �����ְ�, 
//			//	//chunkFinishFlag false�� �ٲ���
//			//	(*iter)->linkedUnit.erase(this);
//			//	linkedUnit.erase(*iter);
//			//	container->setChunkingFinish(false);
//			//}
//		}
//
//	}
//
//
//	//���������� ��, �ѹ��� chunking�� �ϷῩ�� üũ
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






