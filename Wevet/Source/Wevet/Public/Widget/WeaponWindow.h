// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/CharacterBase.h"
#include "WeaponWindow.generated.h"

class UCanvasPanel;
class UBorder;
class UVerticalBox;
class UImage;


UCLASS()
class WEVET_API UWeaponWindow : public UUserWidget
{
	GENERATED_BODY()

public:
	UWeaponWindow(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponWindow|Variable")
	FName ContainerKeyName;
	class UVerticalBox* Container;

	bool bWasVisibility;

#pragma region Header
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponWindow|Variable")
	FName HeaderKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponWindow|Variable")
	FName HeaderRootKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponWindow|Variable")
	FName DisplayKeyName;

	class UTextBlock* DisplayName;
#pragma endregion

#pragma region Image
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponWindow|Variable")
	FName WeaponRootKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponWindow|Variable")
	FName WeaponImageKeyName;

	class UImage* WeaponImage;
#pragma endregion

#pragma region Footer
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponWindow|Variable")
	FName FooterKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponWindow|Variable")
	FName AmmoCounterKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponWindow|Variable")
	FName CurrentAmmoKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponWindow|Variable")
	FName MaxAmmoKeyName;

	class UTextBlock* CurrentAmmoText;
	class UTextBlock* MaxAmmoText;
#pragma endregion

public:
	void RendererImage(AAbstractWeapon* const InWeapon);
	void RendererAmmos(AAbstractWeapon* const InWeapon);
	void Visibility(const bool InVisibility);

protected:
	UWidget* GetTargetWidget(UPanelWidget* const InRootWidget, const FName InTargetWidgetName);
};
