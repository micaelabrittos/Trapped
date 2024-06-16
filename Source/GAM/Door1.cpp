// Fill out your copyright notice in the Description page of Project Settings.


#include "Door1.h"
#include "Kismet/GameplayStatics.h"
#include "GAMCharacter.h"

// Sets default values
ADoor1::ADoor1()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));					    // StaticMesh Component created
	DoorFrame->SetupAttachment(RootComponent);															// StaticMesh attached to RootComponent

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));									// Second StaticMesh Component Created		
	Door->SetupAttachment(DoorFrame);																	// Second Static Mesh Component attached to DoorFrame

}

// Called when the game starts or when spawned
void ADoor1::BeginPlay()
{
	Super::BeginPlay();

	if (CurveFloat)																						// Door only works if a CurveFloat (timeline) has been selected
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &ADoor1::OpenDoor);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}
	
}

// Called every frame
void ADoor1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timeline.TickTimeline(DeltaTime);

}

void ADoor1::OnInteract()																			// As Director is interacted with
{
	UE_LOG(LogTemp, Warning, TEXT("Interacted"));													// DebugLog 'interacted'

	if (!bIsLocked)																					// If the door is not locked
	{
		
		if (bIsDoorClosed)																			// If the door is closed
		{
			Timeline.PlayFromStart();																// Run timeline that opens it
		}
		else
		{
			Timeline.Reverse();																		// If open, reverse timeline to close it
		}

		bIsDoorClosed = !bIsDoorClosed;																// boolean to know if door is open or close
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Door is Locked"));											// If locked, Let player know the door is locked
	}

}

void ADoor1::OpenDoor(float Value)
{
	FRotator Rot = FRotator(0.f, DoorRotateAngle * Value, 0.f);										// Door rotates (90degrees) when called relative to doorframe (attachment)

	Door->SetRelativeRotation(Rot);
}