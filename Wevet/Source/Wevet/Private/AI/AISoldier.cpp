// Copyright © 2018 wevet works All Rights Reserved.

#include "AISoldier.h"
#include "Player/MockCharacter.h"
#include "AIControllerBase.h"
#include "CharacterModel.h"

AAISoldier::AAISoldier(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	SenceWarningTimeOut(10.f)
{
	//
}

void AAISoldier::MainLoop(float DeltaTime)
{
	if (!CanShotup())
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const float TimeSeconds = World->TimeSeconds;
	if (bSeeTarget && (TimeSeconds - LastSeenTime) > SenseTimeOut)
	{
		bSeeTarget = false;
		SetSeeTargetActor(nullptr);
	}

	if (bHearTarget && (TimeSeconds - LastHeardTime) > SenseTimeOut)
	{
		bHearTarget = false;
		SetHearTargetActor(nullptr);
	}

	if (bCrouch && (TimeSeconds - LastWarningTime) > SenceWarningTimeOut)
	{
		OnCrouch();
	}

	// only see target
	if (HasEnemyFound())
	{
		if (!Super::HasEquipWeapon())
		{
			return;
		}
		if (!IDamageInstigator::Execute_IsDeath(TargetCharacter))
		{
			BulletInterval += DeltaTime;
			if (BulletInterval >= BulletDelay)
			{
				BP_FirePressReceive();
				BulletInterval = 0.f;
			}
		}
		else
		{
			bSeeTarget = false;
			SetSeeTargetActor(nullptr);
		}
	}
	else
	{
		// not found
	}
}

void AAISoldier::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor, bool& bDied)
{
	Super::OnTakeDamage_Implementation(BoneName, Damage, Actor, bDied);

	if (!IDamageInstigator::Execute_IsDeath(this))
	{
		if (FMath::IsNearlyZero(LastWarningTime))
		{
			if (!Super::bCrouch)
			{
				LastWarningTime = GetWorld()->GetTimeSeconds();
				Super::OnCrouch();
			}
		}
	}
}

void AAISoldier::OnSeePawnRecieve(APawn* OtherPawn)
{
	if (AMockCharacter* Character = Cast<AMockCharacter>(OtherPawn))
	{
		if (!IDamageInstigator::Execute_IsDeath(Character))
		{
			LastSeenTime = GetWorld()->GetTimeSeconds();
			if (!bSeeTarget)
			{
				bSeeTarget = true;
				Super::SetSeeTargetActor(Character);
			}
		}
	}
	Super::OnSeePawnRecieve(OtherPawn);
}

void AAISoldier::OnHearNoiseRecieve(APawn* OtherActor, const FVector& Location, float Volume)
{
	// hearing time always update
	LastHeardTime = GetWorld()->GetTimeSeconds();

	if (!bHearTarget)
	{
		bHearTarget = true;
		Super::SetHearTargetActor(OtherActor);
	}
	Super::OnHearNoiseRecieve(OtherActor, Location, Volume);
}
