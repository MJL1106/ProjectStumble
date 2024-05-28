// Fill out your copyright notice in the Description page of Project Settings.


#include "StumbleCharacterbase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "StumbleClimbComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "StumblePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


constexpr int CVSphereCastPlayerView = 0;
constexpr int CVSphereCastActorTransform = 1;
constexpr int CVLineCastActorTransform = 2;

// Add cvars for debug
static TAutoConsoleVariable<int> CVarTraceMode(
	TEXT("Tantrum.Character.Debug.TraceMode"),
	0,
	TEXT("	0: Sphere cast PlayerView is used for direction/rotation (default).\n")
	TEXT("	1: Sphere cast using ActorTransform \n")
	TEXT("	2: Line cast using ActorTransform \n"),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarDisplayTrace(
	TEXT("Tantrum.Character.Debug.DisplayTrace"),
	false,
	TEXT("Display Trace"),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarDisplayThrowVelocity(
	TEXT("Tantrum.Character.Debug.DisplayThrowVelocity"),
	false,
	TEXT("Display Throw Velocity"),
	ECVF_Default);

// Sets default values
AStumbleCharacterbase::AStumbleCharacterbase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UStumbleClimbComponent>(ACharacter::CharacterMovementComponentName))
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 30.0f), FRotator(0.0f, 0.0f, 0.0f));
	CameraBoom->TargetArmLength = 400.0f;
	
	CameraBoom->bEnableCameraLag = false;
	CameraBoom->bEnableCameraRotationLag = false;

	CameraMain = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraMain"));
	CameraMain->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	DefaultCameraOffset = CameraBoom->GetRelativeLocation();
	DefaultTargetArmLength = CameraBoom->TargetArmLength;

	PrimaryActorTick.bCanEverTick = true;
	InterpSpeed = 5.0f;
	SlowInterpSpeed = 2.0f;
	TargetMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	MovementComponent = Cast<UStumbleClimbComponent>(GetCharacterMovement());

}

// Called when the game starts or when spawned
void AStumbleCharacterbase::BeginPlay()
{
	Super::BeginPlay();
	if (GetCharacterMovement())
	{
		MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	}

	if (!MovementComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("MovementComponent is not initialized on %s"), *GetName());
	}
	
}

// Called every frame
void AStumbleCharacterbase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SmoothCameraTransition(DeltaTime);

	if (!IsLocallyControlled())
	{
		return;
	}
	

	if (bIsAimingChar && IsCharacterNotMoving())
	{
		UpdateCharacterRotationToCamera(DeltaTime);
	}

	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	bool IsMoving = Velocity.SizeSquared() > KINDA_SMALL_NUMBER;

	// Changes the interp speed depending on player going from stop to moving or moving to stop
	float CurrentInterpSpeed = IsMoving ? InterpSpeed : SlowInterpSpeed;
	float NewTargetSpeed = 300.0f;

	if (bIsCrouching)
	{
		NewTargetSpeed = IsMoving ? TargetMaxWalkSpeed : 10.0f;
	}
	else if (!IsMoving)
	{
		NewTargetSpeed = 10.0f;
	}

	if (!bIsSprinting)
	{
		float PreviousSpeed = GetCharacterMovement()->MaxWalkSpeed;
		GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(PreviousSpeed, NewTargetSpeed, DeltaTime, CurrentInterpSpeed);
	}
}

bool AStumbleCharacterbase::IsCharacterNotMoving() {
	FVector Velocity = GetVelocity();

	return Velocity.SizeSquared() < KINDA_SMALL_NUMBER;
}

void AStumbleCharacterbase::SmoothCameraTransition(float DeltaTime)
{
	// Smoothly adjust the spring arm length
	float TargetArmLength = bIsCrouching ? 300.0f : 400.0f;
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetArmLength, DeltaTime, 5.0f);

	// Smoothly adjust the vertical position
	float TargetHeight = bIsCrouching ? CrouchingCameraHeight : StandingCameraHeight;
	FVector CurrentLocation = CameraBoom->GetRelativeLocation();
	CurrentLocation.Z = FMath::FInterpTo(CurrentLocation.Z, TargetHeight, DeltaTime, 5.0f);
	CameraBoom->SetRelativeLocation(CurrentLocation);
}

// Called to bind functionality to input
void AStumbleCharacterbase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AStumbleCharacterbase::IsCollidingWithWall() const
{
	FVector Start = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	FVector End = Start + ForwardVector * 50.0f; // Sets the distance check from player to wall

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		CollisionParams
	);

	if (bBlockingHit && HitResult.Normal.Z < 0.1f) // Checks that the player is on the ground
	{
		return true;
	}

	return false;
}

