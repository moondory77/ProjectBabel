#include "_T_BuildContainer.h"
#include "System/GameManager.h"
#include "VFX/EffectManager.h"

///생성자는 최대한 경량으로..
BuildContainer::BuildContainer(ObsBatcher& obs_batcher, Character& main_char, ParticlePool& ruins_pool)
	: ObsContainer(obs_batcher, main_char, ruins_pool)
{
	rigidFactor = 1.0f;
	curVeloY = 0.0f;
	curChunkID = 0;
	chunkingFinishFlag = false;
	chunkingPhaseFlag = true;
	chunkingCnt = 0;
	frameDamage = 0;
	brokenCnt = 0;
};

//구성 유닛의 scale과 갯수 확정
void BuildContainer::setMold(float scale_factor, int row_num, int col_num)
{
	scaleFactor = scale_factor;
	numRow = row_num;
	numCol = col_num;

	//Row * Col에 맞춰 Building Texture에 crop해야 할 크기 계산
	cropBlkSize.width = obsBatcher.unitSize.width / numCol;
	cropBlkSize.height = obsBatcher.unitSize.height / numRow;

	//유닛 Sprite 정사각형 scaling
	unitScaleX = scaleFactor * (winSize().width / obsBatcher.unitSize.width);
	unitLength = scaleFactor * winSize().width / numCol;
}

//인스턴스의 상세 specification 설정
void BuildContainer::setSpec(float delta_g, float max_dt, float min_dt, int unit_stren)
{
	curGravity = delta_g;
	maxVeloY = max_dt;
	minVeloY = min_dt;
	unitStrength = unit_stren;
};


//각 유닛 인스턴스 생성
void BuildContainer::initBlkFrames()
{
	blkArray = new BlockUnit[numRow * numCol];
	auto tex = obsBatcher.getTexture();

	for (int i = 0; i < 4; i++)
	{
		//해당 강도의 건물프레임 기준좌표 (좌측 상단)
		auto unit_frame = obsBatcher.unitFrame.at(i);
		Point datum_pos = Point(unit_frame->getRect().getMinX(), unit_frame->getRect().getMinY());

		for (int j = 0; j != numRow * numCol; j++)
		{
			/* 정해진 unit size에 맞게 텍스쳐로부터 이미지를 crop
			해당 좌표의 빌딩타일을 crop 해서 저장 */
			blkArray[j].sprUnitFrames.pushBack(
				SpriteFrame::createWithTexture(tex,
					Rect(datum_pos.x + cropBlkSize.width * get2DIndex(j).second,
						datum_pos.y + cropBlkSize.height * get2DIndex(j).first,
						cropBlkSize.width, cropBlkSize.height)));
		}
	}
	bfsMainStack = new vector<int>();
	bfsSubStack = new vector<int>();

}

//각 유닛을 순회하며, 상세 specification 설정 후, 렌더링 시작
void BuildContainer::buildBlocks(Point& init_pos)
{
	for (int i = 0; i < numRow * numCol; i++)
	{
		int row_idx = get2DIndex(i).first;
		int col_idx = get2DIndex(i).second;

		blkArray[i].sprUnit = Sprite::create();
		
		///각 블럭 스프라이트를 정사각형으로 강제 scailing
		blkArray[i].sprUnit->setScaleX(unitScaleX);
		blkArray[i].sprUnit->setScaleY(unitScaleX * (cropBlkSize.width / cropBlkSize.height));
		blkArray[i].sprUnit->setSpriteFrame(blkArray[i].sprUnitFrames.at(0));
		blkArray[i].sprUnit->setAnchorPoint(Point(0, 1));
		blkArray[i].sprUnit->setPosition(init_pos.x + unitLength * col_idx, init_pos.y - unitLength * row_idx);
		///렌더링 sprite만 배치노드 아래로 삽입
		obsBatcher.addChild(blkArray[i].sprUnit);	///해당 시점부터 렌더링
	
		blkArray[i].isDefensibleFlag = true;
		blkArray[i].maxStrength = this->unitStrength;
		blkArray[i].curStrength = this->unitStrength;
		blkArray[i].unitIdx = i;
		blkArray[i].mainChar = &mainChar;
		blkArray[i].container = this;
		blkArray[i].isAliveFlag = true;

		//상, 하, 좌, 우 Block과 연결 (기본 Chunk 설정)
		static int row_num[] = { 1, 0, -1, 0 };
		static int col_num[] = { 0, 1, 0, -1 };
		for (int j = 0; j < 4; j++)
		{
			int row_joint = row_idx + row_num[j];
			int col_joint = col_idx + col_num[j];

			//CCLOG("joint %d [%d][%d]", i, row_joint, col_joint);
			if (0 <= row_joint && row_joint < numRow
				&& 0 <= col_joint && col_joint < numCol)
			{
				blkArray[i].linkedUnit.insert(&blkArray[get1DIndex(row_joint, col_joint)]);
			}
		}
	}
	featuredFlag = true;
}

