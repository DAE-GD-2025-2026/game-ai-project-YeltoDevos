#include "NavGraphPathfinding.h"

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

using namespace GameAI;

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos,
	NavGraph* const pNavGraph, std::vector<FVector2D>& debugNodePositions, std::vector<NavLine>& debugPortals) 
{
	//Create the path to return
	std::vector<FVector2D> finalPath{};

	//Get the start and endTriangle
	const auto& startTriangle {pNavGraph->GetNavPolygon()->GetTriangleAtPosition(startPos, true)};
	const auto& endTriangle{pNavGraph->GetNavPolygon()->GetTriangleAtPosition(endPos, true)};
	
	if (startTriangle == nullptr || endTriangle == nullptr)
	{
		return finalPath;
	}
	
	if (startTriangle == endTriangle)
	{
		finalPath.push_back(startPos);
		finalPath.push_back(endPos);
		return finalPath;
	}

	//We have valid start/end triangles and they are not the same
	//=> Start looking for a path
	//Copy the graph
	std::shared_ptr<NavGraph> pCopyGraph(pNavGraph->Clone());

	//Create Extra node for the Start Node (Agent's position
	const int startNodeId = pCopyGraph->AddNode(std::make_unique<NavGraphNode>(startPos, -1));
	
	for (const auto& edge : startTriangle->GetEdges())
	{
		auto idx {pNavGraph->GetNavPolygon()->FindEdgeIndex(edge)};
		
		if (idx.has_value())
		{
			auto edgeNodeId{pCopyGraph->GetNodeIdFromEdgeIndex(idx.value())};
			if (edgeNodeId == Graphs::InvalidNodeId)
				continue;
			
			Connection conn{startNodeId, edgeNodeId};
			
			const FVector2D weight =
			pCopyGraph->GetNode(startNodeId)->GetPosition() -
			pCopyGraph->GetNode(edgeNodeId)->GetPosition();
			conn.SetWeight(weight.Length());
			
			pCopyGraph->AddConnection(std::make_unique<Connection>(conn));
		}
	}

	//Create extra node for the endNode
	const int endNodeId = pCopyGraph->AddNode(std::make_unique<NavGraphNode>(endPos, -1));
	
	for (const auto& edge : endTriangle->GetEdges())
	{
		auto idx {pNavGraph->GetNavPolygon()->FindEdgeIndex(edge)};
		
		if (idx.has_value())
		{
			auto edgeNodeId{pCopyGraph->GetNodeIdFromEdgeIndex(idx.value())};
			if (edgeNodeId == Graphs::InvalidNodeId)
				continue;
			
			Connection conn{endNodeId, edgeNodeId};
			
			const FVector2D weight =
			pCopyGraph->GetNode(endNodeId)->GetPosition() -
			pCopyGraph->GetNode(edgeNodeId)->GetPosition();
			conn.SetWeight(weight.Length());
			
			pCopyGraph->AddConnection(std::make_unique<Connection>(conn));
		}
	}
	

	//Run A star on new graph
	AStar astar{pCopyGraph.get(), HeuristicFunctions::Chebyshev};
	
	auto nodePath {astar.FindPath(
		pCopyGraph->GetNode(startNodeId).get(),
		pCopyGraph->GetNode(endNodeId).get()
		)};
	
	std::vector<FVector2D> path{};
	
	for (const auto& node : nodePath)
	{
		path.push_back(node->GetPosition());
	}
	
	finalPath = path;
	//Debug Visualisation
	debugNodePositions = path;

	// Extra: Run optimiser on new graph (First check if everything works without SSFA!)
	debugPortals = SSFA::FindPortals(nodePath, *pNavGraph->GetNavPolygon());
	std::vector<FVector2D> testPath{};
 	finalPath = SSFA::OptimizePortals(debugPortals, *pNavGraph->GetNavPolygon());
	
	return finalPath;
}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos, NavGraph* const pNavGraph)
{
	std::vector<FVector2D> debugNodePositions{};
	std::vector<NavLine> debugPortals{};

	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}