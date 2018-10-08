#include "ParticleCustom.h"
#include "VFX/ParticlePool.h"


//������ ParticleSystem Ŭ������ create �Լ� �������̵�
ParticleCustom* ParticleCustom::create(const string& plist_name)
{
	ParticleCustom* ret = new (nothrow)ParticleCustom(plist_name);

	/*
	��ƼŬ plist�κ��� init�ϸ鼭 �ε�Ǵ� �ؽ��Ĵ�
	��ƼŬ ���� ���� ���� �ؽ��Ŀ� ������ �̸����� ���� ��(Particle Designer����), 
	�̸� �����Ͽ� ���.
	�� ��ƼŬ�� �ؽ��ĵ��� �ϳ��� Sheet�� ��ġ ó���ϱ� ���� ��ȸ���
	*/

	if (ret && ret->initWithFile(plist_name)) {
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


ParticleAnimation* ParticleAnimation::create(const string& plist_name, int frame_cnt)
{
	ParticleAnimation* ret = new (nothrow)ParticleAnimation(plist_name, frame_cnt);

	if (ret && ret->initWithFile(plist_name)) {
		return ret;	//auto-release�� ���� ����
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

	//ĳ���κ���, �ִϸ��̼ǿ� ���� frame���� �ε�
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


