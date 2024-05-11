// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"                     //Included for access to TimelineFeature
#include "Door1.generated.h"

UCLASS()
class GAM_API ADoor1 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

UPROPERTY(VisibleAnywhere, Category = "Mesh")                // Mesh component for DoorFrame
	class UStaticMeshComponent* DoorFrame;

UPROPERTY(VisibleAnywhere, Category = "Mesh")                // Mesh component for Door
	class UStaticMeshComponent* Door;

void OnInteract();											 // Interact Function so that it can be interacted with

FTimeline Timeline;
UPROPERTY(EditAnywhere)										 // Gives me access to add a curvefloat (timeline)
UCurveFloat* CurveFloat;

UPROPERTY(EditAnywhere)
bool bIsDoorClosed = true;									 // boolean to know if door is open or close

UPROPERTY(EditAnywhere)										 // boolean to know if door is locked or unlocked
bool bIsLocked = false;

UPROPERTY(EditAnywhere)
float DoorRotateAngle = 90.f;								 // how much does the door open

UFUNCTION()
void OpenDoor(float value);									 // Function to open door

};
