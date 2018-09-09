#include "ParticlePool.h"
#define MAX_PARTICLE_NUM 70
#define DEFAULT_PARTICLE_NUM 40


//������ ParticleSystem Ŭ������ create �Լ� �������̵�
ParticleCustomUnit* ParticleCustomUnit::create(ParticlePool& my_pool, int unit_idx, const string& plist_name)
{
	ParticleCustomUnit* ret = new (nothrow)ParticleCustomUnit(my_pool, unit_idx);

	/* ��ƼŬ plist�κ��� init�ϸ鼭, ���⿡�� texture ������ ĳ���� �ε��. ������ ���� ���� ��
	������Ʈ ���� ��, ���� �� ��ƼŬ�� �ٽ� �ؽ��ĸ� �����⿡, ���⼭ �ε�� texture ĳ���� ����(�뷮 ���� ����)*/
	if (ret && ret->initWithFile(plist_name))
	{
		return ret;	//auto-release�� ���� ����
	}
	CC_SAFE_DELETE(ret);
	return ret;
}

void ParticleCustomUnit::update(float deltaTime)
{
	super::update(deltaTime);	//���� particleSystem�� update ó��
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

	///##movingLayer�� �ƴ�, onGame Scene�� child�� �ؾ� ��Ȯ�� ������ġ���� ��ƼŬȿ���� ��Ÿ����
	//�ּһ����ŭ pool�� ����
	unitPool.reserve(minSize);
	availableStack.resize(minSize);

	cursor = 0;
	maxSize = MAX_PARTICLE_NUM;

	//default ������ŭ push
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
		//��ƼŬ ������Ʈ ���� (plist from -> ParticleDesigner)
		ParticleCustomUnit* new_ruin 
			= ParticleCustomUnit::create(*this, unitPool.size(), StringUtils::format("ruins/Particle_Unit_%d.plist", unitPool.size() % 3));

		if (new_ruin)
		{
			//��ġ���κ���, �ʿ��� frame ����
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
		//�ش� Ŀ���� ������ ��� Ŀ����, (pool�� ������ resize ��, pop�ǹ��� ���) Ŀ���ٽ� ����
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

	///���⼭ �׿� ��ƼŬ�� �ʹ� ���� ��Ȳ��, ���� ��Ŀ���� ���� �ʿ��ҵ�
};







