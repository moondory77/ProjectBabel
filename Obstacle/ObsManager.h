#ifndef __OBSTACLE_MANAGER_H__
#define __OBSTACLE_MANAGER_H__

#include "cocos2d.h"
#include "Obstacle/ObsContainer/Building/_T_BuildContainer.h"
#include "Obstacle/ObsContainer/Ordeal/_T_OrdContainer.h"
#include "Obstacle/ObsTexBatcher.h"
#include "Util/EntryQueue.h"
#include "Util/EntryQueue.cpp"
#include "OnGame/_Scene_OnGame.h"


USING_NS_CC;
using namespace std;

class ObsManager : public CCNode
{
private:
	static bool isExist;
	EntryQueue<BuildContainer*>* StandByEntry = NULL;
	EntryQueue<BuildContainer*>* SpawningPool = NULL;
	
	bool stageFinished;
public:
	const int ProtoNum;

	virtual void onEnter();
	vector<BuildContainer*>& ActiveList;
	void setProtoType(int target_line, BuildContainer& proto);
	BuildContainer& getProtoType(int target_line);


	ObsManager(int proto_num, vector<BuildContainer*>& active_list) : ProtoNum(proto_num), ActiveList(active_list)
	{
		assert(!isExist);
		SpawningPool = new EntryQueue<BuildContainer*>[proto_num];
		StandByEntry = new EntryQueue<BuildContainer*>(3);
		stageFinished = false;
		isExist = true;
	}

	~ObsManager() {
		delete(StandByEntry);
		delete[] SpawningPool;
		CCLOG("ObsManager deleted!");
		isExist = false;
	};


	void generateMutant(BuildContainer& base, int target_line);

	bool isPoolEmpty()
	{
		for (int i = 0; i < ProtoNum; i++) {
			if (!SpawningPool[i].isEmpty())
				return false;
		}
		return true;
	}

	//int getRemainObsCnt() { 
	//	return EasyEntry->getCurSize() + NormalEntry->getCurSize() + HardEntry->getCurSize(); 
	//}

	bool getStageFinished() { return stageFinished; };
	void ObsScheduler(float deltaTime);
};


#endif
