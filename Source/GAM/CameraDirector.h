// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraDirector.generated.h"

UCLASS()
class GAM_API ACameraDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;							// override

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, category = "cameras")                //CameraOne (original) created
	AActor* CameraOne;

	UPROPERTY(EditAnywhere, category = "cameras")                //CameraTwo (Maze) created
	AActor* CameraTwo;
		
	UPROPERTY(VisibleAnywhere, Category = "Mesh")                //Mesh component for OldTv
	class UStaticMeshComponent* OldTv;

	void OnInteract();											// Interact Function so that it can be interacted with
	
	void goBack();												// Return function for CameraOne

	UPROPERTY(EditAnywhere)                          
	float SmoothBlendTime = 0.25f;                              // BlendTime set here
};
