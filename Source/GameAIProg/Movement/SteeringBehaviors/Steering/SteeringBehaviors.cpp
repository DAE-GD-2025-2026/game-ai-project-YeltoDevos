#include "SteeringBehaviors.h"
#include "DrawDebugHelpers.h"
#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h>

#include "VectorTypes.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "GameFramework/GameNetworkManager.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)

void ISteeringBehavior::DrawTarget(const UWorld * World, const float circleRadius, const int circleSegments)
{
	DrawTarget(World, Target.Position, circleRadius, circleSegments);
}

void ISteeringBehavior::DrawTarget(const UWorld* World, const FVector2D targetPos, const float circleRadius, const int circleSegments)
{
	DrawDebugCircle(World, FVector(targetPos.X, targetPos.Y, 10.f),
	circleRadius, circleSegments, FColor::Red, false, -1, 0, 0, 
	FVector(0,1,0), FVector(1,0,0), true);
}

void ISteeringBehavior::DrawForwardVelocity(const ASteeringAgent& Agent)
{
	const FVector lineStart{Agent.GetPosition(), 10};
	constexpr float lineLengthDevider{4.f};
	const FVector lineEnd{Agent.GetPosition() + Agent.GetLinearVelocity() / lineLengthDevider, 10};
	DrawDebugLine(Agent.GetWorld(), lineStart, lineEnd, FColor::Purple);
}

void ISteeringBehavior::DrawDesiredDirection(const ASteeringAgent& Agent)
{
	FVector2D  direction{Target.Position - Agent.GetPosition()};
	direction.Normalize();
	constexpr float lineLength{200};
	const FVector lineEnd{Agent.GetPosition() + direction * lineLength, 10};

	DrawDebugLine(Agent.GetWorld(), FVector(Agent.GetPosition(), 10), lineEnd, FColor::Green);
}

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	SteeringOutput Steering{};
	
	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	//Steering.LinearVelocity.Normalize();
	//TODO show a cool thing
	
	// Add debug rendering for grades
	if (Agent.GetDebugRenderingEnabled())
	{
		DrawTarget(Agent.GetWorld());
		DrawForwardVelocity(Agent);
		DrawDesiredDirection(Agent);
	}
	
	return Steering;
}


SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	
	
	
	SteeringOutput Steering{};
	
	Steering.LinearVelocity = -(Target.Position - Agent.GetPosition());
	
	if (Agent.GetDebugRenderingEnabled())
	{
		DrawTarget(Agent.GetWorld());
		DrawForwardVelocity(Agent);
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
		

		DrawTarget(Agent.GetWorld());
		DrawForwardVelocity(Agent);
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
		// Predicted target circle
		DrawTarget(Agent.GetWorld(), predicitonTargetPos);
		
		DrawForwardVelocity(Agent);
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
		// Predicted target circle
		DrawTarget(Agent.GetWorld(), predicitonTargetPos);
		
		DrawForwardVelocity(Agent);
	}
	return Steering;
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{
	SteeringOutput Steering{};
	
	constexpr float distance{200.f};
	const FVector circleCenter{FVector(Agent.GetPosition().X, Agent.GetPosition().Y, 10) + Agent.GetActorForwardVector() * distance};
	constexpr float circleRadius{150.f};
	
	const float randomAngle{FMath::DegreesToRadians(std::rand()%360)};
	const FVector2D randomTarget{circleCenter.X + circleRadius * cos(randomAngle),
								 circleCenter.Y + circleRadius * sin(randomAngle)};
	
	Steering.LinearVelocity = randomTarget - Agent.GetPosition();
	
	if (Agent.GetDebugRenderingEnabled())
	{
		// ForwardVector
		// const FVector lineStart{Agent.GetPosition(), 10};
		// DrawDebugLine(Agent.GetWorld(), lineStart, circleCenter, FColor::Purple);
		
		// Wander Circle
		constexpr int circleSegments{20};
		DrawDebugCircle(Agent.GetWorld(), FVector(circleCenter.X, circleCenter.Y, 10.f),
		circleRadius, circleSegments, FColor::Blue, false, -1, 0, 0, 
		FVector(0,1,0), FVector(1,0,0), false);
		
		// Target
		DrawTarget(Agent.GetWorld(), randomTarget, 10.f);
		DrawForwardVelocity(Agent);
	}
	return Steering;
}

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	
	const FVector2D difference{Target.Position - Agent.GetPosition()};
	const float desiredYaw = FMath::RadiansToDegrees(FMath::Atan2(difference.Y, difference.X));
	const float currentYaw = Agent.GetRotation();
	const float yawDifference {FMath::FindDeltaAngleDegrees(desiredYaw, currentYaw)};
	
	if (FMath::IsNearlyZero(yawDifference, .01f)) 
		return Steering;
	
	constexpr float speed{3.f};
	
	if (yawDifference <= 0.f)
	{
		Steering.AngularVelocity = speed;
	}
	else if (yawDifference > 0.f)
	{
		Steering.AngularVelocity = -speed;
	}
	

	
	
	return Steering;
}
