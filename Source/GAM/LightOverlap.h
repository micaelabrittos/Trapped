// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightOverlap.generated.h"

UCLASS()
class GAM_API ALightOverlap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightOverlap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;										// overrode for debugging

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


																			// Light component, collision box, and light intensity
	UPROPERTY(VisibleAnywhere, Category = "LightSwitch")
		class USpotLightComponent* SpotLight;

	UPROPERTY(VisibleAnywhere, Category = "LightSwitch")
		class USphereComponent* BoxLight;

	UPROPERTY(VisibleAnywhere, Category = "LightSwitch")
		float LightIntensity;

	
	// Functions I will need
	UFUNCTION()
		void ToggleLight();

	//Overlap Functions
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere)
		bool isBoxNeeded;																			// A Box needed to trigger?

	UPROPERTY(EditAnywhere)																			// is Light already on? Always starts off
		bool isLightOn = false;
};
