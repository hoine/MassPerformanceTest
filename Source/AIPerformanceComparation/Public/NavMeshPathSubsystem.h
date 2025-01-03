// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassSubsystemBase.h"
#include "NavigationSystem.h"
#include "NavMeshPathFollowProcessor.h"
#include "NavMeshPathSubsystem.generated.h"

/**
 * 
 */

UCLASS()
class UNavMeshPathSubsystem : public UMassSubsystemBase
{
	GENERATED_BODY()

public:
	bool FindPath(const FVector& StartLocation, const FVector& Destination, TArray<FVector>& OutPoints) const;
	bool FindPath(const FVector& StartLocation, const FVector& Destination, FNavMeshPathFragment& OutPathFragment) const;
	
	bool GetRandomPointInNavigableRadius(const FVector& StartLocation, float Radius, FVector& OutRandomLocation) const;
};