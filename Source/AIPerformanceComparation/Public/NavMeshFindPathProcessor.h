// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "NavMeshPathSubsystem.h"
#include "NavMeshFindPathProcessor.generated.h"

/**
 * 
 */
UCLASS()
class AIPERFORMANCECOMPARATION_API UNavMeshFindPathProcessor : public UMassProcessor
{
	GENERATED_BODY()
public:
	UNavMeshFindPathProcessor();
	
protected:
	virtual void Initialize(UObject& Owner) override;
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;

	UPROPERTY()
	TObjectPtr<UNavMeshPathSubsystem> MassNavSubsystem;
};