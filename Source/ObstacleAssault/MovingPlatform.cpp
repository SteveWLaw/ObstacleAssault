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

	if (Waypoints.Num() == 0)
	{
		return;
	}

	FVector CurrentLocation = GetActorLocation();
	FRotator CurrentRotation = GetActorRotation();
	const FPlatformWaypoint& CurrentWaypoint = Waypoints[CurrentWaypointIndex];
	FVector TargetLocation = CurrentWaypoint.Location;
	FRotator TargetRotation = CurrentWaypoint.Rotation;

	// Calculate movement
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	float CurrentSpeed = CurrentWaypoint.Speed;
	float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);
	
	// Calculate time to reach target
	float TimeToReachTarget = (CurrentSpeed > 0.0f) ? (DistanceToTarget / CurrentSpeed) : 0.0f;
	
	// Calculate rotation speed needed to complete rotation in same time as movement
	float RotationSpeed = (TimeToReachTarget > 0.0f) ? (1.0f / TimeToReachTarget) : 10.0f;
	
	// Move towards target
	FVector NewLocation = CurrentLocation + Direction * CurrentSpeed * DeltaTime;

	// Smoothly interpolate rotation to match movement timing
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);

	// Check if reached target
	if (DistanceToTarget <= CurrentSpeed * DeltaTime)
	{
		NewLocation = TargetLocation;
		NewRotation = TargetRotation;  // Snap to final rotation
		CurrentWaypointIndex = (CurrentWaypointIndex + 1) % Waypoints.Num();
	}

	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);
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
