// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Runtime/Engine/Classes/Engine/TargetPoint.h"
#include "Components/TimelineComponent.h"
#include "EnemyAI.h"
#include "MyAIController.generated.h"

/**
 * 
 */
UCLASS()
class GAM_API AMyAIController : public AAIController
{
	GENERATED_BODY()

	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;		// Overrides what happens in AIController's OnMoveCompleted function with my own code

public:
	void BeginPlay() override;																		// Overrides what happens in AIController's BeginPlay function with my own code

private:
	UPROPERTY()
		TArray<AActor*> Waypoints;																	// For getting access to TargetPoints

	UFUNCTION()
		ATargetPoint* GetRandomWaypoint();															// Access to Target point's GetRandomWayPoint function
	
	UFUNCTION()
		void GoToRandomWaypoint();																	// New function for randomness

	FTimerHandle TimerHandle;																		// Time-line access
};
