// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Sound/SoundCue.h"
#include "StumbleCharacterbase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionCancel);

class UStumbleClimbComponent;

UCLASS()
class PROJECTSTUMBLE_API AStumbleCharacterbase : public ACharacter
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure)
	FORCEINLINE UStumbleClimbComponent* GetCustomCharacterMovement() const { return MovementComponent; }

public:
	// Sets default values for this character's properties
	AStumbleCharacterbase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraMain;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UStumbleClimbComponent* MovementComponent;

	bool PlayOpeningDoorMontage();

	bool IsCharacterNotMoving();

	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(Server, Reliable)
	void ServerSprintStart();

	UFUNCTION(Server, Reliable)
	void ServerSprintEnd();

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
	void OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);


	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float CrouchSpeed = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AimSpeed = 300.0;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float InterpSpeed;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SlowInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float StandingCameraHeight = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float AimingCameraHeight = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CrouchingCameraHeight = 42.0f;

	UPROPERTY(EditAnywhere, Category = "Fall Impact")
	float MinImpactSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Fall Impact")
	float MaxImpactSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Fall Impact")
	float MinStunTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Fall Impact")
	float MaxStunTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Fall Impact")
	USoundCue* HeavyLandSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* OpeningDoorMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* HeavyLandMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* PickUpMontage = nullptr;

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;

	FTimerHandle UnfreezeTimerHandle;


	bool bIsCrouching = false;

	bool bIsGrabbing = false;

	bool bIsJumping = false;

	bool bIsAimingChar = false;

	bool bIsOpeningDoor = false;

	bool bIsDroppingWeapon = false;

	bool bIsLookingOverRightShoulder = true;

	float MaxWalkSpeed = 0.0f;

	float TargetMaxWalkSpeed;

	FRotationMatrix GetControlOrientationMatrix() const;

	bool IsCollidingWithWall() const;

	FVector DefaultCameraOffset;
	float DefaultTargetArmLength;
	float MovementInputRight;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	void EnableMovement();


	void RequestSprint();
	void RequestStopSprint();

	void RequestGrabStart();
	void RequestGrabStop();

	void RequestDropWeaponStart();
	void RequestDropWeaponStop();

	void AdjustCameraForAiming(bool bIsAiming);


	void StartCrouch();
	void EndCrouch();

	void TryClimbing();
	void CancelClimb();

	void Jump();

	void UpdateCharacterRotation();

	void SmoothCameraTransition(float DeltaTime);

	void MoveForward(float AxisValue);



	void MoveRight(float AxisValue);

	UFUNCTION(BlueprintPure)
	bool IsCrouching() const { return bIsCrouching; }

	UFUNCTION(BlueprintPure)
	bool IsGrabbing() const { return bIsGrabbing; }

	UFUNCTION(BlueprintPure)
	bool IsAiming() const { return bIsAimingChar; }

	UFUNCTION(BlueprintPure)
	bool IsOpeningDoor() const { return bIsOpeningDoor; }

	UFUNCTION(BlueprintPure)
	bool IsJumping() const { return bIsJumping; }

	UFUNCTION(BlueprintPure)
	bool IsSprinting() const { return bIsSprinting; }

	UFUNCTION(BlueprintPure)
	bool IsDroppingWeapon() const { return bIsDroppingWeapon; }

	UFUNCTION(BlueprintImplementableEvent)
	void DoorOpenInteractionStarted(AActor* InteractableActor);

	UFUNCTION(BlueprintPure)
	UCameraComponent* GetCameraComponent() const { return CameraMain; }

	UFUNCTION(BlueprintPure)
	float GetMoveDirection() const { return MovementInputRight; }

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	bool bIsWeaponAttached = false;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	bool bIsSprinting = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon")
	bool bIsSwingingAxe = false;

	FOnInteractionStart OnInteractionStart;
	FOnInteractionCancel OnInteractionCancel;


};
