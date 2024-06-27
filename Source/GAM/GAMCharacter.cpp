// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAMCharacter.h"
#include "GAMProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "CameraDirector.h"																		// Including so that it can interact with its functions
#include "DrawDebugHelpers.h"																	// Included so that I can display tracers
#include "Door1.h"																				// Included so that Character can interact with door
#include "Kismet/KismetMathLibrary.h"															// So I can use NearlyZero
#include "TimerManager.h"																		// Grants me access to time-lines

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AGAMCharacter

// Character Projection Explanation:

// 1. Initialize Character Components:
//    We start by initializing the necessary components for the character, such as the Mesh component for rendering (lines 59-65), its gun (68-72)
//    and the CapsuleComponent for collision detection (line 46)
//
// 2. Set Character's Initial Location:
//    We set the character's initial location in the world through the Unreal Editor
//
// 3. Attach Camera to Character:
//    We attach a CameraComponent to the character's Mesh component. This camera
//    will follow the character's movements and rotations. (lines 53-56)
//
// 4. Input and Character Movement:
//    We handle input from the player to control the character's movement. This input affects the character's position,
//    orientation, and animations. (Set in Unreal Editor and executed in SetupPlayerInputComponent())


AGAMCharacter::AGAMCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);                          // Capsule component

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent																										///
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)             ////
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component																									////
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multi-player
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/animation blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multi-player
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
}

void AGAMCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
	// ON BEGIN PLAY, FILL HEAT BAR AND ALLOW PLAYER TO SHOOT//////////////////////////////////////////////////////////////
	// BIND FUNCTIONS TO HEAT TIMELINE///////////////////////////////////////////////////////////////////////////////////

	HealthPercentage = 1.0f;
	PreviousHealth = HealthPercentage;

	FullHeat = 100.0f;
	Heat = FullHeat;                            
	HeatPercentage = 1.0f;
	bCanShoot = true;
	PreviousHeat = HeatPercentage;
	HeatValue = 0.0f;

	if (HeatCurve)
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("SetHeatValue"));
		TimelineFinishedCallback.BindUFunction(this, FName("SetHeatState"));
		HeatTimeline.AddInterpFloat(HeatCurve, TimelineCallback);
		HeatTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}

	/////////////////////STARTS TIMER ON BEGINPLAY////////////////////////////////////////////////////////////////////////////
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AGAMCharacter::CountDown, 1.f, true, 0.0);
}

// CHECK EVERY TICK, IF GUN IS OVERHEATED, CHANGE TO OVERHEAT MATERIAL WHEN IT IS////////////////////////////////////////////
void AGAMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HeatTimeline.TickTimeline(DeltaTime);
	if (GunOverheatMaterial && Heat <= 1)
	{
		FP_Gun->SetMaterial(0, GunOverheatMaterial);
	}
}

///////////////////////////////////// Input ////////////////////////////////////////////////////////////////////////////////////////////////

// Character Movement using Linear Algebra:

// 1. Vector Representation:
//    We represent the character's position in 3D space using a vector (x, y, z).

// 2. Movement:
//    To move the character, we calculate a translation vector representing the desired movement direction and distance.
//    This vector is then added to the character's current position vector, resulting in a new position.

// 3. Matrices for Transformations:
//    Transformations such as rotation and scaling are performed using matrices. We can modify them to change its rotation and scale.

// 4. Vector Addition and Scalar Multiplication:
//    Vector addition and scalar multiplication are used to modify the character's position.
//    Adding a velocity vector to the current position results in updated position.

// 5. Dot Product and Cross Product:
//    Dot product helps us calculate values like distance and projection, while cross product is useful for calculating
//    orthogonal vectors and determining rotation axes.
//
// Most of this information was gathered from "Game Programming Algorithms and Techniques: A Platform-Agnostic Approach" by Sanjay Madhav, and through the code itself. 

void AGAMCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up game play key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AGAMCharacter::OnFire);

	// Bind Interact
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AGAMCharacter::Interact);                       // ActionMapping of Interact

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AGAMCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AGAMCharacter::MoveForward);								// Movement Bindings
	PlayerInputComponent->BindAxis("MoveRight", this, &AGAMCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turn rate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGAMCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGAMCharacter::LookUpAtRate);
}

/// /////////////////////////////////////////// INTERACT //////////////////////////////////////////////////////////

// Static Mesh Collision Detection Explanation:
//    Static mesh components represent 3D objects in the game world. They handle rendering and provide collision data for interaction.
//
//    Each static mesh component has an associated collision mesh that approximates the object's shape for collision
//    detection purposes. These collision meshes can be as simple as a cube or as complex as multiple polygons shrink-wrapping the object.
//
//    The game engine offers collision channels and presets that define how objects should interact with each other.
//    When a collision is detected, we can define how objects respond.
//    For example, an object can be set to completely ignore other meshes, trigger as other objects overlap, or completely blocking them from crossing
//    its boundaries. This interaction may also involve triggering events, such as adjusting physics properties, playing animations, or applying damage.

// Game Physics:

//    Physics Simulation:
//    Game physics simulates real physics in a game environment. i.e - gravity, friction, and collision.
//
//    Physics engines handle the dynamics of solid/rigid bodies, allowing them to move, rotate, and collide naturally.
//    They also provide collision detection algorithms that determine when objects intersect. Upon collision,
//    appropriate responses are computed, such as bouncing off surfaces, applying forces, or triggering events.


