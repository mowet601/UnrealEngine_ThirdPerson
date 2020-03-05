// Copyright © 2018 wevet works All Rights Reserved.


#include "Weapon/AbstractWeapon.h"

AAbstractWeapon::AAbstractWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAbstractWeapon::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);
}

void AAbstractWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CharacterOwner.Reset();
	Super::EndPlay(EndPlayReason);
}

void AAbstractWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ACharacterBase* AAbstractWeapon::GetCharacterOwner() const
{
	if (CharacterOwner.IsValid())
	{
		return CharacterOwner.Get();
	}
	return nullptr;
}

