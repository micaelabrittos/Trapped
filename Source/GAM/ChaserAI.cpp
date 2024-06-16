// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaserAI.h"
#include "Kismet/GameplayStatics.h"

#include "NavigationSystem.h"

void AChaserAI::BeginPlay()
{
    Super::BeginPlay();                                                                     // On begin

    NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);                     // Navigation System assigned to NavArea

    RandomPatrol();                                                                         // Start seeking player
}

                                    //  MOVE TO RANDOM LOCATION WITHIN NAVAREA
void AChaserAI::RandomPatrol()
{
    if (NavArea)
    {
        NavArea->K2_GetRandomReachablePointInRadius(GetWorld(), GetPawn()->GetActorLocation(),
            RandomLocation, 15000.0f);

        MoveToLocation(RandomLocation);
    }
}