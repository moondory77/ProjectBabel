#include "EffectInfo.h"


SprEffectInfo::SprEffectInfo(Texture2D& tex, EffectType e_type, string& tex_name, int frame_num)
	: EffectInfo(tex, e_type, tex_name)
{
	int i = 0;
	auto e_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Effect_%d_%i.png", (int)type, i + 1));

	//텍스쳐로부터 프레임 추가
	if (e_frame == NULL){
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("effect/" + texName + ".plist", &tex);
		e_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Effect_%d_%i.png", (int)type, i + 1));
	}
	frames.pushBack(e_frame);

	while (++i < frame_num) {
		e_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Effect_%d_%i.png", (int)type, i + 1));
		frames.pushBack(e_frame);
	}
};


SprEffectInfo::~SprEffectInfo() {
	frames.clear(); 
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
};



