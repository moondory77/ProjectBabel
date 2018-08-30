#include "ParticlePool.h"
#define MAX_PARTICLE_NUM 70
#define DEFAULT_PARTICLE_NUM 40


ParticlePool::ParticlePool(ParticleType p_type, string plist_tex_address, int elem_sort, int default_size)
	: type(p_type), address(plist_tex_address), elementNum(elem_sort), defaultSize(default_size)
{
	//해당 pool에 필요한 Sprite Frame 로드 (manager에 의해, Cache load를 확신가능)
	for (int i = 0; i < this->elementNum; i++){
		frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Frame_%d_Particle_%d.png", i, (int)type)));
	}
	batchNode = ParticleBatchNode::createWithTexture(TextureCache::getInstance()->getTextureForKey("ruins/" + address + ".png"));
	
	///##movingLayer가 아닌, onGame Scene의 child로 해야 정확한 고정위치에서 파티클효과
	unitPool.reserve(defaultSize);
	availableStack.resize(defaultSize);
	cursor = 0;

	//default 갯수만큼 push
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
	//해당 ParticleSystem이 정의된 plist 파일 불러오기
	ParticleCustomUnit* new_ruin = ParticleCustomUnit::create(*this, unitPool.size(), StringUtils::format("ruins/Frame_%d_Particle_%d.plist", unitPool.size() % 3, (int)type));

	if (new_ruin) {
		//생성하는 Particle Unit이 사용하는 Frame을 입힘
		new_ruin->setTextureWithRect(batchNode->getTexture(), frames.at(unitPool.size() % 3)->getRectInPixels());
		new_ruin->setAnchorPoint(Vec2(0.5f, 0.5f));
		new_ruin->setScale(0.1f);
		new_ruin->setDuration(0.7f);
	
		batchNode->addChild(new_ruin);
		new_ruin->stopSystem();
		
		unitPool.push_back(new_ruin);	//pool 맨 끝에 유닛 삽입	
		pushToAvailableStack(new_ruin->unitIdx);	//해당 유닛의 인덱스를 가용pool에 삽입	
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
		//해당 커서가 범위를 벗어난 커서면, (pool의 유닛이 pop되어버려서) 커서다시 내림
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



//랩핑한 ParticleSystem 클래스의 create 함수 오버라이드
ParticleCustomUnit* ParticleCustomUnit::create(ParticlePool& my_pool, int unit_idx, const string& filename)
{
	ParticleCustomUnit* ret = new (nothrow)ParticleCustomUnit(my_pool, unit_idx);
	if (ret && ret->initWithFile(filename))
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