//Changes the rotation of the player 
void AStumbleCharacterbase::UpdateCharacterRotation()
{
	if (IsCollidingWithWall())
	{
		return;
	}

	FVector MovementDirection = GetVelocity();
	MovementDirection.Z = 0;

	if (!MovementDirection.IsNearlyZero())
	{
		FRotator TargetRotation = MovementDirection.Rotation();
		FRotator CurrentRotation = GetActorRotation();
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		float RotationSpeed = 10.0f;

		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed); //Interpolates the rotation speed of the character
		SetActorRotation(NewRotation);
	}
}

void AStumbleCharacterbase::Landed(const FHitResult& Hit)
{
	bIsJumping = false;
	Super::Landed(Hit);
	AStumblePlayerController* StumblePlayerController = GetController<AStumblePlayerController>();
	if (StumblePlayerController)
	{
		const float FallImpactSpeed = FMath::Abs(GetVelocity().Z);
		if (FallImpactSpeed < MinImpactSpeed)
		{
			return;
		}
		else
		{
			if (HeavyLandMontage && GetOwner())
			{
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

				if (AnimInstance && AnimInstance->Montage_IsPlaying(HeavyLandMontage))
				{
					return;
				}
				else
				{
					GetCharacterMovement()->DisableMovement();
					AnimInstance->Montage_Play(HeavyLandMontage);
					GetWorldTimerManager().SetTimer(UnfreezeTimerHandle, this, &AStumbleCharacterbase::EnableMovement, 1.25f, false);

				}


				FVector CharacterLocation = GetOwner()->GetActorLocation();
				UGameplayStatics::PlaySoundAtLocation(this, HeavyLandSound, CharacterLocation);
			}
		}

		const float DeltaImpact = MaxImpactSpeed - MinImpactSpeed;
		const float FallRatio = FMath::Clamp((FallImpactSpeed - MinImpactSpeed) / DeltaImpact, 0.0f, 1.0f);
		const bool bAffectSmall = FallRatio <= 0.5;
		const bool bAffectLarge = FallRatio > 0.5;

		StumblePlayerController->PlayDynamicForceFeedback(FallRatio, 0.5f, bAffectLarge, bAffectSmall, bAffectLarge, bAffectSmall);
	}
}


void AStumbleCharacterbase::EnableMovement()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}


void AStumbleCharacterbase::Jump()
{
	if (MovementComponent->IsClimbing())
	{
		MovementComponent->TryClimbDashing();
	}
	else
	{
		bIsJumping = true;
		Super::Jump();
	}
}

void AStumbleCharacterbase::MoveForward(float Value)
{

	if (Controller == nullptr || Value == 0.0f)
	{
		return;
	}

	FVector Direction;

	if (MovementComponent && MovementComponent->IsClimbing())
	{
		Direction = FVector::CrossProduct(MovementComponent->GetClimbSurfaceNormal(), -GetActorRightVector());
	}
	else
	{
		UpdateCharacterRotation();
		Direction = GetControlOrientationMatrix().GetUnitAxis(EAxis::X);
	}

	AddMovementInput(Direction, Value);
}

void AStumbleCharacterbase::MoveRight(float Value)
{

	if (Controller == nullptr || Value == 0.0f)
	{
		return;
	}

	FVector Direction;
	if (MovementComponent && MovementComponent->IsClimbing())
	{
		Direction = FVector::CrossProduct(MovementComponent->GetClimbSurfaceNormal(), GetActorUpVector());
	}
	else
	{
		UpdateCharacterRotation();
		Direction = GetControlOrientationMatrix().GetUnitAxis(EAxis::Y);
	}

	AddMovementInput(Direction, Value);
}

void AStumbleCharacterbase::AdjustCameraForAiming(bool bIsAiming)
{
	//if (bIsAiming)
	//{
	//	// Adjust camera settings for aiming
	//	CameraBoom->TargetArmLength = 300.0f; // Zoom in
	//}
	//else
	//{
	//	// Revert camera settings back to normal
	//	CameraBoom->TargetArmLength = DefaultTargetArmLength; // Revert zoom
	//	CameraBoom->SetRelativeLocation(DefaultCameraOffset); // Revert camera position
	//}
	bIsAimingChar = bIsAiming;

}

