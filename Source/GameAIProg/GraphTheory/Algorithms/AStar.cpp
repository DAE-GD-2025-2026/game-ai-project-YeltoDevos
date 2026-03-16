#include "AStar.h"

using namespace GameAI;

AStar::AStar(Graph* const pGraph, HeuristicFunctions::Heuristic hFunction)
	: pGraph(pGraph)
	, HeuristicFunction(hFunction)
{
}

std::vector<Node*>AStar::FindPath(Node* const pStartNode, Node* const pGoalNode)
{
	std::vector<Node*> path{};
	
	std::vector<NodeRecord> openList{};
	std::vector<NodeRecord> closedList{};
	NodeRecord startRecord{pStartNode, nullptr, 0.f, GetHeuristicCost(pStartNode, pGoalNode)};
	NodeRecord currentRecord{};
	
	openList.emplace_back(startRecord);
	
	while (!openList.empty())
	{
		auto currentIt = std::min_element(openList.begin(), openList.end());
		currentRecord = *currentIt;
		
		if (currentRecord.pNode == pGoalNode)
		{
			break;
		}
		
		openList.erase(currentIt);
		
		const std::vector<Connection*> Connections{pGraph->FindConnectionsFrom(currentRecord.pNode->GetId())};	
		float heurisiticCost{0};
		
		for (Connection* connection : Connections)
		{
			Node* pNextNode {pGraph->GetNode(connection->GetToId()).get()};
			const float totalGCost {currentRecord.costSoFar + connection->GetWeight()};
			
			auto nodeItClosed = std::find_if(closedList.begin(), closedList.end(), 
			[pNextNode](const NodeRecord& record)
			{
				return record.pNode == pNextNode;
			});
			
			if (nodeItClosed != closedList.end())
			{
				if (nodeItClosed->costSoFar < totalGCost)
				{
					continue;
				}
				heurisiticCost = nodeItClosed->estimatedTotalCost - nodeItClosed->costSoFar;
				closedList.erase(nodeItClosed);
			}
			
			auto nodeItOpen = std::find_if(openList.begin(), openList.end(), 
			[pNextNode](const NodeRecord& record)
			{
				return record.pNode == pNextNode;	
			});
			
			if (nodeItOpen != openList.end())
			{
				if (nodeItOpen->costSoFar < totalGCost)
				{
					continue;
				}
				
				heurisiticCost = nodeItOpen->estimatedTotalCost - nodeItOpen->costSoFar;
				openList.erase(nodeItOpen);
			}
			else
			{
				heurisiticCost = GetHeuristicCost(pNextNode, pGoalNode);
			}
			
			NodeRecord newRecord{pNextNode, 
				connection, 
				totalGCost, 
				totalGCost + heurisiticCost};
			openList.emplace_back(newRecord);
		}
		
		closedList.emplace_back(currentRecord);
		
	}
	
	while (currentRecord.pNode != pStartNode)
	{
		path.emplace_back(currentRecord.pNode);
		
		if (currentRecord.pConnection == nullptr)
		{
			path.clear();
			return path;
		}
		
		const int nodeid{currentRecord.pConnection->GetFromId()};
		auto it = std::ranges::find_if(closedList, [nodeid](const NodeRecord& record)
		{
			return record.pNode->GetId() == nodeid;
		});
		
		currentRecord = *it;
	}
	
	path.emplace_back(pStartNode);
	
	std::ranges::reverse(path);
	return path;
}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
	FVector2D toDestination = pGraph->GetNode(pEndNode->GetId())->GetPosition() - pGraph->GetNode(pStartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}