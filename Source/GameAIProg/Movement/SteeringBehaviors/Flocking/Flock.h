#pragma once

// Toggle this define to enable/disable spatial partitioning
#define GAMEAI_USE_SPACE_PARTITIONING

#include "FlockingSteeringBehaviors.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"
#include "Movement/SteeringBehaviors/SteeringHelpers.h"
#include "Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"
#include <memory>
#include "imgui.h"
#ifdef GAMEAI_USE_SPACE_PARTITIONING
#include "../SpacePartitioning/SpacePartitioning.h"
#endif

class Flock
{
public:
	Flock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize = 10, 
	float WorldSize = 100.f, 
	ASteeringAgent* const pAgentToEvade = nullptr, 
	bool bTrimWorld = false);

	virtual ~Flock();

	virtual void Tick(float DeltaTime);
	virtual void RenderDebug();
	virtual void ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize);

#ifdef GAMEAI_USE_SPACE_PARTITIONING
	virtual const TArray<ASteeringAgent*>& GetNeighbors() const { return Neighbors; }
	virtual int GetNrOfNeighbors() const { return NrOfNeighbors; }
	
	virtual void RegisterNeighbors(ASteeringAgent* const Agent);
	
#else // No space partitioning
	int GetNrOfNeighbors() const { return NrOfNeighbors; }
	const TArray<ASteeringAgent*>& GetNeighbors() const { return Neighbors; }
	
#endif // USE_SPACE_PARTITIONING

	virtual FVector2D GetAverageNeighborPos() const;
	virtual FVector2D GetAverageNeighborVelocity() const;

	virtual void SetTarget_Seek(FSteeringParams const & Target);

protected:
	// For debug rendering purposes
	UWorld* pWorld{nullptr};
	
	int FlockSize{0};
	TArray<ASteeringAgent*> Agents{};
	
	FSteeringParams MouseTarget{};

#ifdef GAMEAI_USE_SPACE_PARTITIONING
	std::unique_ptr<CellSpace> pPartitionedSpace{};
	int NrOfCellsX{ 10 };
	TArray<FVector2D> OldPositions{};
	
#else // No space partitioning
	
#endif // USE_SPACE_PARTITIONING
	
	TArray<ASteeringAgent*> Neighbors{};
	float NeighborhoodRadius{200.f};
	int NrOfNeighbors{0};

	ASteeringAgent* pAgentToEvade{nullptr};
	
	//Steering Behaviors
	std::unique_ptr<Separation> pSeparationBehavior{};
	std::unique_ptr<Cohesion> pCohesionBehavior{};
	std::unique_ptr<VelocityMatch> pVelMatchBehavior{};
	std::unique_ptr<Seek> pSeekBehavior{};
	std::unique_ptr<Wander> pWanderBehavior{};
	std::unique_ptr<Evade> pEvadeBehavior{};
	
	std::unique_ptr<BlendedSteering> pBlendedSteering{};
	std::unique_ptr<PrioritySteering> pPrioritySteering{};

	// UI and rendering
	bool UseSpatialPartition{false};
	bool DebugRenderSteering{false};
	bool DebugRenderNeighborhood{true};
	bool DebugRenderPartitions{true};

	virtual void RenderNeighborhood();
};