void AStumbleCharacterbase::UpdateCharacterRotationToCamera(float DeltaTime)
{
	FRotator CurrentRotation = GetActorRotation();
	FRotator TargetRotation = CameraMain->GetComponentRotation();
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;

	float IS = 8.0f;
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, IS);

	SetActorRotation(NewRotation);
}

FRotationMatrix AStumbleCharacterbase::GetControlOrientationMatrix() const
{
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	return FRotationMatrix(YawRotation);
}

void AStumbleCharacterbase::RequestSprint()
{
	if (bIsCrouching)
	{
		return;
	}
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed += SprintSpeed;
	ServerSprintStart();
}

void AStumbleCharacterbase::RequestStopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	ServerSprintEnd();
}

void AStumbleCharacterbase::StartCrouch()
{
	bIsCrouching = true;
	Crouch();
	TargetMaxWalkSpeed = CrouchSpeed;
}

void AStumbleCharacterbase::EndCrouch()
{
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0, 0, GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 2);

	//Stops the player from uncrouching if the object above is too low to stand
	FHitResult OutHit;
	bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(
		OutHit,
		Start,
		End,
		ECC_Visibility
	);

	if (!bBlockingHit)
	{
		bIsCrouching = false;
		UnCrouch();
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	}
}

void AStumbleCharacterbase::RequestGrabStart()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AnimInstance->Montage_IsPlaying(PickUpMontage))
	{
		return;
	}
	else
	{
		GetCharacterMovement()->DisableMovement();
		AnimInstance->Montage_Play(PickUpMontage);
		GetWorldTimerManager().SetTimer(UnfreezeTimerHandle, this, &AStumbleCharacterbase::EnableMovement, 1.85f, false);

	}
}

void AStumbleCharacterbase::RequestGrabStop()
{
	bIsGrabbing = false;
}

void AStumbleCharacterbase::TryClimbing()
{
	MovementComponent->TryClimbing();
}

void AStumbleCharacterbase::CancelClimb()
{
	MovementComponent->CancelClimbing();
}


void AStumbleCharacterbase::ServerSprintStart_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AStumbleCharacterbase::ServerSprintEnd_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AStumbleCharacterbase::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	// Ignore collisions otherwise the throwable object hits the player capsule and doesn't travel in the desired direction
	//if (ThrowableActor->GetRootComponent())
	//{
	//	UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(ThrowableActor->GetRootComponent());
	//	if (RootPrimitiveComponent)
	//	{
	//		RootPrimitiveComponent->IgnoreActorWhenMoving(this, true);
	//	}
	//}
	//const FVector& Direction = GetMesh()->GetSocketRotation(TEXT("ObjectAttach")).Vector() * -ThrowSpeed;
	//const FVector& Direction = GetActorForwardVector() * ThrowSpeed;
	//ThrowableActor->Launch(Direction);

	//if (CVarDisplayThrowVelocity->GetBool())
	//{
	//	const FVector& Start = GetMesh()->GetSocketLocation(TEXT("ObjectAttach"));
	//	DrawDebugLine(GetWorld(), Start, Start + Direction, FColor::Red, false, 5.0f);
	//}
	//ServerBeginThrow();
}

void AStumbleCharacterbase::OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{

}

void AStumbleCharacterbase::StartInteraction()
{
	bIsOpeningDoor = true;
	if (PlayOpeningDoorMontage()) {
		OnInteractionStart.Broadcast();
	}
	else
	{
		bIsOpeningDoor = false;
	}
}

void AStumbleCharacterbase::StopInteraction()
{
	OnInteractionCancel.Broadcast();
}


bool AStumbleCharacterbase::PlayOpeningDoorMontage()
{
	const float PlayRate = 1.0f;
	GetCharacterMovement()->DisableMovement();
	bool bPlayedSuccessfully = PlayAnimMontage(OpeningDoorMontage, PlayRate) > 0.0f;
	GetWorldTimerManager().SetTimer(UnfreezeTimerHandle, this, &AStumbleCharacterbase::EnableMovement, 1.85f, false);
	if (bPlayedSuccessfully)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (!BlendingOutDelegate.IsBound())
		{
			BlendingOutDelegate.BindUObject(this, &AStumbleCharacterbase::OnMontageBlendingOut);
		}
		AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, OpeningDoorMontage);

		AnimInstance->Montage_SetEndDelegate(BlendingOutDelegate, OpeningDoorMontage);
	}
	bIsOpeningDoor = false;
	return bPlayedSuccessfully;
}

void AStumbleCharacterbase::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{

}