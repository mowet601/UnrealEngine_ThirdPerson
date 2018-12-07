// Copyright © 2018 wevet works All Rights Reserved.

#include "CharacterPickupComponent.h"


UCharacterPickupComponent::UCharacterPickupComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	PickupActor(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCharacterPickupComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UCharacterPickupComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCharacterPickupComponent::SetPickupActor(AActor* NewPickupActor)
{
	PickupActor = NewPickupActor;
}

AActor* UCharacterPickupComponent::GetPickupActor() const
{
	return PickupActor;
}
