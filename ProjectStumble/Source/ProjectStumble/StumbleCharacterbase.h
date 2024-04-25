// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "StumbleCharacterbase.generated.h"

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
	float SprintSpeed = 1200.0f;

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

	float MaxWalkSpeed = 0.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	void RequestSprint();
	void RequestStopSprint();

	void StartCrouch();
	void EndCrouch();

	UFUNCTION(BlueprintPure)
	bool IsCrouching() const { return bIsCrouching; }

};
