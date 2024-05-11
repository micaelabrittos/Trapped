// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAMGameMode.h"
#include "GAMHUD.h"
#include "Kismet/GameplayStatics.h"
#include "GAMCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGAMGameMode::AGAMGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGAMHUD::StaticClass();
}

void AGAMGameMode::BeginPlay()
{
	Super::BeginPlay();

	//SetCurrentState(EGamePlayState::EPlaying);

	MyCharacter = Cast<AGAMCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

// Commented out because I decided against using GameStates ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


/*
void AGAMGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyCharacter)
	{
		if (FMath::IsNearlyZero(MyCharacter->GetHeat(), 0.001f))
		{
			SetCurrentState(EGamePlayState::EGameOver);
		}
	}
}


void AGAMGameMode::SetCurrentState(EGamePlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(CurrentState);
}


void AGAMGameMode::HandleNewState(EGamePlayState NewState)
{
	switch(NewState)
	{
		case EGamePlayState::EPlaying:
		{
			//do nothing
		}
		break;

		case EGamePlayState::EGameOver:
		{
			UGamePlayStatics::OpenLevel()this, FName(*GetWorld()->GetName()), false);
		}
		break;

		default:
		case EGamePlayState::EUnknown:
		{
			// do nothing
		}
		break;
	}
}

*/