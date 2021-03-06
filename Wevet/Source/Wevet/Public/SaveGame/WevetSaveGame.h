// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "WevetTypes.h"
#include "WevetSaveGame.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class WEVET_API UWevetSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UWevetSaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	FString UserId;
protected:
	UPROPERTY()
	TArray<FPrimaryAssetId> InventoryItems_DEPRECATED;

	UPROPERTY()
	int32 SavedDataVersion;

	virtual void Serialize(FArchive& Ar) override;
};
