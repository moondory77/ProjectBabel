#include "LineTracer.h"
#include "Manager/GameManager.h"

LineTracer::LineTracer() 
{
	_dash = 10;
	_dashSpace = 10;
	_lineType = LINE_NONE;
	_energyLineX = winSize().width * 0.96f;
	_energyHeight = winSize().height * 0.8f;

	glLineWidth(8.0 * CC_CONTENT_SCALE_FACTOR());

	this->reset();
}


LineTracer* LineTracer::create() 
{
	LineTracer* tracer = new LineTracer();

	if (tracer){
		tracer->autorelease();
		return tracer;
	}
	CC_SAFE_DELETE(tracer);
	return NULL;
}

void LineTracer::reset() {
	_energy = 1.0;
	_energyDecrement = 0.015f;
}

void LineTracer::update(float dt) {
	_energy -= dt * _energyDecrement;
	if (_energy < 0) _energy = 0;
	drawing();
}

void LineTracer::setEnergyDecrement(float value) {
	_energyDecrement += value;
	//if (_energyDecrement > 0.07) _energyDecrement = 0.07;
}


void LineTracer::drawing()
{
	switch (_lineType) {
	case LINE_NONE:
		break;
	case LINE_TEMP:

		ccDrawColor4F(5.0, 5.0, 5.0, 5.0);
		ccDrawLine(_tip, _pivot);
		ccDrawCircle(_pivot, 10, CC_DEGREES_TO_RADIANS(360), 10, false);
		CCLOG("Drawing [%.2f, %.2f]", _pivot.x, _pivot.y);
		break;

	case LINE_DASH:
		ccDrawColor4F(5.0, 5.0, 5.0, 5.0);
		ccDrawCircle(_pivot, 10, M_PI, 10, false);

		int segments = _lineLength / (_dash + _dashSpace);

		float t = 0.0f;
		float x_;
		float y_;

		for (int i = 0; i < segments + 1; i++) {

			x_ = _pivot.x + t * (_tip.x - _pivot.x);
			y_ = _pivot.y + t * (_tip.y - _pivot.y);

			ccDrawCircle(ccp(x_, y_), 4, M_PI, 6, false);

			t += (float)1 / segments;
		}
		break;
	}

	//draw energy bar
	ccDrawColor4F(0.0, 0.0, 0.0, 1.0);
	ccDrawLine(ccp(_energyLineX, winSize().height * 0.1f),
		ccp(_energyLineX, winSize().height * 0.9f));
	ccDrawColor4F(1.0, 0.5, 0.0, 1.0);
	ccDrawLine(ccp(_energyLineX, winSize().height * 0.1f),
		ccp(_energyLineX, winSize().height * 0.1f + _energy * _energyHeight));
}

