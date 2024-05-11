// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "Runtime/Engine/Classes/Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyAI.h"

// Path-Finding Algorithm Explanation:
//		The algorithm involves moving the AI to a series of random waypoints within the game world.

//    On BeginPlay()), the GetAllActorsOfClass function gathers all instances of the ATargetPoint class, representing waypoints in the game world.
//    These waypoints are stored in the Waypoints array. The waypoints serve as markers on the virtual map, defining possible navigation destinations.

//    The GetRandomWaypoint function randomly selects a waypoint from the Waypoints array. This is done by generating a
//    random index within the valid range of waypoints and casting the selected actor to the ATargetPoint class.
//    This choice is like deciding a destination on the virtual map. The AI's MoveToActor function then calculates a path-finding route to reach
//    the chosen waypoint, navigating around obstacles and terrain.

//    The GoToRandomWaypoint function is called in BeginPlay() and invokes the MoveToActor function, which initiates
//    path-finding and navigation for the AI character to reach the selected random waypoint.
//    The AI's movement to the selected waypoint involves the navigation system interacting with the virtual map's representation. It uses data from the map
//    to calculate a collision-free path to the destination.

//    The OnMoveCompleted function is triggered when the AI reaches its target waypoint. After a brief delay (0.5 seconds),
//    as set by the SetTimer function, the GoToRandomWaypoint function is called again to make the AI move to a new random waypoint.

//    The interaction of the path-finding algorithm with the virtual map elements determines the AI's route and behavior. The
//    AI's movement and decision-making are influenced by the positions and accessibility of waypoints as well as the layout
//    and obstacles on the virtual map.

void AMyAIController::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), Waypoints);			// On begin, find all TargetPoints
																		            
    GoToRandomWaypoint();                                                                               // and go to a random one
}   

// Math for going to random waypoint
ATargetPoint* AMyAIController::GetRandomWaypoint()
{
	auto index = FMath::RandRange(0, Waypoints.Num() - 1);
	return Cast<ATargetPoint>(Waypoints[index]);														// Casting to number of targetpoints in map
}

void AMyAIController::GoToRandomWaypoint()
{
	MoveToActor(GetRandomWaypoint());																	// Moves AI to the random targetpoint chosen
}

																		// When AI reaches target point wait half a second and then look for new targetpoint
void AMyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyAIController::GoToRandomWaypoint, 0.5f, false);
}
