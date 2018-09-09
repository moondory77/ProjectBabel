#include "ParticleManager.h"

bool ParticleManager::isExist = false;

void  ParticleManager::initPool(ParticleType p_type, const char* tex_name, int elem_num, int default_size)
{
	if (PoolTable.find(p_type) == PoolTable.end())
	{
		string join_tex_name = string(tex_name);	
		Texture2D* tex_in_cache = texPool->joinTexLink("ruins/" + join_tex_name + ".png");
		
		PoolTable.insert({ p_type, new ParticlePool(*tex_in_cache, p_type, join_tex_name, elem_num, default_size) });
		this->getParent()->addChild(PoolTable[p_type]->getBatchNode(), 10);
	}
	//CCLOG(TextureCache::getInstance()->getCachedTextureInfo().c_str());
}

void ParticleManager::removePool(ParticleType p_type)
{
	if (PoolTable.find(p_type) != PoolTable.end())
	{
		string detach_tex_address = string("ruins/" + PoolTable[p_type]->getTexName() + ".png");
		PoolTable[p_type]->removeFromParent();
		PoolTable[p_type]->release();
		PoolTable.erase(p_type);

		//texture Pool에서 링크 해제
		texPool->detachTexLink(detach_tex_address);
	}
}


