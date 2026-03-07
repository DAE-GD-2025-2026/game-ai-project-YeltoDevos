#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "VectorTypes.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput steering{};
	
	FVector2D direction {pFlock->GetAverageNeighborPos() - pAgent.GetPosition()};
	steering.LinearVelocity = direction;
	steering.LinearVelocity.Normalize();
	
	return steering;
}

//*********************
//SEPARATION (FLOCKING)

SteeringOutput Separation::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput steering{};
	
	FVector2D totalVelocity{};
	
	for (ASteeringAgent* neighbor : pFlock->GetNeighbors())
	{
		FVector2D velocity {pAgent.GetPosition() - neighbor->GetPosition()};
		velocity *= (1.f/velocity.Length());
		
		totalVelocity += velocity;
	}
	
	steering.LinearVelocity = totalVelocity;
	steering.LinearVelocity.Normalize();
	
	return steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)

SteeringOutput VelocityMatch::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput steering{};
	
	if (pFlock->GetNrOfNeighbors() > 0)
	{
		FVector2D avgVelocity { pFlock->GetAverageNeighborVelocity()};
	
		steering.LinearVelocity = avgVelocity;
		steering.LinearVelocity.Normalize();
	}
	else
	{
		steering.LinearVelocity = FVector2D(pAgent.GetVelocity().X, pAgent.GetVelocity().Y);
		steering.LinearVelocity.Normalize();
	}
	
	return steering;
}


