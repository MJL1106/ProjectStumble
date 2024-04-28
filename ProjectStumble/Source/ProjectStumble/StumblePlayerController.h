// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Sound/SoundCue.h"
#include "StumblePlayerController.generated.h"

class AStumbleGameModeBase;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionCancel);

/**
 * 
 */
UCLASS()
class PROJECTSTUMBLE_API AStumblePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;

	void RequestJump();
	void RequestStopJump();

	void RequestGrabStart();
	void RequestGrabStop();

	void StartInteraction();
	void StopInteraction();

	void RequestCrouchStart();
	void RequestCrouchEnd();

	void RequestSprint();
	void RequestStopSprint();

	void RequestMoveForward(float AxisValue);
	void RequestMoveRight(float AxisValue);
	void RequestLookUp(float AxisValue);
	void RequestLookRight(float AxisValue);

	UPROPERTY(EditAnywhere, Category = "Look")
	float BaseLookUpRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Look")
	float BaseLookRightRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundCue* JumpSound = nullptr;
	

public:
	virtual void BeginPlay() override;


	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION(BlueprintImplementableEvent)
	void DoorOpenInteractionStarted(AActor* InteractableActor);

	FOnInteractionStart OnInteractionStart;
	FOnInteractionCancel OnInteractionCancel;
};
