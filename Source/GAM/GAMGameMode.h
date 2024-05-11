// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GAMCharacter.h"                // Added to interact with character (HUD)
#include "GAMGameMode.generated.h"

//UENUM()
//enum class EGamePlayState
//{
	//EPlaying,
	//EGameOver,
	//Eunknown
//};

UCLASS(minimalapi)
class AGAMGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGAMGameMode();

	virtual void BeginPlay() override;

	AGAMCharacter* MyCharacter;

	//Commented out because I decided against using GameStates +++++++++++++++++++++++++++++++++++++++++++++
	
	//virtual void Tick(float DeltaTime) override;
	
	//UFUNCTION(BlueprintPure, Category = "Heat")
	//	EGamePlayState GetCurrentState() const;

	//void SetCurrentState(EGamePlayState NewState);

private:
	
	//EGamePlayState CurrentState;

	//void HandleNewState(EGamePlayState NewState);
};



