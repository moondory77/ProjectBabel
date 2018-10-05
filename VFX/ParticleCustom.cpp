#include "ParticleCustom.h"
#include "VFX/ParticlePool.h"


//랩핑한 ParticleSystem 클래스의 create 함수 오버라이드
ParticleCustom* ParticleCustom::create(const string& plist_name)
{
	ParticleCustom* ret = new (nothrow)ParticleCustom(plist_name);

	/* 파티클 plist로부터 init하면서, 여기에서 texture 정보도 캐쉬에 로드됨.
	하지만 설계 상 오브젝트 생성 후,
	따로 각 파티클에 다시 텍스쳐를 입히기에, 여기서 로드된 texture 캐쉬는 더미(용량 차지 안함)
	ParticleDesigner로 만들 때, 편법으로 텍스쳐이름을 나중에 실제 사용할 텍스쳐 이름으로 지정해서 생성
	*/
	if (ret && ret->initWithFile(plist_name))
	{
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


//
//
//ParticleAnimation* ParticleAnimation::create(ParticlePool& host, int unit_idx, const string& plist_name)
//{
//	ParticleAnimation* ret = new (nothrow)ParticleAnimation(host, unit_idx);
//
//	if (ret && ret->initWithFile(plist_name))
//	{
//		return ret;	//auto-release를 빼고 리턴
//	}
//	CC_SAFE_DELETE(ret);
//	return ret;
//}
//
//
//void ParticleAnimation::initFrames(ParticleType p_type, int frame_cnt)
//{
//	for (int i = 0; i < frame_cnt; i++) {
//		animFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("Frame_%d_SubType_0_Particle_%d.png", i, (int)p_type)));
//	}
//}
//
//
//void ParticleAnimation::update(float deltaTime)
//{
//	ParticleCustom::update(deltaTime);
//
//	if (!_isActive && isRunning)
//	{
//		int cur_frame = (int)_elapsed * (_duration / (float)animFrames.size());
//		this->setTextureWithRect(container.getBatchNode()->getTexture(), animFrames.at(cur_frame)->getRectInPixels());
//	}
//}
//
