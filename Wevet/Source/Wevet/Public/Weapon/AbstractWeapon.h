// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbstractWeapon.generated.h"

class ACharacterBase;

UCLASS(ABSTRACT)
class WEVET_API AAbstractWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AAbstractWeapon(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	TWeakObjectPtr<ACharacterBase> CharacterOwner;

public:
	ACharacterBase* GetCharacterOwner() const;
};
