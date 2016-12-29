#include "Map.h"

Map::Map(RobotLib *robotLib)
{
	this->robotLib = robotLib;
	minXY = std::make_pair(0, 0);
	maxXY = std::make_pair(0, 0);
	mapModX = 0;
	mapModY = 0;
}

Map::~Map()
{
	robotLib->Log("Destroying Map()");
	std::map<std::pair<int, int>, MapNode *>::iterator it;
	for (it = mapContents.begin(); it != mapContents.end(); it++)
	{
		delete(it->second);
	}
}

int Map::getPathMap(int x, int y)
{
	return (pathMap[x][y]);	
}

void Map::getExtents(std::pair<int, int> &minXY, std::pair<int, int> &maxXY)
{
	minXY = this->minXY;
	maxXY = this->maxXY;
}

void Map::setNode(int x, int y, map_node_t nodeType, std::pair<double, double> location)	
{
	std::map<std::pair<int, int>, MapNode *>::iterator it = mapContents.find(std::pair<int, int>{ x, y });
	MapNode *mapNode;
			
	if (it != mapContents.end())
	{
		mapNode = it->second;
		mapNode->setContents(nodeType);
	}
	else
	{
		mapDirty = true;
		std::pair<int, int> coord;
		coord.first = x;
		coord.second = y;
		if (x < minXY.first)
		{
			minXY.first = x;
		}
		if (x > maxXY.first)
		{
			maxXY.first = x;
		}
		if (y < minXY.second)
		{
			minXY.second = y;
		}
		if (y > maxXY.second)
		{
			maxXY.second = x;
		}
		if (minXY.first < 0)
			mapModX = abs(minXY.first);
		if (minXY.second < 0)
			mapModY = abs(minXY.second);
		mapNode = new MapNode(coord, nodeType, location);
		mapContents.emplace(coord, mapNode);		
#ifdef TRACE
		std::stringstream ss;
		ss << "Added node (" << x << "," << y << ") with type: " << nodeType << " to map";		
		robotLib->Log(ss.str());
#endif
		if (nodeType == map_node_t::BLOCK_BASE_STATION)
		{
			if (std::find(baseStations.begin(), baseStations.end(), mapNode) == baseStations.end())
			{
#ifdef TRACE
				std::stringstream ss;
				ss << "Adding base station at (" << x << "," << y << ")";
				robotLib->Log(ss.str());
#endif
				baseStations.push_back(mapNode);
			}
		}
	}
}

int Map::getCartNodeWeight(int x, int y)
{
	MapNode *node = getCartNode(x, y);
	if (node)
	{
		return node->getMapWeight();
	}
	// Now create an unknown node, just so we can save doing this in the
	// future
	setNode(x, y, map_node_t::BLOCK_UNKNOWN, std::make_pair<int, int>(-1, -1));
	return map_node_t::BLOCK_UNKNOWN;
}

void Map::createPathMap()
{
	int h = getHeight();
	int w = getWidth();
	std::vector<int> row;
	for (int y = 0; y < h; y++)
	{
		row.clear();
		for (int x = 0; x < w; x++)
		{
			if (getCartNodeWeight(x, y) > 1)
				row.emplace_back(1);
			else
				row.emplace_back(0);
		}
		pathMap.emplace_back(row);
	}
}

MapNode *Map::getCartNode(int x, int y)
{
	return getNode(x + mapModX, y + mapModY);
}

std::vector<MapNode *> Map::getBaseStations()
{
	return baseStations;
}

MapNode* Map::getNode(int x, int y)
{
	std::map<std::pair<int, int>, MapNode *>::iterator it = mapContents.find(std::pair<int, int>{ x, y });
	if (it == mapContents.end())
	{
		return NULL;
	}
	return it->second;
}

// Map format:
// <map minX=int minY=int maxX=int maxY=int>
//	<Node x=int, y=int, type=enum>
// </map>

