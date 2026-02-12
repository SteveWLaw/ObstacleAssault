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
	
	// Calculate times on startup
	CalculateTotalTimeOverAllWaypoints();
	TimeToCompleteLoop = TotalTimeOverAllWaypoints;

	SetActorLocation(Waypoints[0].Location);
	SetActorRotation(FRotator::ZeroRotator);
	
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

	if (Waypoints.Num() == 0) return;

	if (WaitTimeAtCurrentWaypoint > 0.0f)
	{
		WaitTimeAtCurrentWaypoint -= DeltaTime;
		return;
	}

	FVector CurrentLocation = GetActorLocation();
	FRotator CurrentRotation = GetActorRotation();
	
	// Get previous waypoint index (the one we're traveling FROM)
	int32 PreviousWaypointIndex = (CurrentWaypointIndex == 0) ? Waypoints.Num() - 1 : CurrentWaypointIndex - 1;
	const FPlatformWaypoint& PreviousWaypoint = Waypoints[PreviousWaypointIndex];
	const FPlatformWaypoint& CurrentWaypoint = Waypoints[CurrentWaypointIndex];
	
	FVector TargetLocation = CurrentWaypoint.Location;

	// Calculate movement - use PREVIOUS waypoint's speed (the one we're leaving from)
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	float CurrentSpeed = PreviousWaypoint.Speed;
	float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);
	
	// Move towards target
	FVector NewLocation = CurrentLocation + Direction * CurrentSpeed * DeltaTime;

	// Apply rotation based on pre-calculated speed from PREVIOUS waypoint
	FRotator RotationDelta = PreviousWaypoint.RotationSpeedForSegment * DeltaTime;
	FRotator NewRotation = CurrentRotation + RotationDelta;

	// Check if reached target
	if (DistanceToTarget <= CurrentSpeed * DeltaTime)
	{
		NewLocation = TargetLocation;
		CurrentWaypointIndex = (CurrentWaypointIndex + 1) % Waypoints.Num();
		WaitTimeAtCurrentWaypoint = CurrentWaypoint.WaitTimeAtWaypoint;
	}

	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);
}

#if WITH_EDITOR
void AMovingPlatform::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.Property != nullptr)
	{
		// Recalculate when waypoints change
		CalculateTotalTimeOverAllWaypoints();
		
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

void AMovingPlatform::CalculateTotalTimeOverAllWaypoints()
{
	TotalTimeOverAllWaypoints = 0.0f;
	
	if (Waypoints.Num() < 2)
	{
		return;
	}

	float CumulativeTime = 0.0f;

	// Calculate time for each segment and store cumulative time
	for (int32 i = 0; i < Waypoints.Num(); i++)
	{
		int32 NextIndex = (i + 1) % Waypoints.Num();
		
		FVector CurrentPos = Waypoints[i].Location;
		FVector NextPos = Waypoints[NextIndex].Location;
		float Speed = Waypoints[i].Speed;  // Changed from NextIndex to i

		if (Speed > 0.0f)
		{
			float Distance = FVector::Dist(CurrentPos, NextPos);
			float TimeForSegment = Distance / Speed;
			CumulativeTime += TimeForSegment;
			
			// Store cumulative time at the destination waypoint
			Waypoints[NextIndex].TimeToReach = CumulativeTime;
			
			// Store rotation speed AT THE CURRENT WAYPOINT
			// (because we use it when LEAVING this waypoint, heading TO the next)
			Waypoints[i].RotationSpeedForSegment.Pitch = 
				Waypoints[NextIndex].RotationDuringTransit.Pitch / TimeForSegment;
			Waypoints[i].RotationSpeedForSegment.Yaw = 
				Waypoints[NextIndex].RotationDuringTransit.Yaw / TimeForSegment;
			Waypoints[i].RotationSpeedForSegment.Roll = 
				Waypoints[NextIndex].RotationDuringTransit.Roll / TimeForSegment;
		}
		else
		{
			Waypoints[i].RotationSpeedForSegment = FRotator::ZeroRotator;
		}
		
		if (Waypoints[NextIndex].WaitTimeAtWaypoint > 0.0f)
		{
			CumulativeTime += Waypoints[NextIndex].WaitTimeAtWaypoint;
		}
	}

	TotalTimeOverAllWaypoints = CumulativeTime;
}