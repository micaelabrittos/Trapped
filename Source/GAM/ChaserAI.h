// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "Components/TimelineComponent.h"
#include "ChaserAI.generated.h"

/**
 THIS IS THE AI CONTROLLER FOR THE CHASING AI
 */
UCLASS()
class GAM_API AChaserAI : public AAIController
{
	GENERATED_BODY()

public:
    void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    bool isAlive = true;

private:

    class UNavigationSystemV1* NavArea;                                     // Needed to Navigate area, Also added "NavigationSystem" in GAM.Build.cs

    FVector RandomLocation;                                                 // Random location used while seeking player



public:

    UFUNCTION()
        void RandomPatrol();                                                // Function to Patrol and seek
};