void BuildContainer::onEnterTransitionDidFinish()
{
	if (!featuredFlag) {
		buildBlocks(initPos);	
	}
	for (int i = 0; i < 2; i++)
		bufferCrash[i].clear();
	isAliveFlag = true;		//해당 시점부터 update
}


//chunk 단위로 묶인 블록에 대해 충돌검사
Vec2 BuildContainer::getBounceVec(set<int>& unit_buffer, CollisionDetector& collider)
{
	int cursor = *unit_buffer.begin();

	//유닛의 충돌체에 대한 침투벡터를 계산 (frame 사이의, 두 오브젝트는 선운동이라 가정한다)
	Vec2 prev_relative_pos = blkArray[cursor].CrashInfo.prevPos - mainChar.getPrevPos();
	Vec2 cur_relative_pos = blkArray[cursor].CrashInfo.curPos - mainChar.bodyCollider->getPosition();
	
	Vec2 penetrate_direction = cur_relative_pos - prev_relative_pos;

	//침투 방향에 따른, chunking 순서를 선택 (행/열)
	bool row_first = fabsf(penetrate_direction.x) < fabsf(penetrate_direction.y);

	// first - 분리축, second - 침투구간
	map<Vec2, pair<float, float>> p_table = {};
	
	//행-열 chunking 하면서, total 침투구간 갱신
	while (!unit_buffer.empty())
	{
		cursor = *unit_buffer.begin();
		//루프를 순회하며, sub chunk에 대한 경계 구간(일반 축 상에서)
		pair<float, float> seg_x = blkArray[cursor].CrashInfo.boundX;
		pair<float, float> seg_y = blkArray[cursor].CrashInfo.boundY;
		unit_buffer.erase(cursor);
		
		//행-열 우선 순위에 따라 chunking 시도
		if (row_first)
		{
			//행-방향으로 유닛 chunking
			if (unit_buffer.find((cursor + 1) % numCol) != unit_buffer.end()) 
			{
				while (++cursor % numCol != 0 && unit_buffer.find(cursor) != unit_buffer.end()) {
					updateBound(seg_x, blkArray[cursor].CrashInfo.boundX);
					unit_buffer.erase(cursor);

				}
			}
			//열 방향으로 유닛 scan
			else if (unit_buffer.find(cursor + numCol) != unit_buffer.end()) {
				cursor += numCol;
				while (unit_buffer.find(cursor) != unit_buffer.end())
				{
					updateBound(seg_y, blkArray[cursor].CrashInfo.boundY);
					unit_buffer.erase(cursor);
					cursor += numCol;
				}
			}
		}
		else
		{
			if (unit_buffer.find(cursor + numCol) != unit_buffer.end()) {
				cursor += numCol;
				while (unit_buffer.find(cursor) != unit_buffer.end()) {
					updateBound(seg_y, blkArray[cursor].CrashInfo.boundY);
					unit_buffer.erase(cursor);
					cursor += numCol;
				}
			}
			else if (unit_buffer.find((cursor + 1) % numCol) != unit_buffer.end()) {
				while (++cursor % numCol != 0 && unit_buffer.find(cursor) != unit_buffer.end()) {
					updateBound(seg_x, blkArray[cursor].CrashInfo.boundX);
					unit_buffer.erase(cursor);
				}
			}
		}

		//CCLOG("seg_x [%.2f, %.2f]", seg_x.first, seg_x.second);
		//CCLOG("seg_y [%.2f, %.2f]", seg_y.first, seg_y.second);

		vector<Vec2> seg_vertices = {
			this->convertToWorldSpace(Vec2(seg_x.first, seg_y.first)),
			this->convertToWorldSpace(Vec2(seg_x.second, seg_y.first)),
			this->convertToWorldSpace(Vec2(seg_x.second, seg_y.second)),
			this->convertToWorldSpace(Vec2(seg_x.first, seg_y.second))
		};

		Vec2 cur_axis = Vec2::ZERO;
		pair<float, float> cur_bound = collider.getMinPenetrateBound(penetrate_direction, seg_vertices, cur_axis);
		float depth = cur_bound.second - cur_bound.first;
		
		CCLOG("sub bounce vector is [%.2f, %.2f]", cur_axis.x, cur_axis.y);
		CCLOG("--> bound is [%.2f, %.2f]", cur_bound.first, cur_bound.second);

		if (p_table.find(cur_axis) == p_table.end()){
			p_table.insert({ cur_axis, cur_bound });
		}
		else
		{
			if (cur_bound.second - cur_bound.first > 0) {
				p_table[cur_axis].first = max(p_table[cur_axis].first, cur_bound.first);
				p_table[cur_axis].second = min(p_table[cur_axis].second, cur_bound.second);
			}
			else{
				p_table[cur_axis].first = min(p_table[cur_axis].first, cur_bound.first);
				p_table[cur_axis].second = max(p_table[cur_axis].second, cur_bound.second);
			}
		}
		//CCLOG("%d'th chunk check, composed by %d unit", i, unit_num);
	}

	float bounce_x = 0;
	float bounce_y = 0;
	auto iter = p_table.begin();
	while (iter != p_table.end())
	{
		Vec2 unit_p_vec = (*iter).first;
		//CCLOG("unit norm vector is (%.2f, %.2f)", unit_p_vec.x, unit_p_vec.y);
		float unit_p_depth = (*iter).second.second - (*iter).second.first;
		//CCLOG("unit depth range is [%.2f -> %.2f]", (*iter).second.first, (*iter).second.second);
		unit_p_vec *= unit_p_depth;
		bounce_x += unit_p_vec.x;
		bounce_y += unit_p_vec.y;
		iter++;
	}

	return Vec2(bounce_x, bounce_y);
}

