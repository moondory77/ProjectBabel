#include "ParticlePool.h"
#define MAX_PARTICLE_NUM 70
#define DEFAULT_PARTICLE_NUM 40


ParticlePool::ParticlePool(ParticleType p_type, Texture2D& p_tex, string tex_name, int sort_num)
	: type(p_type), texture(p_tex), texName(tex_name), sortNum(sort_num)
{
	batchNode = ParticleBatchNode::createWithTexture(&texture);

	int i = 0;
	protoType = new ParticleCustom*[sortNum];	//프로토타입 저장 배열 할당

	///##movingLayer가 아닌, onGame Scene의 child로 해야 정확한 고정위치에서 파티클효과가 나타난다
	cursor = 0;
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




//구성 파티클 원형을 pool에 장착
void ParticlePool::setProtoType(initializer_list<ParticleCustom*> targets)
{
	auto iter = targets.begin();

	for (int i = 0; i < sortNum; i++, iter++)
	{
		protoType[i] = *iter;	//프로토타입 설정
		auto p_name = (*iter)->plistName;
		p_name.erase(p_name.find("Particles/"), 10);
		p_name.erase(p_name.find(".plist"), 6);

		//필요한 파티클의 sprite frame 받아와 저장
		SpriteFrame* p_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(p_name + ".png");
		if (p_frame == NULL) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Particles/" + texName + ".plist", &texture);
			p_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(p_name + ".png");
		}

		frames.pushBack(p_frame);
		(*iter)->setHost(this);
	}
};


//파티클 유닛 생산 시작
void ParticlePool::runSpawning(int min_size, int max_size)
{
	PoolSize.min = min_size;
	PoolSize.max = max_size;
	PoolSize.cur = 0;

	unitPool.reserve(PoolSize.min);
	availableStack.reserve(PoolSize.min);

	for (int i = 0; i < PoolSize.min; i++)
		pushParticle(PoolSize.cur % sortNum);
}



void ParticlePool::pushParticle(int sort_idx)
{
	if (PoolSize.cur < PoolSize.max)
	{
		auto new_ruin = protoType[sort_idx]->spawnChild(PoolSize.cur++);	//현재 pool 사이즈를 id로 해서 삽입	
		assert(new_ruin != NULL);

		//배치노드로부터, 필요한 frame 입힘
		new_ruin->setTextureWithRect(&texture, frames.at(sort_idx)->getRectInPixels());
		//new_ruin->setAnchorPoint(Vec2(0.5f, 0.5f));
		//new_ruin->setScale(0.1f);
		new_ruin->setDuration(0.1f);

		batchNode->addChild(new_ruin);
		new_ruin->stopSystem();
		unitPool.push_back(new_ruin);
		pushToAvailableStack(new_ruin->getID());
	}
}


void ParticlePool::popParticle()
{
	if (unitPool.size() > 0) {
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
		unitPool[available_idx]->setTextureWithRect(&texture, frames.at(0)->getRectInPixels());
		unitPool[available_idx]->resetSystem();
		//unitPool[available_idx]->isRunning = true;
		//CCLOG("now available particle is (%d / %d)", cursor, unitPool.size());
	}
	else if (getCurSize() < PoolSize.max)
	{
		pushParticle(PoolSize.cur % sortNum);
		playParticleEffect(world_pos);
	}
	///여기서 잉여 파티클이 너무 많은 상황에, 정리 메커니즘 삽입 필요할듯
};







