// Fill out your copyright notice in the Description page of Project Settings.


#include "StumblePlayerController.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "StumbleCharacterbase.h"


void AStumblePlayerController::BeginPlay()
{
	Super::BeginPlay();
	//StumbleGameState = GetWorld()->GetGameState<AStumbleGameStateBase>();

}

void AStumblePlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	UE_LOG(LogTemp, Warning, TEXT("OnPossess: %s"), *GetName());
}

void AStumblePlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	UE_LOG(LogTemp, Warning, TEXT("OnUnPossess: %s"), *GetName());
}


void AStumblePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AStumblePlayerController::RequestJump);
		InputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &AStumblePlayerController::RequestStopJump);

		InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AStumblePlayerController::RequestCrouchStart);
		InputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &AStumblePlayerController::RequestCrouchEnd);
		InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AStumblePlayerController::RequestSprint);
		InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AStumblePlayerController::RequestStopSprint);

		InputComponent->BindAction("Grab", EInputEvent::IE_Pressed, this, &AStumblePlayerController::RequestGrabStart);
		InputComponent->BindAction("Grab", EInputEvent::IE_Released, this, &AStumblePlayerController::RequestGrabStop);

		InputComponent->BindAxis("MoveForward", this, &AStumblePlayerController::RequestMoveForward);
		InputComponent->BindAxis("MoveRight", this, &AStumblePlayerController::RequestMoveRight);
		InputComponent->BindAxis("LookUp", this, &AStumblePlayerController::RequestLookUp);
		InputComponent->BindAxis("LookRight", this, &AStumblePlayerController::RequestLookRight);


		//InputComponent->BindAction(TEXT("PullObject"), EInputEvent::IE_Pressed, this, &AStumblePlayerController::RequestPullObjectStart);
		//InputComponent->BindAction(TEXT("PullObject"), EInputEvent::IE_Released, this, &AStumblePlayerController::RequestPullObjectStop);
		//InputComponent->BindAxis(TEXT("ThrowObject"), this, &AStumblePlayerController::RequestThrowObject);
	}
}


void AStumblePlayerController::RequestJump()
{
	//if (!CanProcessRequest())
	//{
	//	return;
	//}

	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->Jump();

		//SoundCue Triggers
		if (JumpSound && StumbleCharacterbase->GetCharacterMovement()->IsMovingOnGround())
		{
			FVector CharacterLocation = StumbleCharacterbase->GetActorLocation();
			UGameplayStatics::PlaySoundAtLocation(this, JumpSound, CharacterLocation);
		}
	}
}

void AStumblePlayerController::RequestStopJump()
{
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->StopJumping();
	}
}

void AStumblePlayerController::RequestCrouchStart()
{
	AStumbleCharacterbase* StumbleCharacterBase = Cast<AStumbleCharacterbase>(GetCharacter());
	if (!StumbleCharacterBase || !StumbleCharacterBase->GetCharacterMovement()->IsMovingOnGround())
	{
		return;
	}

	StumbleCharacterBase->StartCrouch();
}

void AStumblePlayerController::RequestCrouchEnd()
{
	if (AStumbleCharacterbase* StumbleCharacterBase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterBase->EndCrouch();
	}
}

void AStumblePlayerController::RequestSprint()
{
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->RequestSprint();
	}
}

void AStumblePlayerController::RequestStopSprint()
{
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->RequestStopSprint();
	}
}

void AStumblePlayerController::RequestGrabStart()
{
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->RequestGrabStart();
	}
}

void AStumblePlayerController::RequestGrabStop()
{
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->RequestGrabStop();
	}
}

void AStumblePlayerController::RequestMoveForward(float AxisValue)
{

	if (AxisValue != 0.f)
	{
		FRotator const ControlSpaceRot = GetControlRotation();
		GetPawn()->AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), AxisValue);
	}
}

void AStumblePlayerController::RequestMoveRight(float AxisValue)
{

	if (AxisValue != 0.f)
	{
		FRotator const ControlSpaceRot = GetControlRotation();
		GetPawn()->AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y), AxisValue);
	}
}

void AStumblePlayerController::RequestLookUp(float AxisValue)
{
	AddPitchInput(AxisValue * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AStumblePlayerController::RequestLookRight(float AxisValue)
{
	AddYawInput(AxisValue * BaseLookRightRate * GetWorld()->GetDeltaSeconds());
}