Vec2 BuildContainer::getBounceVec(set<int>& unit_buffer, Rect& bounding_box)
{
	int cursor = *unit_buffer.begin();

	//유닛의 충돌체에 대한 침투벡터를 계산(frame에, 각 유닛은 동일한 선운동)
	Vec2 prev_relative_pos = blkArray[cursor].CrashInfo.prevPos - mainChar.getPrevPos();
	Vec2 cur_relative_pos = blkArray[cursor].CrashInfo.curPos - mainChar.bodyCollider->getPosition();
	Vec2 penetrate_direction = cur_relative_pos - prev_relative_pos;

	//입력 AABB의 x-y축 경계범위
	pair<float, float> bound_x = { bounding_box.getMinX(), bounding_box.getMaxX() };
	pair<float, float> bound_y = { bounding_box.getMinY(), bounding_box.getMaxY() };

	//각 축별로, 침투구간의 총합
	pair<float, float> total_x = { 0,0 };
	pair<float, float> total_y = { 0,0 };

	//침투 방향에 따른, chunking 순서를 선택 (행/열)
	bool row_first = fabsf(penetrate_direction.x) < fabsf(penetrate_direction.y);

	//행-열 chunking 하면서, total 침투구간 갱신
	while (!unit_buffer.empty())
	{
		cursor = *unit_buffer.begin();
		//루프를 순회하며, sub chunk에 대한 경계 구간(일반 축 상에서)
		pair<float, float> seg_x = blkArray[cursor].CrashInfo.boundX;
		pair<float, float> seg_y = blkArray[cursor].CrashInfo.boundY;
		unit_buffer.erase(cursor);

		//행-열 우선 순위에 따라 chunking 시도
		if (row_first)
		{
			//행-방향으로 유닛 chunking
			if (unit_buffer.find((cursor + 1) % numCol) != unit_buffer.end()){
				while (++cursor % numCol != 0 && unit_buffer.find(cursor) != unit_buffer.end()) {
					updateBound(seg_x, blkArray[cursor].CrashInfo.boundX);
					unit_buffer.erase(cursor);
				}
			}
			//열 방향으로 유닛 scan
			else if (unit_buffer.find(cursor + numCol) != unit_buffer.end()){
				cursor += numCol;
				while (unit_buffer.find(cursor) != unit_buffer.end())
				{
					updateBound(seg_y, blkArray[cursor].CrashInfo.boundY);
					unit_buffer.erase(cursor);
					cursor += numCol;
				}
			}
		}
		else
		{
			if (unit_buffer.find(cursor + numCol) != unit_buffer.end()){
				cursor += numCol;
				while (unit_buffer.find(cursor) != unit_buffer.end()) {
					updateBound(seg_y, blkArray[cursor].CrashInfo.boundY);
					unit_buffer.erase(cursor);
					cursor += numCol;
				}
			}
			else if (unit_buffer.find((cursor + 1) % numCol) != unit_buffer.end()){
				while (++cursor % numCol != 0 && unit_buffer.find(cursor) != unit_buffer.end()) {
					updateBound(seg_x, blkArray[cursor].CrashInfo.boundX);
					unit_buffer.erase(cursor);
				}
			}
		}


		//(운동방향을 고려해) x 방향 침투 구간 탐색
		pair<float, float> p_seg_x = { 0, 0 };
		
		if (penetrate_direction.x < 0) {
			p_seg_x = { bound_x.second, seg_x.first };
		}
		else if (penetrate_direction.x > 0) {
			p_seg_x = { bound_x.first, seg_x.second };
		}
		else {
			float negative = seg_x.first - bound_x.second;
			float positive = seg_x.second - bound_x.first;

			if (fabsf(negative) < fabsf(positive)) {
				p_seg_x = { bound_x.second,  seg_x.first };
			}
			else {
				p_seg_x = { bound_x.first, seg_x.second };
			}
		}

		//y축 방향 침투 구간 탐색
		pair<float, float> p_seg_y = { 0, 0 };
		
		if (penetrate_direction.y < 0) {
			p_seg_y = { bound_y.second, seg_y.first };
		}
		else if (penetrate_direction.y > 0) {
			p_seg_y = { bound_y.first, seg_y.second };
		}
		else {
			float negative = seg_y.first - bound_y.second;
			float positive = seg_y.second - bound_y.first;

			if (fabsf(negative) < fabsf(positive)) {
				p_seg_y = { bound_y.second,  seg_y.first };
			}
			else {
				p_seg_y = { bound_y.first, seg_y.second };
			}
		}


		float depth_x = p_seg_x.second - p_seg_x.first;
		float depth_y = p_seg_y.second - p_seg_y.first;

		//더 작은 depth 값을 축으로 선택 및 갱신
		if (fabsf(depth_x) < fabsf(depth_y))
		{
			if (total_x.first == 0 && total_x.second == 0) {
				total_x = p_seg_x;
			}
			else {
				total_x.first = p_seg_x.first;
				if (penetrate_direction.x < 0)
					total_x.second = total_x.second < p_seg_x.second ? total_x.second : p_seg_x.second;
				else
					total_x.second = total_x.second > p_seg_x.second ? total_x.second : p_seg_x.second;
			}
		}
		else if (fabsf(depth_x) > fabsf(depth_y))
		{
			if (total_y.first == 0 && total_y.second == 0) {
				total_y = p_seg_y;
			}
			else {
				total_y.first = p_seg_y.first;
				if (penetrate_direction.y <= 0)
					total_y.second = total_y.second < p_seg_y.second ? total_y.second : p_seg_y.second;
				else
					total_y.second = total_y.second > p_seg_y.second ? total_y.second : p_seg_y.second;
			}
		}
		else
		{
			if (row_first) {
				if (total_y.first == 0 && total_y.second == 0) {
					total_y = p_seg_y;
				}
				else {
					total_y.first = p_seg_y.first;
					if (penetrate_direction.y <= 0)
						total_y.second = total_y.second < p_seg_y.second ? total_y.second : p_seg_y.second;
					else
						total_y.second = total_y.second > p_seg_y.second ? total_y.second : p_seg_y.second;
				}
			}
			else {
				if (total_x.first == 0 && total_x.second == 0) {
					total_x = p_seg_x;
				}
				else {
					total_x.first = p_seg_x.first;
					if (penetrate_direction.x < 0)
						total_x.second = total_x.second < p_seg_x.second ? total_x.second : p_seg_x.second;
					else
						total_x.second = total_x.second > p_seg_x.second ? total_x.second : p_seg_x.second;
				}
			}
		}
	}

	float bounce_x = total_x.second - total_x.first;
	float bounce_y = total_y.second - total_y.first;

	//CCLOG("Bounce Vector (%.2f, %.2f)", bounce_x, bounce_y);
	return Vec2(bounce_x, bounce_y);
}



