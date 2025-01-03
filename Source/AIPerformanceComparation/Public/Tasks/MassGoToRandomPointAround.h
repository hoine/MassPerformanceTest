// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassCommonFragments.h"
#include "MassStateTreeTypes.h"
#include "NavMeshPathFollowProcessor.h"
#include "MassGoToRandomPointAround.generated.h"


class UNavMeshPathSubsystem;

USTRUCT()
struct FMassGoToRandomPointAroundData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Input)
	int32 Radius = 10.f;
};


/**
 * 
 */
USTRUCT()
struct AIPERFORMANCECOMPARATION_API FMassGoToRandomPointAround : public FMassStateTreeTaskBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FMassGoToRandomPointAroundData;

protected:
	
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
	virtual bool Link(FStateTreeLinker& Linker) override;

protected:
	TStateTreeExternalDataHandle<UNavMeshPathSubsystem> NavMeshPathSubsystem;
	TStateTreeExternalDataHandle<FTransformFragment> EntityTransformHandle;
	TStateTreeExternalDataHandle<FNavMeshPathFragment> PathFragmentHandle;
	
};
