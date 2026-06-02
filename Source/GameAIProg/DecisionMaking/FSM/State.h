#pragma once

//#include <memory>
#include "Movement/SteeringBehaviors/SteeringAgent.h"
#include "Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

namespace GameAI::FSM
{
	class State
	{
	public:
		virtual ~State();
		
		virtual void execute(ASteeringAgent* agent) = 0;

	private:
		
	};
	
	class Patrol : public State
	{
		public:
		virtual void execute(ASteeringAgent* agent) override
		{
			//agent->SetSteeringBehavior(pWanderBehavior.get());
		}
		
		private:
		//std::unique_ptr<ISteeringBehavior> pWanderBehavior = std::make_unique<Wander>();
	};
	
	
}