//
////해당 프레임에 충돌한 유닛들을 scan하여, bounce vector 리턴
//Vec2 BuildContainer::getPenetrateVec()
//{
//	int cursor = *bufferCrash.begin();
//	float blk_velo = getVeloY() + blkArray[cursor].curRepulsion;
//
//	//침투벡터의 방향
//	Vec2 prev_relative_pos = blkArray[cursor].CrashInfo.prevPos - mainChar.getPrevPos();
//	Vec2 cur_relative_pos = blkArray[cursor].CrashInfo.curPos - mainChar.bodyCollider->getPosition();
//	Vec2 penetrate_direction = cur_relative_pos - prev_relative_pos;
//
//	pair<float, float> body_x = { mainChar.bodyCollider->getLeftX(),   mainChar.bodyCollider->getRightX() };
//	pair<float, float> body_y = { mainChar.bodyCollider->getBottomY(),   mainChar.bodyCollider->getTopY() };
//
//	pair<float, float> total_x = { 0,0 };
//	pair<float, float> total_y = { 0,0 };
//
//	//블록 침투방향에 따른, chunking 순서 선택(행/열)
//	bool row_first = fabsf(penetrate_direction.x) < fabsf(penetrate_direction.y);
//
//	while (!bufferCrash.empty())
//	{
//		cursor = *bufferCrash.begin();
//		pair<float, float> segment_x = blkArray[cursor].CrashInfo.boundX;
//		pair<float, float> segment_y = blkArray[cursor].CrashInfo.boundY;
//		bufferCrash.erase(cursor);
//
//		//행-열 우선 순위에 따라 chunking 시도
//		if (row_first)
//		{
//			//행-방향으로 유닛 chunking
//			if (bufferCrash.find((cursor + 1) % numCol) != bufferCrash.end())
//			{
//				while (++cursor % numCol != 0 && bufferCrash.find(cursor) != bufferCrash.end()) {
//					updateBound(segment_x, blkArray[cursor].CrashInfo.boundX);
//					bufferCrash.erase(cursor);
//				}
//			}
//			//열 방향으로 유닛 scan
//			else if (bufferCrash.find(cursor + numCol) != bufferCrash.end())
//			{
//				cursor += numCol;
//				while (bufferCrash.find(cursor) != bufferCrash.end())
//				{
//					updateBound(segment_y, blkArray[cursor].CrashInfo.boundY);
//					bufferCrash.erase(cursor);
//					cursor += numCol;
//				}
//			}
//		}
//		else
//		{
//			if (bufferCrash.find(cursor + numCol) != bufferCrash.end())
//			{
//				cursor += numCol;
//				while (bufferCrash.find(cursor) != bufferCrash.end()) {
//					updateBound(segment_y, blkArray[cursor].CrashInfo.boundY);
//					bufferCrash.erase(cursor);
//					cursor += numCol;
//				}
//			}
//			else if (bufferCrash.find((cursor + 1) % numCol) != bufferCrash.end())
//			{
//				while (++cursor % numCol != 0 && bufferCrash.find(cursor) != bufferCrash.end()) {
//					updateBound(segment_x, blkArray[cursor].CrashInfo.boundX);
//					bufferCrash.erase(cursor);
//				}
//			}
//		}
//
//
//		//x축 방향 침투 구간 탐색
//		pair<float, float> penetrate_x = { 0, 0 };
//		if (penetrate_direction.x < 0) {
//			penetrate_x = { body_x.second, segment_x.first };
//		}
//		else if (penetrate_direction.x > 0) {
//			penetrate_x = { body_x.first, segment_x.second };
//		}
//		else {
//			float negative = segment_x.first - body_x.second;
//			float positive = segment_x.second - body_x.first;
//
//			if (fabsf(negative) < fabsf(positive)) {
//				penetrate_x = { body_x.second,  segment_x.first };
//			}
//			else {
//				penetrate_x = { body_x.first, segment_x.second };
//			}
//		}
//
//		//y축 방향 침투 구간 탐색
//		pair<float, float> penetrate_y = { 0, 0 };
//		if (penetrate_direction.y < 0) {
//			penetrate_y = { body_y.second, segment_y.first };
//		}
//		else if (penetrate_direction.y > 0) {
//			penetrate_y = { body_y.first, segment_y.second };
//		}
//		else {
//			float negative = segment_y.first - body_y.second;
//			float positive = segment_y.second - body_y.first;
//
//			if (fabsf(negative) < fabsf(positive)) {
//				penetrate_y = { body_y.second,  segment_y.first };
//			}
//			else {
//				penetrate_x = { body_y.first, segment_y.second };
//			}
//		}
//
//
//		float depth_x = penetrate_x.second - penetrate_x.first;
//		float depth_y = penetrate_y.second - penetrate_y.first;
//
//		//더 작은 depth 값을 축으로 선택 및 갱신
//		if (fabsf(depth_x) < fabsf(depth_y))
//		{
//			if (total_x.first == 0 && total_x.second == 0) {
//				total_x = penetrate_x;
//			}
//			else {
//				total_x.first = penetrate_x.first;
//				if (penetrate_direction.x < 0)
//					total_x.second = total_x.second < penetrate_x.second ? total_x.second : penetrate_x.second;
//				else
//					total_x.second = total_x.second > penetrate_x.second ? total_x.second : penetrate_x.second;
//			}
//		}
//		else if (fabsf(depth_x) > fabsf(depth_y))
//		{
//			if (total_y.first == 0 && total_y.second == 0) {
//				total_y = penetrate_y;
//			}
//			else {
//				total_y.first = penetrate_y.first;
//				if (penetrate_direction.y <= 0)
//					total_y.second = total_y.second < penetrate_y.second ? total_y.second : penetrate_y.second;
//				else
//					total_y.second = total_y.second > penetrate_y.second ? total_y.second : penetrate_y.second;
//			}
//		}
//		else
//		{
//			if (row_first) {
//				if (total_y.first == 0 && total_y.second == 0) {
//					total_y = penetrate_y;
//				}
//				else {
//					total_y.first = penetrate_y.first;
//					if (penetrate_direction.y <= 0)
//						total_y.second = total_y.second < penetrate_y.second ? total_y.second : penetrate_y.second;
//					else
//						total_y.second = total_y.second > penetrate_y.second ? total_y.second : penetrate_y.second;
//				}
//			}
//			else {
//				if (total_x.first == 0 && total_x.second == 0) {
//					total_x = penetrate_x;
//				}
//				else {
//					total_x.first = penetrate_x.first;
//					if (penetrate_direction.x < 0)
//						total_x.second = total_x.second < penetrate_x.second ? total_x.second : penetrate_x.second;
//					else
//						total_x.second = total_x.second > penetrate_x.second ? total_x.second : penetrate_x.second;
//				}
//			}
//		}
//
//	}
//
//
//	float bounce_x = total_x.second - total_x.first;
//	float bounce_y = total_y.second - total_y.first;
//
//	if (bounce_x != 0)
//		mainChar.setVeloX(mainChar.getVeloX()*0.9f);
//
//	if (bounce_y < 0) {
//		mainChar.addState(CRASH);
//		mainChar.setOuterVeloY(blk_velo);
//	}
//	else if (bounce_y != 0) {
//		mainChar.setVeloY(blk_velo*0.9f);
//	}
//
//
//	//CCLOG("Bounce Vector (%.2f, %.2f)", bounce_x, bounce_y);
//	return Vec2(bounce_x, bounce_y);
//}
//



