#ifndef __OBSTACLE_BATCH_MANAGER_H__
#define __OBSTACLE_BATCH_MANAGER_H__

#include "cocos2d.h"
#include "Util/TextureBatcher.h"

USING_NS_CC;
using namespace std;


//어떤 스케쥴러(이벤트 큐)에 의해 제어될 지 
enum ObsMainType
{
	BUILDING = 0,
	ORDEAL = 1,
	BOSS = 2
};

//어떤 텍스쳐를 매핑할 지..
enum ObsTexType
{
	COMPANY = 0,
	APARTMENT = 1,
	GOTHIC = 2,
	MYSTERY = 3,

	METEOR = 30,
	MUSE = 50,
};



class ObsBatchUnit : public CCNode
{
public :

	const ObsTexType texType;
	Vector<SpriteFrame*> texFrames = {};
	
	Size frameSize = Size::ZERO;
	SpriteBatchNode* batchNode = NULL;	//해당 텍스쳐를 렌더링하는 배치노드
	
	ObsBatchUnit(Texture2D& tex, ObsTexType tex_type);
	~ObsBatchUnit();
};



class ObsBatchManager : public CCNode
{
private:
	static bool isExist;
	TextureBatcher* texPool;
	map<ObsTexType, ObsBatchUnit*> BatchUnitTable = {};	//(로드 된) 사용 할 obstacle의 리소스가 저장

public:

	ObsBatchManager() {
		assert(!isExist);
		texPool = new TextureBatcher();
		isExist = true;
	}


	~ObsBatchManager() {	
		
		while (!BatchUnitTable.empty()){
			removeBatchUnit(BatchUnitTable.begin()->first);
		}
		//CCLOG(TextureCache::getInstance()->getCachedTextureInfo().c_str());
		texPool->release();
		isExist = false;
	};

	void initBatchUnit(ObsTexType obs_tex_type);
	void removeBatchUnit(ObsTexType obs_tex_type);


	const ObsBatchUnit* getBatchUnit(ObsTexType obs_tex_type) {
		
		if (BatchUnitTable.find(obs_tex_type) == BatchUnitTable.end()){
			initBatchUnit(obs_tex_type);
		}
		return BatchUnitTable[obs_tex_type];
	}

};

#endif