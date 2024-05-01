// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "StumbleCharacterbase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionCancel);

UCLASS()
class PROJECTSTUMBLE_API AStumbleCharacterbase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStumbleCharacterbase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



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
	float InterpSpeed;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SlowInterpSpeed;

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

	bool bIsSprinting = false;

	bool bIsCrouching = false;

	bool bIsGrabbing = false;

	bool bIsOpeningDoor = false;

	float MaxWalkSpeed = 0.0f;

	float TargetMaxWalkSpeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	void RequestSprint();
	void RequestStopSprint();

	void RequestGrabStart();
	void RequestGrabStop();

	void StartInteraction();
	void StopInteraction();

	void StartCrouch();
	void EndCrouch();

	UFUNCTION(BlueprintPure)
	bool IsCrouching() const { return bIsCrouching; }

	UFUNCTION(BlueprintPure)
	bool IsGrabbing() const { return bIsGrabbing; }

	UFUNCTION(BlueprintPure)
	bool IsOpeningDoor() const { return bIsOpeningDoor; }

	UFUNCTION(BlueprintImplementableEvent)
	void DoorOpenInteractionStarted(AActor* InteractableActor);

	FOnInteractionStart OnInteractionStart;
	FOnInteractionCancel OnInteractionCancel;

};
