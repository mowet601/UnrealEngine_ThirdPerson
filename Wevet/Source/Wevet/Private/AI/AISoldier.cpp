// Copyright © 2018 wevet works All Rights Reserved.

#include "AI/AISoldier.h"
#include "AI/AIControllerBase.h"
#include "Player/MockCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Lib/WevetBlueprintFunctionLibrary.h"


AAISoldier::AAISoldier(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PawnSensingComponent->HearingThreshold = 200.f;
}

void AAISoldier::MainLoop(float DeltaTime)
{
	UpdateSeeTarget();
	UpdateHearTarget();
}

void AAISoldier::UpdateSeeTarget()
{
	if (!IAIPawnOwner::Execute_IsSeeTarget(this))
	{
		return;
	}

	if (TargetCharacter)
	{
		if (!IDamageInstigator::Execute_IsDeath(TargetCharacter))
		{
			UpdateAttackState();

			const float TimeSeconds = GetWorld()->GetTimeSeconds();
			const float Diff = (TimeSeconds - LastSeenTime);
			if (Diff > SenseTimeOut)
			{
				OnSeePawnRecieveCallback();
			}
			//UE_LOG(LogWevetClient, Log, TEXT("SenseTimeOut : %f, Diff : %f, LastSeenTime : %f"), SenseTimeOut, Diff, LastSeenTime);
		}
		else
		{
			OnSeePawnRecieveCallback();
		}
	}

}

void AAISoldier::UpdateHearTarget()
{
	if (!IAIPawnOwner::Execute_IsHearTarget(this))
	{
		return;
	}

	const float TimeSeconds = GetWorld()->GetTimeSeconds();
	const float Diff = (TimeSeconds - LastHeardTime);
	if (Diff > HearTimeOut)
	{
		HearNoiseRecieveCallback(nullptr);
	}
	//UE_LOG(LogWevetClient, Log, TEXT("HearTimeOut : %f, Diff : %f, LastHeardTime : %f"), HearTimeOut, Diff, LastHeardTime);
}

bool AAISoldier::CanMeleeStrike_Implementation() const
{
	if (ALSMovementMode != ELSMovementMode::Grounded)
	{
		return false;
	}

	return Super::CanMeleeStrike_Implementation();
}

void AAISoldier::UpdateAttackState()
{
	if (!AIController)
	{
		return;
	}

	switch (AIController->GetBlackboardActionState())
	{
		case EAIActionState::Attack:
		{
			if (!CanMeleeStrike_Implementation())
			{
				AIController->SetBlackboardActionState(EAIActionState::Observation);
				UWevetBlueprintFunctionLibrary::DrawDebugString(this, FString(TEXT("Observation")));
				return;
			}
			else
			{
				if (CurrentWeapon.Get()->WasEmpty())
				{
					AIController->SetBlackboardActionState(EAIActionState::ConditionCheck);
					return;
				}
				FirePressed();
			}
		}
		break;
		case EAIActionState::Observation:
		{
			if (CanMeleeStrike_Implementation())
			{
				AttackUnInitialize();
				AIController->SetBlackboardActionState(EAIActionState::Attack);
				UWevetBlueprintFunctionLibrary::DrawDebugString(this, FString(TEXT("Attack")));
				StopSprint();
			}
			else 
			{
				Sprint();
			}
		}
		break;
		case EAIActionState::ConditionCheck:
		{
			//
		}
		break;
		case EAIActionState::Cover:
		{
			//
		}
		break;
	}
}

void AAISoldier::FirePressed()
{
	if (WasTakeDamagePlaying() || !Super::HasEquipWeapon())
	{
		return;
	}

	if (WasMeleeAttacked())
	{
		MeleeAttackTimeOut -= GetWorld()->GetDeltaSeconds();
		BulletInterval += GetWorld()->GetDeltaSeconds();

		if (BulletInterval >= BulletDelay)
		{
			Super::FirePressed();
			BulletInterval = ZERO_VALUE;
		}
	}
	else
	{
		bAttackInitialized = false;
	}
}

void AAISoldier::OnSeePawnRecieveCallback()
{
	SeePawnRecieveCallback(nullptr);
	AttackUnInitialize();
	StopSprint();
	AIController->SetBlackboardActionState(EAIActionState::None);
}

#pragma region PawnSence
void AAISoldier::OnSeePawnRecieve(APawn* OtherPawn)
{
	if (WasStanning())
		return;

	AMockCharacter* Character = Cast<AMockCharacter>(OtherPawn);
	if (!Character)
		return;

	if (!IDamageInstigator::Execute_IsDeath(Character))
	{
		LastSeenTime = GetWorld()->GetTimeSeconds();
		SeePawnRecieveCallback(Character);
		Sprint();
	}
	else
	{
		OnSeePawnRecieveCallback();
		StopSprint();
	}

}

void AAISoldier::OnHearNoiseRecieve(APawn* OtherActor, const FVector& Location, float Volume)
{
	if (WasStanning())
		return;

	LastHeardTime = GetWorld()->GetTimeSeconds();
	HearNoiseRecieveCallback(OtherActor, Location);
}
#pragma endregion

void AAISoldier::TakeDamageActionMontage(const bool InForcePlaying)
{
	Super::FireReleassed();
	Super::TakeDamageActionMontage(true);
}

