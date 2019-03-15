// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "MainLevelScriptActor.generated.h"

class ACharacterBase;
class AMockCharacter;
class AAICharacterBase;

UCLASS()
class WEVET_API AMainLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
	
public:
	AMainLevelScriptActor(const class FObjectInitializer& ObjectInitializer);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;

protected:
	TArray<ACharacterBase*> CharacterArray;
	TArray<AAICharacterBase*> AICharacterArray;
	AMockCharacter* Player;

	UPROPERTY(EditDefaultsOnly, Category = "MainLevelScriptActor|Variable")
	TArray<TSubclassOf<class ACharacterBase>> CharacterTemplates;

	virtual void CreateCharacter();
};
