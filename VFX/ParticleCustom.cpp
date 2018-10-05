#include "ParticleCustom.h"
#include "VFX/ParticlePool.h"


//������ ParticleSystem Ŭ������ create �Լ� �������̵�
ParticleCustom* ParticleCustom::create(const string& plist_name)
{
	ParticleCustom* ret = new (nothrow)ParticleCustom(plist_name);

	/* ��ƼŬ plist�κ��� init�ϸ鼭, ���⿡�� texture ������ ĳ���� �ε��.
	������ ���� �� ������Ʈ ���� ��,
	���� �� ��ƼŬ�� �ٽ� �ؽ��ĸ� �����⿡, ���⼭ �ε�� texture ĳ���� ����(�뷮 ���� ����)
	ParticleDesigner�� ���� ��, ������� �ؽ����̸��� ���߿� ���� ����� �ؽ��� �̸����� �����ؼ� ����
	*/
	if (ret && ret->initWithFile(plist_name))
	{
		return ret;	//auto-release�� ���� ����
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
//		return ret;	//auto-release�� ���� ����
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
