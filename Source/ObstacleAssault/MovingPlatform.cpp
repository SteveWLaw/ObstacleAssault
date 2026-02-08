// Fill out your copyright notice in the Description page of Project Settings.

#include "MovingPlatform.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
	
#if WITH_EDITOR
	if (bShowPath)
	{
		DrawPath();
	}
#endif
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Return early if no waypoints are set
	if (Waypoints.Num() == 0)
	{
		return;
	}

	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = Waypoints[CurrentWaypointIndex];

	// Calculate direction vector (normalized)
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();

	// Move towards target
	FVector NewLocation = CurrentLocation + Direction * Speed * DeltaTime;

	// Check if we've reached the target (within a small threshold)
	float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);
	if (DistanceToTarget <= Speed * DeltaTime)
	{
		// Snap to target and move to next waypoint
		NewLocation = TargetLocation;
		CurrentWaypointIndex = (CurrentWaypointIndex + 1) % Waypoints.Num();
	}

	SetActorLocation(NewLocation);
}

#if WITH_EDITOR
void AMovingPlatform::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	// Redraw path when properties change in editor
	if (PropertyChangedEvent.Property != nullptr)
	{
		FlushPersistentDebugLines(GetWorld());
		if (bShowPath)
		{
			DrawPath();
		}
	}
}
#endif

void AMovingPlatform::DrawPath()
{
	if (Waypoints.Num() == 0 || GetWorld() == nullptr)
	{
		return;
	}

	FVector CurrentPos = GetActorLocation();
	// Record current position before we move
	FVector PreviousLocation = CurrentPos;

	for (const FVector& Waypoint : Waypoints)
	{
		FVector WaypointWorldLocation = Waypoint;

		// Draw line
		DrawDebugLine(
			GetWorld(),
			PreviousLocation,
			WaypointWorldLocation,
			FColor::Cyan,
			true,
			-1.0f,
			0,
			3.0f
		);

		// Draw sphere at waypoint
		DrawDebugSphere(
			GetWorld(),
			WaypointWorldLocation,
			20.0f,
			12,
			FColor::Yellow,
			true,
			-1.0f
		);

		PreviousLocation = WaypointWorldLocation;
	}

	// Close the loop
	DrawDebugLine(
		GetWorld(),
		PreviousLocation,
		CurrentPos,
		FColor::Cyan,
		true,
		-1.0f,
		0,
		3.0f
	);
}

