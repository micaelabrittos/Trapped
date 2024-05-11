// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "MyOnHit.generated.h"

UCLASS()
class GAM_API AMyOnHit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyOnHit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* MyComp;							// Creates Box Component

	// OnHit Function
	// What is hit, and what hit it
	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	UPROPERTY(VisibleAnywhere, Category = "Mesh")                //Mesh component for DoorFrame
		class UStaticMeshComponent* DoorFrame;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")                //Mesh component for Door
		class UStaticMeshComponent* Door;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")                //Mesh component for ButtonFrame
		class UStaticMeshComponent* ButtonFrame;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")                //Mesh component for Button
		class UStaticMeshComponent* Button;

	FTimeline Timeline;
	UPROPERTY(EditAnywhere)										// Gives me access to add a curvefloat (timeline)
		UCurveFloat* CurveFloat;

	FTimeline Timeline2;
	UPROPERTY(EditAnywhere)										// Gives me access to a second curvefloat (timeline2)
		UCurveFloat* CurveFloat2;

	UPROPERTY(EditAnywhere)
		float DoorTranslateDistance = 200.f;					// Distance Door Moves up

	UPROPERTY(EditAnywhere)
		float ButtonTranslateDistance = -5.5f;					// Distance Button Moves in

	UFUNCTION()
		void OpenDoor(float value);								// Open Door function
	
	UFUNCTION()
		void PressButton(float value);							// Press Button Function

	UPROPERTY(EditAnywhere)
		bool bIsDoorClosed = true;								// boolean door is closed
};
