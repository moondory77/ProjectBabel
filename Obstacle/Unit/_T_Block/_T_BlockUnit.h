#ifndef __T_BLOCK_UNIT_H__
#define __T_BLOCK_UNIT_H__

#include "cocos2d.h"
#include "Obstacle/Unit/_ABS_ObsUnit.h"

USING_NS_CC;
using namespace std;

class Character;
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
	
	struct {
		Vec2 prevPos;	//(이전 프레임) world position
		Vec2 curPos;	//(현재 프레임) world position
		pair<float, float> boundX;
		pair<float, float> boundY;
	}CrashInfo;

	Action* actionCrash = NULL;
	set<BlockUnit*> linkedUnit = {};	//연결되어 있는 다른 BlockUnit으로의 포인터
	bool isDefensibleFlag;
	int maxStrength;
	int curStrength;
	int unitIdx;
	float curWeight;			//chunk의 구성유닛 수(낙하 가속도)

public:

	BlockUnit();
	BlockUnit(const BlockUnit& blk) {};

	virtual ~BlockUnit() {};
	bool isVisitable();
	bool isSearchable(int chunk_id, const Point& pos);

	vector<int>& getUnitChunk();
	int getChunkID() { return chunkID; };
	void setChunkID(int id) { chunkID = id; }

	void setUser(Character* character) { this->mainChar = character; }
	Character* getUser() { return mainChar; };
	void setContainer(BuildContainer* host) { this->container = host; }
	BuildContainer* getContainer() { return container; }

	float getWeight() { return curWeight; }
	void setWeight(float weight) { this->curWeight = weight; }

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


	virtual void updateState(float deltaTime);
	virtual void updatePosition(float deltaTime);


	virtual void setPosition(Vec2 pos);
	virtual void setPositionX(float newX);
	virtual void setPositionY(float newY);
	virtual void breakSelf();
};

#endif


