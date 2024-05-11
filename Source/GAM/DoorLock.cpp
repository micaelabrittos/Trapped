// Fill out your copyright notice in the Description page of Project Settings.


//																		IGNORE THIS CLASS

#include "DoorLock.h"
#include "GAMCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Door1.h"

// Sets default values
ADoorLock::ADoorLock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LockFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockFrame"));					    //MB    StaticMesh Component created
	LockFrame->SetupAttachment(RootComponent);															//StaticMesh attached to RootComponent

	Lock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lock"));									// Second StaticMesh Component Created		
	Lock->SetupAttachment(LockFrame);																	// Second Static Mesh Component attached to LockFrame
}

// Called when the game starts or when spawned
void ADoorLock::BeginPlay()
{
	Super::BeginPlay();

	if (CurveFloat)																						// Door only works if a CurveFloat (timeline) has been selected
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &ADoorLock::MoveLock);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}
	
}

// Called every frame
void ADoorLock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Timeline.TickTimeline(DeltaTime);
}

void ADoorLock::OnInteract()																	//MB As Director is interacted with
{
	UE_LOG(LogTemp, Warning, TEXT("Interacted"));													// DebugLog 'interacted'


	if (bIsEngaged)																			// If the door is closed
	{
		Timeline.PlayFromStart();																// Run timeline that opens it
	}
	else
	{
		Timeline.Reverse();																		// If open, reverse timeline to close it
	}

	bIsEngaged = !bIsEngaged;																// boolean to know if door is open or close
	//ADoor1* Door = Door;
//	Door->UnlockDoor();
//	DoorToOpen = ADoor1* Cast<ADoor1>(this->GetClass());
//	Door->UnlockDoor();
}

void ADoorLock::MoveLock(float value)
{
	//FRotator num = FRotator(0.f, 0.f * 2, 0.f);

	//Lock->SetRelativeRotation(num);
}