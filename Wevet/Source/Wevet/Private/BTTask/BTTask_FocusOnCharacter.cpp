// Copyright © 2018 wevet works All Rights Reserved.

#include "BTTask_FocusOnCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "AIControllerBase.h"
#include "AICharacterBase.h"
#include "Engine.h"

EBTNodeResult::Type UBTTask_FocusOnCharacter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIControllerBase* AIController = Cast<AAIControllerBase>(OwnerComp.GetAIOwner()))
	{
		IAIPawnOwner* Interface = Cast<IAIPawnOwner>(AIController->GetPawn());
		if (Interface)
		{
			APawn* const Pawn = Cast<APawn>(IAIPawnOwner::Execute_GetTarget(Interface->_getUObject()));
			if (!Pawn)
			{
				return EBTNodeResult::Failed;
			}

			AIController->SetFocus(Pawn->GetController());
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}

