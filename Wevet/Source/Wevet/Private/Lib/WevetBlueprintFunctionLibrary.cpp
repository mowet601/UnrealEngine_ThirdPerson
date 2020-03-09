// Copyright © 2018 wevet works All Rights Reserved.


#include "Lib/WevetBlueprintFunctionLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "LoadingScreen.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "AI/WayPointBase.h"
#include "Kismet/KismetSystemLibrary.h"


void UWevetBlueprintFunctionLibrary::PlayLoadingScreen(bool bPlayUntilStopped, float PlayTime)
{
	ILoadingScreenModule& LoadingScreenModule = ILoadingScreenModule::Get();
	LoadingScreenModule.StartInGameLoadingScreen(bPlayUntilStopped, PlayTime);
}

void UWevetBlueprintFunctionLibrary::StopLoadingScreen()
{
	ILoadingScreenModule& LoadingScreenModule = ILoadingScreenModule::Get();
	LoadingScreenModule.StopInGameLoadingScreen();
}

bool UWevetBlueprintFunctionLibrary::IsInEditor()
{
	return GIsEditor;
}

void UWevetBlueprintFunctionLibrary::CreateDynamicMaterialInstance(
	UPrimitiveComponent* PrimitiveComponent,
	TArray<UMaterialInstanceDynamic*>& OutMaterialArray)
{
	const int32 MaterialNum = PrimitiveComponent->GetNumMaterials();
	for (int Index = 0; Index < MaterialNum; ++Index)
	{
		if (UMaterialInstanceDynamic * Instance = PrimitiveComponent->CreateDynamicMaterialInstance(
			Index, PrimitiveComponent->GetMaterial(Index)))
		{
			OutMaterialArray.Emplace(Instance);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("null ptr material instance"));
		}
	}
}

void UWevetBlueprintFunctionLibrary::OrderByDistance(
	AActor* PlayerActor,
	TArray<AActor*>InArray,
	TArray<AActor*>& OutArray)
{
	InArray.Sort([PlayerActor](const AActor& A, const AActor& B)
	{
		float DistanceA = A.GetDistanceTo(PlayerActor);
		float DistanceB = B.GetDistanceTo(PlayerActor);
		return DistanceA > DistanceB;
	});
	OutArray = InArray;
}

AActor* UWevetBlueprintFunctionLibrary::CloneActor(AActor* InputActor)
{
	UWorld* World = InputActor->GetWorld();
	FActorSpawnParameters params;
	params.Template = InputActor;

	UClass* ItemClass = InputActor->GetClass();
	AActor* const SpawnedActor = World->SpawnActor<AActor>(ItemClass, params);
	return SpawnedActor;
}

AActor* UWevetBlueprintFunctionLibrary::SpawnActorDeferred(AActor* Owner, UClass* ItemClass, const FTransform InTransform)
{
	AActor* SpawningObject = Owner->GetWorld()->SpawnActorDeferred<AActor>(
		ItemClass,
		InTransform,
		Owner,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	return SpawningObject;
}

float UWevetBlueprintFunctionLibrary::GetMeanValue(TArray<float> Values)
{
	int32 Size = Values.Num();
	float Sum = 0;
	for (int i = 0; i < Size; ++i)
	{
		Sum += Values[i];
	}
	return Sum / Size;
}

void UWevetBlueprintFunctionLibrary::CircleSpawnPoints(
	const int32 InSpawnCount,
	const float InRadius,
	const FVector InRelativeLocation,
	TArray<FVector>& OutPointArray)
{
	float AngleDiff = 360.f / (float)InSpawnCount;
	for (int i = 0; i < InSpawnCount; ++i)
	{
		FVector Position = InRelativeLocation;
		float Ang = FMath::DegreesToRadians(90 - AngleDiff * i);
		Position.X += InRadius * FMath::Cos(Ang);
		Position.Y += InRadius * FMath::Sin(Ang);
		OutPointArray.Add(Position);
	}
}

void UWevetBlueprintFunctionLibrary::WorldPawnIterator(
	APawn* Owner,
	const float InDistance,
	TArray<class APawn*>& OutPawnArray)
{
	check(Owner->GetWorld());
	for (FConstPawnIterator It = Owner->GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* Pawn = Cast<APawn>(*It);
		if (Pawn == nullptr || (Pawn && Pawn == Owner))
		{
			continue;
		}
		float Dist = (Owner->GetActorLocation() - Pawn->GetActorLocation()).Size();
		if (Dist >= InDistance)
		{
			continue;
		}
		OutPawnArray.AddUnique(Pawn);
	}
}

void UWevetBlueprintFunctionLibrary::GetWorldWayPointsArray(
	AActor* Owner,
	const float InDistance,
	TArray<class AWayPointBase*>& OutWayPointArray)
{
	for (TActorIterator<AWayPointBase> ActorIterator(Owner->GetWorld()); ActorIterator; ++ActorIterator)
	{
		if (AWayPointBase * WayPoint = *ActorIterator)
		{
			if (WayPoint == nullptr)
			{
				continue;
			}
			const float Dist = (Owner->GetActorLocation() - WayPoint->GetActorLocation()).Size();
			if (Dist >= InDistance)
			{
				continue;
			}
			OutWayPointArray.Emplace(WayPoint);
		}
	}
}

const FString UWevetBlueprintFunctionLibrary::NormalizeFileName(const char* String)
{
	return NormalizeFileName(FString(UTF8_TO_TCHAR(String)));
}

const FString UWevetBlueprintFunctionLibrary::NormalizeFileName(const FString& String)
{
	FString Ret = String;
	FText ErrorText;

	if (!FName::IsValidXName(*Ret, INVALID_OBJECTNAME_CHARACTERS INVALID_LONGPACKAGE_CHARACTERS, &ErrorText))
	{
		FString InString = INVALID_OBJECTNAME_CHARACTERS;
		InString += INVALID_LONGPACKAGE_CHARACTERS;

		TArray<TCHAR> CharArray = InString.GetCharArray();
		for (int i = 0; i < CharArray.Num(); ++i)
		{
			FString Template;
			Template.AppendChar(CharArray[i]);
			Ret = Ret.Replace(Template.GetCharArray().GetData(), TEXT("_"));
		}
	}
	return Ret;
}

void UWevetBlueprintFunctionLibrary::DrawDebugString(
	AActor* const Owner, 
	const FString LogString, 
	FLinearColor DebugColor)
{
	if (!Owner->IsValidLowLevel())
	{
		return;
	}

	UKismetSystemLibrary::DrawDebugString(
		Owner->GetWorld(),
		Owner->GetActorLocation(),
		*LogString,
		nullptr,
		DebugColor,
		2.0);
}
