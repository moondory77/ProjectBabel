#include "ParticlePool.h"
#define MAX_PARTICLE_NUM 70
#define DEFAULT_PARTICLE_NUM 40


ParticlePool::ParticlePool(ParticleType p_type, string plist_tex_address, int elem_sort, int default_size)
	: type(p_type), address(plist_tex_address), elementNum(elem_sort), defaultSize(default_size)
{
	//�ش� pool�� �ʿ��� Sprite Frame �ε� (manager�� ����, Cache load�� Ȯ�Ű���)
	for (int i = 0; i < this->elementNum; i++){
		frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Frame_%d_Particle_%d.png", i, (int)type)));
	}
	batchNode = ParticleBatchNode::createWithTexture(TextureCache::getInstance()->getTextureForKey("ruins/" + address + ".png"));
	
	///##movingLayer�� �ƴ�, onGame Scene�� child�� �ؾ� ��Ȯ�� ������ġ���� ��ƼŬȿ��
	unitPool.reserve(defaultSize);
	availableStack.resize(defaultSize);
	cursor = 0;

	//default ������ŭ push
	for (int i = 0; i < default_size; i++)
		pushParticle();
};
ParticlePool::~ParticlePool()
{
	for (int i = 0; i < unitPool.size(); i++)
		popParticle();

	frames.clear();
	batchNode->removeFromParent();
	batchNode->release();
};


void ParticlePool::pushParticle()
{
	//�ش� ParticleSystem�� ���ǵ� plist ���� �ҷ�����
	ParticleCustomUnit* new_ruin = ParticleCustomUnit::create(*this, unitPool.size(), StringUtils::format("ruins/Frame_%d_Particle_%d.plist", unitPool.size() % 3, (int)type));

	if (new_ruin) {
		//�����ϴ� Particle Unit�� ����ϴ� Frame�� ����
		new_ruin->setTextureWithRect(batchNode->getTexture(), frames.at(unitPool.size() % 3)->getRectInPixels());
		new_ruin->setAnchorPoint(Vec2(0.5f, 0.5f));
		new_ruin->setScale(0.1f);
		new_ruin->setDuration(0.7f);
	
		batchNode->addChild(new_ruin);
		new_ruin->stopSystem();
		
		unitPool.push_back(new_ruin);	//pool �� ���� ���� ����	
		pushToAvailableStack(new_ruin->unitIdx);	//�ش� ������ �ε����� ����pool�� ����	
		//CCLOG("Pushed a new particle! Current Pool size is %d", unitPool.size());
	}
}
void ParticlePool::popParticle()
{
	if (unitPool.size() > 0)
	{
		unitPool.back()->removeFromParent();
		unitPool.back()->release();
		unitPool.pop_back();
		CCLOG("Poped a particle! Current Pool size is %d", unitPool.size());
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
		//�ش� Ŀ���� ������ ��� Ŀ����, (pool�� ������ pop�Ǿ������) Ŀ���ٽ� ����
		do { available_idx = availableStack[--cursor]; } while (available_idx >= unitPool.size());


		//CCLOG("burst point is [%.1f, %.1f]", pos.x, pos.y);
		unitPool[available_idx]->setPosition(world_pos);


		unitPool[available_idx]->resetSystem();
		unitPool[available_idx]->isRunning = true;
		//CCLOG("now available particle is (%d / %d)", cursor, unitPool.size());
	}
	else if (unitPool.size() < MAX_PARTICLE_NUM)
	{
		pushParticle();
		playParticleEffect(world_pos);
	}

};



//������ ParticleSystem Ŭ������ create �Լ� �������̵�
ParticleCustomUnit* ParticleCustomUnit::create(ParticlePool& my_pool, int unit_idx, const string& filename)
{
	ParticleCustomUnit* ret = new (nothrow)ParticleCustomUnit(my_pool, unit_idx);
	if (ret && ret->initWithFile(filename))
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







