#include "ObsManager.h"
#include "Obstacle/Container/_T_Build/DefaultBuilding.h"
#include "System/GameManager.h"

bool ObsManager::isExist = false;

ObsManager::ObsManager(int line_num, vector<BuildContainer*>& active_list) : LineNum(line_num), ActiveList(active_list)
{
	assert(!isExist);
	texPool = new TextureBatcher();
	SpawningPool = new EntryQueue<BuildContainer*>[LineNum];
	StandByEntry = new EntryQueue<BuildContainer*>(3);
	stageFinished = false;
	isExist = true;
}

ObsManager::~ObsManager() {
	delete(StandByEntry);
	delete[] SpawningPool;
	texPool->release();
	CCLOG("ObsManager deleted!");
	isExist = false;
};


//OnGame ���� ���� ���� ����, ������ ����
void ObsManager::onEnter()
{
	CCNode::onEnter();
	Director::getInstance()->getScheduler()->schedule(std::bind(&ObsManager::ObsScheduler, this, std::placeholders::_1), this, 4.0f, false, "ObsMgr");
	CCLOG("Obstacle Manager schedule start!");
	return;
}

void ObsManager::initBatcher(ObsTexType type)
{
	if (BatchTable.find(type) == BatchTable.end())
	{
		//texture Pool���� ��ũ �����
		Texture2D* tex_in_cache = texPool->joinTexLink(StringUtils::format("Obstacles/Texture_%d.png", (int)type));

		//�ش� texture�� ����ϴ� Obstacle Batcher ����
		auto obs_batcher = ObsBatcher::createWithTexture(type, tex_in_cache);
		if (obs_batcher != NULL) {
			BatchTable.insert({ type , obs_batcher });
			this->getParent()->addChild(BatchTable[type], 9);
		}
	}
}
void ObsManager::removeBatcher(ObsTexType type)
{
	if (BatchTable.find(type) != BatchTable.end())
	{
		BatchTable[type]->removeFromParent();
		BatchTable[type]->release();
		BatchTable.erase(type);

		//texture Pool���� ��ũ ����
		texPool->detachTexLink(StringUtils::format("Obstacles/Texture_%d.png", (int)type));
	}
}

void ObsManager::setProtoType(int line_idx, BuildContainer& proto_obj)
{
	//proto line�� �� ù �ε����� ����
	if (SpawningPool[line_idx].getCurSize() == 0)
		SpawningPool[line_idx].pushBack(&proto_obj);
}
BuildContainer& ObsManager::getProtoType(int line_idx)
{
	return *SpawningPool[line_idx].getFront();
}


void ObsManager::generateMutant(BuildContainer& mother, int target_line)
{
	if (SpawningPool[target_line].isEmpty()){
		do {
			target_line = (target_line + 1) % LineNum;
		} while (SpawningPool[target_line].isEmpty());
	}

	SpawningPool[target_line].pushBack(&mother.spawnChild(getProtoType(target_line)));
};

void ObsManager::ObsScheduler(float deltaTime)
{
	//�ð� ����� ����, ���� ��ü�鿡 ����ġ �ο�
	for (int i = 0; i < LineNum; i++){
		SpawningPool[i].updateWeights(deltaTime);
	}

	//pool ���� �ν��Ͻ� stand by
	while (!StandByEntry->isFull() && !isAllEmpty())
	{
		int target_pool = RandomHelper::random_int(0, LineNum - 1);
		if (SpawningPool[target_pool].isEmpty()){
			do {
				target_pool = (target_pool + 1) % LineNum;
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