bool Map::saveMap()
{
	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* root = doc.allocate_node(rapidxml::node_element, "Map");
	root->append_attribute(doc.allocate_attribute("minX", std::to_string(minXY.first).c_str()));
	root->append_attribute(doc.allocate_attribute("minY", std::to_string(minXY.second).c_str()));
	root->append_attribute(doc.allocate_attribute("maxX", std::to_string(maxXY.first).c_str()));
	root->append_attribute(doc.allocate_attribute("maxY", std::to_string(maxXY.second).c_str()));
	doc.append_node(root);
	for (std::map<std::pair<int, int>, MapNode *>::iterator it = mapContents.begin(); it != mapContents.end(); it++)
	{
		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "Node");
		node->append_attribute(doc.allocate_attribute("x", std::to_string(it->first.first).c_str()));
		node->append_attribute(doc.allocate_attribute("y", std::to_string(it->first.second).c_str()));
		node->append_attribute(doc.allocate_attribute("latitude", std::to_string(it->second->getLocation().first).c_str()));
		node->append_attribute(doc.allocate_attribute("longitude", std::to_string(it->second->getLocation().second).c_str()));
		node->append_attribute(doc.allocate_attribute("type", std::to_string(it->second->blockContents()).c_str()));
		root->append_node(node);
	}
	
	std::string xml;
	rapidxml::print(std::back_inserter(xml), doc);
	std::ofstream filestream;
	try
	{
		filestream.open(MAP_FILE);
		filestream << doc;
		filestream.close();
	}
	catch (std::exception &e)
	{
		robotLib->LogException(e);
		return false;
	}	
	return true;
}

bool Map::loadMap()
{
	
}

// Adjusts the map to be 0->x as opposed to 0,0 being center
std::pair<int, int> Map::getPathCoord(MapNode *node)
{
	int x=0;
	int y=0;
	std::pair<int, int> min, max;
	this->getExtents(min, max);
	std::pair<int, int> retVal;
	if (min.first < 0)
	{
		x = abs(min.first);
	}
	if (min.second < 0)
	{
		y = abs(min.second);
	}
	retVal.first = node->getGridCoord().first + x;
	retVal.second = node->getGridCoord().second + y;
	return retVal;
}
	
int Map::getWidth()
{
	std::pair<int, int> min, max;
	getExtents(min, max);
	return max.first - min.first+1;
}

int Map::getHeight()
{
	std::pair<int, int> min, max;
	getExtents(min, max);
	return max.second - min.second+1;
}

void Map::setMapDirty(bool dirty)
{
	mapDirty = dirty;
}

bool Map::getMapDirty()
{
	return mapDirty;
}

// Determine priority (in the priority queue)
bool operator<(const node & a, const node & b)
{
	return a.getPriority() > b.getPriority();
}

