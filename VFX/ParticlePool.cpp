#include "ParticlePool.h"
#define MAX_PARTICLE_NUM 70
#define DEFAULT_PARTICLE_NUM 40


//랩핑한 ParticleSystem 클래스의 create 함수 오버라이드
ParticleCustomUnit* ParticleCustomUnit::create(ParticlePool& my_pool, int unit_idx, const string& plist_name)
{
	ParticleCustomUnit* ret = new (nothrow)ParticleCustomUnit(my_pool, unit_idx);

	/* 파티클 plist로부터 init하면서, 여기에서 texture 정보도 캐쉬에 로드됨. 하지면 현재 설계 상
	오브젝트 생성 후, 따로 각 파티클에 다시 텍스쳐를 입히기에, 여기서 로드된 texture 캐쉬는 더미(용량 차지 안함)*/
	if (ret && ret->initWithFile(plist_name))
	{
		return ret;	//auto-release를 빼고 리턴
	}
	CC_SAFE_DELETE(ret);
	return ret;
}

void ParticleCustomUnit::update(float deltaTime)
{
	super::update(deltaTime);	//기존 particleSystem의 update 처리
	if (!_isActive && isRunning)
	{
		if (_particleCount == 0) {
			isRunning = false;
			container.pushToAvailableStack(this->unitIdx);
		}
	}
}


ParticlePool::ParticlePool(Texture2D& tex_in_cache, ParticleType p_type, string tex_name, int elem_sort, int default_size)
	: type(p_type), texName(tex_name), elementNum(elem_sort), minSize(default_size)
{
	batchNode = ParticleBatchNode::createWithTexture(&tex_in_cache);

	int i = 0;
	SpriteFrame* p_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Frame_%d_Particle_%d.png", i, (int)type));

	if (p_frame == NULL) {
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ruins/" + texName + ".plist", &tex_in_cache);
		p_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Frame_%d_Particle_%d.png", i, (int)type));
	}
	frames.pushBack(p_frame);

	while (++i < this->elementNum) {
		p_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Frame_%d_Particle_%d.png", i, (int)type));
		frames.pushBack(p_frame);
	}

	///##movingLayer가 아닌, onGame Scene의 child로 해야 정확한 고정위치에서 파티클효과가 나타난다
	//최소사이즈만큼 pool을 셋팅
	unitPool.reserve(minSize);
	availableStack.resize(minSize);

	cursor = 0;
	maxSize = MAX_PARTICLE_NUM;

	//default 갯수만큼 push
	for (int i = 0; i < default_size; i++)
		pushParticle();
};
ParticlePool::~ParticlePool()
{
	for (int i = 0; i < unitPool.size(); i++)
		popParticle();

	batchNode->removeFromParent();
	batchNode->release();
	frames.clear();

	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
};


void ParticlePool::pushParticle()
{
	if (getCurSize() < maxSize)
	{
		//파티클 오브젝트 생성 (plist from -> ParticleDesigner)
		ParticleCustomUnit* new_ruin 
			= ParticleCustomUnit::create(*this, unitPool.size(), StringUtils::format("ruins/Particle_Unit_%d.plist", unitPool.size() % 3));

		if (new_ruin)
		{
			//배치노드로부터, 필요한 frame 입힘
			new_ruin->setTextureWithRect(batchNode->getTexture(), frames.at(unitPool.size() % 3)->getRectInPixels());
			new_ruin->setAnchorPoint(Vec2(0.5f, 0.5f));
			new_ruin->setScale(0.1f);
			new_ruin->setDuration(0.7f);

			batchNode->addChild(new_ruin);
			new_ruin->stopSystem();
			unitPool.push_back(new_ruin);

			pushToAvailableStack(new_ruin->unitIdx);
		}
	}
}
void ParticlePool::popParticle()
{
	if (unitPool.size() > 0){
		unitPool.back()->removeFromParent();
		unitPool.back()->release();
		unitPool.pop_back();
	}
}


void ParticlePool::pushToAvailableStack(int unit_idx)
{
	if (cursor == availableStack.size()) {
		availableStack.push_back(unit_idx);
	}
	else {
		availableStack[cursor] = unit_idx;
	}
	cursor++;
}
void ParticlePool::playParticleEffect(Point& world_pos)
{
	if (cursor > 0)
	{
		int available_idx = 0;
		//해당 커서가 범위를 벗어난 커서면, (pool의 유닛이 resize 중, pop되버린 경우) 커서다시 내림
		do { available_idx = availableStack[--cursor]; } while (available_idx >= unitPool.size());

		unitPool[available_idx]->setPosition(world_pos);
		unitPool[available_idx]->resetSystem();
		unitPool[available_idx]->isRunning = true;
		//CCLOG("now available particle is (%d / %d)", cursor, unitPool.size());
	}
	else if (getCurSize() < maxSize)
	{
		pushParticle();
		playParticleEffect(world_pos);
	}

	///여기서 잉여 파티클이 너무 많은 상황에, 정리 메커니즘 삽입 필요할듯
};







