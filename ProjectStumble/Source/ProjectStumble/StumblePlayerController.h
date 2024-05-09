// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Sound/SoundCue.h"
#include "StumblePlayerController.generated.h"

class AStumbleGameModeBase;
class UStumbleClimbComponent;

/**
 * 
 */
UCLASS()
class PROJECTSTUMBLE_API AStumblePlayerController : public APlayerController
{
	GENERATED_BODY()
	UFUNCTION(BlueprintPure)
	FORCEINLINE UStumbleClimbComponent* GetCustomCharacterMovement() const { return MovementComponent; }

protected:
	virtual void SetupInputComponent() override;

	UPROPERTY()
	UStumbleClimbComponent* MovementComponent;

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

	void Climb();
	void CancelClimb();

	UPROPERTY(EditAnywhere, Category = "Look")
	float BaseLookUpRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Look")
	float BaseLookRightRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundCue* JumpSound = nullptr;
	
	FRotationMatrix GetControlOrientationMatrix() const;

public:
	virtual void BeginPlay() override;


	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

};
