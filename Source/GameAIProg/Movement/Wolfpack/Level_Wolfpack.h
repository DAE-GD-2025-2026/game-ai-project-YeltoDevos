// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WolfpackFlock.h"
#include "Movement/SteeringBehaviors/Flocking/Flock.h"
#include "Shared/Level_Base.h"
#include "Level_Wolfpack.generated.h"

UCLASS()
class GAMEAIPROG_API ALevel_Wolfpack : public ALevel_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevel_Wolfpack();

	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	bool bUseMouseTarget{true};

	int const FlockSize{5};

	TUniquePtr<WolfPackFlock> pFlock{};
	
	UPROPERTY(EditAnywhere, Category = "Flocking")
	ASteeringAgent* pAgentToChase{nullptr}; // non owning ref
	
	ISteeringBehavior* ChaseSteeringBehavior{nullptr};
};
