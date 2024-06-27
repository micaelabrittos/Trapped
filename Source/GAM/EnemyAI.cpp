// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAI.h"
#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyAI::AEnemyAI()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemyAI::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}