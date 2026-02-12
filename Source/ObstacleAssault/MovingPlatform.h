// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

USTRUCT(BlueprintType)
struct FPlatformWaypoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint", 
		meta = (DisplayName = "Location (X, Y, Z)"))
	FVector Location{FVector::ZeroVector};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint",
		meta = (DisplayName = "Rotation During Transit (degrees)"))
	FRotator RotationDuringTransit{FRotator::ZeroRotator};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float Speed{100.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float WaitTimeAtWaypoint{0.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Waypoint", 
		meta = (DisplayName = "Time to Reach (seconds)"))
	float TimeToReach{0.0f};

	// Internal: Pre-calculated constant rotation speed for this segment
	FRotator RotationSpeedForSegment{FRotator::ZeroRotator};
};

UCLASS()
class OBSTACLEASSAULT_API AMovingPlatform : public AActor
{
	GENERATED_BODY()

public:
	AMovingPlatform();

	virtual void Tick(float DeltaTime) override;

	void GetTotalTimeOverAllWaypoints(float& OutTotalTime) const { OutTotalTime = TotalTimeOverAllWaypoints; }

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bShowPath{true};
#endif

	UPROPERTY(EditAnywhere, Category = "Movement")
	TArray<FPlatformWaypoint> Waypoints;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	float TimeToCompleteLoop{ 0.0f };

protected:
	virtual void BeginPlay() override;

private:
	int32 CurrentWaypointIndex{0};
	float DistanceTraveled{0.0f};
	float TotalTimeOverAllWaypoints{0.0f};
	float WaitTimeAtCurrentWaypoint{0.0f};
	
	void CalculateTotalTimeOverAllWaypoints();

#if WITH_EDITOR
	void DrawPath();
#endif
};
