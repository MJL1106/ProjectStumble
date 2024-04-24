// Fill out your copyright notice in the Description page of Project Settings.


#include "StumbleGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StumblePlayerController.h"

AStumbleGameModeBase::AStumbleGameModeBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AStumbleGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void AStumbleGameModeBase::AttemptStartGame()
{
	StartGame();
}

void AStumbleGameModeBase::StartGame()
{

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->PlayerState && !MustSpectate(PlayerController))
		{
			//cast and start?
			//this does not work on all controllers...
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}