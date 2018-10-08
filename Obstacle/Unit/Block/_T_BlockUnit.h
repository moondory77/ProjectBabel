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

	//ĳ���� 1ȸ�� ��ǲ�� ���� 1ȸ state ��ȭ�� �����ϱ� ���� ID
	int attackID;
	int defenseID;
	int specialID;

protected:

	BuildContainer* container;
	Character* mainChar;

	int chunkID;				//(Container)���� �ȿ����� ������ ��Ÿ���� ID
	Point prevPos;				//(���� ������) world position

	Vec2 prevRelativePos;		//(���� ������) unit <-> character ��뺯��	
	Vec2 curRelativePos;		//(���� ������) unit <-> character ��뺯�� 	
	float curRepulsion;			//unit ������ delta velocity

	Vec2 collisionVec;				//(�浹 �߻� ��) unit <-> hero collider ��뺯���� ��ȭ��(�ӵ�)
	float collisionAngle;			//(�浹 �߻� ��) unit <-> heoro collider �浹�ϴ� ����

	set<BlockUnit*> linkedUnit = {};	//����Ǿ� �ִ� �ٸ� BlockUnit������ ������
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
	Vec2 getCollisionVec() { return collisionVec; }
	float getCollisionAngle() { return collisionAngle; }


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
	float isDefensibleRange();	//����� ƨ�ܳ����� �Ÿ� ����, ���� ���̸� 0 ����
	bool isCrashed(Character& mainChar);


	virtual void updateState(float deltaTime);
	virtual void updatePosition(float deltaTime);


	virtual void setPosition(Point pos);
	virtual void setPositionX(float newX);
	virtual void setPositionY(float newY);
	virtual void breakSelf();
};

#endif


