#ifndef __COLLISION_DETECTOR__
#define __COLLISION_DETECTOR__
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

//�浹������ ����, �и��� ������ ��� ������ Ŭ����
class SeparateAxis : public CCNode
{
	Vec2 normAxis;			//(�ʱ�ȭ ��, �Է¹��� vector�� ��ֺ���) �и���
	float projMinTarget;	//(�࿡ projection �� �� ��) �ּڰ�
	float projMaxTarget;	//(�࿡ projection �� �� ��) �ִ�

public:

	SeparateAxis(Vec2 start, Vec2 dst) { initAxis(start, dst); }
	~SeparateAxis() {};
	
	void setProjMin(float min) { projMinTarget = min; }
	void setProjMax(float max) { projMaxTarget = max; }

	float getProjMinTarget() { return projMinTarget; }
	float getProjMaxTarget() { return projMaxTarget; }
	
	//�Է¹��� vector�� ����, normal-vector�� ���� �и����� ����
	inline void initAxis(Vec2 start, Vec2 dst) {
		this->normAxis = (dst - start).getPerp().getNormalized();
		projMinTarget = projMaxTarget = 0.0f;
	}
	//�и��࿡ projection ��Ų ���� ��ȯ
	inline float getProjection(Vec2 input_vec) { return normAxis.dot(input_vec); }
};



class CollisionDetector : public Layer
{
	const Sprite& target;

	AutoPolygon* tPolygon;
	DrawNode* tLiner;
	vector<SeparateAxis*> tAxises = {};

	///vertex��, scaling�� �ȵ� �̹��� �������¿����� ��ǥ��
	vector<Vec2> protoVertices;					//���� vertex container
	vector<Vec2> curVertices;					//reduce ��Ų ��, vertices ���� -> �浹üũ ��� vertex	
	float reduceEpsilon;

public:

	///filename�� target�� ������ ��������Ʈ���� ��
	CollisionDetector(const string& filename, Sprite* spr_target, float reduce_epsilon) : target(*spr_target)
	{
		tLiner = DrawNode::create();
		tLiner->retain();
		this->addChild(tLiner);
		tPolygon = new AutoPolygon(filename);
		protoVertices = tPolygon->trace(Rect(0, 0, target.getContentSize().width, target.getContentSize().height));
	
		setEpsilon(reduce_epsilon);
	};	
	~CollisionDetector() {
		while (!tAxises.empty()) {
			tAxises.back()->release();
			tAxises.pop_back();
		}
		tLiner->removeFromParent();
		tLiner->release();
		delete(tPolygon);
	};


	const vector<Vec2>& getVertices() { return curVertices; };

	inline float getEpsilon() { return this->reduceEpsilon; }
	inline void setEpsilon(float reduce_epsilon) {
		this->reduceEpsilon = reduce_epsilon;
		initAxises();
	}

	///epsilon���� ����, �ܰ����� �ٽ� Ʈ��ŷ�� ��, �и��� ������ �ʱ�ȭ
	inline void initAxises() {
		{
			curVertices = tPolygon->reduce(protoVertices, Rect(0, 0, target.getContentSize().width, target.getContentSize().height), reduceEpsilon);

			///sprite���� �ణ �� �а� �ݶ��̴� ���� ����(�浹ó�� �ε巴���ϱ�����)
			for (auto iter = curVertices.begin(); iter != curVertices.end(); iter++) {
				(*iter).scale(1.1f);
			}
			//CCLOG("vertices size is %d", curVertices.size());

			for (int i = 1; i < curVertices.size(); i++)
			{
				//�и��� ������ initialize
				if (tAxises.size() < i)
					tAxises.push_back(new SeparateAxis(curVertices[i - 1], curVertices[i]));
				else
					tAxises[i - 1]->initAxis(curVertices[i - 1], curVertices[i]);

				//�� vertex ��ȸ�ϸ�, �и��࿡ ���� target�� projection ���� �̸� ���
				auto iter = curVertices.begin();
				float cur_val = tAxises[i - 1]->getProjection(*iter);
				tAxises[i - 1]->setProjMin(cur_val);
				tAxises[i - 1]->setProjMax(cur_val);

				while (++iter != curVertices.end())
				{
					cur_val = tAxises[i - 1]->getProjection(*iter);

					//�࿡ projection �� ���� min-max ������ �����Ѵ� 
					if (cur_val < tAxises[i - 1]->getProjMinTarget()) {
						tAxises[i - 1]->setProjMin(cur_val);
					}
					else if (cur_val > tAxises[i - 1]->getProjMaxTarget()) {
						tAxises[i - 1]->setProjMax(cur_val);
					}
				}
			}
		}
	}	
	
	inline void drawOutline() {
		tLiner->clear();
		for (int i = 0; i < curVertices.size(); i++) {
			tLiner->drawLine(curVertices[i], curVertices[(i + 1) % curVertices.size()], Color4F::RED);
		}
	}
	
	///position�� radius�� ��� world����
	inline bool detectCollision(Vec2 invade_pos, float invade_radius)
	{
		Vec2 opt_pos = target.convertToNodeSpace(invade_pos);
		float opt_radius = invade_radius * target.getScale();

		//��� �и��࿡ ����, gap �߻����� �׽�Ʈ
		auto iter = tAxises.begin();
		do
		{
			float proj_val = (*iter)->getProjection(opt_pos);
			float proj_min = proj_val - opt_radius;
			float proj_max = proj_val + opt_radius;

			//gap �߻� ��, �浹���� Ȯ��
			if (proj_max < (*iter)->getProjMinTarget() || (*iter)->getProjMaxTarget() < proj_min)
				return false;

		} while (++iter != tAxises.end());

		return true;
	};
};


#endif // !__COLLISION_DETECTOR__
