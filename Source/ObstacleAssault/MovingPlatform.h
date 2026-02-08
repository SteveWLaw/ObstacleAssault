// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

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

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditAnywhere, Category = "Movement")
	TArray<FVector> Waypoints;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed{ 100.0f };

	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bShowPath{ true };

private:
	int CurrentWaypointIndex{ 0 };
	FVector StartLocation;

	void DrawPath();
};
