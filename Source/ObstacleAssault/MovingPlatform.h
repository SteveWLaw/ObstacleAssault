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
		meta = (DisplayName = "Rotation (Pitch, Yaw, Roll)"))
	FRotator Rotation{FRotator::ZeroRotator};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float Speed{100.0f};
};

UCLASS()
class OBSTACLEASSAULT_API AMovingPlatform : public AActor
{
	GENERATED_BODY()

public:
	AMovingPlatform();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bShowPath{true};
#endif

	UPROPERTY(EditAnywhere, Category = "Movement")
	TArray<FPlatformWaypoint> Waypoints;

private:
	int CurrentWaypointIndex{0};

#if WITH_EDITOR
	void DrawPath();
#endif
};
