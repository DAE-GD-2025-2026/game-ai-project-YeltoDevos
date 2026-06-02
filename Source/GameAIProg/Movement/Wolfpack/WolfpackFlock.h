#pragma once

// Toggle this define to enable/disable spatial partitioning
#define GAMEAI_USE_SPACE_PARTITIONING

#include "Movement/SteeringBehaviors/Flocking/FlockingSteeringBehaviors.h"
#include "Movement/SteeringBehaviors/Flocking/Flock.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"
#include "Movement/SteeringBehaviors/SteeringHelpers.h"
#include "Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"
#include <memory>
#include "imgui.h"
#ifdef GAMEAI_USE_SPACE_PARTITIONING
#include "Movement/SteeringBehaviors/SpacePartitioning/SpacePartitioning.h"
#endif

class WolfPackFlock : public Flock
{
public:
	WolfPackFlock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize = 10, 
	float WorldSize = 100.f, 
	ASteeringAgent* const pAgentToEvade = nullptr, 
	bool bTrimWorld = false);

	virtual ~WolfPackFlock() override;
	
	virtual void Tick(float DeltaTime) override;
	virtual void ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize) override;
	
protected:
	
	std::unique_ptr<Flee> pFleeBehavior{};
	std::unique_ptr<Arrive> pArrive{};
	
	
};
