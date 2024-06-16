// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/ShapeComponent.h"
#include "ChaserAI.h"
#include "GAMCharacter.h"
#include "ChasingAI.generated.h"



UCLASS()
class GAM_API AChasingAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AChasingAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool PlayerDetected;										// Is player detected?
	bool CanAttackPlayer;										// Is player within attacking distance?

	UPROPERTY(BlueprintReadWrite)
		bool CanDealDamage;										// Can Damage be dealt?

	class AGAMCharacter* PlayerREF;											// Reference to playable character

	UPROPERTY(EditAnywhere)
		class USphereComponent* PlayerCollisionDetection;					// Sphere for how far chaser sees

	UPROPERTY(EditAnywhere)
		class USphereComponent* PlayerAttackCollisionDetection;				// Sphere for how far chaser can attack

	class AChaserAI* MutantAIController;									// Chaser controller internal renaming to MutantAIController

	void OnAIMoveCompleted(struct FAIRequestID RequestID, const struct FPathFollowingResult& Result);	// Function for Move Completed

	UPROPERTY(EditAnywhere)
		float StoppingDistance = 50.0f;										// How close to the player it needs to be before it stops moving

// Health management
	void TakeDamage(float DamageAmount);


//	ALL	FUNCTIONS

	UFUNCTION()
		void MoveToPlayer();	// Moves to player

	UFUNCTION()
		void SeekPlayer();		// Seeks player

	UFUNCTION()
		void OnPlayerDetectedOverlapBegin(class UPrimitiveComponent* OverlappedComp,	// On player detected
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnPlayerDetectedOverlapEnd(class UPrimitiveComponent* OverlappedComp,		// On player lost
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	UFUNCTION()
		void OnPlayerAttackOverlapBegin(class UPrimitiveComponent* OverlappedComp,		// On player within attacking distance
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnPlayerAttackOverlapEnd(class UPrimitiveComponent* OverlappedComp,		// On player leaving attacking distance
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

private:

	//Health
	UPROPERTY(VisibleAnywhere, Category = "Health")
		float Health;

	// Max health
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float MaxHealth;
};
