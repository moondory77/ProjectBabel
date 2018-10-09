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

ParticleCustom* ParticleCustom::spawnChild(int unit_id) {
	
	auto child = this->create(plistName);	
	child->setHost(this->getHost());
	child->setSubFrame(*(this->subFrame));
	child->setID(unit_id);
	return child;
}

void ParticleCustom::updateParticleQuads()
{

	if (isRunning && (frameCnt > 1))
	{
		int cur_frame = frameCnt * (float)(_elapsed / _duration);
		if (cur_frame < 1) cur_frame = 1;
		this->setTextureWithRect(&host->getTexture(), subFrame->at(cur_frame - 1)->getRectInPixels());
	}

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
	this->setTextureWithRect(&host->getTexture(), subFrame->at(0)->getRectInPixels());
	ParticleSystemQuad::resetSystem();
}


