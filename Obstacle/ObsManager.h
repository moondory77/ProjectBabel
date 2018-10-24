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
	EntryQueue<BuildContainer*>* SpawningPool = NULL;	//pool�� �� line�� ù��° ���Ҵ� ������Ÿ��
	bool stageFinished;

public:
	const int LineNum;
	map<ObsTexType, ObsBatcher*> BatchTable = {};
	vector<BuildContainer*>& ActiveList;	//onGame �� update �Ǵ� ��ֹ� ����Ʈ

	ObsManager(int line_num, vector<BuildContainer*>& active_list);
	~ObsManager();

	virtual void onEnter();

	//�ؽ��� Batch ó���� ����, obsBatcher ����
	void initBatcher(ObsTexType obs_tex_type);
	void removeBatcher(ObsTexType obs_tex_type);
	inline ObsBatcher& getBatcher(ObsTexType obs_tex_type) {
		if (BatchTable.find(obs_tex_type) == BatchTable.end())
			initBatcher(obs_tex_type);
		return *BatchTable[obs_tex_type];
	}

	//Obstacle ������ ���� ������Ÿ�� ����
	void setProtoType(int line_idx, BuildContainer& proto);
	BuildContainer& getProtoType(int line_idx);

	//base�� line�� ������Ÿ���� mixing �� ���ο� ��ü�� line�� �߰� ���� 
	void generateMutant(BuildContainer& mix_base, int spawing_line);

	///���� pool�� ��� ������� �˻�(�ش� ������ ������Ÿ�Ա��� ��� �������..)
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
