#ifndef __OBSTACLE_MANAGER_H__
#define __OBSTACLE_MANAGER_H__

#include "cocos2d.h"
#include "Obstacle/Container/_T_Build/_T_BuildContainer.h"
#include "Obstacle/Container/_T_Ordeal/_T_OrdContainer.h"
#include "Obstacle/ObsBatcher.h"

#include "Util/EntryQueue.h"
#include "Util/EntryQueue.cpp"
#include "Util/TextureBatcher.h"

USING_NS_CC;
using namespace std;

class ObsManager : public CCNode
{
private:
	static bool isExist;
	TextureBatcher* texPool;

	EntryQueue<BuildContainer*>* StandByEntry = NULL;
	EntryQueue<BuildContainer*>* SpawningPool = NULL;	//pool의 각 line의 첫번째 원소는 프로토타입
	bool stageFinished;

public:
	const int LineNum;
	map<ObsTexType, ObsBatcher*> BatchTable = {};
	vector<BuildContainer*>& ActiveList;	//onGame 내 update 되는 장애물 리스트

	ObsManager(int line_num, vector<BuildContainer*>& active_list);
	~ObsManager();

	virtual void onEnter();

	//텍스쳐 Batch 처리를 위한, obsBatcher 설정
	void initBatcher(ObsTexType obs_tex_type);
	void removeBatcher(ObsTexType obs_tex_type);
	inline ObsBatcher& getBatcher(ObsTexType obs_tex_type) {
		if (BatchTable.find(obs_tex_type) == BatchTable.end())
			initBatcher(obs_tex_type);
		return *BatchTable[obs_tex_type];
	}

	//Obstacle 생산을 위한 프로토타입 셋팅
	void setProtoType(int line_idx, BuildContainer& proto);
	BuildContainer& getProtoType(int line_idx);

	//base와 line의 프로토타입을 mixing 한 새로운 개체를 line에 추가 삽입 
	void generateMutant(BuildContainer& mix_base, int spawing_line);

	///현재 pool이 모두 비었는지 검사(해당 라인의 프로토타입까지 모두 비워져야..)
	inline bool isAllEmpty(){
		for (int i = 0; i < LineNum; i++) {
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
