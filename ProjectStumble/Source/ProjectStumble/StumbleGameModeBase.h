// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StumbleGameModeBase.generated.h"

class AController;
class AStumblePlayerController;

/**
 * 
 */
UCLASS()
class PROJECTSTUMBLE_API AStumbleGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AStumbleGameModeBase();

	virtual void BeginPlay() override;

private:

	void AttemptStartGame();
	void StartGame();
};
