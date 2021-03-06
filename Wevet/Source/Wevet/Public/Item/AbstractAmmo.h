// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/AbstractItem.h"
#include "WevetTypes.h"
#include "Structs/BaseItem.h"
#include "AbstractAmmo.generated.h"

class ACharacterBase;

UCLASS()
class WEVET_API AAbstractAmmo : public AAbstractItem
{
	GENERATED_BODY()
	
public:
	AAbstractAmmo(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	virtual void BeginPlay() override;


protected:
	virtual void OverlapActor(AActor* OtherActor) override;
};
