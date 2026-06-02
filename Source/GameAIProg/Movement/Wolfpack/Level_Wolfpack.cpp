// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_Wolfpack.h"


// Sets default values
ALevel_Wolfpack::ALevel_Wolfpack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_Wolfpack::BeginPlay()
{
	Super::BeginPlay();
	
	TrimWorld->SetTrimWorldSize(1500.f);
	TrimWorld->bShouldTrimWorld = true;
	
	ChaseSteeringBehavior = new Arrive();
	pAgentToChase = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, FVector{ 0,0,90 }, FRotator::ZeroRotator);
	pAgentToChase->SetSteeringBehavior(ChaseSteeringBehavior);

	pFlock = TUniquePtr<WolfPackFlock>(
		new WolfPackFlock(
			GetWorld(),
			SteeringAgentClass,
			FlockSize,
			TrimWorld->GetTrimWorldSize(),
			pAgentToChase,
			true)
			);
	
	pFlock->SetTarget_Seek(FTargetData(pAgentToChase->GetPosition()));
}

// Called every frame
void ALevel_Wolfpack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	pFlock->ImGuiRender(WindowPos, WindowSize);
	pFlock->Tick(DeltaTime);
	pFlock->RenderDebug();
	pFlock->SetTarget_Seek(FTargetData(pAgentToChase->GetPosition()));
	
	if (bUseMouseTarget)
	{
		ChaseSteeringBehavior->SetTarget(MouseTarget);
	}
}

