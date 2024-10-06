// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "FindRandomPointProcessor.generated.h"

/**
 * 
 */
UCLASS()
class AIPERFORMANCECOMPARATION_API UFindRandomPointProcessor: public UMassProcessor
{
	GENERATED_BODY()

public:
	UFindRandomPointProcessor();
	
protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;
};