void BuildContainer::dumpRemoveBuffer()
{
	for (auto iter = bufferRemove.begin(); iter != bufferRemove.end(); iter++)
	{
		blkArray[*iter].breakSelf();
		blkArray[*iter].linkedUnit.clear();
	}
	bufferRemove.clear();
}

int BuildContainer::getNewChunkID()
{
	//chunk ID를 새로 부여함과 동시에, Block 포인터를 저장할 버퍼 생성
	if (chunkingPhaseFlag) {
		///+(chunkID - 1) -> 버퍼 내 인덱스
		curChunkID++;
		bufferChunkPositive.push_back({});
	}
	else {
		///-(chunkID + 1) => 버퍼내 index
		curChunkID--;
		bufferChunkNegative.push_back({});
	}
	//CCLOG("new Chunk ID [%d] is generated!", curChunkID);
	return curChunkID;
}


void BuildContainer::resetChunkingPhase()
{
	curChunkID = 0;
	chunkingCnt = 0;
	chunkingFinishFlag = false;
	chunkingPhaseFlag = !chunkingPhaseFlag;

	if (chunkingPhaseFlag)
		bufferChunkPositive.clear();
	else
		bufferChunkNegative.clear();

	return;
}

void BuildContainer::BFS(int chunk_id, int caller_idx)
{
	if (!bfsMainStack->empty())
		bfsMainStack->clear();

	bfsMainStack->push_back(caller_idx);

	while (!bfsMainStack->empty() && !getChunkingFinish()) {
		bfsMainStack = &breadthSearch(chunk_id, *bfsMainStack);
	}
	return;
}
vector<int>& BuildContainer::breadthSearch(int chunk_id, vector<int>& main_stack)
{
	while (!main_stack.empty())
	{
		BlockUnit& cursor = blkArray[main_stack.back()];

		//아직 방문안한 노드인지 검사
		if (cursor.isAlive() && cursor.isVisitable())
		{
			cursor.setChunkID(chunk_id);
			chunkingCnt++;

			if (getChunkingPhase())
				bufferChunkPositive.at(chunk_id - 1).push_back(cursor.unitIdx);
			else
				bufferChunkNegative.at(-(chunk_id + 1)).push_back(cursor.unitIdx);

			//묶음분류(chunking)에 성공 할 때마다, 카운트 업. 모든 분류 시 loop 탈출
			if (chunkingCnt >= getAliveUnitCnt()) {
				chunkingFinishFlag = true;
				break;
			}

			for (auto iter = cursor.linkedUnit.begin(); iter != cursor.linkedUnit.end(); iter++) {
				if ((*iter)->isSearchable(chunk_id, cursor.getPosition()))
					bfsSubStack->push_back((*iter)->unitIdx);
			}
		}
		main_stack.pop_back();
	}

	if (!main_stack.empty())
		main_stack.clear();
	vector<int>& next_main_stack = *bfsSubStack;
	bfsSubStack = &main_stack;

	return next_main_stack;
}



