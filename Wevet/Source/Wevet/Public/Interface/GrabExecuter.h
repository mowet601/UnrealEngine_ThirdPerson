// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GrabExecuter.generated.h"

class USoundBase;

/*
* Climb System interface
*/
UINTERFACE(BlueprintType)
class WEVET_API UGrabExecuter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class WEVET_API IGrabExecuter
{
	GENERATED_IINTERFACE_BODY()

public :

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IGrabExecuter")
	void CanGrab(bool InCanGrab);
};