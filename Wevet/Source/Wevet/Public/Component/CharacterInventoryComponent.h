// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterInventoryComponent.generated.h"

class AWeaponBase;
class AItemBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEVET_API UCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterInventoryComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TArray<AWeaponBase*> WeaponArray;
	TArray<AItemBase*> ItemArray;

public:
	void AddWeaponInventory(AWeaponBase* const NewWeaponBase);
	void RemoveWeaponInventory(AWeaponBase* const WeaponBase);
	void ClearWeaponInventory();
	const TArray<AWeaponBase*>& GetWeaponInventory();
	TArray<AWeaponBase*> GetWeaponInventoryOriginal();

	void AddItemInventory(AItemBase* const NewItemBase);
	void RemoveItemInventory(AItemBase* const ItemBase);
	void ClearItemInventory();
	const TArray<AItemBase*>& GetItemInventory();
	TArray<AItemBase*> GetItemInventoryOriginal();

	void RemoveAllInventory();
	bool HasInventoryWeaponItems() const;
	bool HasInventoryItems() const;
};
