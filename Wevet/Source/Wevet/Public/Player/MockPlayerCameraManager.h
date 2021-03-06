// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "MockPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API AMockPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	AMockPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
};
