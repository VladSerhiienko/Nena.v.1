#include "StaticPathFinder.h"
#include "DynamicPathFinder.h"

void TestStaticPathFinder();
void TestDynamicPathFinder();

void main()
{
	//TestStaticPathFinder();
	TestDynamicPathFinder();
}

void TestDynamicPathFinder()
{
	using namespace Demo;

	DyTerrain terrain(7, 7);
	/*terrain.GetNodeAt(uint32x2(3, 6))->Info.Obstacle = true;
	terrain.GetNodeAt(uint32x2(3, 5))->Info.Obstacle = true;
	terrain.GetNodeAt(uint32x2(3, 4))->Info.Obstacle = true;
	terrain.GetNodeAt(uint32x2(3, 3))->Info.Obstacle = true;
	terrain.GetNodeAt(uint32x2(3, 2))->Info.Obstacle = true;
	terrain.GetNodeAt(uint32x2(3, 1))->Info.Obstacle = true;
	terrain.GetNodeAt(uint32x2(2, 1))->Info.Obstacle = true;
	terrain.GetNodeAt(uint32x2(1, 1))->Info.Obstacle = true;*/
	terrain.GetNodeAt(uint32x2(3, 6))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(3, 5))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(3, 4))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(3, 3))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(3, 2))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(3, 1))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(2, 1))->Info.Obstacle = true;
	terrain.GetNodeAt(uint32x2(1, 1))->Info.Obstacle = true;
	/*terrain.GetNodeAt(uint32x2(2, 1))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(1, 1))->Info.Walkable = false;*/

	terrain.Draw();

	DyRouteExplorer explorer;
	DyRouteGuide planner;

	explorer.InitNode = &terrain.GetNodeAt(uint32x2(1, 6))->Info;
	explorer.GoalNode = &terrain.GetNodeAt(uint32x2(5, 6))->Info;
	explorer.Terrain = &terrain;

	planner.Explorer = &explorer;
	planner.Launch();

}

void TestStaticPathFinder()
{
	using namespace Demo;

	SaTerrain terrain(7, 7);

	terrain.GetNodeAt(uint32x2(3, 6))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(3, 5))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(3, 4))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(3, 3))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(3, 2))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(3, 1))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(2, 1))->Info.Walkable = false;
	terrain.GetNodeAt(uint32x2(1, 1))->Info.Walkable = false;

	terrain.Draw();

	SaPathFinder pathFinder;
	pathFinder.SetTerrain(&terrain);
	pathFinder.SetInitNode(uint32x2(1, 6));
	pathFinder.SetGoalNode(uint32x2(5, 6));
	pathFinder.Launch();

}