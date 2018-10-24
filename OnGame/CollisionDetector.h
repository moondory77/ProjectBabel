#ifndef __COLLISION_DETECTOR__
#define __COLLISION_DETECTOR__
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

//(1차원 상에서) 두 segment에 대해, overlap 범위를 반환 (각 pair는 segment의 <min, max>) 
static inline pair<float, float> isOneDimensionOverlap(const pair<float, float>& AB, const pair<float, float>& CD)
{
	float minAB = AB.first;
	float maxAB = AB.second;
	float minCD = CD.first;
	float maxCD = CD.second;

	if (maxAB < minCD || minAB > maxCD) {
		return{ 0, 0 };
	}
	else {
		//AB를 right 방향 맨 끝에서 left로 조금씩 이동시킨다 생각하며 overlap 측정
		if (minCD <= minAB && minAB <= maxCD) {
			return{ minAB ,  (maxAB < maxCD ? maxAB : maxCD) };
		}
		else if (minCD <= maxAB && maxAB <= maxCD) {
			return{ minCD , maxAB };
		}
		else {
			return{ minCD , maxCD };
		}
	}
}


//충돌 감지를 위한, 분리축 정보를 담는 데이터 클래스
class SeparateAxis : public CCNode
{
	Vec2 normAxis;			//(초기화 시, 입력받은 vector의 노멀벡터) 분리축 벡터
	float projMinTarget;	//(현재 target에 대해, projection 된 값 중) 최솟값
	float projMaxTarget;	//(현재 target에 대해, projection 된 값 중) 최댓값

public:

	SeparateAxis(Vec2 start, Vec2 dst) { initAxis(start, dst); }
	~SeparateAxis() {};

	void setProjMin(float min) { projMinTarget = min; }
	void setProjMax(float max) { projMaxTarget = max; }

	float getProjMin() { return projMinTarget; }
	float getProjMax() { return projMaxTarget; }
	Vec2 getNormAxis() { return normAxis; }

	//perpendicular normal vector (start -> dst)
	inline void initAxis(Vec2 start, Vec2 dst) {
		this->normAxis = (dst - start).getPerp().getNormalized();
		projMinTarget = projMaxTarget = 0.0f;
	}

	//(임의의 점을) 해당 축에 projection 시킨 값을 반환
	inline float getProjection(Vec2 input_vec) { return normAxis.dot(input_vec); }

	//해당 축에 대한, 충돌체의 penetration 구간 (min, max)
	inline pair<float, float> getPenetrationBound(Vec2 invade_direction, Vec2 center_pos, float radius)
	{
		float invade_center = getProjection(center_pos);
		float invade_min = invade_center - radius;
		float invade_max = invade_center + radius;

		if (invade_max < projMinTarget || invade_min > projMaxTarget) {
			return{ 0, 0 };
		}
		else {
			if (getProjection(invade_direction) < 0)
				return{ projMaxTarget , invade_min };
			else
				return{ projMinTarget, invade_max };
		}
	}
	inline pair<float, float> getPenetrationBound(Vec2 invade_direction, vector<Vec2>& invade_vertices)
	{
		auto iter = invade_vertices.begin();
		float cur_val = getProjection(*iter);
		pair<float, float> invade_bound = { cur_val , cur_val };

		while (++iter != invade_vertices.end()) {
			cur_val = getProjection(*iter);
			if (cur_val < invade_bound.first)
				invade_bound.first = cur_val;
			if (cur_val > invade_bound.second)
				invade_bound.second = cur_val;
		}

		if (invade_bound.second < projMinTarget || invade_bound.first > projMaxTarget) {
			return{ 0, 0 };
		}
		else {
			if (getProjection(invade_direction) < 0)
				return{ projMaxTarget , invade_bound.first };
			else
				return{ projMinTarget , invade_bound.second };
		}
	}

	//해당 축에 대한, penetration depth 
	inline float getPenetrationDepth(Vec2 invade_direction_vec, Vec2 center_pos, float radius)
	{
		pair<float, float> p_bound = getPenetrationBound(invade_direction_vec, center_pos, radius);
		return p_bound.second - p_bound.first;
	}
	inline float getPenetrationDepth(Vec2 invade_direction_vec, vector<Vec2>& invade_vertices)
	{
		pair<float, float> p_bound = getPenetrationBound(invade_direction_vec, invade_vertices);
		return p_bound.second - p_bound.first;
	}
};


