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

	Vec2 getNormAxis() { return normAxis; }


	//�Է¹��� vector�� ����, normal-vector�� ���� �и����� ����
	inline void initAxis(Vec2 start, Vec2 dst) {
		this->normAxis = (dst - start).getPerp().getNormalized();
		projMinTarget = projMaxTarget = 0.0f;
	}
	//(������ ����) �ش� �࿡ projection ��Ų ���� ��ȯ
	inline float getProjection(Vec2 input_vec) { return normAxis.dot(input_vec); }

	inline float calOverlapped(Vec2 node_pos, float node_radius)
	{
		float proj_val = this->getProjection(node_pos);	//�ش� ������ projection
		float proj_min = proj_val - node_radius;
		float proj_max = proj_val + node_radius;
		float overlap_val = 0.0f;

		//(projection �� �� ��) �ּڰ��� Ÿ�� ���翵 ������ ����
		if (getProjMinTarget() < proj_min && proj_min < getProjMaxTarget())
		{
			float max_clamp = proj_max < getProjMaxTarget() ? proj_max : getProjMaxTarget();
			overlap_val = max_clamp - proj_min;
		}
		//(projection �� �� ��) �ִ��� Ÿ�� ���翵 ������ ����
		else if (getProjMinTarget() < proj_max && proj_max < getProjMaxTarget())
		{
			float min_clamp = proj_min > getProjMinTarget() ? proj_min : getProjMinTarget();
			overlap_val = proj_max - min_clamp;
		}

		return overlap_val;
	};
};



class CollisionDetector : public Layer
{
	const Sprite& target;

	AutoPolygon* tPolygon;
	DrawNode* tLiner;
	vector<SeparateAxis*> tAxises = {};

	///vertex��, scaling�� �ȵ� �̹��� �������¿����� ��ǥ��
	vector<Vec2> protoVertices;			//�ؽ��� ���� vertex container
	vector<Vec2> curVertices;			//reduce ��Ų ��, vertices ���� -> �浹üũ ��� vertex	
	float reduceEpsilon;
	float scaleFactor;		//(Ÿ�� ��������Ʈ�� ����) �ݶ��̴��� ������ ����

public:

	///filename�� target�� ������ ��������Ʈ���� �Ѵ�
	CollisionDetector(const string& filename, Sprite* spr_target, float scale, float reduce_epsilon) : target(*spr_target)
	{
		tLiner = DrawNode::create();
		tLiner->retain();
		this->addChild(tLiner);
		tPolygon = new AutoPolygon(filename);
		protoVertices = tPolygon->trace(Rect(0, target.getContentSize().height / 2, target.getContentSize().width, target.getContentSize().height / 2));
		scaleFactor = scale;
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
			curVertices = tPolygon->reduce(protoVertices, Rect(0, target.getContentSize().height / 2, target.getContentSize().width, target.getContentSize().height / 2), reduceEpsilon);

			///sprite���� �ణ �� �а� �ݶ��̴� ���� ����(�浹ó�� �ε巴���ϱ�����)
			for (auto iter = curVertices.begin(); iter != curVertices.end(); iter++) {
				*iter -= Vec2(target.getContentSize().width / 2, 0);
				(*iter).scale(scaleFactor);
				*iter += Vec2(target.getContentSize().width / 2, 0);
			}

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

	/*
	1. �浹 ���θ� Ž�� �ʿ� ��, detectCollision
	2. SAT�˻縦 ����, �и� Vec2�� �ʿ��ϸ� detectCollisionBounce
	*/
	inline bool detectCollision(Vec2 invade_world_pos, float invade_world_radius)
	{
		Vec2 opt_pos = target.convertToNodeSpace(invade_world_pos);
		float opt_radius = invade_world_radius * target.getScale();

		//��� �и��࿡ ����, gap �߻����� �׽�Ʈ
		auto iter = tAxises.begin();
		do
		{
			float proj_val = (*iter)->getProjection(opt_pos);
			float proj_min = proj_val - opt_radius;
			float proj_max = proj_val + opt_radius;

			//gap �߻� ��, �浹 ���� ������ �� ����
			if (proj_max < (*iter)->getProjMinTarget() || (*iter)->getProjMaxTarget() < proj_min)
				return false;

		} while (++iter != tAxises.end());

		return true;
	};



	inline Vec2 detectCollisionBounce(Vec2 invade_world_pos, float invade_world_radius)
	{
		Vec2 opt_pos = target.convertToNodeSpace(invade_world_pos);
		float opt_radius = invade_world_radius * target.getScale();

		int min_idx = 0;
		float min_overlapped = 0.0f;
		float cur_overlapped = 0.0f;

		//�� �и��� ��ȸ�ϸ�, overlap�� ũ�Ⱑ ���� ���� �и���� ���� ����
		for (int i = 0; i < tAxises.size(); i++)
		{
			if ((cur_overlapped = tAxises[i]->calOverlapped(opt_pos, opt_radius)) == 0.0f)
				return Vec2::ZERO;
			if (min_overlapped != 0.0f) {
				if (cur_overlapped < min_overlapped) {
					min_overlapped = cur_overlapped;
					min_idx = i;
				}
			}
			else {
				min_overlapped = cur_overlapped;
				min_idx = i;
			}
		}
		//���� ���� ����� ���� �и��� ���� ��ȯ
		return tAxises[min_idx]->getNormAxis() * min_overlapped;
	}

};


#endif // !__COLLISION_DETECTOR__
