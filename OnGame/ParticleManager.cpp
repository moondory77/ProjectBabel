#include "ParticleManager.h"

bool ParticleManager::isExist = false;


void  ParticleManager::initPool(ParticleType p_type, const char* plist_tex_name, int elem_num, int default_size)
{
	if (Pools.find(p_type) == Pools.end())
		Pools.insert({p_type, NULL});

	if (Pools[p_type] == NULL)
	{
		string casted_name = string(plist_tex_name);
		Texture2D* tex_in_cache = TextureCache::getInstance()->getTextureForKey("ruins/" + casted_name + ".png");
		
		//���� ĳ���� �ε�� �ؽ��� ���� ���, ���� �ε�
		if (tex_in_cache == NULL){

			tex_in_cache = TextureCache::getInstance()->addImage("ruins/" + casted_name + ".png");
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
		Pools[p_type]->removeFromParent();
		Pools[p_type]->release();	
		Pools.erase(p_type);
		
		//���̻� �������� �ʴ� ���ҽ��� ĳ������ ����
		SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
		TextureCache::getInstance()->removeUnusedTextures();
	}
}


