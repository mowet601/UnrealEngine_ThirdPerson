// Copyright © 2018 wevet works All Rights Reserved.

#include "BTTask_MoveNearCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AI/AIControllerBase.h"
#include "AI/AICharacterBase.h"
#include "Engine.h"
#include "NavigationSystem.h"
#include "Interface/AIPawnOwner.h"

UBTTask_MoveNearCharacter::UBTTask_MoveNearCharacter()
	: Super()
{
	NodeName = TEXT("BTTask_MoveNearCharacter");
	BlackboardKey.SelectedKeyName = FName(TEXT("Target"));
}

EBTNodeResult::Type UBTTask_MoveNearCharacter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner()))
	{
		APawn* AIPawn = AIController->GetPawn();
		IAIPawnOwner* Interface = Cast<IAIPawnOwner>(AIPawn);
		if (Interface)
		{
			FVector SearchOrigin = FVector::ZeroVector;
			const float SearchRadius = IAIPawnOwner::Execute_GetMeleeDistance(Interface->_getUObject());
			APawn* const Pawn = Cast<APawn>(IAIPawnOwner::Execute_GetTarget(Interface->_getUObject()));

			if (!Pawn)
			{
				return EBTNodeResult::Failed;
			}

			SearchOrigin = Pawn->GetActorLocation();
			FNavLocation ResultLocation;
			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(AIController);
			if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(SearchOrigin, SearchRadius, ResultLocation))
			{
				AIController->SetBlackboardPatrolLocation(ResultLocation.Location);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}

