#include "ParticlePool.h"
#define MAX_PARTICLE_NUM 70
#define DEFAULT_PARTICLE_NUM 40


ParticlePool::ParticlePool(ParticleType p_type, Texture2D& p_tex, string tex_name, int sort_num)
	: type(p_type), texture(p_tex), texName(tex_name), sortNum(sort_num)
{
	batchNode = ParticleBatchNode::createWithTexture(&texture);

	int i = 0;
	protoType = new ParticleCustom*[sortNum];	//������Ÿ�� ���� �迭 �Ҵ�

	///##movingLayer�� �ƴ�, onGame Scene�� child�� �ؾ� ��Ȯ�� ������ġ���� ��ƼŬȿ���� ��Ÿ����
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




//���� ��ƼŬ ������ pool�� ����
void ParticlePool::setProtoType(initializer_list<ParticleCustom*> targets)
{
	auto iter = targets.begin();

	for (int i = 0; i < sortNum; i++, iter++)
	{
		protoType[i] = *iter;	//������Ÿ�� ����
		auto p_name = (*iter)->plistName;
		p_name.erase(p_name.find("Particles/"), 10);
		p_name.erase(p_name.find(".plist"), 6);

		//�ʿ��� ��ƼŬ�� sprite frame �޾ƿ� ����
		SpriteFrame* p_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(p_name + ".png");
		if (p_frame == NULL) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Particles/" + texName + ".plist", &texture);
			p_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(p_name + ".png");
		}

		frames.pushBack(p_frame);
		(*iter)->setHost(this);
	}
};


//��ƼŬ ���� ���� ����
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
		auto new_ruin = protoType[sort_idx]->spawnChild(PoolSize.cur++);	//���� pool ����� id�� �ؼ� ����	
		assert(new_ruin != NULL);

		//��ġ���κ���, �ʿ��� frame ����
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
		//�ش� Ŀ���� ������ ��� Ŀ����, (pool�� ������ resize ��, pop�ǹ��� ���) Ŀ���ٽ� ����
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
	///���⼭ �׿� ��ƼŬ�� �ʹ� ���� ��Ȳ��, ���� ��Ŀ���� ���� �ʿ��ҵ�
};







