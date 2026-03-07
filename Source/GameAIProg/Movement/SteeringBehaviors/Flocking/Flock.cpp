#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "VectorTypes.h"
#include "Shared/ImGuiHelpers.h"


Flock::Flock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize,
	float WorldSize,
	ASteeringAgent* const pAgentToEvade,
	bool bTrimWorld)
	: pWorld{pWorld}
	, FlockSize{ FlockSize }
	, pAgentToEvade{pAgentToEvade}
{
	Agents.SetNum(FlockSize);

 // TODO: initialize the flock and the memory pool
	
	for (int i = 0; i < FlockSize; i++)
	{
		
		const float x = FMath::RandRange(-WorldSize, WorldSize);
		const float y = FMath::RandRange(-WorldSize, WorldSize);
		
		Agents[i] = pWorld->SpawnActor<ASteeringAgent>(AgentClass, FVector{ x,y,90 }, FRotator::ZeroRotator);
		
		if (Agents[i] == nullptr)
			--i;
		
	}
	
	pSeparationBehavior = std::make_unique<Separation>(this);
	pCohesionBehavior = std::make_unique<Cohesion>(this);
	pVelMatchBehavior = std::make_unique<VelocityMatch>(this);
	pSeekBehavior = std::make_unique<Seek>();
	pWanderBehavior = std::make_unique<Wander>();
	pEvadeBehavior = std::make_unique<Evade>();
	pEvadeBehavior->SetTarget(FTargetData(pAgentToEvade->GetPosition()));
	
	std::vector<BlendedSteering::WeightedBehavior> BlendedSteeringVec;
	BlendedSteeringVec.emplace_back(BlendedSteering::WeightedBehavior(pSeparationBehavior.get(),0.6f));
	BlendedSteeringVec.emplace_back(BlendedSteering::WeightedBehavior(pCohesionBehavior.get(),0.25f));
	BlendedSteeringVec.emplace_back(BlendedSteering::WeightedBehavior(pVelMatchBehavior.get(),0.55f));
	BlendedSteeringVec.emplace_back(BlendedSteering::WeightedBehavior(pSeekBehavior.get(),0.3));
	BlendedSteeringVec.emplace_back(BlendedSteering::WeightedBehavior(pWanderBehavior.get(),0.25f));
	
	pBlendedSteering = std::make_unique<BlendedSteering>(BlendedSteeringVec);
	
	std::vector<ISteeringBehavior*> SteeringVec;
	SteeringVec.emplace_back(pEvadeBehavior.get());
	SteeringVec.emplace_back(pBlendedSteering.get());
	
	pPrioritySteering = std::make_unique<PrioritySteering>(SteeringVec);
	
	for (ASteeringAgent* agent : Agents)
	{
		agent->SetSteeringBehavior(pPrioritySteering.get());
		agent->SetActorTickEnabled(false);
		agent->SetDebugRenderingEnabled(false);
	}
	
}

Flock::~Flock()
{
 // TODO: Cleanup any additional data
}

void Flock::Tick(float DeltaTime)
{
 // TODO: update the flock
 // TODO: for every agent:
  // TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
  // TODO: update the agent (-> the steeringbehaviors use the neighbors in the memory pool)
  // TODO: trim the agent to the world
	
	for (ASteeringAgent* agent: Agents)
	{
		if (agent != nullptr)
		{
			RegisterNeighbors(agent);
			agent->Tick(DeltaTime);
		}
	}
	
	SetTarget_Seek(MouseTarget);
	pEvadeBehavior->SetTarget(FTargetData(pAgentToEvade->GetPosition()));
	
	RenderDebug();
}

void Flock::RenderDebug()
{
 // TODO: Render all the agents in the flock
	RenderNeighborhood();
	
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();

  // TODO: implement ImGUI checkboxes for debug rendering here

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();

  // TODO: implement ImGUI sliders for steering behavior weights here
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Separation",
	pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
	[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Cohesion",
	pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
	[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Velocity Match",
	pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight, 0.f, 1.f,
	[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
	pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight, 0.f, 1.f,
	[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
	pBlendedSteering->GetWeightedBehaviorsRef()[4].Weight, 0.f, 1.f,
	[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[4].Weight = InVal; }, "%.2f");
		
		
		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
 // TODO: Debugrender the neighbors for the first agent in the flock
	ASteeringAgent* agent = Agents[0];
	
	RegisterNeighbors(agent);
	
	constexpr int circleSegments{20};
	DrawDebugCircle(agent->GetWorld(), FVector(agent->GetPosition(), 20.f),
NeighborhoodRadius, circleSegments, FColor::Yellow, false, -1, 0, 0, 
FVector(0,1,0), FVector(1,0,0), false);
	
	for (ASteeringAgent* neighbor : Neighbors)
	{
		DrawDebugLine(pWorld, FVector(agent->GetPosition(), 10.f), FVector(neighbor->GetPosition(), 10.f), FColor::Red);
	}
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent* const pAgent)
{
 // TODO: Implement
	
	NrOfNeighbors = 0;
	Neighbors.Empty();
	Neighbors.Reserve(20);
	
	for (ASteeringAgent* Agent: Agents)
	{
		const float distance {static_cast<float>(FVector2D::Distance(Agent->GetPosition(), pAgent->GetPosition()))};
		if (distance < NeighborhoodRadius && Agent != pAgent)
		{
			Neighbors.Add(Agent);
			++NrOfNeighbors;
		}
	}
	
	Neighbors.Shrink();
}
#endif

FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D avgPosition = FVector2D::ZeroVector;

 // TODO: Implement
	
	for (ASteeringAgent* neighbor : Neighbors)
	{
		avgPosition += neighbor->GetPosition();
	}
	
	avgPosition /= NrOfNeighbors;
	
	return avgPosition;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D avgVelocity = FVector2D::ZeroVector;

 // TODO: Implement
	
	for (ASteeringAgent* neighbor : Neighbors)
	{
		avgVelocity += FVector2D(neighbor->GetVelocity().X, neighbor->GetVelocity().Y);
	}
	
	avgVelocity /= NrOfNeighbors;

	return avgVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
 // TODO: Implement
 	pSeekBehavior->SetTarget(Target);
}

