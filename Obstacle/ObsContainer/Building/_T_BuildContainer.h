#ifndef __T_BUILDING_CONTAINER_H__
#define __T_BUILDING_CONTAINER_H__

#include "cocos2d.h"
#include "Obstacle/ObsContainer/_ABS_ObsContainer.h"
#include "Obstacle/ObsUnit/Block/_T_BlockUnit.h"

USING_NS_CC;
using namespace std;

class BuildContainer : public ObsContainer
{
	friend class BlockUnit;


protected:

	Size cropBlkSize = Size::ZERO;	//텍스쳐로부터 crop 하는 블록 너비와 높이
	int numCol = -1;				//column 수
	int numRow = -1;				//row 수
	int unitStrength = -1;			//(구성 유닛 하나의) 강도

	float curGravity = 0;			//(deltaTime에 대한) (y축)가속도
	float maxVeloY = 0;				//(deltaTime에 대한) (y축)속도
	float minVeloY = 0;
	float curVeloY = 0;

	float unitScaleX;			//각 유닛의 정사각형 scaling을 위해
	float unitLength;
	float rigidFactor = 1.0f;	//(공격 시)경직효과를 내는 상수	
	int brokenCnt;				// 깨진 unit 갯수

	int frameDamage = 0;


	/* 빌딩을 형성하는 블럭 배열 (row * col)
	2차원 형태가 직관적이지만, 메모리 연속 저장을 위해 1차원 저장하여 2차원에 맵핑*/
	BlockUnit* blkArray;
	//vector<BlockUnit> blkArray = {};


	/****** ObsUnit 구성요소를 Container로 일괄 처리하기 위한 이중버퍼 *******/
	vector<int> bufferRemove = {};
	vector<int> bufferCrashDefense = {};
	vector<int> bufferCrashX = {};
	vector<int> bufferCrashY = {};


	//BFS에 사용되는 두개의 스택
	vector<int>* bfsMainStack = NULL;
	vector<int>* bfsSubStack = NULL;

	//Chunk ID - 소속 Block 포인터
	vector<vector<int>> bufferChunkPositive = {};	//chunkPhaseFlag - true
	vector<vector<int>> bufferChunkNegative = {};	//chunkPhaseFlag - false

	bool chunkingPhaseFlag;		// true => 양수id 발급  |  false => 음수id 발급
	bool chunkingFinishFlag;	// (1 프레임 중) Chunking이 완료됐는지
	int chunkingCnt;			// (1 프레임 중) 몇개의 블록을 분류했는지 (DFS 호출횟수에 따라)
	int curChunkID;				// (1 프레임 중) 가장 최근 발급된 id


public:

	virtual void onEnterTransitionDidFinish();	//addChild 완료 후 호출
	BuildContainer(BatchElement& batch_elem, Character& main_char, ParticlePool& ruins_pool);

	virtual ~BuildContainer()
	{
		delete(bfsMainStack);
		delete(bfsSubStack);

		bufferChunkPositive.clear();
		bufferChunkNegative.clear();
		///배열 생성/삭제보다, 벡터 clear가 더 프레임 안정적...(왜지?)
		delete[] blkArray;
	};


	void setMold(float scale_factor, int row_num, int col_num);		//크기 & 유닛 수 셋팅 
	void setSpec(float delta_g, float max_dt, float min_dt, int unit_stren);	//건물 상세스펙 셋팅

	float getVeloY() { return curVeloY; };
	void setVeloY(float delta_velo) { this->curVeloY = delta_velo; }
	float getGravity() { return curGravity; }
	void setGravity(float gravity) { this->curGravity = gravity; }

	void setInitPos(Point& init_pos) { initPos = init_pos; };
	void initBlkFrames();
	void buildBlocks(Point& init_pos);


	int getNumRow() { return numRow; }
	int getNumCol() { return numCol; }
	void setBrokenCnt(int broken_cnt) { this->brokenCnt = broken_cnt; };
	int getBrokenCnt() { return this->brokenCnt; };
	void getRigidTime(float frame_damage);


	Vec2 getCrashBounceVec();	//충돌 시 발생하는 캐릭터 bounce를 vec2로 리턴
	void dumpCrashBuffer();		//충돌 버퍼 일괄 처리
	void dumpRemoveBuffer();	//파괴 버퍼 일괄 처리

	
	/*** chunking 관련 함수 ***/
	bool getChunkingFinish() { return chunkingFinishFlag; };
	void setChunkingFinish(bool flag) { chunkingFinishFlag = flag; }
	int getChunkingCnt() { return this->chunkingCnt; }
	void setChunkingCnt(int count) { chunkingCnt = count; }
	bool getChunkingPhase() { return chunkingPhaseFlag; };
	void resetChunkingPhase();		//chunking phase를 전환
	int getNewChunkID();			//chunking phase에 따라 적절한 id 발급

	void chunkBlocks();		//연결된 블록끼리 묶음
	vector<int>& breadthSearch(int chunk_id, vector<int>& main_stack);
	void BFS(int chunk_id, int caller_idx);



	inline int get1DIndex(int row_idx, int col_idx) { return row_idx * numCol + col_idx; };
	inline pair<int, int> get2DIndex(int linked_idx) { return pair<int, int> {linked_idx / numCol, linked_idx % numCol}; };
	inline int getAliveUnitCnt() { return numRow * numCol - brokenCnt; };
	
	//현재 Phase에 맞는 Chunk Buffer 리턴
	inline vector<vector<int>>& getBufferChunk() {
		if (chunkingPhaseFlag)	return bufferChunkPositive;
		else
			return bufferChunkNegative;
	}; 


	virtual void update(float deltaTime) {};
	virtual BuildContainer& spawnChild(BuildContainer& mate) = 0;

	//virtual void updateVelocity(float deltaTime) {};
	//virtual void getDamageStiff(float frame_damage) {};
};

#endif

