// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassSubsystemBase.h"
#include "NavigationSystem.h"
#include "NavMeshPathSubsystem.generated.h"

/**
 * 
 */

UCLASS()
class UNavMeshPathSubsystem : public UMassSubsystemBase
{
	GENERATED_BODY()

public:
	bool FindPath(const FVector& StartLocation, const FVector& Destination, TArray<FVector>& OutPoints);
	
protected:
	// USubsystem BEGIN
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// USubsystem END

protected:
	UPROPERTY(Transient)
	TSoftObjectPtr<UNavigationSystemV1> NavSys;
};