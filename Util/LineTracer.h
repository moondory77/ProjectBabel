#ifndef __LINE_TRACER_H__
#define __LINE_TRACER_H__

#include "cocos2d.h"

USING_NS_CC;

typedef enum LineType 
{
	LINE_TEMP,
	LINE_DASH,
	LINE_NONE
};

class LineTracer : public CCNode
{
private:

	float _lineAngle;
	float _energyLineX;
	float _energyHeight;
	float _energyDecrement;

	int _dash;
	int _dashSpace;


public:
	CC_SYNTHESIZE(float, _energy, Energy);
	CC_SYNTHESIZE(Point, _pivot, Pivot);
	CC_SYNTHESIZE(Point, _tip, Tip);
	CC_SYNTHESIZE(float, _lineLength, LineLength);
	CC_SYNTHESIZE(LineType, _lineType, LineType);

	LineTracer();
	static LineTracer* create();

	void drawing();
	void update(float dt);
	void setEnergyDecrement(float value);
	void reset(void);


};

#endif