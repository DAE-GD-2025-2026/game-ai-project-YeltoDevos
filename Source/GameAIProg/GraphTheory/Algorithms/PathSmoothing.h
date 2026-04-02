#pragma once
#include <vector>

#include "NavGraphPathfinding.h"
#include "StaticMeshAttributes.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

namespace GameAI
{
	class SSFA final
{
public:
	//=== SSFA Functions ===
	//--- References ---
	//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
	//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
	static std::vector<NavLine> FindPortals(std::vector<Node*> const & Path, TriPolygon const & NavPoly)
	{
		//Container
		std::vector<NavLine> Portals = {};
		
		//For each node received, get it's corresponding line
		
		const std::vector<TriPolygon::Edge>& edges {NavPoly.GetEdges()};
		
		for (int idx{0}; idx < Path.size() - 1; ++idx)
		{
			const auto edge{edges[Path[idx]->GetId()]};
			
			//Redetermine it's "orientation" based on the required path (left-right vs right-left) - p1 should be right point
			FVector2D P1{edge.GetP1(NavPoly).X, edge.GetP1(NavPoly).Y};
			FVector2D P2{edge.GetP2(NavPoly).X, edge.GetP2(NavPoly).Y};
			
			FVector2D direction{Path[idx + 1]->GetPosition() - Path[idx]->GetPosition()};
			
			auto value = FVector2D::CrossProduct(P1 - Path[idx]->GetPosition(), direction);
			
			//Store portal
			if (value >= 0)
			{
				Portals.emplace_back(P1, P2);
			}
			else
			{
				Portals.emplace_back(P2, P1);
			}
		}

		//Add degenerate portal to force end evaluation
		Portals.emplace_back(Path[Path.size() - 1]->GetPosition(), Path[Path.size() - 1]->GetPosition());

		return Portals;
	}

	static std::vector<FVector2D> OptimizePortals( std::vector<NavLine> const & Portals, TriPolygon const & NavPoly)
	{
		std::vector<FVector2D> Path{};
		//P1 == right point of portal, P2 == left point of portal
		
			//--- RIGHT CHECK ---
			//1. See if moving funnel inwards - RIGHT
			
				//2. See if new line degenerates a line segment - RIGHT
				
					//Leftleg becomes new apex point

					//Calculate new legs (if not the end)


			//--- LEFT CHECK ---
			//1. See if moving funnel inwards - LEFT

				//2. See if new line degenerates a line segment - LEFT

					//Rightleg becomes new apex point

					//Calculate new legs (if not the end)


		// Add last path point

		return Path;
	}
private:
	SSFA() {};
	~SSFA() {};
};
}
