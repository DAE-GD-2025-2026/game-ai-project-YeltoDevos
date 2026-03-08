#include "SpacePartitioning.h"

#include "GeometryCollection/GeometryCollectionConvexUtility.h"

// --- Cell ---
// ------------
Cell::Cell(float Left, float Bottom, float Width, float Height)
{
	BoundingBox.Min = { Left, Bottom };
	BoundingBox.Max = { BoundingBox.Min.X + Width, BoundingBox.Min.Y + Height };
}

std::vector<FVector2D> Cell::GetRectPoints() const
{
	const float left = BoundingBox.Min.X;
	const float bottom = BoundingBox.Min.Y;
	const float width = BoundingBox.Max.X - BoundingBox.Min.X;
	const float height = BoundingBox.Max.Y - BoundingBox.Min.Y;

	std::vector<FVector2D> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(UWorld* pWorld, float Width, float Height, int Rows, int Cols, int MaxEntities)
	: pWorld{pWorld}
	, SpaceWidth{Width}
	, SpaceHeight{Height}
	, NrOfRows{Rows}
	, NrOfCols{Cols}
	, NrOfNeighbors{0}
{
	Neighbors.SetNum(MaxEntities);
	
	//calculate bounds of a cell
	CellWidth = Width / Cols;
	CellHeight = Height / Rows;

	// TODO create the cells
	
	Cells.reserve(Rows * Cols);
	
	for (std::size_t i = 0; i < Rows; ++i)
	{
		for (std::size_t j = 0; j < Cols; ++j)
		{
			Cells.emplace_back(i * CellWidth - (SpaceWidth / 2.f), j * CellHeight- (SpaceHeight / 2.f), CellWidth, CellHeight);
		}
	}
	
}

void CellSpace::AddAgent(ASteeringAgent& Agent)
{
	// TODO Add the agent to the correct cell
	
	Cells[PositionToIndex(Agent.GetPosition())].Agents.emplace_back(&Agent);
}

void CellSpace::UpdateAgentCell(ASteeringAgent& Agent, const FVector2D& OldPos)
{
	//TODO Check if the agent needs to be moved to another cell.
	//TODO Use the calculated index for oldPos and currentPos for this
	
	
	const int newCell {PositionToIndex(Agent.GetPosition())};
	const int oldCell {PositionToIndex(OldPos)};
	
	if (oldCell != newCell)
	{
		auto agentIterator = std::ranges::find(Cells[oldCell].Agents, &Agent);
		Cells[oldCell].Agents.erase(agentIterator);
		
		Cells[newCell].Agents.emplace_back(&Agent);
	}
}

void CellSpace::RegisterNeighbors(ASteeringAgent& Agent, float QueryRadius)
{
	// TODO Register the neighbors for the provided agent
	// TODO Only check the cells that are within the radius of the neighborhood
	
	NrOfNeighbors = 0;
	Neighbors.Empty();
	Neighbors.Reserve(20);
	
	for (Cell c : Cells)
	{
		const FRect rect{FVector2D(Agent.GetPosition().X - QueryRadius, Agent.GetPosition().Y - QueryRadius),
						 FVector2D(Agent.GetPosition().X + QueryRadius, Agent.GetPosition().Y + QueryRadius)};
		
		if (DoRectsOverlap(rect, c.BoundingBox))
		{
			for (ASteeringAgent* neighbour : c.Agents)
			{
				const float distance {static_cast<float>(FVector2D::Distance(Agent.GetPosition(), neighbour->GetPosition()))};
				if (distance < QueryRadius && neighbour != &Agent)
				{
					Neighbors.Add(neighbour);
					++NrOfNeighbors;
				}
			}
		}
	}
	
	Neighbors.Shrink();
}

void CellSpace::EmptyCells()
{
	for (Cell& c : Cells)
		c.Agents.clear();
}

void CellSpace::RenderCells() const
{
	// TODO Render the cells with the number of agents inside of it
	
	FlushDebugStrings(pWorld);
	
	for (Cell c : Cells)
	{
		DrawDebugLine(pWorld, FVector(c.GetRectPoints()[0], 10.f), FVector(c.GetRectPoints()[1], 10.f), FColor::Blue);
		DrawDebugLine(pWorld, FVector(c.GetRectPoints()[1], 10.f), FVector(c.GetRectPoints()[2], 10.f), FColor::Blue);
		DrawDebugLine(pWorld, FVector(c.GetRectPoints()[2], 10.f), FVector(c.GetRectPoints()[3], 10.f), FColor::Blue);
		DrawDebugLine(pWorld, FVector(c.GetRectPoints()[3], 10.f), FVector(c.GetRectPoints()[0], 10.f), FColor::Blue);
		
		FString nrOfNeighborsStr{std::to_string(c.Agents.size()).c_str()};
		DrawDebugString(pWorld, FVector(c.GetRectPoints()[3], 10.f), nrOfNeighborsStr);
	}
}

int CellSpace::PositionToIndex(FVector2D const & Pos) const
{
	// TODO Calculate the index of the cell based on the position
	
	for (std::size_t i = 0; i < Cells.size(); ++i)
	{
		TArray<UE::Math::TVector2<double>> arrPoints{};
		for (UE::Math::TVector2<double> point : Cells[i].GetRectPoints())
		{
			arrPoints.Add(point);
		}
		
		FBox2D box{arrPoints};
		if (box.IsInsideOrOn(Pos))
		{
			return i;
		}
	}
	
	return 0;
}

bool CellSpace::DoRectsOverlap(FRect const & RectA, FRect const & RectB)
{
	// Check if the rectangles are separated on either axis
	if (RectA.Max.X < RectB.Min.X || RectA.Min.X > RectB.Max.X) return false;
	if (RectA.Max.Y < RectB.Min.Y || RectA.Min.Y > RectB.Max.Y) return false;
    
	// If they are not separated, they must overlap
	return true;
}