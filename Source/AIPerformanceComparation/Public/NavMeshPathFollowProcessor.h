// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassMovementFragments.h"
#include "MassProcessor.h"

#include "NavMeshPathFollowProcessor.generated.h"

struct FMassMoveTargetFragment;
class UMassSignalSubsystem;
class UNavMeshPathSubsystem;

USTRUCT()
struct FNaveMeshSimpleMovementFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float MovementSpeed = 100.0f;
};

UENUM()
enum class ENavMeshPathFragmentStatus : uint8
{
	Invalid,
	PathFound,
	InProgress,
	Finished
};

USTRUCT()
struct FNavMeshPathFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	FVector DestinationPosition = FVector::Zero();
	
	FVector GetTargetPosition();

	bool IsValid() const { return Status != ENavMeshPathFragmentStatus::Invalid; }

	void SetNewPathPoints(const TArray<FVector>& NewPathPoints);

	ENavMeshPathFragmentStatus GetStatus() const { return Status; }

	void CreateNewMoveAction(FMassMoveTargetFragment& MoveTarget, const UWorld& World, const FVector& EntityPosition, const FMassMovementParameters& MovementParams);

	void Update(float DeltaSeconds, const FVector& EntityPosition, float MaxSpeed, FMassMoveTargetFragment& MoveTarget);

	int32 GetIndex() { return CurrentPathIndex; }
protected:
	UPROPERTY(Transient)
	TArray<FVector> PathPoints;
	
	UPROPERTY(Transient)
	int32 CurrentPathIndex = 0;

	ENavMeshPathFragmentStatus Status = ENavMeshPathFragmentStatus::Invalid;
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
	virtual void Initialize(UObject& Owner) override;
	
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;

	UPROPERTY()
	TObjectPtr<UNavMeshPathSubsystem> MassNavSubsystem;

	UPROPERTY(Transient)
	TObjectPtr<UMassSignalSubsystem> SignalSubsystem = nullptr;
};
