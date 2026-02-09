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
	const FPlatformWaypoint& CurrentWaypoint = Waypoints[CurrentWaypointIndex];
	FVector TargetLocation = CurrentWaypoint.Location;

	// Calculate direction vector (normalized)
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();

	// Move towards target using the waypoint's speed
	float CurrentSpeed = CurrentWaypoint.Speed;
	FVector NewLocation = CurrentLocation + Direction * CurrentSpeed * DeltaTime;

	// Check if we've reached the target (within a small threshold)
	float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);
	if (DistanceToTarget <= CurrentSpeed * DeltaTime)
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

void AMovingPlatform::DrawPath()
{
	if (Waypoints.Num() == 0 || GetWorld() == nullptr)
	{
		return;
	}

	FVector CurrentPos = GetActorLocation();
	FVector PreviousLocation = CurrentPos;

	for (int32 i = 0; i < Waypoints.Num(); i++)
	{
		const FPlatformWaypoint& Waypoint = Waypoints[i];
		FVector WaypointWorldLocation = Waypoint.Location;

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

		// Draw sphere at waypoint (color varies by speed)
		FColor SphereColor = FColor::MakeRedToGreenColorFromScalar(FMath::Clamp(Waypoint.Speed / 200.0f, 0.0f, 1.0f));
		DrawDebugSphere(
			GetWorld(),
			WaypointWorldLocation,
			20.0f,
			12,
			SphereColor,
			true,
			-1.0f
		);

		// Draw speed text at waypoint
		DrawDebugString(
			GetWorld(),
			WaypointWorldLocation + FVector(0.0f, 0.0f, 30.0f),
			FString::Printf(TEXT("%.0f u/s"), Waypoint.Speed),
			nullptr,
			FColor::White,
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
#endif

