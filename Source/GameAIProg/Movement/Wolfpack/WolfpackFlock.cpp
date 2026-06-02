#include "WolfpackFlock.h"
#include "VectorTypes.h"
#include "Shared/ImGuiHelpers.h"


WolfPackFlock::WolfPackFlock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize,
	float WorldSize,
	ASteeringAgent* const pAgentToEvade,
	bool bTrimWorld):
	Flock(pWorld, AgentClass, FlockSize, WorldSize, pAgentToEvade, bTrimWorld)
{
	
	pFleeBehavior = std::make_unique<Flee>();
	pArrive = std::make_unique<Arrive>();
	
	std::vector<BlendedSteering::WeightedBehavior> BlendedSteeringVec;
	BlendedSteeringVec.emplace_back(BlendedSteering::WeightedBehavior(pSeparationBehavior.get(),0.3f));
	BlendedSteeringVec.emplace_back(BlendedSteering::WeightedBehavior(pCohesionBehavior.get(),0.f));
	BlendedSteeringVec.emplace_back(BlendedSteering::WeightedBehavior(pVelMatchBehavior.get(),0.f));
	BlendedSteeringVec.emplace_back(BlendedSteering::WeightedBehavior(pArrive.get(),0.75f));
	BlendedSteeringVec.emplace_back(BlendedSteering::WeightedBehavior(pWanderBehavior.get(),0.f));
	BlendedSteeringVec.emplace_back(BlendedSteering::WeightedBehavior(pFleeBehavior.get(),1.f));
	pFleeBehavior->SetTarget(FTargetData(pAgentToEvade->GetPosition()));
	pFleeBehavior->SetFleeDistance(150.f);
	pArrive->SetTargetRadius(0.f);
	pArrive->SetTargetMaxRadius(200.f);
	
	pBlendedSteering = std::make_unique<BlendedSteering>(BlendedSteeringVec);
	
	std::vector<ISteeringBehavior*> SteeringVec;
	SteeringVec.emplace_back(pEvadeBehavior.get());
	SteeringVec.emplace_back(pBlendedSteering.get());
	
	pPrioritySteering = std::make_unique<PrioritySteering>(SteeringVec);
	
	for (ASteeringAgent* agent : Agents)
	{
		agent->SetSteeringBehavior(pBlendedSteering.get());
		agent->SetActorTickEnabled(false);
		agent->SetDebugRenderingEnabled(false);
	}
}


WolfPackFlock::~WolfPackFlock()
{
 // TODO: Cleanup any additional data
}

void WolfPackFlock::Tick(float DeltaTime)
{
	if (UseSpatialPartition)
	{
		for (std::size_t i{0}; i < FlockSize; i++)
		{
			pPartitionedSpace->UpdateAgentCell(*Agents[i], OldPositions[i]);
			pPartitionedSpace->RegisterNeighbors(*Agents[i], NeighborhoodRadius);
			
			Neighbors = pPartitionedSpace->GetNeighbors();
			NrOfNeighbors = pPartitionedSpace->GetNrOfNeighbors();
			
			Agents[i]->Tick(DeltaTime);
			OldPositions[i] = Agents[i]->GetPosition();
		}
		
	}
	else
	{
		for (ASteeringAgent* agent: Agents)
		{
			if (agent != nullptr)
			{
				RegisterNeighbors(agent);
				agent->Tick(DeltaTime);
			}
		}
	}

	SetTarget_Seek(MouseTarget);
	pArrive->SetTarget(FTargetData(pAgentToEvade->GetPosition()));
	pFleeBehavior->SetTarget(FTargetData(pAgentToEvade->GetPosition()));
	RenderDebug();
}

void WolfPackFlock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
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
		
		ImGui::Checkbox("Use SpatialPartition", &UseSpatialPartition);
		ImGui::Checkbox("Draw Neighborhood", &DebugRenderNeighborhood);
		ImGui::Checkbox("Draw Partitions", &DebugRenderPartitions);

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
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Arrive",
	pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight, 0.f, 1.f,
	[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
	pBlendedSteering->GetWeightedBehaviorsRef()[4].Weight, 0.f, 1.f,
	[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[4].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Flee",
	pBlendedSteering->GetWeightedBehaviorsRef()[5].Weight, 0.f, 1.f,
	[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[5].Weight = InVal; }, "%.2f");
		
		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

