// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterPickupComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCharacterPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterPickupComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	void SetPickupActor(AActor* NewPickupActor);

	AActor* GetPickupActor() const;

private:
	AActor* PickupActor;
	
};
