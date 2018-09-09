#include "ObsBatchManager.h"

bool ObsBatchManager::isExist = false;

ObsBatchUnit::ObsBatchUnit(Texture2D& tex, ObsTexType tex_type) : texType(tex_type)
{	
	batchNode = SpriteBatchNode::createWithTexture(&tex);
	
	//sprite frame들도 캐쉬에 로드
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(StringUtils::format("Obstacles/Texture_%d.plist", (int)tex_type), &tex);
	
	for (int i = 0; i < 4; i++) {
		auto unit_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Frame_%d_%d.png", i, (int)tex_type));
		texFrames.pushBack(unit_frame);
	}
	frameSize = texFrames.at(0)->getOriginalSize();
};


ObsBatchUnit::~ObsBatchUnit()
{
	texFrames.clear();
	auto tex = batchNode->getTexture();
	SpriteFrameCache::getInstance()->removeSpriteFramesFromTexture(tex);

	batchNode->removeFromParent();
	batchNode->release();
};



void ObsBatchManager::initBatchUnit(ObsTexType obs_tex_type)
{
	if (BatchUnitTable.find(obs_tex_type) == BatchUnitTable.end())
	{
		//texture Pool에서 링크 끌어옴
		Texture2D* tex_in_cache = texPool->joinTexLink(StringUtils::format("Obstacles/Texture_%d.png", (int)obs_tex_type));
		BatchUnitTable.insert({ obs_tex_type , new ObsBatchUnit(*tex_in_cache, obs_tex_type)});
		this->addChild(BatchUnitTable[obs_tex_type]->batchNode);
	}
}


void ObsBatchManager::removeBatchUnit(ObsTexType obs_tex_type)
{
	if (BatchUnitTable.find(obs_tex_type) != BatchUnitTable.end())
	{
		BatchUnitTable[obs_tex_type]->release();
		BatchUnitTable.erase(obs_tex_type);

		//texture Pool에서 링크 해제
		texPool->detachTexLink(StringUtils::format("Obstacles/Texture_%d.png", (int)obs_tex_type));
	}
}
