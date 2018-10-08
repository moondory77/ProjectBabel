#include "ObsBatchManager.h"

bool ObsBatchManager::isExist = false;


ObsBatcher* ObsBatcher::createWithTexture(ObsTexType type, Texture2D* tex, ssize_t capacity)
{
	ObsBatcher* obs_batcher = new (std::nothrow) ObsBatcher(type, tex);

	if (obs_batcher && obs_batcher->initWithTexture(tex, capacity)) {
		obs_batcher->retain();
		return obs_batcher;
	}
	delete obs_batcher;
	return nullptr;
}


ObsBatcher::ObsBatcher(ObsTexType type, Texture2D* tex)
{
	//texture로부터, Obstacle의 각 프레임을 나누어 저장
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(StringUtils::format("Obstacles/Texture_%d.plist", (int)type), tex);

	for (int i = 0; i < 4; i++) {
		auto unit_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Frame_%d_%d.png", i, (int)type));
		unitFrame.pushBack(unit_frame);
	}
	unitSize = unitFrame.at(0)->getOriginalSize();
};


ObsBatcher::~ObsBatcher()
{
	unitFrame.clear();
	SpriteFrameCache::getInstance()->removeSpriteFramesFromTexture(this->getTexture());
};



void ObsBatchManager::initObsBatcher(ObsTexType type)
{
	if (BatchTable.find(type) == BatchTable.end())
	{
		//texture Pool에서 링크 끌어옴
		Texture2D* tex_in_cache = texPool->joinTexLink(StringUtils::format("Obstacles/Texture_%d.png", (int)type));

		auto obs_batcher = ObsBatcher::createWithTexture(type, tex_in_cache);
		if (obs_batcher != NULL) {
			BatchTable.insert({ type , obs_batcher });
			this->addChild(BatchTable[type]);
		}
	}
}

void ObsBatchManager::removeObsBatcher(ObsTexType type)
{
	if (BatchTable.find(type) != BatchTable.end())
	{
		BatchTable[type]->removeFromParent();
		BatchTable[type]->release();
		BatchTable.erase(type);

		//texture Pool에서 링크 해제
		texPool->detachTexLink(StringUtils::format("Obstacles/Texture_%d.png", (int)type));
	}
}
