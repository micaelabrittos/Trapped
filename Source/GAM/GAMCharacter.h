// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"													// Add Timeline
#include "GAMCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class AGAMCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere)
		float InteractLineTraceLength = 350.f;																	/////////// Length of Interact Pointer

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* L_MotionController;


public:
	AGAMCharacter();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;																// overriding Tick function

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AGAMProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

	
	/////////// HUD propertys and function to manage Gun Overheating //////////////////////////////////////////////////////////////////////////////////////
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heat")
		float Heat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heat")
		float FullHeat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heat")
		float HeatPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heat")
		float PreviousHeat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heat")
		float HeatValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heat")
		UCurveFloat *HeatCurve;

	UPROPERTY(EditAnywhere, Category = "Heat")
		FTimeline HeatTimeline;

	UPROPERTY(EditAnywhere, Category = "Heat")
		FTimerHandle MemberTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Heat")
		FTimerHandle HeatTimerHandle;

	float CurveFloatValue;
	float TimelineValue;
	bool bCanShoot;

	UFUNCTION(BlueprintPure, Category = "Heat")
		float GetHeat();

	UFUNCTION(BlueprintPure, Category = "Heat")
		FText GetHeatIntText();

	UFUNCTION()
		void SetHeatValue();
	UFUNCTION()
		void SetHeatState();

	UFUNCTION()
		void SetHeatChange(float HeatChange);

	UFUNCTION()
		void UpdateHeat();

	UPROPERTY(EditAnywhere, Category = "Heat")
		class UMaterialInterface* GunDefaultMaterial;

	UPROPERTY(EditAnywhere, Category = "Heat")
		class UMaterialInterface* GunOverheatMaterial;

	/////////// HUD Properties and Functions for TIMER ///////////////////////////////////////////////////////////////////////////////////

	UFUNCTION()
		void CountDown();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	int Minutes = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	int Seconds = 0;

	////////// GameOVER ///////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION()
		void GameOver();

protected:
	
	/** Fires a projectile. */
	void OnFire();

	void Interact();																					//Interact Function exists

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

