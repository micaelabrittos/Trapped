// Fill out your copyright notice in the Description page of Project Settings.


#include "LightOverlap.h"
#include "Components/SpotLightComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "MyCrate.h"

// Sets default values
ALightOverlap::ALightOverlap()
{
	PrimaryActorTick.bCanEverTick = true;

	LightIntensity = 30000.f;																				// Set Light Intensity

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spot Light"));							// SpotLight Created
	SpotLight->Intensity = LightIntensity;																	// Intesity Set
	SpotLight->SetupAttachment(RootComponent);

	BoxLight = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));							// Collision Box Created
	BoxLight->InitSphereRadius(300.f);																		// Set Size
	BoxLight->SetCollisionProfileName(TEXT("Trigger"));
	BoxLight->SetupAttachment(RootComponent);

	RootComponent = BoxLight;

	BoxLight->OnComponentBeginOverlap.AddDynamic(this, &ALightOverlap::OnOverlapBegin);						// Dynamics for event overlap begin and end
	BoxLight->OnComponentEndOverlap.AddDynamic(this, &ALightOverlap::OnOverlapEnd);


}

void ALightOverlap::BeginPlay()
{
	Super::BeginPlay();


//	Commented Out Since Debug has finished
//	DrawDebugSphere(GetWorld(), GetActorLocation(), 80.f, 8, FColor(0,0, 255), true, -1, 0, 2);			// Debug Sphere size, color, and stuff 


}

void ALightOverlap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALightOverlap::ToggleLight()
{
	SpotLight->ToggleVisibility();																			// Turns Light On and Off
}

void ALightOverlap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCrate* CrateBox = Cast<AMyCrate>(OtherActor);																// Casting to MyCrate
	if (!isLightOn)
	{
		if (!isBoxNeeded)																							// If no Crate needed
		{
			if (OtherActor && (OtherActor != this) && OtherComp)
			{
				ToggleLight();																						// Toggle when player enters sphere
				isLightOn = true;
			}
		}

		else
		{
			if ((OtherActor) && (OtherActor != this) && (OtherActor == CrateBox) && OtherComp)						// If crate is needed
			{
				ToggleLight();																						// Toggle when crate enters sphere
				isLightOn = true;
			}
		}
	}
}

void ALightOverlap::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyCrate* CrateBox = Cast<AMyCrate>(OtherActor);
	if (!isBoxNeeded)																							// If box not needed
	{
		if (OtherActor && (OtherActor != this) && (OtherActor != CrateBox) && OtherComp)
		{
			ToggleLight();																						// Toggle when player exits sphere
																												// if box is needed, then never turn off once it's on
			isLightOn = false;
		}
	}
}
