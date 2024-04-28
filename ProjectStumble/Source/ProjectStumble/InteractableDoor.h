// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Components/AudioComponent.h"
#include "InteractableDoor.generated.h"

class UDoorInteractionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorOpen);
/**
 * 
 */
UCLASS()
class PROJECTSTUMBLE_API AInteractableDoor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AInteractableDoor();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OpenDoor();

	float InteractionTime = 5.0f;

	UPROPERTY(BlueprintAssignable, Category = "Door Interaction")
	FOnDoorOpen OnDoorOpen;

protected:
	UFUNCTION()
	void OnInteractionSuccess();

	UPROPERTY(EditAnywhere, NoClear)
	UDoorInteractionComponent* DoorInteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* DoorOpenSound;

};
