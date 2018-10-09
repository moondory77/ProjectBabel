#ifndef __COLLISION_DETECTOR__
#define __COLLISION_DETECTOR__
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

//충돌감지를 위한, 분리축 정보를 담는 데이터 클래스
class SeparateAxis : public CCNode
{
	Vec2 normAxis;			//(초기화 시, 입력받은 vector의 노멀벡터) 분리축
	float projMinTarget;	//(축에 projection 된 값 중) 최솟값
	float projMaxTarget;	//(축에 projection 된 값 중) 최댓값

public:

	SeparateAxis(Vec2 start, Vec2 dst) { initAxis(start, dst); }
	~SeparateAxis() {};

	void setProjMin(float min) { projMinTarget = min; }
	void setProjMax(float max) { projMaxTarget = max; }

	float getProjMinTarget() { return projMinTarget; }
	float getProjMaxTarget() { return projMaxTarget; }

	Vec2 getNormAxis() { return normAxis; }


	//입력받은 vector에 대해, normal-vector를 구해 분리축을 설정
	inline void initAxis(Vec2 start, Vec2 dst) {
		this->normAxis = (dst - start).getPerp().getNormalized();
		projMinTarget = projMaxTarget = 0.0f;
	}
	//(임의의 점을) 해당 축에 projection 시킨 값을 반환
	inline float getProjection(Vec2 input_vec) { return normAxis.dot(input_vec); }

	inline float calOverlapped(Vec2 node_pos, float node_radius)
	{
		float proj_val = this->getProjection(node_pos);	//해당 축으로 projection
		float proj_min = proj_val - node_radius;
		float proj_max = proj_val + node_radius;
		float overlap_val = 0.0f;

		//(projection 된 값 중) 최솟값이 타겟 정사영 범위에 속함
		if (getProjMinTarget() < proj_min && proj_min < getProjMaxTarget())
		{
			float max_clamp = proj_max < getProjMaxTarget() ? proj_max : getProjMaxTarget();
			overlap_val = max_clamp - proj_min;
		}
		//(projection 된 값 중) 최댓값이 타겟 정사영 범위에 속함
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

	///vertex는, scaling이 안된 이미지 원본상태에서의 좌표값
	vector<Vec2> protoVertices;			//텍스쳐 상의 vertex container
	vector<Vec2> curVertices;			//reduce 시킨 후, vertices 저장 -> 충돌체크 대상 vertex	
	float reduceEpsilon;
	float scaleFactor;		//(타겟 스프라이트에 대해) 콜라이더의 스케일 비율

public:

	///filename은 target과 동일한 스프라이트여야 한다
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

	///epsilon값에 의해, 외곽선을 다시 트랙킹한 후, 분리축 데이터 초기화
	inline void initAxises() {
		{
			curVertices = tPolygon->reduce(protoVertices, Rect(0, target.getContentSize().height / 2, target.getContentSize().width, target.getContentSize().height / 2), reduceEpsilon);

			///sprite보다 약간 더 넓게 콜라이더 면적 적용(충돌처리 부드럽게하기위해)
			for (auto iter = curVertices.begin(); iter != curVertices.end(); iter++) {
				*iter -= Vec2(target.getContentSize().width / 2, 0);
				(*iter).scale(scaleFactor);
				*iter += Vec2(target.getContentSize().width / 2, 0);
			}

			for (int i = 1; i < curVertices.size(); i++)
			{
				//분리축 데이터 initialize
				if (tAxises.size() < i)
					tAxises.push_back(new SeparateAxis(curVertices[i - 1], curVertices[i]));
				else
					tAxises[i - 1]->initAxis(curVertices[i - 1], curVertices[i]);

				//각 vertex 순회하며, 분리축에 대한 target의 projection 범위 미리 계산
				auto iter = curVertices.begin();
				float cur_val = tAxises[i - 1]->getProjection(*iter);
				tAxises[i - 1]->setProjMin(cur_val);
				tAxises[i - 1]->setProjMax(cur_val);

				while (++iter != curVertices.end())
				{
					cur_val = tAxises[i - 1]->getProjection(*iter);

					//축에 projection 한 값의 min-max 범위를 갱신한다 
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
	1. 충돌 여부만 탐지 필요 시, detectCollision
	2. SAT검사를 통해, 분리 Vec2이 필요하면 detectCollisionBounce
	*/
	inline bool detectCollision(Vec2 invade_world_pos, float invade_world_radius)
	{
		Vec2 opt_pos = target.convertToNodeSpace(invade_world_pos);
		float opt_radius = invade_world_radius * target.getScale();

		//모든 분리축에 대해, gap 발생여부 테스트
		auto iter = tAxises.begin();
		do
		{
			float proj_val = (*iter)->getProjection(opt_pos);
			float proj_min = proj_val - opt_radius;
			float proj_max = proj_val + opt_radius;

			//gap 발생 시, 충돌 없음 보장할 수 있음
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

		//각 분리축 순회하며, overlap의 크기가 가장 작은 분리축과 값을 추출
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
		//가장 작은 계수에 대한 분리축 벡터 반환
		return tAxises[min_idx]->getNormAxis() * min_overlapped;
	}

};


#endif // !__COLLISION_DETECTOR__