void BuildContainer::chunkBlocks()
{
	chunkingFinishFlag = false;

	int i = 0;
	//Alive 상태의 모든 Block을 순회 update
	do {
		if (blkArray[i].isAlive() && blkArray[i].isVisitable())
			BFS(getNewChunkID(), i);
		i++;
	} while (!chunkingFinishFlag);


	//CCLOG("=======================================================================");
	//CCLOG("Chunking count is %d", chunkingCnt);
	//int j = 0;
	//for (auto iter = getBufferChunk().begin(); iter != getBufferChunk().end(); iter++, j++)
	//{
	//	CCLOG("Chunk [%d]'s size %d, repulsion is %f", j, (*iter).size(), (*iter).size() * 0.005f);
	//}
	//CCLOG("=======================================================================");

}
void BuildContainer::getRigidTime(float frame_damage)
{
	float rigid_time = frame_damage*0.001f;

	if (rigid_time > 0.16f)
		rigid_time = 0.16f;

	if (rigid_time > 0.09f)
	{
		rigidFactor = 0.0f;

		auto delay = DelayTime::create(rigid_time);
		auto rigid_in_attack = Sequence::create(delay,
			CallFunc::create([&]() {
			CCLOG("rigidity finish!");
			rigidFactor = 1.0f;
		}), nullptr);

		//this->runAction(rigid_in_attack);
	}
}


