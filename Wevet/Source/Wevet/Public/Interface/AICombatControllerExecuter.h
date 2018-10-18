// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AICombatControllerExecuter.generated.h"

/**
 *
 */
UINTERFACE(BlueprintType)
class WEVET_API UAICombatControllerExecuter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IAICombatControllerExecuter
{
	GENERATED_IINTERFACE_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UAICombatControllerExecuter")
	void Patrolling();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UAICombatControllerExecuter")
	void CheckEnemySighting();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UAICombatControllerExecuter")
	void Hunting();
};