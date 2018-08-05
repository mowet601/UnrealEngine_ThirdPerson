// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "WeaponBase.h"
#include "WayPointBase.h"
#include "MockCharacter.h"
#include "Perception/AiPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/PawnSensingComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "AICharacterActorBase.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class WEVET_API AAICharacterActorBase : public ACharacterBase
{
	GENERATED_BODY()
	
	
public:
	AAICharacterActorBase(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override;
	virtual void Die_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAICharacterActorBase|ReadOnlyValiable")
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAICharacterActorBase|ReadOnlyValiable")
	UWidgetComponent* WidgetComponent;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|ReadOnlyValiable")
	virtual void SetTargetActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|ReadOnlyValiable")
	virtual void SetEnemyFound(bool EnemyFound);

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|ReadOnlyValiable")
	virtual AActor* GetTarget()const 
	{
		return this->Target;
	}

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|ReadOnlyValiable")
	virtual AController* GetController() const
	{
		return Cast<APawn>(GetTarget())->Controller;
	}

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|ReadOnlyValiable")
	virtual AMockCharacter* GetPlayerCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|ReadOnlyValiable")
	TArray<AWayPointBase*> GetWayPointList() const
	{
		return this->WayPointList;
	}

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|ReadOnlyValiable")
	virtual void UpdateWeaponEvent();

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|ReadOnlyValiable")
	virtual void UpdateWayPointEvent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterActorBase|Valiable")
	bool IsEnemyFound;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|ReadOnlyValiable")
	bool GetEnemyFound() const
	{
		return this->IsEnemyFound;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterActorBase|Valiable")
	class UBehaviorTree* BehaviorTree;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|ReadOnlyValiable")
	virtual void InitializePosses();

protected:
	AActor * Target;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|PawnSensing")
	virtual void Scanning();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterActorBase|Valiable")
	TArray<AWayPointBase*> WayPointList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAICharacterActorBase|Valiable")
	TSubclassOf<class AWeaponBase> SpawnWeapon;

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|PawnSensing")
	virtual	void OnSeePawnRecieve(APawn* OtherPawn);

	UFUNCTION(BlueprintCallable, Category = "AAICharacterActorBase|PawnSensing")
	virtual	void OnHearNoiseRecieve(APawn *OtherActor, const FVector &Location, float Volume);
};
