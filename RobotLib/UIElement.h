#pragma once
#include <vector>
#include <chrono>
#include "Rectangle.h"
#include "../RobotController/SensorLib/DigoleLCD.h"

class UIElement
{
	public:
		UIElement(Point location);
		UIElement(Point location, bool clickable);
		virtual bool pointTouches(Point pt);
		void setClickable(bool value);
		void setUpdateCycle(int msBetweenUpdate);
		bool updateNeeded();		
		void setElementArea(Rectangle newArea);
	
		virtual void update(DigoleLCD *lcdDriver)
		{};
		virtual Rectangle calcSize()
		{};
		
		void forceUpdate();
		void enabled(bool value);
		Rectangle getElementArea()
		{
			return elementArea;
		}
		int getUpdateCycle()
		{
			return updateCycle;
		}
	
	protected:		
		bool clickable;
		Point location;			
		Rectangle elementArea;
		int updateCycle;
		bool elementEnabled;
		std::vector<bool> touchEvents;
	
	private:
		std::chrono::system_clock::time_point updateTime;	
};
