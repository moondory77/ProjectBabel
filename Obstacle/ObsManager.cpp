#include "ObsManager.h"
#include "Obstacle/Container/Building/DefaultBuilding.h"
#include "System/GameManager.h"

bool ObsManager::isExist = false;

//OnGame ���� ���� ���� ����, ������ ����
void ObsManager::onEnter()
{
	CCNode::onEnter();
	Director::getInstance()->getScheduler()->schedule(std::bind(&ObsManager::ObsScheduler, this, std::placeholders::_1), this, 4.0f, false, "ObsMgr");
	CCLOG("Obstacle Manager schedule start!");
	return;
}


void ObsManager::setProtoType(int target_line, BuildContainer& proto_obj)
{
	//proto line�� �� ù �ε����� ����
	if (SpawningPool[target_line].getCurSize() == 0)
		SpawningPool[target_line].pushBack(&proto_obj);
}


BuildContainer& ObsManager::getProtoType(int target_line)
{
	return *SpawningPool[target_line].getFront();
}



void ObsManager::generateMutant(BuildContainer& mother, int target_line)
{
	if (SpawningPool[target_line].isEmpty())
	{
		do {
			target_line = (target_line + 1) % ProtoNum;
		} while (SpawningPool[target_line].isEmpty());
	}

	SpawningPool[target_line].pushBack(&mother.spawnChild(getProtoType(target_line)));
};



void ObsManager::ObsScheduler(float deltaTime)
{
	//�ð� ����� ����, ���� ��ü�鿡 ����ġ �ο�
	for (int i = 0; i < ProtoNum; i++){
		SpawningPool[i].updateWeights(deltaTime);
	}

	//pool ���� �ν��Ͻ� stand by
	while (!StandByEntry->isFull() && !isPoolEmpty())
	{
		int target_pool = RandomHelper::random_int(0, ProtoNum - 1);
		
		if (SpawningPool[target_pool].isEmpty()){
			do {
				target_pool = (target_pool + 1) % ProtoNum;
			} while (SpawningPool[target_pool].isEmpty());
		}

		BuildContainer* cursor = SpawningPool[target_pool].popFront();
		cursor->initBlkFrames();
		StandByEntry->pushBack(cursor);
	}


	if (ActiveList.size() < 3)
	{
		auto tmp = StandByEntry->popFront();	
		tmp->buildBlocks(Point(winSize().width * 2.0f / RandomHelper::random_int(2, 10), winSize().height * RandomHelper::random_real(2.8f, 3.3f)));
		ActiveList.push_back(tmp);
		this->addChild(tmp);
	}


	//��� ��⿭ ������ ���� �Ǹ�, stage ����
	if (StandByEntry->isEmpty())
	{
		Director::getInstance()->getScheduler()->unschedule("ObsMgr", this);
		CCLOG("================Stage Finished====================");
		
		this->removeFromParent();
		this->release();
		return;
		///�������� ����
	}
}

