#include "SteeringBehaviors.h"
#include "DrawDebugHelpers.h"
#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h>

#include "VectorTypes.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	SteeringOutput Steering{};
	
	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	//Steering.LinearVelocity.Normalize();
	//TODO show a cool thing
	
	// Add debug rendering for grades
	if (Agent.GetDebugRenderingEnabled())
	{
		constexpr int circleSegments{20};
		constexpr float radius{20.f};
		
		// Target pos
		DrawDebugCircle(Agent.GetWorld(), FVector(Target.Position.X, Target.Position.Y, 10.f),
			radius, circleSegments, FColor::Red, false, -1, 0, 0, 
			FVector(0,1,0), FVector(1,0,0), true);
		
		const FVector lineStart{Agent.GetPosition(), 10};
		constexpr float lineLengthDevider{4.f};
		const FVector lineEnd{Agent.GetPosition() + Agent.GetLinearVelocity() / lineLengthDevider, 10};
		DrawDebugLine(Agent.GetWorld(), lineStart, lineEnd, FColor::Purple);
		
		
	}
	
	return Steering;
}


SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	SteeringOutput Steering{};
	
	Steering.LinearVelocity = -(Target.Position - Agent.GetPosition());
	
	if (Agent.GetDebugRenderingEnabled())
	{
		constexpr int circleSegments{20};
		constexpr float radius{20.f};
		
		// Target pos
		DrawDebugCircle(Agent.GetWorld(), FVector(Target.Position.X, Target.Position.Y, 10.f),
			radius, circleSegments, FColor::Red, false, -1, 0, 0, 
			FVector(0,1,0), FVector(1,0,0), true);
	}
	
	return Steering;
}


SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	SteeringOutput Steering{};
	
	constexpr float outerRadius{200.f};
	constexpr float innerRadius{50.f};
	
	const float distanceToTarget{static_cast<float>((Target.Position - Agent.GetPosition()).Length())};
	
	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	
	constexpr float maxSpeed{700.f};
	
	if (distanceToTarget > outerRadius)
	{
		Agent.SetMaxLinearSpeed(maxSpeed);
	}
	else if (distanceToTarget < innerRadius)
	{
		Agent.SetMaxLinearSpeed(0.f);
	}
	else if (distanceToTarget < outerRadius)
	{
		const float speedMultiplier{(distanceToTarget - innerRadius) / (outerRadius - innerRadius)};
		Agent.SetMaxLinearSpeed(maxSpeed * speedMultiplier);
	}
	
	if (Agent.GetDebugRenderingEnabled())
	{
		constexpr int circleSegments{20};
		
		// Target radius
		DrawDebugCircle(Agent.GetWorld(), FVector((Agent.GetPosition().X),Agent.GetPosition().Y, 10.f),
			innerRadius, circleSegments, FColor::Red, false, -1, 0, 0, 
			FVector(0,1,0), FVector(1,0,0), false);
	
		// Slow radius
		DrawDebugCircle(Agent.GetWorld(), FVector((Agent.GetPosition().X),Agent.GetPosition().Y, 10.f),
		outerRadius, circleSegments, FColor::Blue, false, -1, 0, 0, 
		FVector(0,1,0), FVector(1,0,0), false);
		
		// Target pos
		constexpr float targetRadius{20.f};
		DrawDebugCircle(Agent.GetWorld(), FVector(Target.Position.X, Target.Position.Y, 10.f),
			targetRadius, circleSegments, FColor::Red, false, -1, 0, 0, 
			FVector(0,1,0), FVector(1,0,0), true);
	}
	
	return Steering;
}

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	SteeringOutput Steering{};
	
	const float distanceTotTarget{static_cast<float>((Target.Position - Agent.GetPosition()).Length())};
	const float timeToTarget{distanceTotTarget / static_cast<float>(Target.LinearVelocity.Length())};
	
	const FVector2D predictionDistance{Target.LinearVelocity * timeToTarget};
	const FVector2D predicitonTargetPos{Target.Position + predictionDistance};
	
	Steering.LinearVelocity = predicitonTargetPos - Agent.GetPosition();
	
	if (Agent.GetDebugRenderingEnabled())
	{
		constexpr int circleSegments{20};
		
		// Predicted target circle
		DrawDebugCircle(Agent.GetWorld(), FVector(predicitonTargetPos.X, predicitonTargetPos.Y, 10.f),
			30.f, circleSegments, FColor::Red, false, -1, 0, 0, 
			FVector(0,1,0), FVector(1,0,0), true);
	}
	
	return Steering;
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	SteeringOutput Steering{};
	
	const float distanceTotTarget{static_cast<float>((Target.Position - Agent.GetPosition()).Length())};
	const float timeToTarget{distanceTotTarget / static_cast<float>(Target.LinearVelocity.Length())};
	
	const FVector2D predictionDistance{Target.LinearVelocity * timeToTarget};
	const FVector2D predicitonTargetPos{Target.Position + predictionDistance};
	
	Steering.LinearVelocity = -(predicitonTargetPos - Agent.GetPosition());
	
	if (Agent.GetDebugRenderingEnabled())
	{
		constexpr int circleSegments{20};
		
		// Predicted target circle
		DrawDebugCircle(Agent.GetWorld(), FVector(predicitonTargetPos.X, predicitonTargetPos.Y, 10.f),
			30.f, circleSegments, FColor::Red, false, -1, 0, 0, 
			FVector(0,1,0), FVector(1,0,0), true);
	}
	
	return Steering;
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	SteeringOutput Steering{};
	
	
	
	return Steering;
}