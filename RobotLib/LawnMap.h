#pragma once
#include <map>
#include <vector>
#include <algorithm>
#include <queue>
#include "MapNode.h"
#include "MapLibConstants.h"
#include "Node.h"
#include "RobotLib.h"
#include "../RobotController/SensorLib/SensorLib.h"
#include "Point.h"

class LawnMap
{
public:
	LawnMap(RobotLib *robotLib);
	~LawnMap();
	void setNode(int x, int y, map_node_t nodeType, std::pair<double, double>location);
	void setNodeMowed(int x, int y);

	MapNode *getNode(int x, int y);
	bool saveMap();
	bool loadMap();
	void getExtents(std::pair<int, int> &minXY, std::pair<int, int> &maxXY);
	std::vector<MapNode *> getBaseStations();
	std::pair<int, int> getPathCoord(MapNode *node);
	int getWidth();
	int getHeight();
	bool getMapDirty();
	void setMapDirty(bool dirty);
	MapNode *getCartNode(int x, int y);
	std::vector<MapNode *> findPath(std::pair<int, int> start, std::pair<int, int> end);
	int getPathMap(int x, int y);
	int getCartNodeWeight(int x, int y);
	std::vector<MapNode *> getPathToClosestBase(std::pair<int, int> start);
	void storeMapNode(MapNode *node);
	MapNode *loadMapNode(std::pair<int, int> coord);
	MapNode *closestNodeOfType(std::pair<int, int> currentLocation, map_node_t nodeType);
	MapNode *closestNodeOfType(Point currentLocation, map_node_t nodeType);
	MapNode *closestUnmowedNode(std::pair<int, int> currentLocation);
	MapNode *closestUnmowedNode(Point currentLocation);
	void clearMowedFlags();
	void clear();
	map_node_t getNodeType(std::pair<int, int> location);
	map_node_t getNodeType(Point location);
	map_node_t getNodeType(int x, int y);

private:
	RobotLib *robotLib;
	std::map<std::pair<int, int>, MapNode*> mapContents;
	std::pair<int, int> minXY, maxXY;
	std::vector<MapNode *> baseStations;
	bool mapDirty;
	int mapModX, mapModY;
	std::vector <std::vector <int> > pathMap, closedNodesMap, openNodesMap, dirMap;
	void createPathMap();
	int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
};
