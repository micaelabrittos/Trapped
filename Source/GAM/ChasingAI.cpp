// Fill out your copyright notice in the Description page of Project Settings.


#include "ChasingAI.h"
#include "ChaserAI.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Navigation/PathFollowingComponent.h"                                                 // Path Following component!
#include "AITypes.h"

// Path-Finding Algorithm Explanation:

//    On the controller's (ChaserAI) BeginPlay()), the RandomPatrol() function is called, initiating
//    the AI's random patrolling behavior within a designated navigation area.

//    The controller RandomPatrol() function uses the navigation system (NavArea) to find a random reachable location within a
//    specified radius around the AI's current location. The AI then moves to this random location using MoveToLocation().
//    This function uses the navigation system to calculate a navigable path from the its current position to the specified location
//    (randomly chosen point or player's location). The navigation system ensures that
//    the AI avoids obstacles and follows navigable surfaces.

//    The OnAIMoveCompleted() function is triggered when the AI finishes moving to a designated location. If no player
//    is detected (PlayerDetected is false), the AI continues patrolling by invoking RandomPatrol() again.

//    When a player enters the AI's collision detection area, the OnPlayerDetectedOverlapBegin() function is called.
//    The AI detects the player, sets PlayerDetected to true, and triggers SeekPlayer() to move toward the player.
//    The AI's collision component, interacting with the navigation system, determines when the player
//    crosses into or out of the AI's awareness zone.

//    The SeekPlayer() function is responsible for moving the AI toward the detected player's location using
//    MoveToLocation(). This simulates the AI chasing the player. During the AI's movement to a new location or the player,
//    the navigation system continuously recalculates the path based on the current environment and obstacles. This ensures
//    that the AI's movement remains smooth and realistic.

//    If the player exits the AI's collision detection area, the OnPlayerDetectedOverlapEnd() function is triggered.
//    The AI sets PlayerDetected to false, resumes patrolling by invoking RandomPatrol(), and sends an on-screen debug
//    message indicating the player was lost.

//    The AI's interaction with the virtual map contributes to realism and immersion. By navigating through the virtual map
//    using the navigation system, the AI's movements mimic those of an intelligent entity in response to the virtual world.

AChasingAI::AChasingAI()
{
    PrimaryActorTick.bCanEverTick = true;

    PlayerCollisionDetection =                                                                   // Creates detection Sphere
        CreateDefaultSubobject<USphereComponent>(TEXT("Player Collision Detection"));

    PlayerCollisionDetection->SetupAttachment(RootComponent);

    PlayerAttackCollisionDetection =                                                            // Creates AtackingDistance Sphere
        CreateDefaultSubobject<USphereComponent>(TEXT("Player Attack Collision Detection"));

    PlayerAttackCollisionDetection->SetupAttachment(RootComponent);

    MaxHealth = 100.0f;
    Health = MaxHealth;

}

// Called when the game starts or when spawned
                                                            // Casts to ChaserAI and creates overlap dynamics
void AChasingAI::BeginPlay()
{
    Super::BeginPlay();

    MutantAIController = Cast<AChaserAI>(GetController());                                          
    MutantAIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject
    (this, &AChasingAI::OnAIMoveCompleted);

    PlayerCollisionDetection->OnComponentBeginOverlap.AddDynamic(this,
        &AChasingAI::OnPlayerDetectedOverlapBegin);

    PlayerCollisionDetection->OnComponentEndOverlap.AddDynamic(this,
        &AChasingAI::OnPlayerDetectedOverlapEnd);

    PlayerAttackCollisionDetection->OnComponentBeginOverlap.AddDynamic(this,
        &AChasingAI::OnPlayerAttackOverlapBegin);

    PlayerAttackCollisionDetection->OnComponentEndOverlap.AddDynamic(this,
        &AChasingAI::OnPlayerAttackOverlapEnd);

}


// Called every frame
void AChasingAI::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AChasingAI::OnAIMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)              // AI finishes a move
{
    if (!PlayerDetected)                                                                                    // If no player detected
    {                                                                                                       // Continue Patrolling
        MutantAIController->RandomPatrol();
    }

    else if (PlayerDetected && CanAttackPlayer)                                                                 // If player detected and within attacking distance
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Player Attacked")));     // Throw Message
    }
}

void AChasingAI::MoveToPlayer()                                                                                 // Moves to player charater using a reference to the character
{
    MutantAIController->MoveToLocation(PlayerREF->GetActorLocation(), StoppingDistance, true);
}

void AChasingAI::SeekPlayer()                                                                                   // When player seen, walk to it
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Player Found")));
    MoveToPlayer();
}

void AChasingAI::OnPlayerDetectedOverlapBegin(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    PlayerREF = Cast<AGAMCharacter>(OtherActor);                                                               // When player overlaps detection area 
    if (PlayerREF)
    {
        PlayerDetected = true;                                                                                  // Change player Detected bool to true and seek player (walk to it)
        SeekPlayer();
    }
}


void AChasingAI::OnPlayerDetectedOverlapEnd(UPrimitiveComponent* OverlappedComp,                                // When player leaves detection area
    AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    PlayerREF = Cast<AGAMCharacter>(OtherActor);
    if (PlayerREF)
    {
        PlayerDetected = false;                                                                                    // set detection to false
        MutantAIController->RandomPatrol();                                                                         // resume patrolling

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Player Lost")));
    }
}

void AChasingAI::OnPlayerAttackOverlapBegin(UPrimitiveComponent* OverlappedComp,                                   // when player enters attacking distance
    AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,                                      // set CanAttackPlayer to true
    bool bFromSweep, const FHitResult& SweepResult)
{

    PlayerREF = Cast<AGAMCharacter>(OtherActor);
    if (PlayerREF)
    {
        CanAttackPlayer = true;
        PlayerREF->ChangeHealth(-10.0f);
        PlayerREF->UpdateHealth();

    }
}

void AChasingAI::OnPlayerAttackOverlapEnd(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)                                       // when player leaves attacking distance
{
    PlayerREF = Cast<AGAMCharacter>(OtherActor);                                                                    // Can't attack player anymore
    if (PlayerREF)
    {
        CanAttackPlayer = false;                                                                                    // Walk towards player

        SeekPlayer();
    }
}

void AChasingAI::TakeDamage(float DamageAmount)
{
    Health -= DamageAmount;

    // Check if health drops below zero
    if (Health <= 0)
    {
        //Handle Death
        Destroy();
    }
}