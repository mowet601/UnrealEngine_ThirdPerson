// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/AICharacterBase.h"
#include "AISoldier.generated.h"

UCLASS()
class WEVET_API AAISoldier : public AAICharacterBase
{
	GENERATED_BODY()
	
public:
	AAISoldier(const FObjectInitializer& ObjectInitializer);

public:
	virtual bool CanMeleeStrike_Implementation() const override;
	virtual void FirePressed() override;

protected:
	virtual void MainLoop(float DeltaTime) override;
	virtual void TakeDamageActionMontage(const bool InForcePlaying) override;
	virtual	void OnSeePawnRecieve(APawn* OtherPawn) override;
	virtual	void OnHearNoiseRecieve(APawn *OtherActor, const FVector &Location, float Volume) override;

private:
	void UpdateAttackState();
	void UpdateSeeTarget();
	void UpdateHearTarget();
	void OnSeePawnRecieveCallback();

};

