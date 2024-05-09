// Fill out your copyright notice in the Description page of Project Settings.


#include "StumbleCharacterbase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "StumbleClimbComponent.h"
#include "GameFramework/Character.h"
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

	if (!IsLocallyControlled())
	{
		return;
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

// Called to bind functionality to input
void AStumbleCharacterbase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AStumbleCharacterbase::Landed(const FHitResult& Hit)
{
	bIsJumping = false;
	Super::Landed(Hit);
	AStumblePlayerController* StumblePlayerController = GetController<AStumblePlayerController>();
	if (StumblePlayerController)
	{
		const float FallImpactSpeed = FMath::Abs(GetVelocity().Z);
		UE_LOG(LogTemp, Warning, TEXT("Fall Impact Speed: %f"), FallImpactSpeed)
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
		Direction = GetControlOrientationMatrix().GetUnitAxis(EAxis::Y);
	}

	AddMovementInput(Direction, Value);
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
	TargetMaxWalkSpeed = CrouchSpeed;
}

void AStumbleCharacterbase::EndCrouch()
{
	bIsCrouching = false;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
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
		UE_LOG(LogTemp, Warning, TEXT("Grabbing::Pickup object"));
		GetCharacterMovement()->DisableMovement();
		AnimInstance->Montage_Play(PickUpMontage);
		GetWorldTimerManager().SetTimer(UnfreezeTimerHandle, this, &AStumbleCharacterbase::EnableMovement, 1.85f, false);

	}
	//bIsGrabbing = true;
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
	bool bPlayedSuccessfully = PlayAnimMontage(OpeningDoorMontage, PlayRate) > 0.0f;
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