//void Building::DFS(int chunk_id, int linked_idx)
//{
//	//상, 하, 좌, 우 Block과 연결 (기본 Chunk 설정을 위해)
//	static int row_num[] = { -1, 0, 0, 1 };
//	static int col_num[] = { 0, -1, 1, 0 };
//
//	//int linked_idx = numCol * row + col;
//	if (chunkingCnt < getAliveUnitCnt())
//	{
//
//		chunkingCnt++;
//		blkArray[linked_idx].chunkID = chunk_id;
//
//
//		//phase 분류하여 chunk 묶기
//		if (getChunkingPhase()) {
//
//			bufferChunkPositive.at(chunk_id - 1).pushBack(&blkArray[linked_idx]);
//			blkArray[linked_idx].ptrChunkPositive = &bufferChunkPositive.at(chunk_id - 1);
//		}
//		else {
//			bufferChunkNegative.at(-(chunk_id + 1)).pushBack(&blkArray[linked_idx]);
//			blkArray[linked_idx].ptrChunkNegative = &bufferChunkNegative.at(-(chunk_id + 1));
//		}
//	}
//	else {
//		//CCLOG("DFS Finished 11111111 now chunk count %d", chunkingCnt);
//		//CCLOG("Exposion!!!");
//
//		chunkingFinishFlag = true;
//		return;
//	}
//
//
//
//
//
//	for (int i = 0; i < 4; i++)
//	{
//		int row_joint = (linked_idx / numCol) + row_num[i];
//		int col_joint = (linked_idx % numCol) + col_num[i];
//
//		if (0 <= row_joint && row_joint < numRow
//			&& 0 <= col_joint && col_joint < numCol)
//		{
//			if (blkArray[numCol * row_joint + col_joint].isVisitable(chunk_id))
//			{
//				//if ((blkArray[numCol * row_joint + col_joint].sprUnit != NULL)
//					//&& blkArray[numCol * row_joint + col_joint].isSprConnected(blkArray[linked_idx].getPosition()))
//				return DFS(chunk_id, numCol * row_joint + col_joint);
//			}
//		}
//	}
//
//
//	if (chunkingCnt >= getAliveUnitCnt())
//	{
//		CCLOG("DFS Finished!2222222222 now chunk count %d", chunkingCnt);
//		chunkingFinishFlag = true;
//		return;
//	}
//
//
//};
//
//

