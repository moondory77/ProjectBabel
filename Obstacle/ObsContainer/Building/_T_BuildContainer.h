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

	Size cropBlkSize = Size::ZERO;	//�ؽ��ķκ��� crop �ϴ� ��� �ʺ�� ����
	int numCol = -1;				//column ��
	int numRow = -1;				//row ��
	int unitStrength = -1;			//(���� ���� �ϳ���) ����

	float curGravity = 0;			//(deltaTime�� ����) (y��)���ӵ�
	float maxVeloY = 0;				//(deltaTime�� ����) (y��)�ӵ�
	float minVeloY = 0;
	float curVeloY = 0;

	float unitScaleX;			//�� ������ ���簢�� scaling�� ����
	float unitLength;
	float rigidFactor = 1.0f;	//(���� ��)����ȿ���� ���� ���	
	int brokenCnt;				// ���� unit ����

	int frameDamage = 0;


	/* ������ �����ϴ� �� �迭 (row * col)
	2���� ���°� ������������, �޸� ���� ������ ���� 1���� �����Ͽ� 2������ ����*/
	BlockUnit* blkArray;
	//vector<BlockUnit> blkArray = {};


	/****** ObsUnit ������Ҹ� Container�� �ϰ� ó���ϱ� ���� ���߹��� *******/
	vector<int> bufferRemove = {};
	vector<int> bufferCrashDefense = {};
	vector<int> bufferCrashX = {};
	vector<int> bufferCrashY = {};


	//BFS�� ���Ǵ� �ΰ��� ����
	vector<int>* bfsMainStack = NULL;
	vector<int>* bfsSubStack = NULL;

	//Chunk ID - �Ҽ� Block ������
	vector<vector<int>> bufferChunkPositive = {};	//chunkPhaseFlag - true
	vector<vector<int>> bufferChunkNegative = {};	//chunkPhaseFlag - false

	bool chunkingPhaseFlag;		// true => ���id �߱�  |  false => ����id �߱�
	bool chunkingFinishFlag;	// (1 ������ ��) Chunking�� �Ϸ�ƴ���
	int chunkingCnt;			// (1 ������ ��) ��� ����� �з��ߴ��� (DFS ȣ��Ƚ���� ����)
	int curChunkID;				// (1 ������ ��) ���� �ֱ� �߱޵� id


public:

	virtual void onEnterTransitionDidFinish();	//addChild �Ϸ� �� ȣ��
	BuildContainer(BatchElement& batch_elem, Character& main_char, ParticlePool& ruins_pool);

	virtual ~BuildContainer()
	{
		delete(bfsMainStack);
		delete(bfsSubStack);

		bufferChunkPositive.clear();
		bufferChunkNegative.clear();
		///�迭 ����/��������, ���� clear�� �� ������ ������...(����?)
		delete[] blkArray;
	};


	void setMold(float scale_factor, int row_num, int col_num);		//ũ�� & ���� �� ���� 
	void setSpec(float delta_g, float max_dt, float min_dt, int unit_stren);	//�ǹ� �󼼽��� ����

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


	Vec2 getCrashBounceVec();	//�浹 �� �߻��ϴ� ĳ���� bounce�� vec2�� ����
	void dumpCrashBuffer();		//�浹 ���� �ϰ� ó��
	void dumpRemoveBuffer();	//�ı� ���� �ϰ� ó��

	
	/*** chunking ���� �Լ� ***/
	bool getChunkingFinish() { return chunkingFinishFlag; };
	void setChunkingFinish(bool flag) { chunkingFinishFlag = flag; }
	int getChunkingCnt() { return this->chunkingCnt; }
	void setChunkingCnt(int count) { chunkingCnt = count; }
	bool getChunkingPhase() { return chunkingPhaseFlag; };
	void resetChunkingPhase();		//chunking phase�� ��ȯ
	int getNewChunkID();			//chunking phase�� ���� ������ id �߱�

	void chunkBlocks();		//����� ��ϳ��� ����
	vector<int>& breadthSearch(int chunk_id, vector<int>& main_stack);
	void BFS(int chunk_id, int caller_idx);



	inline int get1DIndex(int row_idx, int col_idx) { return row_idx * numCol + col_idx; };
	inline pair<int, int> get2DIndex(int linked_idx) { return pair<int, int> {linked_idx / numCol, linked_idx % numCol}; };
	inline int getAliveUnitCnt() { return numRow * numCol - brokenCnt; };
	
	//���� Phase�� �´� Chunk Buffer ����
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

