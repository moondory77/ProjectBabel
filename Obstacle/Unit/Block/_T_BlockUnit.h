#ifndef __T_BLOCK_UNIT_H__
#define __T_BLOCK_UNIT_H__

#include "cocos2d.h"
#include "OnGame/Character.h"
#include "Obstacle/Unit/_ABS_ObsUnit.h"

USING_NS_CC;
using namespace std;

class BlockUnit : public ObsUnit
{
	friend class BuildContainer;
private:

	//캐릭터 1회의 인풋에 대해 1회 state 변화를 보장하기 위한 ID
	int attackID;
	int defenseID;
	int specialID;

protected:

	BuildContainer* container;
	Character* mainChar;

	int chunkID;				//(Container)빌딩 안에서의 묶음을 나타내는 ID
	Point prevPos;			//(이전 프레임) world position
	Vec2 prevRelativePos;	//(이전 프레임) unit <-> player 상대변위	
	Vec2 curRelativePos;	//(갱신 프레임) unit <-> player 상대변위	
	float curRepulsion;	//unit 고유의 delta velocity

	Vec2 crashVec;			//(갱신 프레임) unit의 delta 변위 벡터
	float crashAngle;		//충돌직전, 캐릭터와의 상대위치 각도

	set<BlockUnit*> linkedUnit = {};	//연결되어 있는 다른 BlockUnit으로의 포인터
	bool isDefensibleFlag;
	int maxStrength;
	int curStrength;
	int unitIdx;
	//Point breakPos;

public:

	BlockUnit();	
	BlockUnit(const BlockUnit& blk) {};
	
	virtual ~BlockUnit() {};
	bool isVisitable();
	bool isSearchable(int chunk_id, const Point& pos);

	vector<int>& getUnitChunk();
	int getChunkID() { return chunkID; };
	void setChunkID(int id) { chunkID = id; }
	Vec2 getCrashVec() { return crashVec; }
	float getCrashAngle() { return crashAngle; }
	

	void setUser(Character* character) { this->mainChar = character; }
	Character* getUser() { return mainChar; };
	void setContainer(BuildContainer* host) { this->container = host; }
	BuildContainer* getContainer() { return container; }


	float getRepulsion() { return curRepulsion; }
	void setRepulsion(float delta_repulsion) { this->curRepulsion = delta_repulsion; }


	bool isDefensible() { return isDefensibleFlag; }
	int getAttackID() { return this->attackID; }
	void setAttackID(int id) { attackID = id; }
	int getDefenseID() { return this->defenseID; }
	int getSpecialID() { return this->specialID; }

	void updateForDefense();
	void updateForSpecial();
	float isSpecialRange();
	float isDefensibleRange();	//블록이 튕겨나가는 거리 리턴, 범위 밖이면 0 리턴
	bool isCrashed(Character& mainChar);


	virtual void stateUpdate(float deltaTime);
	virtual void positionUpdate(float deltaTime);	
	virtual void setPosition(Point pos);
	virtual void setPositionX(float newX);
	virtual void setPositionY(float newY);
	virtual void breakSelf();
};

#endif


