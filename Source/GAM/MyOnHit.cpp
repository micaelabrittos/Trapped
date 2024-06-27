// Fill out your copyright notice in the Description page of Project Settings.


#include "MyOnHit.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyOnHit::AMyOnHit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	MyComp->SetSimulatePhysics(true);																	// Object has physics
	MyComp->SetNotifyRigidBodyCollision(true);															// Can interact with other objects through collision
	MyComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");									// Blocks/Collides with everything/anything
	MyComp->OnComponentHit.AddDynamic(this, &AMyOnHit::OnCompHit);										// When hit, run OnCompHit function
	MyComp->SetupAttachment(RootComponent);																// attached to root

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));					    //   StaticMesh for door frame
	DoorFrame->SetMobility(EComponentMobility::Static);													//   Now it won't move when pushed
	DoorFrame->SetupAttachment(RootComponent);															//	 Attached to root

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));									//	static mesh for door	
	Door->SetupAttachment(DoorFrame);																	// attached to frame	

	ButtonFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonFrame"));					//	static mesh for button frame
	ButtonFrame->SetMobility(EComponentMobility::Static);												//  Now it won't move when pushed
	ButtonFrame->SetupAttachment(RootComponent);														// attached to root

	Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button"));								// static mesh for button
	Button->SetupAttachment(ButtonFrame);																// attached to frame
}

// Called when the game starts or when spawned
void AMyOnHit::BeginPlay()
{
	Super::BeginPlay();
	
	if (CurveFloat && CurveFloat2)																						// On Hit only works if both Curve floats are selected
	{
		FOnTimelineFloat TimelineProgress;						
		FOnTimelineFloat TimelineProgress2;
		TimelineProgress.BindDynamic(this, &AMyOnHit::PressButton);														// calls Press Button
		TimelineProgress2.BindDynamic(this, &AMyOnHit::OpenDoor);														// calls open door
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);															// time-line uses curve float
		Timeline2.AddInterpFloat(CurveFloat2, TimelineProgress2);														// time-line 2 uses curve float 2
	}
}

// Called every frame
void AMyOnHit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Timeline.TickTimeline(DeltaTime);																					// time-lines are on tick
	Timeline2.TickTimeline(DeltaTime);

}

void AMyOnHit::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && (bIsDoorClosed))						// if door is closed and box is hit by anything but itself
	{
		if (GEngine)
		{
			Timeline.PlayFromStart();																														// Run time-line that presses button
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Button Has been pressed by : %s" ), *OtherActor->GetName()));	// Debug message
			bIsDoorClosed = false;																		// Door now open

			FTimerHandle TimerHandle;																	// Internal Timer created
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()									// Timer started, 2 seconds
				{
					Timeline2.PlayFromStart();															// Run Time-line that open door
				}, 2, false);
		}
	}
}

void AMyOnHit::PressButton(float value)
{
	FVector relativeLoc = FVector(0.f, ButtonTranslateDistance * value, 0.f);									// Button Moves in (-5.5 on Y) when on hit is called 
	Button->SetRelativeLocation(relativeLoc);	
}


void AMyOnHit::OpenDoor(float value)
{
	FVector relativeLoc = FVector(0.f, 0.f, DoorTranslateDistance * value);									// Door Moves up (200 on Z) when open door is called 
	Door->SetRelativeLocation(relativeLoc);
}