// Fill out your copyright notice in the Description page of Project Settings.


//																			IGNORE THIS CLASS
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"                     //Included for access to TimelineFeature
#include "DoorLock.generated.h"

UCLASS()
class GAM_API ADoorLock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorLock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

UPROPERTY(VisibleAnywhere, Category = "Mesh")                //Mesh component for LockFrame
	class UStaticMeshComponent* LockFrame;

UPROPERTY(VisibleAnywhere, Category = "Mesh")                //Mesh component for Lock
	class UStaticMeshComponent* Lock;

void OnInteract();

void MoveLock();

FTimeline Timeline;
UPROPERTY(EditAnywhere)										// Gives me access to add a curve float (time-line)
UCurveFloat* CurveFloat;

UPROPERTY(EditAnywhere)										// boolean to know if door is locked or unlocked
bool bIsEngaged = false;

UPROPERTY(EditAnywhere)
float LockPosition;



UPROPERTY(EditAnywhere)                // A Door to control
AActor* DoorToOpen;

UFUNCTION()
void MoveLock(float value);

};
