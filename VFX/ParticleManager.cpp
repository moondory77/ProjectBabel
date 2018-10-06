#include "ParticleManager.h"

bool ParticleManager::isExist = false;

void ParticleManager::createPool(ParticleType p_type, const char* tex_name, int sort_num)
{
	if (PoolTable.find(p_type) == PoolTable.end())
	{
		string join_tex_address = string(tex_name);

		//텍스쳐에 대한 참조링크 추가
		Texture2D* tex_in_cache = texPool->joinTexLink("Particles/" + join_tex_address + ".png");

		PoolTable.insert({ p_type, new ParticlePool(p_type, *tex_in_cache, join_tex_address, sort_num) });
		this->getParent()->addChild(PoolTable[p_type]->getBatchNode(), 10);
	}
}

void ParticleManager::removePool(ParticleType p_type)
{
	if (PoolTable.find(p_type) != PoolTable.end())
	{
		string detach_tex_address = string("Particles/" + PoolTable[p_type]->getTexName() + ".png");
		PoolTable[p_type]->removeFromParent();
		PoolTable[p_type]->release();
		PoolTable.erase(p_type);

		//texture Pool에서 링크 해제
		texPool->detachTexLink(detach_tex_address);
	}
}


