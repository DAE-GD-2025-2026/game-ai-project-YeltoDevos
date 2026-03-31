#include "NavGraph.h"

#include "NavGraphNode.h"

GameAI::NavGraph::NavGraph(std::unique_ptr<TriPolygon> && NavPoly)
	: Graph{false}
	, pNavPoly{std::move(NavPoly)}
{
	CreateNavigationGraph();
}

GameAI::NavGraph::NavGraph(const NavGraph& Other)
	: Graph(false)
{
	Nodes.reserve(Other.Nodes.size());
	for (std::unique_ptr<Node> const & OtherNode : Other.Nodes)
	{
		Nodes.push_back(std::make_unique<NavGraphNode>(*dynamic_cast<NavGraphNode*>(OtherNode.get())));
	}
        
	Connections.reserve(Other.Connections.size());
	for (std::unique_ptr<Connection> const & OtherConnection : Other.Connections)
	{
		Connections.push_back(std::make_unique<Connection>(*OtherConnection.get()));
	}
}

std::unique_ptr<GameAI::NavGraph> GameAI::NavGraph::Clone() const
{
	return std::make_unique<NavGraph>(*this);
}

int GameAI::NavGraph::GetNodeIdFromEdgeIndex(int EdgeIdx) const
{
	if (EdgeIdx >= 0)
	{
		for (auto const & pNode : Nodes)
		{
			if (reinterpret_cast<NavGraphNode*>(pNode.get())->GetEdgeIdx() == EdgeIdx)
			{
				return pNode->GetId();
			}
		}
	}
	
	return Graphs::InvalidNodeId;
}

void GameAI::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigation mesh and create nodes
			// Create node here
	
	const auto edges{pNavPoly->GetEdges()};
	
	for (const auto& edge : edges)
	{
		auto idx = pNavPoly->FindEdgeIndex(edge);
		
		if (idx.has_value())
		{
			const auto triangles {pNavPoly->GetTriangleNeighbors(idx.value())};
			//if (triangles.size() > 1)
			{
				const auto p1{edge.GetP1(*pNavPoly)};
				const auto p2{edge.GetP2(*pNavPoly)};
				
				const float x {static_cast<float>(p1.X + p2.X) /2.f};
				const float y {static_cast<float>(p1.Y + p2.Y) /2.f};
				
				auto node {std::make_unique<NavGraphNode>(FVector2D(x,y), idx.value())};
				
				this->AddNode(std::move(node));
			}
		}
	}
	

	//2. Create connections now that every node is created	
		//2 valid nodes -> 1 connection
		//3 valid nodes -> 3 connections
	
	const auto triangles{pNavPoly->GetTriangles()};
	
	for (const auto & triangle : triangles)
	{
		std::vector<int> tmpNodeIds{};
		
		for (const auto& edge : triangle.GetEdges())
		{
			auto idx = pNavPoly->FindEdgeIndex(edge);
			if (idx.has_value())
			{
				const auto nodeId{GetNodeIdFromEdgeIndex(idx.value())};
				if (nodeId != Graphs::InvalidNodeId)
				{
					tmpNodeIds.emplace_back(GetNodeIdFromEdgeIndex(idx.value()));
				}
				
			}
		}
		
		if (tmpNodeIds.size() == 2)
		{
			AddConnection(tmpNodeIds[0], tmpNodeIds[1]);
		}
		else if (tmpNodeIds.size() == 3)
		{
			AddConnection(tmpNodeIds[0], tmpNodeIds[1]);
			AddConnection(tmpNodeIds[1], tmpNodeIds[2]);
			AddConnection(tmpNodeIds[2], tmpNodeIds[0]);
		}
		
	}
	
		
	//3. Set the connections cost to the actual distance
	
	for (auto& connection : Connections)
	{
		const FVector2D& p1 {GetNode(connection->GetFromId())->GetPosition()};
		const FVector2D& p2 {GetNode(connection->GetToId())->GetPosition()};
		
		const float length{static_cast<float>((p1 - p2).Length())};
		
		connection->SetWeight(length);
	}
}
