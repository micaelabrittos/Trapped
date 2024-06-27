// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCrate.h"
//#include "Components/BoxComponent.h"
#include "GAMProjectile.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GAMCharacter.h"

// Sets default values
AMyCrate::AMyCrate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AreaBox = CreateDefaultSubobject<USphereComponent>(TEXT("AreaBox"));
	AreaBox->SetCollisionProfileName(TEXT("Trigger"));
	AreaBox->InitSphereRadius(300.f);																		// Set Size
	AreaBox->OnComponentEndOverlap.AddDynamic(this, &AMyCrate::OnOverlapEnd);								
	AreaBox->SetupAttachment(RootComponent);


	Crate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Crate"));					    //   StaticMesh for door frame
	Crate->SetSimulatePhysics(true);															// Crate has physics
	Crate->OnComponentHit.AddDynamic(this, &AMyCrate::OnCompHit);								 // When hit, run OnCompHit function
	Crate->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyCrate::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyCrate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyCrate::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AGAMCharacter* MyCharacter = Cast<AGAMCharacter>(OtherActor);																// Reference to Character

	if ((OtherActor != NULL) && (OtherActor != this) && (OtherActor != MyCharacter) && (OtherComp != NULL))						// If crate is hit by anything but itself or the character (bullet okay)
	{
		if (GEngine)
		{																															// Send Debug Message
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Crate hit by: %s"), *OtherActor->GetName()));
			if (hitCounter >= 1)																											
			{
				hitCounter--;																										// Decrease hitCounter by 1
			}
			else
			{
				Destroy();																											// Destroy Crate when hitCount is 0
			}		
		}
	}
}

void AMyCrate::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)							// Overlap End Function
{	
	if (OtherActor && (OtherActor == this) && OtherComp)																											// If crate leaves area
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Box %s Destroyed after leaving area"), *OtherActor->GetName()));
		Destroy();																																					// Destroy Crate
	}
}