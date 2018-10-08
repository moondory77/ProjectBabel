#include "ParticleCustom.h"
#include "VFX/ParticlePool.h"


//랩핑한 ParticleSystem 클래스의 create 함수 오버라이드
ParticleCustom* ParticleCustom::create(const string& plist_name)
{
	ParticleCustom* ret = new (nothrow)ParticleCustom(plist_name);

	/*
	파티클 plist로부터 init하면서 로드되는 텍스쳐는
	파티클 위에 실제 입힐 텍스쳐와 동일한 이름으로 생성 후(Particle Designer에서), 
	이름 변경하여 사용.
	각 파티클의 텍스쳐들을 하나의 Sheet로 배치 처리하기 위한 우회방법
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


