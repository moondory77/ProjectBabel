#include "BGManager.h"
#include "System/GameManager.h"


//*************************** #BGManager	**************************************// 
BGManager* BGManager::BGManager_instance = NULL;

BGManager* BGManager::getInstance() {

	if (!BGManager_instance){
		BGManager_instance = new BGManager();
		BGManager_instance->initBG();
	}
	return BGManager_instance;
}


//처음 싱글톤 객체 만들어 질 때 호출 (런타임 시, 처음 한번)
void BGManager::initBG()
{
	//초기화 시, 저장되어있는 karma 양에 따라, 생성frame index 결정
	int cur_idx = curKarma * KARMA_BG_FACTOR;
	bottomFrame = new FrameBG();
	bottomFrame->frameIdx = cur_idx - 2;

	if (0 <= bottomFrame->frameIdx && bottomFrame->frameIdx <= 14) {
		TextureCache::getInstance()->addImage(StringUtils::format("BackGround/BG_%d.png", bottomFrame->frameIdx));
	}
	topFrame = bottomFrame;

	//순차적으로 FrameBG 생성
	for (int i = 0; i < 5; i++){
		auto frame = new FrameBG();
		topFrame->upper = frame;
		frame->lower = topFrame;
		frame->frameIdx = topFrame->frameIdx + 1;

		//유효한 텍스쳐 인덱스 일 시, 캐쉬에 로드
		if (0 <= frame->frameIdx && frame->frameIdx <= 14) {
			TextureCache::getInstance()->addImage(StringUtils::format("BackGround/BG_%d.png", frame->frameIdx));
		}
		topFrame = topFrame->upper;	//top 커서를 한칸 올림
	}
	curFrame = bottomFrame->upper->upper;	//curFrame은 밑에서 3번째
}




//새로운 씬에 진입 할 때마다 호출
void BGManager::loadBG()
{
	layerBG = Layer::create();
	layerBG->setScale(1.25f, 1.0f);
	layerBG->setOpacity(0);
	parallax = ParallaxNode::create();

	int cur_idx = curKarma * KARMA_BG_FACTOR;					//curFrame의 인덱스
	float cur_offset = (curKarma * KARMA_BG_FACTOR) - cur_idx;	//curFrame 안에서의 offset

	//bottom frame 부터 순차적으로 배경프레임 맵핑
	auto iter = bottomFrame;
	while (iter)
	{
		if (0 <= iter->frameIdx && iter->frameIdx <= 14)
		{
			iter->sprFrame = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey(StringUtils::format("BackGround/BG_%d.png", iter->frameIdx)));
			iter->sprFrame->setAnchorPoint(Point::ZERO);	
			iter->sprFrame->setScale(DivForHorizontal(iter->sprFrame), DivForVertical(iter->sprFrame));
			iter->sprFrame->setPositionY((iter->frameIdx - cur_idx) * winSize().height - cur_offset);
		
			iter->sprFrame->setOpacity(180);
			iter->sprFrame->setColor(Color3B::WHITE);	
			layerBG->addChild(iter->sprFrame);
		}
		iter = iter->upper;
	}
	parallax->addChild(layerBG, 1, Point(0.25f, 0.5f), Point::ZERO);	//** 메인배경 배치
}



void BGManager::moveBG(float input_karma)
{
	float rounded_input = input_karma;
	
	if (curKarma + input_karma > 1000) {
		rounded_input = 1000 - curKarma;
	}

	if (!isBossFlag && curKarma < 1000)
	{
		int cur_idx = curKarma * KARMA_BG_FACTOR;	//정수 부분을 index로 발췌
		float cur_offset = curKarma * KARMA_BG_FACTOR - cur_idx;	//나머지 부분을 offset으로 설정
		float move_dist = rounded_input * KARMA_BG_FACTOR;	
	
		int i = 0;
		float update_offset = cur_offset + move_dist;
		// 1 이상의 offset이 변화할 경우에, (정수치만큼) 프레임 재구성 (bottom이 top으로)
		while (update_offset > 1)
		{
			//Pop 할 Bottom Frame에 유효한 텍스쳐 있으면, 캐쉬와 스프라이트 삭제
			if (bottomFrame->sprFrame){
				TextureCache::getInstance()->removeTextureForKey(StringUtils::format("BackGround/BG_%d.png", bottomFrame->frameIdx));
				bottomFrame->sprFrame->removeFromParentAndCleanup(true);
				bottomFrame->sprFrame = NULL;
			}

			//bottom frame을 새로운 top frame으로
			auto next_bottom = bottomFrame->upper;
			bottomFrame->upper = NULL;
			bottomFrame->lower = topFrame;
			bottomFrame->frameIdx = topFrame->frameIdx + 1;
			topFrame->upper = bottomFrame;

			topFrame = bottomFrame;
			bottomFrame = next_bottom;

			//새로운 top frame에 텍스쳐 확인하여 맵핑
			if (topFrame->frameIdx <= 14){
				TextureCache::getInstance()->addImage(StringUtils::format("BackGround/BG_%d.png", topFrame->frameIdx));
				topFrame->sprFrame = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey(StringUtils::format("BackGround/BG_%d.png", topFrame->frameIdx)));
				topFrame->sprFrame->setAnchorPoint(Point::ZERO);
				topFrame->sprFrame->setScale(DivForHorizontal(topFrame->sprFrame), DivForVertical(topFrame->sprFrame));
				topFrame->sprFrame->setPositionY(topFrame->lower->sprFrame->getBoundingBox().getMaxY());

				topFrame->sprFrame->setOpacity(180);
				topFrame->sprFrame->setColor(Color3B::WHITE);	
				layerBG->addChild(topFrame->sprFrame);
			}
			//커서 위치까지 바꿔 줌으로써, 완료
			curFrame->lower = NULL;
			curFrame = curFrame->upper;

			update_offset--;	//update offset의 정수값을 1 내림 
			i++;
		}
		
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

		auto move = MoveBy::create(0.7f, Vec2(0, -rounded_input * KARMA_BG_FACTOR * winSize().height * 2.0f));
		parallax->runAction(move);
		curKarma += rounded_input;

		//CCLOG("%d Frame reconstructed", i);
		CCLOG("KARMA is being Piled! curKarma is %.2f", curKarma);
	}

	if (curKarma >= 1000) {
		isBossFlag = true;
		CCLOG("now it is BOSS stage!");
	}

	return;
}



