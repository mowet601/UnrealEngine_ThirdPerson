// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/BaseItem.h"
#include "Interface/WeaponInstigator.h"
#include "Interface/InteractionInstigator.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "WevetTypes.h"
#include "AbstractWeapon.generated.h"

class ACharacterBase;
class ABulletBase;

UCLASS(ABSTRACT)
class WEVET_API AAbstractWeapon : public AActor, public IWeaponInstigator, public IInteractionInstigator
{
	GENERATED_BODY()
	
public:	
	AAbstractWeapon(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginDestroy() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
#pragma region Interface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoFirePressed();
	virtual void DoFirePressed_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoFireRelease();
	virtual void DoFireRelease_Implementation() override;	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoMeleeAttack(const bool InEnableMeleeAttack);
	virtual void DoMeleeAttack_Implementation(const bool InEnableMeleeAttack) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoReload();
	virtual void DoReload_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	void DoReplenishment(const int32 InAddAmmo);
	virtual void DoReplenishment_Implementation(const int32 InAddAmmo) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|WeaponInstigator")
	bool CanMeleeStrike() const;
	virtual bool CanMeleeStrike_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|InteractionInstigator")
	void Take(ACharacterBase* NewCharacter);
	virtual void Take_Implementation(ACharacterBase* NewCharacter) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|InteractionInstigator")
	void Release(ACharacterBase* NewCharacter);
	virtual void Release_Implementation(ACharacterBase* NewCharacter) override;	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbstractWeapon|InteractionInstigator")
	EItemType GetItemType() const;
	virtual EItemType GetItemType_Implementation() const override
	{
		return WeaponItemInfo.GetItemType();
	}
#pragma endregion

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* MeleeCollisionComponent;

public:
	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }

public:
	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	ACharacterBase* GetPointer() const;

	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	APawn* GetPawnOwner() const;

public:
	virtual void Initialize(ACharacterBase* const NewCharacterOwner);
	void ReloadBulletParams();
	void SetOwnerNoSeeMesh(const bool NewOwnerNoSee);
	void SpawnToWorld();

public:
	struct FWeaponItemInfo GetWeaponItemInfo() const { return WeaponItemInfo; }

	void SetEquip(const bool InEquip)
	{
		bEquip = InEquip;
	}

	void SetReload(const bool InReload)
	{
		bReload = InReload;
	}

	FORCEINLINE bool WasEquip() const { return bEquip; }
	
	FORCEINLINE bool WasReload() const { return bReload; }

	FORCEINLINE bool WasEmpty() const { return (WeaponItemInfo.MaxAmmo <= 0) && (WeaponItemInfo.CurrentAmmo <= 0); }

	FORCEINLINE float GetTraceDistance() const { return WeaponItemInfo.TraceDistance; }

	FORCEINLINE bool WasSameWeaponType(EWeaponItemType InWeaponItemType) const { return WeaponItemInfo.WeaponItemType == InWeaponItemType; }

	FORCEINLINE FTransform GetMuzzleTransform() const { return SkeletalMeshComponent->GetSocketTransform(MuzzleSocketName); }

	FORCEINLINE FTransform GetGripTransform() const { return SkeletalMeshComponent->GetSocketTransform(GripSocketName); }

	UClass* GetTemplateClass() const { return WeaponTemplate ? WeaponTemplate : GetClass(); }

	void CopyWeaponItemInfo(const FWeaponItemInfo* RefWeaponItemInfo)
	{
		WeaponItemInfo.CurrentAmmo = RefWeaponItemInfo->CurrentAmmo;
		WeaponItemInfo.MaxAmmo = RefWeaponItemInfo->MaxAmmo;
		RefWeaponItemInfo = nullptr;
	}

protected:
	// Apply to Blueprint
	UFUNCTION(BlueprintCallable, Category = "AbstractWeapon|Function")
	virtual void OnFirePressInternal();

	UFUNCTION()
	virtual void BeginOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual	void EndOverlapRecieve(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnMeleeOverlapRecieve(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void ResetCharacterOwner();
	virtual void PrepareDestroy();
	void CheckIfValid(AActor* OtherActor, bool& bCanDamageResult);
	void TakeDamageOuter(const FHitResult& HitResult);
	void OnHitTriggerAction(const FTransform InTransform, const FHitResult HitResult);
	void PlayEffect(const FHitResult& HitResult, USceneComponent* const InComponent, bool& OutHitActor);
	void UpdateCustomDepth(const bool bEnable);
	void AddDelegate();
	void RemoveDelegate();
	void CreateCollisionQueryParams(FCollisionQueryParams &OutCollisionQueryParams);

#pragma region Uproperty
protected:
	TWeakObjectPtr<class ACharacterBase> CharacterPtr;
	TArray<class AActor*> IgnoreActors;
	FTimerHandle ReloadTimerHandle;
	EGiveDamageType GiveDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	FWeaponItemInfo WeaponItemInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	FName GripSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	int32 NeededAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	float BulletDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	float ReloadDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Variable")
	bool bDebugTrace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbstractWeapon|Variable")
	bool bWasMeleeAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbstractWeapon|Variable")
	bool bEquip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbstractWeapon|Variable")
	bool bReload;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbstractWeapon|Variable")
	bool bCanFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class USoundBase* PickupSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class UParticleSystem* FlashEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	class UParticleSystem* ImpactEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	TSubclassOf<class ABulletBase> BulletsTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbstractWeapon|Asset")
	TSubclassOf<class AAbstractWeapon> WeaponTemplate;
#pragma endregion


};
