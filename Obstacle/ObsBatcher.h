#ifndef __OBSTACLE_BATCHER_H__
#define __OBSTACLE_BATCHER_H__

#include "cocos2d.h"
#include "Util/TextureBatcher.h"

USING_NS_CC;
using namespace std;

//어떤 스케쥴러(이벤트 큐)에 의해 제어될 지 
enum ObsMainType {
	BUILDING = 0, ORDEAL = 1, BOSS = 2
};

//어떤 텍스쳐를 매핑할 지..
enum ObsTexType
{
	COMPANY = 0,
	APARTMENT = 1,
	GOTHIC = 2,
	MYSTERY = 3,

	METEOR = 30,
	MUSE = 50
};

//Obstacle 텍스쳐를 Batch Drawing 하기위한 데이터 클래스
class ObsBatcher : public SpriteBatchNode
{
	static const int DEFAULT_CAPACITY = 29;
public:

	static ObsBatcher* createWithTexture(ObsTexType type, Texture2D* tex, ssize_t capacity = DEFAULT_CAPACITY) {
		ObsBatcher* obs_batcher = new (std::nothrow) ObsBatcher(type, tex);

		if (obs_batcher && obs_batcher->initWithTexture(tex, capacity)) {
			obs_batcher->retain();
			return obs_batcher;
		}
		delete obs_batcher;
		return nullptr;
	};
	
	Vector<SpriteFrame*> unitFrame = {};
	Size unitSize = Size::ZERO;				///장애물 오브젝트 하나의 기본 frame size

	ObsBatcher(ObsTexType type, Texture2D* tex) 
	{	
		//texture로부터, Obstacle의 각 프레임을 나누어 저장
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(StringUtils::format("Obstacles/Texture_%d.plist", (int)type), tex);
		for (int i = 0; i < 4; i++) {
			auto unit_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Frame_%d_%d.png", i, (int)type));
			unitFrame.pushBack(unit_frame);
		}
		unitSize = unitFrame.at(0)->getOriginalSize();

	};
	virtual ~ObsBatcher()
	{
		unitFrame.clear();
		SpriteFrameCache::getInstance()->removeSpriteFramesFromTexture(this->getTexture());
	};
};


#endif