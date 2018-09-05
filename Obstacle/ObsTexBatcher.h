#ifndef __OBSTACLE_TEXTURE_BATCHER_H__
#define __OBSTACLE_TEXTURE_BATCHER_H__

#include "cocos2d.h"

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



struct BatchElement
{
	Texture2D* texture = NULL;
	Vector<SpriteFrame*> frames = {};
	Size frameSize = Size::ZERO;

	SpriteBatchNode* batchNode = NULL;	//해당 텍스쳐를 렌더링하는 배치노드
	BatchElement(ObsTexType type);
	~BatchElement();
};



class ObsTexBatcher : public CCNode
{
private:
	static bool isExist;
	map<ObsTexType, BatchElement*> texBatchTable = {};	//(로드 된) 사용 할 obstacle의 리소스가 저장

public:

	ObsTexBatcher() {
		assert(!isExist);
		isExist = true;
	}

	~ObsTexBatcher() {
		for (auto iter = texBatchTable.begin(); iter != texBatchTable.end(); iter++) {
			//초기화 되어있는 모든 batchElement를 삭제
			if ((*iter).second != NULL)
				delete((*iter).second);
		}
		isExist = false;
	};

	void initTex(ObsTexType texture_type);

	BatchElement& getTexBatcher(ObsTexType tex_type) {
		if (texBatchTable.find(tex_type) == texBatchTable.end())
			initTex(tex_type);

		return *texBatchTable[tex_type];
	}

};

#endif