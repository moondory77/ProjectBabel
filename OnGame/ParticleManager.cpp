#include "ParticleManager.h"

bool ParticleManager::isExist = false;


void  ParticleManager::initPool(ParticleType p_type, const char* plist_tex_name, int elem_num, int default_size)
{
	if (Pools.find(p_type) == Pools.end())
		Pools.insert({p_type, NULL});

	if (Pools[p_type] == NULL)
	{
		string casted_name = string(plist_tex_name);
	
		///Cache�� texture & spriteFrame ��� load
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

		//pool ���� ��, ���̻� �������� �ʴ� frame���� ĳ������ ����
		SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
		
		
		//�����ϴ� frame�� ��� ��������, texture�� Cache���� ����
		if (SpriteFrameCache::getInstance()->isSpriteFramesWithFileLoaded("ruins/" + chk_addr + ".plist") == false)
		{
			TextureCache::getInstance()->removeTextureForKey("ruins/" + chk_addr + ".png");
			AddressTable.erase(chk_addr);		
		}
	}
}


