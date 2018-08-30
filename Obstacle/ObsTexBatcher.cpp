#include "ObsTexBatcher.h"
bool ObsTexBatcher::isExist = false;


BatchElement::BatchElement(ObsTexType type)
{
	texture = TextureCache::getInstance()->addImage(StringUtils::format("Obstacles/Texture_%d.png", (int)type));
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(StringUtils::format("Obstacles/Texture_%d.plist", (int)type), texture);
	batchNode = SpriteBatchNode::createWithTexture(texture);

	for (int i = 0; i < 4; i++) {
		auto unit_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Frame_%d_%d.png", i, (int)type));
		frames.pushBack(unit_frame);
	}
	frameSize = frames.at(0)->getOriginalSize();
};


BatchElement::~BatchElement()
{
	frames.clear();
	SpriteFrameCache::getInstance()->removeSpriteFramesFromTexture(texture);
	TextureCache::getInstance()->removeTexture(texture);
};



void ObsTexBatcher::initTex(ObsTexType tex_type)
{
	//기존에 이미 texture가 로드 되어있으면 제거하고 로드
	if (texBatchTable.find(tex_type) != texBatchTable.end())
		delete(texBatchTable[tex_type]);
	
	texBatchTable.insert({ tex_type, new BatchElement(tex_type) });
	this->addChild(texBatchTable[tex_type]->batchNode);

}