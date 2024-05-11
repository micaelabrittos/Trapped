// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDirector.h"
#include "Kismet/GameplayStatics.h"																	// To be able to access OurPlayerController
#include "GAMCharacter.h"


// 

//  Camera Components:
//    The game engine provides camera components that handle the rendering and positioning of in-game cameras. These
//    components allow us to define and manipulate camera behavior.
//
//    We can set up multiple cameras in the scene, each with distinct properties, to achieve various camera styles. This
//    allows us to seamlessly switch between different views during gameplay.
//
//	  Follow Camera Style:
//    To implement a follow camera style, we attach the camera to a target. The camera then follows the target's movements.
//
//    Top-Down Camera Style:
//    For a top-down view, I positioned the camera above the game world and adjust its orientation.
//
//   The camera component also allows for creativity by allowing us to change the field of view, and cinematic shots by combining static
//   and follow cameras. Adding a post process to the scene will further improve the look and feel of the game


// Sets default values
ACameraDirector::ACameraDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OldTv = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OldTvYAY"));							// StaticMesh Component created
	OldTv->SetupAttachment(RootComponent);															// StaticMesh attached to RootComponent

}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACameraDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// When the player interact with the old TV, the camera view will switch between CameraOne (the character's follow camera), to Camera Two
// which has been set up to be a top-down view of the maze. And then switch back to follow camera after 3 seconds have passed.

void ACameraDirector::OnInteract()																	// As Director is interacted with
{
	UE_LOG(LogTemp, Warning, TEXT("Interacted"));													// DebugLog 'interacted'
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);		// Take over Game Controller
	
	if (OurPlayerController)																		// If player is interacting with this
	{
		OurPlayerController->SetViewTargetWithBlend(CameraTwo, SmoothBlendTime);					// Switch to Camera Two with Blend
		FTimerHandle TimerHandle;																	// Interior Timer created
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()									// Timer started, 3 seconds
			{
				goBack();																			// After 3 seconds, Run goBack function
				UE_LOG(LogTemp, Warning, TEXT("This text will appear in the console 3 seconds after execution")); // DebugLog to make sure timer is working correctly
			}, 3, false);																			// Length of timer
	}
}

void ACameraDirector::goBack()																		// When goBack function is called
{
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);		// Game Controller is taken over again
	if (OurPlayerController)
	{

		OurPlayerController->SetViewTargetWithBlend(CameraOne, SmoothBlendTime);					// Return to CameraOne(Original)
	}
}