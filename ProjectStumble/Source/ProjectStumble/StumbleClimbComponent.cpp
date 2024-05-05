// Fill out your copyright notice in the Description page of Project Settings.


#include "StumbleClimbComponent.h"
#include "ECustomMovement.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

void UStumbleClimbComponent::BeginPlay()
{
	Super::BeginPlay();

	UAnimInstance* AnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance();

	ClimbQueryParams.AddIgnoredActor(GetOwner());
}

void UStumbleClimbComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SweepAndStoreWallHits();
}

void UStumbleClimbComponent::SweepAndStoreWallHits()
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CollisionCapsuleRadius, CollisionCapsuleHalfHeight);

	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 20;

	// Avoid using the same Start/End location for a Sweep, as it doesn't trigger hits on Landscapes.
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();

	TArray<FHitResult> Hits;
	const bool HitWall = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity,
		ECC_WorldStatic, CollisionShape, ClimbQueryParams);

	if (HitWall) {
		CurrentWallHits = Hits; // Assign the hits if a wall was hit
	}
	else {
		CurrentWallHits.Reset(); // Reset if no wall was hit
	}

}

bool UStumbleClimbComponent::CanStartClimbing()
{
	for (FHitResult& Hit : CurrentWallHits)
	{
		const FVector HorizontalNormal = Hit.Normal.GetSafeNormal2D();

		const float HorizontalDot = FVector::DotProduct(UpdatedComponent->GetForwardVector(), -HorizontalNormal);
		const float VerticalDot = FVector::DotProduct(Hit.Normal, HorizontalNormal);

		const float HorizontalDegrees = FMath::RadiansToDegrees(FMath::Acos(HorizontalDot));

		const bool bIsCeiling = FMath::IsNearlyZero(VerticalDot);

		if (HorizontalDegrees <= MinHorizontalDegreesToStartClimbing &&
			!bIsCeiling && IsFacingSurface(VerticalDot))
		{
			return true;
		}
	}

	return false;
}

bool UStumbleClimbComponent::EyeHeightTrace(const float TraceDistance) const
{
	FHitResult UpperEdgeHit;

	const FVector Start = UpdatedComponent->GetComponentLocation() +
		(UpdatedComponent->GetUpVector() * GetCharacterOwner()->BaseEyeHeight);
	const FVector End = Start + (UpdatedComponent->GetForwardVector() * TraceDistance);

	return GetWorld()->LineTraceSingleByChannel(UpperEdgeHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}

bool UStumbleClimbComponent::IsFacingSurface(const float Steepness) const
{
	constexpr float BaseLength = 80;
	const float SteepnessMultiplier = 1 + (1 - Steepness) * 5;

	return EyeHeightTrace(BaseLength * SteepnessMultiplier);
}

void UStumbleClimbComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	if (bWantsToClimb)
	{
		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_Climbing);
	}

	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UStumbleClimbComponent::TryClimbing()
{
	if (CanStartClimbing())
	{
		bWantsToClimb = true;
	}
}

void UStumbleClimbComponent::CancelClimbing()
{
	bWantsToClimb = false;
}

bool UStumbleClimbComponent::IsClimbing() const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::CMOVE_Climbing;
}

FVector UStumbleClimbComponent::GetClimbSurfaceNormal() const
{
	if (CurrentWallHits.Num() > 0) {
		return CurrentWallHits[0].ImpactPoint;
	}
	else {
		return FVector::Zero();
	}

}