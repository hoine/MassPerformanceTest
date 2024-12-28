// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassProcessor.h"

#include "NavMeshPathFollowProcessor.generated.h"

class UNavMeshPathSubsystem;

USTRUCT()
struct FNaveMeshSimpleMovementFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float MovementSpeed = 100.0f;
};

USTRUCT()
struct FNavMeshPathFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<FVector> CurrentPath;

	UPROPERTY(Transient)
	int32 CurrentPathIndex;

	UPROPERTY()
	FVector DestinationPosition = FVector::Zero();
	
	UPROPERTY(Transient)
	bool bInProgress = false;
	
	bool IsNear(const FVector& InPosition);
	
	FVector GetCurrentDestinationLocation();
	void Reset();

	bool IsValid() const { return (!CurrentPath.IsEmpty() && CurrentPathIndex < CurrentPath.Num()) || !DestinationPosition.IsZero(); }

	void Next();
};


UCLASS()
class AIPERFORMANCECOMPARATION_API UNavMeshMovementTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY(Category="Steering", EditAnywhere, meta=(EditInline))
	FNaveMeshSimpleMovementFragment MovementFragment;
};
/**
 * 
 */
UCLASS()
class AIPERFORMANCECOMPARATION_API UNavMeshPathFollowProcessor : public UMassProcessor
{
	GENERATED_BODY()
public:
	UNavMeshPathFollowProcessor();
	
protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;

	UPROPERTY()
	TObjectPtr<UNavMeshPathSubsystem> MassNavSubsystem;
};
