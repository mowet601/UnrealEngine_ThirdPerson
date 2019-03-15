// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnvironmentPawn.generated.h"

UCLASS()
class WEVET_API AEnvironmentPawn : public APawn
{
	GENERATED_BODY()

public:
	AEnvironmentPawn(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPawnNoiseEmitterComponent* PawnNoiseEmitterComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variable")
	USoundBase* EffectSoundAsset;

	bool bPlaying;

	TArray<UParticleSystemComponent*> ParticleSystemComponents;

public:
	FORCEINLINE class UStaticMeshComponent* GetStaticMeshComponent() const
	{
		return StaticMeshComponent;
	}

	FORCEINLINE class USphereComponent* GetSphereComponent() const
	{
		return SphereComponent;
	}

	UFUNCTION(BlueprintCallable, Category = Components)
	virtual	void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION(BlueprintCallable, Category = Components)
	virtual	void EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	virtual void StartEffect();
	virtual void StopEffect();
};
