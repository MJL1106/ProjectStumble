// Fill out your copyright notice in the Description page of Project Settings.


#include "StumblePlayerController.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "StumbleCharacterbase.h"
#include "Components/InputComponent.h"


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

		InputComponent->BindAction("Drop", EInputEvent::IE_Pressed, this, &AStumblePlayerController::RequestDropWeaponStart);
		InputComponent->BindAction("Drop", EInputEvent::IE_Released, this, &AStumblePlayerController::RequestDropWeaponStop);

		InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AStumblePlayerController::RequestAimStart);
		InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AStumblePlayerController::RequestAimStop);

		InputComponent->BindAction("Climb", IE_Pressed, this, &AStumblePlayerController::Climb);
		InputComponent->BindAction("Cancel Climb", IE_Pressed, this, &AStumblePlayerController::CancelClimb);

		InputComponent->BindAxis("MoveForward", this, &AStumblePlayerController::RequestMoveForward);
		InputComponent->BindAxis("MoveRight", this, &AStumblePlayerController::RequestMoveRight);
		InputComponent->BindAxis("LookUp", this, &AStumblePlayerController::RequestLookUp);
		InputComponent->BindAxis("LookRight", this, &AStumblePlayerController::RequestLookRight);


		//InputComponent->BindAction(TEXT("PullObject"), EInputEvent::IE_Pressed, this, &AStumblePlayerController::RequestPullObjectStart);
		//InputComponent->BindAction(TEXT("PullObject"), EInputEvent::IE_Released, this, &AStumblePlayerController::RequestPullObjectStop);
		//InputComponent->BindAxis(TEXT("ThrowObject"), this, &AStumblePlayerController::RequestThrowObject);

		bIsAiming = false;
	}
}


void AStumblePlayerController::RequestJump()
{

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

void AStumblePlayerController::RequestDropWeaponStart()
{
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->RequestDropWeaponStart();
	}
}

void AStumblePlayerController::RequestDropWeaponStop()
{
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->RequestDropWeaponStop();
	}
}

void AStumblePlayerController::RequestMoveForward(float AxisValue)
{
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->MoveForward(AxisValue);
	}
}

void AStumblePlayerController::RequestMoveRight(float AxisValue)
{
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->MoveRight(AxisValue);
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

void AStumblePlayerController::Climb()
{
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->TryClimbing();
	}
}

void AStumblePlayerController::CancelClimb()
{
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->CancelClimb();
	}
}

void AStumblePlayerController::RequestAimStart()
{
	bIsAiming = true;
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->AdjustCameraForAiming(true);
	}
}

void AStumblePlayerController::RequestAimStop()
{
	bIsAiming = false;
	if (AStumbleCharacterbase* StumbleCharacterbase = Cast<AStumbleCharacterbase>(GetCharacter()))
	{
		StumbleCharacterbase->AdjustCameraForAiming(false);
	}
}