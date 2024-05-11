// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GAMHUD.generated.h"

UCLASS()
class AGAMHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGAMHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

//Adding New Widget
	UPROPERTY(EditAnywhere, Category = "Heat")
		TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "heat")
		class UUserWidget* CurrentWidget;

};

