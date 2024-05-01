// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TriggerBox.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
//#include "ObjectiveComponent.h"
#include "StumblePlayerController.h"
#include "StumbleCharacterbase.h"

constexpr float FLT_METERS(float meters) { return meters * 100.0f; }

static TAutoConsoleVariable<bool> CVarToggleDebugDoor(
	TEXT("Abstraction.DoorInteractionComponent.Debug"),
	false,
	TEXT("Toggle DoorInteractionComponent debug display."),
	ECVF_Default);


// Sets default values for this component's properties
UDoorInteractionComponent::UDoorInteractionComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
	DoorState = EDoorState::DS_Closed;

	CVarToggleDebugDoor.AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&UDoorInteractionComponent::OnDebugToggled));

}

void UDoorInteractionComponent::InteractionStart()
{
	Super::InteractionStart();
	UE_LOG(LogTemp, Error, TEXT("DoorInteractionComponent::InteractionStart"));
	if (InteractingActor)
	{
		UE_LOG(LogTemp, Error, TEXT("DoorInteractionComponent::Trying to call OpenDoor()"));
		OpenDoor();
	}
}

// Called when the game starts
void UDoorInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	StartRotation = GetOwner()->GetActorRotation();
	FinalRotation = GetOwner()->GetActorRotation() + DesiredRotation;
	//ensure TimeToRotate is greater than EPSILON
	CurrentRotationTime = 0.0f;

}

void UDoorInteractionComponent::OpenDoor()
{
	if (IsOpen() || DoorState == EDoorState::DS_Opening)
	{
		UE_LOG(LogTemp, Error, TEXT("DoorInteractionComponent::Door is already opening"));
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("DoorInteractionComponent::Set door state to opening"));
	DoorState = EDoorState::DS_Opening;
	CurrentRotationTime = 0.0f;


	InteractionRequested();
}

void UDoorInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (DoorState == EDoorState::DS_Opening) {
		CurrentRotationTime += DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
		const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
		GetOwner()->SetActorRotation(CurrentRotation);
		if (TimeRatio >= 1.0f) {
			OnDoorOpen();
		}
	}

	DebugDraw();
}

void UDoorInteractionComponent::OnDoorOpen()
{
	DoorState = EDoorState::DS_Open;
	//UObjectiveComponent* ObjectiveComponent = GetOwner()->FindComponentByClass<UObjectiveComponent>();
	//if (ObjectiveComponent)
	//{
	//	ObjectiveComponent->SetState(EObjectiveState::OS_Completed);
	//}
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("DoorOpened"));
	InteractionSuccess.Broadcast();
}

void UDoorInteractionComponent::OnDebugToggled(IConsoleVariable* Var)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDebugToggled"));
}

void UDoorInteractionComponent::DebugDraw()
{
	if (CVarToggleDebugDoor->GetBool())
	{
		FVector Offset(FLT_METERS(-0.75f), 0.0f, FLT_METERS(2.5f));
		FVector StartLocation = GetOwner()->GetActorLocation() + Offset;
		FString EnumAsString = TEXT("DoorState: ") + UEnum::GetDisplayValueAsText(DoorState).ToString();
		DrawDebugString(GetWorld(), Offset, EnumAsString, GetOwner(), FColor::Blue, 0.0f);
	}
}

void UDoorInteractionComponent::InteractionRequested()
{
	UE_LOG(LogTemp, Error, TEXT("DoorInteractionComponent::InteractionRequested"));
	AStumbleCharacterbase* ASCB = Cast<AStumbleCharacterbase>(InteractingActor);
	if (ASCB)
	{
		UE_LOG(LogTemp, Error, TEXT("DoorInteractionComponent::Valid ASPC"));
		ASCB->DoorOpenInteractionStarted(GetOwner());
	}
}

