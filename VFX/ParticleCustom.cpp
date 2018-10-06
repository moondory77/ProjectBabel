#include "ParticleCustom.h"
#include "VFX/ParticlePool.h"


//랩핑한 ParticleSystem 클래스의 create 함수 오버라이드
ParticleCustom* ParticleCustom::create(const string& plist_name)
{
	ParticleCustom* ret = new (nothrow)ParticleCustom(plist_name);

	/*
	파티클 plist로부터 init하면서, 여기에서 texture 정보도 캐쉬에 로드됨.
	하지만 설계 상 오브젝트 생성 후,
	따로 각 파티클에 다시 텍스쳐를 입히기에, 여기서 로드된 texture 캐쉬는 더미(용량 차지 안함)
	ParticleDesigner로 만들 때, 편법으로 텍스쳐이름을 나중에 실제 사용할 텍스쳐 이름으로 지정해서 생성
	*/

	if (ret && ret->initWithFile(plist_name)) {
		return ret;	//auto-release를 빼고 리턴
	}
	CC_SAFE_DELETE(ret);
	return ret;
}
ParticleCustom* ParticleCustom::spawnChild(int id) {
	auto child = this->create(plistName);
	child->setHost(this->getHost());
	child->setID(id);
	return child;
}

void ParticleCustom::updateParticleQuads()
{
	ParticleSystemQuad::updateParticleQuads();

	if (_particleCount <= 0) {
		if (!_isActive && isRunning) {
			isRunning = false;
			host->pushToAvailableStack(this->unitID);
		}
	}
}


void ParticleCustom::resetSystem()
{
	isRunning = true;
	ParticleSystemQuad::resetSystem();
}



ParticleAnimation* ParticleAnimation::create(const string& plist_name, int frame_cnt)
{
	ParticleAnimation* ret = new (nothrow)ParticleAnimation(plist_name, frame_cnt);

	if (ret && ret->initWithFile(plist_name)) {
		return ret;	//auto-release를 빼고 리턴
	}
	CC_SAFE_DELETE(ret);
	return ret;
}
ParticleCustom* ParticleAnimation::spawnChild(int id)
{
	auto child = this->create(plistName, animFrames.capacity());
	child->setHost(this->getHost());
	child->setID(id);
	return child;
}


void ParticleAnimation::initFrames()
{
	auto p_name = plistName;
	p_name.erase(p_name.find("Particles/"), 10);
	p_name.erase(p_name.find(".plist"), 6);

	//캐쉬로부터, 애니메이션에 쓰일 frame들을 로드
	for (int i = 0; i < animFrames.capacity(); i++) {
		animFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(p_name + StringUtils::format("_Frame_%d.png", i + 1)));
	}
}

void ParticleAnimation::updateParticleQuads()
{

	if (isRunning) {
		int cur_frame = animFrames.size() * (float)(_elapsed / _duration);
		if (cur_frame < 1) cur_frame = 1;
		this->setTextureWithRect(&host->getTexture(), animFrames.at(cur_frame - 1)->getRectInPixels());
	}
	ParticleCustom::updateParticleQuads();
}


