// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "NavMeshSimpleMovementProcessor.generated.h"

/**
 * 
 */
UCLASS()
class AIPERFORMANCECOMPARATION_API UNavMeshSimpleMovementProcessor: public UMassProcessor
{
	GENERATED_BODY()

public:
	UNavMeshSimpleMovementProcessor();
protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;
};