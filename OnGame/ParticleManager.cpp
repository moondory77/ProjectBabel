#include "ParticleManager.h"

bool ParticleManager::isExist = false;


void  ParticleManager::initPool(ParticleType p_type, const char* plist_tex_name, int elem_num, int default_size)
{
	if (Pools.find(p_type) == Pools.end())
		Pools.insert({p_type, NULL});

	if (Pools[p_type] == NULL)
	{
		string casted_name = string(plist_tex_name);
	
		///Cache에 texture & spriteFrame 모두 load
		if (AddressTable.find(casted_name) == AddressTable.end())
		{
			AddressTable.insert(casted_name);
			auto tex_in_cache = TextureCache::getInstance()->addImage("ruins/" + casted_name + ".png");
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ruins/" + casted_name + ".plist", tex_in_cache);
		}
		Pools[p_type] = new ParticlePool(p_type, casted_name, elem_num, default_size);
		this->getParent()->addChild(Pools[p_type]->getBatchNode(), 10);
	}
}

void ParticleManager::removePool(ParticleType p_type)
{
	if (Pools[p_type] != NULL)
	{
		auto chk_addr = Pools[p_type]->getAddress();
		Pools[p_type]->removeFromParent();
		Pools[p_type]->release();
		Pools[p_type] = NULL;	

		//pool 삭제 후, 더이상 참조되지 않는 frame들은 캐쉬에서 해제
		SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
		
		
		//참조하는 frame이 모두 없어지면, texture도 Cache에서 해제
		if (SpriteFrameCache::getInstance()->isSpriteFramesWithFileLoaded("ruins/" + chk_addr + ".plist") == false)
		{
			TextureCache::getInstance()->removeTextureForKey("ruins/" + chk_addr + ".png");
			AddressTable.erase(chk_addr);		
		}
	}
}


