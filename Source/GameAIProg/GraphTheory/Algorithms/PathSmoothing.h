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
		Portals.emplace_back(Path[0]->GetPosition(), Path[0]->GetPosition());
		
		const std::vector<TriPolygon::Edge>& edges {NavPoly.GetEdges()};
		
		for (int idx{1}; idx < Path.size() - 1; ++idx)
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
		
		Path.emplace_back(Portals[0].P1);
		
		int rightLegIdx{1};
		int leftLegIdx{1};
		FVector2D rightLeg{Portals[rightLegIdx].P1 - Path[Path.size() - 1]};
		FVector2D leftLeg{Portals[leftLegIdx].P2 - Path[Path.size() - 1]};

		
		//P1 == right point of portal, P2 == left point of portal
		for (int portalIdx{1}; portalIdx < Portals.size() ; ++portalIdx)
		{
			NavLine currentPortal = Portals[portalIdx];
			
			//--- RIGHT CHECK ---
			FVector2D newRightLeg{currentPortal.P1 - Path[Path.size() - 1]};
			
			auto crossResult{FVector2D::CrossProduct(rightLeg, newRightLeg)};
			if (crossResult > 0)
			{
				auto crossLeftLeg{FVector2D::CrossProduct(leftLeg, newRightLeg)};
				if (crossLeftLeg >= 0)
				{
					Path.emplace_back(Portals[leftLegIdx].P2);
					portalIdx = leftLegIdx + 1;
					leftLegIdx = portalIdx;
					rightLegIdx = portalIdx;
					
					if(portalIdx < Portals.size())
					{
						rightLeg=Portals[rightLegIdx].P1 - Path[Path.size() - 1];
						leftLeg=Portals[leftLegIdx].P2 - Path[Path.size() - 1];
						continue;
					}
				}
				else
				{
					rightLeg = newRightLeg;
					rightLegIdx = portalIdx;
				}
			}
			
			//--- LEFT CHECK ---
			FVector2D newLeftLeg{currentPortal.P2 - Path[Path.size() - 1]};
			
			crossResult = FVector2D::CrossProduct(leftLeg, newLeftLeg);
			if (crossResult < 0)
			{
				auto crossLeftLeg{FVector2D::CrossProduct(rightLeg, newLeftLeg)};
				if (crossLeftLeg <= 0)
				{
					Path.emplace_back(Portals[rightLegIdx].P1);
					portalIdx = rightLegIdx + 1;
					rightLegIdx = portalIdx;
					leftLegIdx = portalIdx;
					
					if(portalIdx < Portals.size())
					{
						rightLeg=Portals[rightLegIdx].P1 - Path[Path.size() - 1];
						leftLeg=Portals[leftLegIdx].P2 - Path[Path.size() - 1];
						continue;
					}
				}
				else
				{
					leftLeg = newLeftLeg;
					leftLegIdx = portalIdx;
				}
			}
		}
		
		// Add last path point
		Path.emplace_back(Portals[Portals.size() - 1].P1);

		return Path;
	}
private:
	SSFA() {};
	~SSFA() {};
};
}