std::vector<MapNode *> Map::findPath(std::pair<int, int> start, std::pair<int, int> end)
{
	int i, j, x, y, xdx, ydy;
	if (mapDirty)
	{
 		createPathMap();
		mapDirty = false;
	}
	// clear node maps
	std::vector<int> clearRow;
	openNodesMap.clear();
	closedNodesMap.clear();
	dirMap.clear();
	for (x = 0; x < getWidth(); x++)
		clearRow.emplace_back(0);
	for (y = 0; y < getHeight(); y++)
	{
		openNodesMap.emplace_back(clearRow);
		closedNodesMap.emplace_back(clearRow);
		dirMap.emplace_back(clearRow);
	}
	std::vector<MapNode *> path;
	
	int xStart = start.first;
	int yStart = start.second;
	int xFinish = end.first;
	int yFinish = end.second;
	
	std::priority_queue<node> pq[2]; // list of open (not-yet-tried) nodes
	int pqi; // pq index
	node* n0;
	node* m0;
	
	pqi = 0;
	// create the start node and push into list of open nodes
	n0 = new node(xStart, yStart, 0, 0);
	n0->updatePriority(xFinish, yFinish);
	pq[pqi].push(*n0);
	openNodesMap[yStart][xStart] = n0->getPriority(); // mark it on the open nodes map
	int width = getWidth();
	int height = getHeight();
	// A* search
	while (!pq[pqi].empty())
	{
	    // get the current node w/ the highest priority
	    // from the list of open nodes
		n0 = new node( pq[pqi].top().getxPos(),
			pq[pqi].top().getyPos(), 
			pq[pqi].top().getLevel(),
			pq[pqi].top().getPriority());

		x = n0->getxPos(); y = n0->getyPos();

		pq[pqi].pop(); // remove the node from the open list
		openNodesMap[y][x] = 0;
		// mark it on the closed nodes map
		closedNodesMap[y][x] = 1;

		        // quit searching when the goal state is reached
		        //if((*n0).estimate(xFinish, yFinish) == 0)
		if (x == xFinish && y == yFinish) 
		{
		    // generate the path from finish to start
		    // by following the directions
			path.clear();
			while (!(x == xStart && y == yStart))
			{
				j = dirMap[y][x];
				path.emplace_back(getCartNode(x, y));
				x += dx[j];
				y += dy[j];
			}
			// garbage collection
			delete n0;
			// empty the leftover nodes
			while (!pq[pqi].empty()) pq[pqi].pop();           
			return path;
		}

		// generate moves (child nodes) in all possible directions
		for (i = 0; i < 8; i++)
		{
			xdx = x + dx[i]; ydy = y + dy[i];

			if ((xdx >= 0 || xdx <= width - 1 || ydy >= 0 || ydy <= height - 1))
			{
				if (pathMap[ydy][xdx] != 1 && closedNodesMap[ydy][xdx] != 1)		
				{
				    // generate a child node
					m0 = new node( xdx,
						ydy,
						n0->getLevel(), 
						n0->getPriority());
					m0->nextLevel(i);
					m0->updatePriority(xFinish, yFinish);

									// if it is not in the open list then add into that
					if (openNodesMap[ydy][xdx] == 0)
					{
						openNodesMap[ydy][xdx] = m0->getPriority();
						pq[pqi].push(*m0);
						// mark its parent node direction
						dirMap[ydy][xdx] = (i + 8 / 2) % 8;
					}
					else if (openNodesMap[ydy][xdx] > m0->getPriority())
					{
					    // update the priority info
						openNodesMap[ydy][xdx] = m0->getPriority();
						// update the parent direction info
						dirMap[ydy][xdx] = (i + 8 / 2) % 8;

											// replace the node
											// by emptying one pq to the other one
											// except the node to be replaced will be ignored
											// and the new node will be pushed in instead
						while (!(pq[pqi].top().getxPos() == xdx && 
						       pq[pqi].top().getyPos() == ydy))
						{                
							pq[1 - pqi].push(pq[pqi].top());
							pq[pqi].pop();       
						}
						pq[pqi].pop(); // remove the wanted node
                    
						// empty the larger size pq to the smaller one
						if (pq[pqi].size() > pq[1 - pqi].size()) pqi = 1 - pqi;
						while (!pq[pqi].empty())
						{                
							pq[1 - pqi].push(pq[pqi].top());
							pq[pqi].pop();       
						}
						pqi = 1 - pqi;
						pq[pqi].push(*m0); // add the better node instead
					}
					else delete m0; // garbage collection
				}
			}
		}
		delete n0; // garbage collection
	}
	path.empty();
	return path;
}

std::vector<MapNode *> Map::getPathToClosestBase(std::pair<int, int> start)
{
	std::vector<MapNode *> path;
	
	for (int a=0; a<baseStations.size(); a++)
	{
		std::vector<MapNode *> p = findPath(start, std::make_pair<int, int>(baseStations[a]->getGridCoord().first, baseStations[a]->getGridCoord().second));
		if (p.size() > 0 && ((p.size() < path.size()) || path.size()==0 ))
			path = p;
	}
	return path;
}

void Map::clearMowedFlags()
{
	std::map<std::pair<int, int>, MapNode*>::iterator it;
	for (it = mapContents.begin(); it != mapContents.end(); it++)
	{
		it->second->setMowedFlag(false);
	}
}