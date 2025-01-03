// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassStateTreeProcessors.h"
#include "MyMassStateTreeProcessor.generated.h"

/**
 * 
 */
UCLASS()
class AIPERFORMANCECOMPARATION_API UMyMassStateTreeProcessor : public UMassStateTreeProcessor
{
	GENERATED_BODY()

protected:
	virtual void Initialize(UObject& Owner) override;
};
