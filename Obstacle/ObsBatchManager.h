#ifndef __OBSTACLE_BATCH_MANAGER_H__
#define __OBSTACLE_BATCH_MANAGER_H__

#include "cocos2d.h"
#include "Util/TextureBatcher.h"

USING_NS_CC;
using namespace std;

//어떤 스케쥴러(이벤트 큐)에 의해 제어될 지 
enum ObsMainType {
	BUILDING = 0, ORDEAL = 1, BOSS = 2
};

//어떤 텍스쳐를 매핑할 지..
enum ObsTexType {
	COMPANY = 0,
	APARTMENT = 1,
	GOTHIC = 2,
	MYSTERY = 3,

	METEOR = 30,
	MUSE = 50,
};



//타겟으로 하는 Obstacle의 텍스쳐를 batch drawing
class ObsBatcher : public SpriteBatchNode
{
	static const int DEFAULT_CAPACITY = 29;
public:

	static ObsBatcher* createWithTexture(ObsTexType type, Texture2D* tex, ssize_t capacity = DEFAULT_CAPACITY);
	Vector<SpriteFrame*> unitFrame = {};
	Size unitSize = Size::ZERO;	//장애물 오브젝트 하나의 기본 frame size

	ObsBatcher(ObsTexType type, Texture2D* tex);
	virtual ~ObsBatcher();
};




class ObsBatchManager : public CCNode
{
private:
	static bool isExist;
	TextureBatcher* texPool;
	map<ObsTexType, ObsBatcher*> BatchTable = {};	//(로드 된) 사용 할 obstacle의 리소스가 저장

public:

	//한번에 1개의 인스터스만 존재하게 강제
	ObsBatchManager() {
		assert(!isExist);
		texPool = new TextureBatcher();
		isExist = true;
	}
	~ObsBatchManager() {

		while (!BatchTable.empty()) {
			removeObsBatcher(BatchTable.begin()->first);
		}
		//CCLOG(TextureCache::getInstance()->getCachedTextureInfo().c_str());
		texPool->release();
		isExist = false;
	};

	void initObsBatcher(ObsTexType obs_tex_type);
	void removeObsBatcher(ObsTexType obs_tex_type);


	ObsBatcher& getBatcher(ObsTexType obs_tex_type) {
		if (BatchTable.find(obs_tex_type) == BatchTable.end()) {
			initObsBatcher(obs_tex_type);
		}
		return *BatchTable[obs_tex_type];
	}

};

#endif