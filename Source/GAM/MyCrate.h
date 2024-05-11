// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyCrate.generated.h"

UCLASS()
class GAM_API AMyCrate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyCrate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//	UPROPERTY(VisibleAnywhere)
//		class UBoxComponent* BoxComponent;								// Collision Box

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* AreaBox;								// Confines box to area

	UPROPERTY(VisibleAnywhere, Category = "Mesh")						// Crate Mesh
	class UStaticMeshComponent* Crate;
	
	UPROPERTY(EditAnywhere, Category = "Health")						// Counter before destroy
		int32 hitCounter = 2;
	
	// Component hit function
	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);	// On Component Hit function

	// Component OverlapEnd function
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