void AGAMCharacter::Interact()																			 
{
	UE_LOG(LogTemp, Warning, TEXT("Pressed F"));                                                         // For debugging
	if (FirstPersonCameraComponent == nullptr) return;													 // If pointer hits nothing, Do nothing
	FHitResult HitResult;																				 // Pointer can hit stuff
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();									 // Pointer starts where Camera is
	FVector End = Start + FirstPersonCameraComponent->GetForwardVector() * InteractLineTraceLength;      // Pointer ends at InteractLineTraceLength

	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();									 // Get tracer info if it hits something

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);      // Pointer Created

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, *TraceParams))		 // If it hits something
	{
		DrawDebugLine(GetWorld(), Start, End, FColor(255, 0, 0), true);									 // Display tracer
	}

	ACameraDirector* OldTv = Cast<ACameraDirector>(HitResult.GetActor());								 // If HitResult is Actor CameraDirector's OldTV
	if (OldTv)
	{
		OldTv->OnInteract();																			 // Something happens in GameDirector on Interact
	}

	ADoor1* Door = Cast<ADoor1>(HitResult.GetActor());													 // If HitResult is Actor Door1's Door

	if (Door)
	{
		Door->OnInteract();																				 // Something happens in Door1 on Interact
	}
}

void AGAMCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != nullptr && !FMath::IsNearlyZero(Heat, 0.001f) && bCanShoot)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AGAMProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AGAMProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}

		// try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}

		// TIMELINE MANAGER AND HOW MUCH HEAT GENERATED PER SHOT
		HeatTimeline.Stop();
		GetWorldTimerManager().ClearTimer(HeatTimerHandle);
		SetHeatChange(-20.0f);
		GetWorldTimerManager().SetTimer(HeatTimerHandle, this, &AGAMCharacter::UpdateHeat, 5.0f, false);
	}
}

void AGAMCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AGAMCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AGAMCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AGAMCharacter::MoveForward(float Value)																// MOVING
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AGAMCharacter::MoveRight(float Value)																	// TURNING
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AGAMCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGAMCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AGAMCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AGAMCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AGAMCharacter::EndTouch);

		return true;
	}
	
	return false;
}

// CURRENT HEAT///////////////////////////////////////////////////////////////////////////////////////////////////////
float AGAMCharacter::GetHeat()
{
	return HeatPercentage;
}

// TRANSFORMS HEAT VALUE TO TEXT FOR HUD DISPLAY////////////////////////////////////////////////////////////////////////////
FText AGAMCharacter::GetHeatIntText()
{
	int32 HT = FMath::RoundHalfFromZero(HeatPercentage * 100);
	FString HTS = FString::FromInt(HT);
	FString FullHTS = FString::FromInt(FullHeat);
	FString HeatHUD = HTS + FString(TEXT("/") + FullHTS);
	FText HTTEXT = FText::FromString(HeatHUD);
	return HTTEXT;
}

// SET CURRENT AMOUNT OF HEAT IN GUN////////////////////////////////////////////////////////////////////////////
void AGAMCharacter::SetHeatValue()
{
	TimelineValue = HeatTimeline.GetPlaybackPosition();
	CurveFloatValue = PreviousHeat + HeatValue * HeatCurve->GetFloatValue(TimelineValue);
	Heat = CurveFloatValue * FullHeat;
	Heat = FMath::Clamp(Heat, 0.0f, FullHeat);
	HeatPercentage = CurveFloatValue;
	HeatPercentage = FMath::Clamp(HeatPercentage, 0.0f, 1.0f);
}

// IS GUN OVERHEATED? IF HEATVALUE IS 0, THEN IT CAN SHOOT //////////////////////////////////////////////////////////
void AGAMCharacter::SetHeatState()
{
	bCanShoot = true;
	HeatValue = 0.0f;
	if (GunDefaultMaterial)
	{
		FP_Gun->SetMaterial(0, GunDefaultMaterial);
	}
}

// UPDATES CURRENT HEAT, CAN'T SHOOT IF GUN TOO HOT ////////////////////////////////////////////////////////////////////
void AGAMCharacter::UpdateHeat()
{
	PreviousHeat = HeatPercentage;
	HeatPercentage = Heat / FullHeat;
	HeatValue = 1.0f;
	HeatTimeline.PlayFromStart();
	if (Heat <= 1)
	{
		bCanShoot = false;
	}
}

// Heat value change, for HUD /////////////////////////////////////////////////////////////////////////////////////////////
void AGAMCharacter::SetHeatChange(float HeatChange)
{
	PreviousHeat = HeatPercentage;
	HeatValue = HeatChange / FullHeat;
	HeatTimeline.PlayFromStart();
}

//TIMER COUNTDOWN /////////////////////////////////////////////////////////////////////////////////////////////////////
void AGAMCharacter::CountDown()
{
	if (Seconds != 0)
	{
		Seconds = Seconds - 1;
	}
	else
	{
		if (Minutes == 0)
		{
			GameOver();
		}
		else
		{
			Minutes = Minutes - 1;
			Seconds = 59;
		}
	}
}

void AGAMCharacter::ChangeHealth(float Delta)
{
	CurrentHealth += Delta;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		HandleDeath();
	}
}

float AGAMCharacter::GetHealth()
{
	return HealthPercentage;
}

FText AGAMCharacter::GetHealthIntText()
{
	int32 HT = FMath::RoundHalfFromZero(HealthPercentage * 100);
	FString HTS = FString::FromInt(HT);
	FString FullHTS = FString::FromInt(MaxHealth);
	FString HealthHUD = HTS + FString(TEXT("/") + FullHTS);
	FText HTTEXT = FText::FromString(HealthHUD);
	return HTTEXT;
}

void AGAMCharacter::UpdateHealth()
{
	PreviousHealth = HealthPercentage;
	HealthPercentage = CurrentHealth / MaxHealth;
}

void AGAMCharacter::HandleDeath()
{
	GameOver();

}

// When player loses, Game ends
void AGAMCharacter::GameOver()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Type::Quit, false);
}