class CollisionDetector : public Layer
{
	Sprite* target;
	
	/*부딪쳐 오는 object에 대해, 상대속도의 방향으로 분리축의 방향 결정 시퀀스 삽입 필요
	///target의 현재 위치와 이전 프레임 위치 추적 필요*/

	DrawNode* tLiner;
	AutoPolygon* tPolygon;
	vector<SeparateAxis*> tAxises = {};

	vector<Vec2> protoVertices;			//텍스쳐 상의 vertex container
	vector<Vec2> curVertices;			//reduce 시킨 후, vertices 저장 -> 충돌체크 대상 vertex	

	float reduceEpsilon;
	float scaleFactor;					//(타겟 스프라이트에 대해) 콜라이더의 스케일 비율

public:

	///Target으로 할 스프라이트와 동일한 파일주소 삽입
	CollisionDetector(const string& filename, Sprite* spr_target, float scale, float reduce_epsilon)
		: target(spr_target)
	{
		tPolygon = new AutoPolygon(filename);
		tLiner = DrawNode::create();
		tLiner->retain();
		this->addChild(tLiner);

		protoVertices = tPolygon->trace(Rect(0, 0, target->getContentSize().width, target->getContentSize().height));
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
			curVertices = tPolygon->reduce(protoVertices, Rect(0, 0, target->getContentSize().width, target->getContentSize().height), reduceEpsilon);

			for (auto iter = curVertices.begin(); iter != curVertices.end(); iter++) {
				*iter -= Vec2(target->getContentSize().width / 2, 0);
				(*iter).scale(scaleFactor);
				*iter += Vec2(target->getContentSize().width / 2, 0);
			}

			for (int i = 1; i < curVertices.size(); i++)
			{
				if (tAxises.size() < i)
					tAxises.push_back(new SeparateAxis(curVertices[i - 1], curVertices[i]));
				else
					tAxises[i - 1]->initAxis(curVertices[i - 1], curVertices[i]);

				//각 분리축에 대해, target의 projection 범위 미리 계산(방향 )
				auto iter = curVertices.begin();
				float cur_val = tAxises[i - 1]->getProjection(*iter);
				tAxises[i - 1]->setProjMin(cur_val);
				tAxises[i - 1]->setProjMax(cur_val);

				while (++iter != curVertices.end())
				{
					cur_val = tAxises[i - 1]->getProjection(*iter);	
					if (cur_val < tAxises[i - 1]->getProjMin())
						tAxises[i - 1]->setProjMin(cur_val);
					if (cur_val > tAxises[i - 1]->getProjMax()) 
						tAxises[i - 1]->setProjMax(cur_val);
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

	//충돌 여부 탐지 
	inline bool detectCollision(Vec2 obj_world_pos, float obj_world_radius)
	{
		Vec2 trans_pos = target->convertToNodeSpace(obj_world_pos);
		float trans_radius = obj_world_radius * target->getScale();

		//모든 분리축에 대해, gap 발생여부 테스트
		auto iter = tAxises.begin();
		do
		{
			float proj_val = (*iter)->getProjection(trans_pos);
			float proj_min = proj_val - trans_radius;
			float proj_max = proj_val + trans_radius;
			//gap 발생 시, 충돌없음 보장 가능
			if (proj_max < (*iter)->getProjMin() || (*iter)->getProjMax() < proj_min)
				return false;

		} while (++iter != tAxises.end());

		return true;
	};

	//Minimum Separate Axis와 depth 구간 반환
	inline pair<float, float> getMinPenetrateBound(Vec2 invade_direction, Vec2 center_pos, float radius, Vec2& norm_axis)
	{
		Vec2 opt_pos = target->convertToNodeSpace(center_pos);
		float opt_radius = radius * target->getScale();

		int min_idx = 0;
		float min_depth = 0.0f;
		float cur_depth = 0.0f;

		//각 분리축 순회하며, overlap의 크기가 가장 작은 분리축과 값을 추출
		for (int i = 0; i < tAxises.size(); i++)
		{
			//분리축에 대해 gap 발생 시, zero vec2 반환하며 함수 종료
			if ((cur_depth = tAxises[i]->getPenetrationDepth(invade_direction, opt_pos, opt_radius)) == 0.0f)
				return{ 0, 0 };

			if (min_depth != 0.0f) {
				if (fabsf(cur_depth) < fabsf(min_depth)) {
					min_depth = cur_depth;
					min_idx = i;
				}
			}
			else {
				min_depth = cur_depth;
				min_idx = i;
			}
		}

		norm_axis = convertToWorldSpace(tAxises[min_idx]->getNormAxis());
		return tAxises[min_idx]->getPenetrationBound(invade_direction, center_pos, radius);
	}
	inline pair<float, float> getMinPenetrateBound(Vec2 invade_direction, vector<Vec2>& invade_vertices, Vec2& norm_axis)
	{
		vector<Vec2> opt_vertices;

		auto iter = invade_vertices.begin();
		while (iter != invade_vertices.end()) {
			opt_vertices.push_back(target->convertToNodeSpace(*iter));
			iter++;
		}
		int min_idx = 0;
		float min_depth = 0.0f;
		float cur_depth = 0.0f;

		//각 분리축 순회하며, overlap의 크기가 가장 작은 분리축과 값을 추출
		for (int i = 0; i < tAxises.size(); i++)
		{
			//분리축에 대해 gap 발생 시, zero vec2 반환하며 함수 종료
			if ((cur_depth = tAxises[i]->getPenetrationDepth(invade_direction, invade_vertices)) == 0.0f) {
				return{ 0, 0 };
			}

			if (min_depth == 0.0f){
				min_depth = cur_depth;
				min_idx = i;
			}
			else{
				if (fabsf(cur_depth) < fabsf(min_depth)) {
					min_depth = cur_depth;
					min_idx = i;
				}
			}
		}
		norm_axis = target->convertToWorldSpace(tAxises[min_idx]->getNormAxis());
		return tAxises[min_idx]->getPenetrationBound(invade_direction, invade_vertices);
	}

	inline Vec2 getMinPenetrateVec(Vec2 invade_direction, Vec2 center_pos, float radius)
	{
		Vec2 opt_pos = target->convertToNodeSpace(center_pos);
		float opt_radius = radius * target->getScale();

		int min_idx = 0;
		float min_depth = 0.0f;
		float cur_depth = 0.0f;

		//각 분리축 순회하며, overlap의 크기가 가장 작은 분리축과 값을 추출
		for (int i = 0; i < tAxises.size(); i++)
		{
			//분리축에 대해 gap 발생 시, zero vec2 반환하며 함수 종료
			if ((cur_depth = tAxises[i]->getPenetrationDepth(invade_direction, opt_pos, opt_radius)) == 0.0f)
				return Vec2::ZERO;

			if (min_depth != 0.0f) {
				if (fabsf(cur_depth) < fabsf(min_depth)) {
					min_depth = cur_depth;
					min_idx = i;
				}
			}
			else {
				min_depth = cur_depth;
				min_idx = i;
			}
		}
		CCLOG("penetrate [%.2f, %.2f]", (tAxises[min_idx]->getNormAxis() * min_depth).x, (tAxises[min_idx]->getNormAxis() * min_depth).y);
		return tAxises[min_idx]->getNormAxis() * min_depth;
	}
	inline Vec2 getMinPenetrateVec(Vec2 invade_direction, vector<Vec2>& invade_vertices)
	{
		vector<Vec2> opt_vertices;

		auto iter = invade_vertices.begin();
		while (iter != invade_vertices.end()) {
			opt_vertices.push_back(target->convertToNodeSpace(*iter));
		}

		int min_idx = 0;
		float min_depth = 0.0f;
		float cur_depth = 0.0f;


		//각 분리축 순회하며, overlap의 크기가 가장 작은 분리축과 값을 추출
		for (int i = 0; i < tAxises.size(); i++)
		{
			//분리축에 대해 gap 발생 시, zero vec2 반환하며 함수 종료
			if ((cur_depth = tAxises[i]->getPenetrationDepth(invade_direction, invade_vertices)) == 0.0f)
				return Vec2::ZERO;

			if (min_depth != 0.0f) {
				if (fabsf(cur_depth) < fabsf(min_depth)) {
					min_depth = cur_depth;
					min_idx = i;
				}
			}
			else {
				min_depth = cur_depth;
				min_idx = i;
			}
		}
		CCLOG("penetrate [%.2f, %.2f]", (tAxises[min_idx]->getNormAxis() * min_depth).x, (tAxises[min_idx]->getNormAxis() * min_depth).y);
		return tAxises[min_idx]->getNormAxis() * min_depth;
	}
};

#endif // !__COLLISION_DETECTOR__
