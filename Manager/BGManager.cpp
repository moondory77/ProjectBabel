#include "BGManager.h"
#include "Manager/GameManager.h"


//*************************** #BGManager	**************************************// 
BGManager* BGManager::BGManager_instance = NULL;

BGManager* BGManager::getInstance() {

	if (!BGManager_instance)
	{
		BGManager_instance = new BGManager();

		BGManager_instance->initBG();
	}
	return BGManager_instance;
}


//ó�� �̱��� ��ü ����� �� �� ȣ�� (��Ÿ�� ��, ó�� �ѹ�)
void BGManager::initBG()
{
	//�ʱ�ȭ ��, ����Ǿ��ִ� karma �翡 ����, ����frame�� ����
	int cur_idx = curKarma * KARMA_BG_FACTOR;

	bottomFrame = new FrameBG();
	bottomFrame->frameIdx = cur_idx - 1;

	if (0 <= bottomFrame->frameIdx && bottomFrame->frameIdx <= 14) {
		TextureCache::getInstance()->addImage(StringUtils::format("BackGround/BG_%d.png", bottomFrame->frameIdx));
	}
	topFrame = bottomFrame;

	//���������� FrameBG ����
	for (int i = 0; i < 4; i++){
		auto frame = new FrameBG();
		topFrame->upper = frame;
		frame->lower = topFrame;
		frame->frameIdx = topFrame->frameIdx + 1;

		//��ȿ�� �ؽ��� �ε��� �� ��, ĳ���� �ε�
		if (0 <= frame->frameIdx && frame->frameIdx <= 14) {
			TextureCache::getInstance()->addImage(StringUtils::format("BackGround/BG_%d.png", frame->frameIdx));
		}
		topFrame = topFrame->upper;	//top Ŀ���� ��ĭ �ø�
	}

	curFrame = bottomFrame->upper;	//curFrame�� bottom �ٷ� ����
}




//���ο� ���� ���� �� ������ ȣ��
void BGManager::loadBG()
{
	layerBG = Layer::create();
	parallax = ParallaxNode::create();

	int cur_idx = curKarma * KARMA_BG_FACTOR;					//curFrame�� �ε���
	float cur_offset = (curKarma * KARMA_BG_FACTOR) - cur_idx;	//curFrame �ȿ����� offset


	//bottom frame ���� ���������� ��������� ����
	auto iter = bottomFrame;
	while (iter)
	{
		if (0 <= iter->frameIdx && iter->frameIdx <= 14)
		{
			iter->sprFrame = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey(StringUtils::format("BackGround/BG_%d.png", iter->frameIdx)));
			iter->sprFrame->setAnchorPoint(Point::ZERO);
			iter->sprFrame->setOpacity(160);
			iter->sprFrame->setScale(DivForHorizontal(iter->sprFrame) * 1.25f, DivForVertical(iter->sprFrame));
			iter->sprFrame->setPosition(Point(-winSize().width / 8, -cur_offset + (iter->frameIdx - cur_idx) * winSize().height));
			layerBG->addChild(iter->sprFrame);
		}
		iter = iter->upper;
	}
	parallax->addChild(layerBG, 1, Point(0.25f, 0.5f), Point::ZERO);	//** ���ι�� ��ġ
}



void BGManager::moveBG(float input_karma)
{
	float rounded_input = input_karma;

	if (curKarma + input_karma > 1000) {
		rounded_input = 1000 - curKarma;
		isBossFlag = true;
	}


	if (!isBossFlag && curKarma < 1000)
	{
		int cur_idx = curKarma * KARMA_BG_FACTOR;	//���� �κ��� index�� ����
		float cur_offset = curKarma * KARMA_BG_FACTOR - cur_idx;	//������ �κ��� offset���� ����
		float move_dist = rounded_input * KARMA_BG_FACTOR;


		//���� index�� ��ȭ ����Ƿ�, ������ �籸�� (bottom�� top����)
		if (cur_offset + move_dist > 1)
		{
			//bottom�� ��ȿ�� BG �ؽ��� �־�����, ĳ���ϰ� ��������Ʈ�� ����
			if (bottomFrame->sprFrame)
			{
				TextureCache::getInstance()->removeTextureForKey(StringUtils::format("BackGround/BG_%d.png", bottomFrame->frameIdx));
				bottomFrame->sprFrame->removeFromParentAndCleanup(true);
				bottomFrame->sprFrame = NULL;
			}

			//bottom frame�� ���ο� top frame����
			auto next_bottom = bottomFrame->upper;

			bottomFrame->upper = NULL;
			bottomFrame->lower = topFrame;
			bottomFrame->frameIdx = topFrame->frameIdx + 1;
			topFrame->upper = bottomFrame;

			topFrame = bottomFrame;
			bottomFrame = next_bottom;

			//���ο� top frame�� �ؽ��� Ȯ���Ͽ� ����
			if (topFrame->frameIdx <= 14)
			{
				TextureCache::getInstance()->addImage(StringUtils::format("BackGround/BG_%d.png", topFrame->frameIdx));
				topFrame->sprFrame = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey(StringUtils::format("BackGround/BG_%d.png", topFrame->frameIdx)));
				topFrame->sprFrame->setAnchorPoint(Point::ZERO);
				topFrame->sprFrame->setOpacity(160);
				topFrame->sprFrame->setScale(DivForHorizontal(topFrame->sprFrame)  * 1.25f, DivForVertical(topFrame->sprFrame));
				topFrame->sprFrame->setPosition(Point(-winSize().width / 8, topFrame->lower->sprFrame->getBoundingBox().getMaxY()));

				layerBG->addChild(topFrame->sprFrame);
			}

			//Ŀ�� ��ġ���� �ٲ� �����ν�, �Ϸ�
			curFrame->lower = NULL;
			curFrame = curFrame->upper;

			//CCLOG("==================================================");
			//auto iter = topFrame;
			//while (iter)
			//{
			//	if (iter->sprFrame)
			//		CCLOG("[%d]frame is exist!! and sprite also exist!", iter->frameIdx);
			//	else
			//		CCLOG("[%d]frame is exist!! But Sprite doesn't exist!", iter->frameIdx);
			//	iter = iter->lower;
			//}
			//CCLOG("==================================================");
		}

		auto move = MoveBy::create(0.7f, Vec2(0, -rounded_input * KARMA_BG_FACTOR * winSize().height * 2.0f));
		curKarma += input_karma;
		parallax->runAction(move);
	}
	return;
}



