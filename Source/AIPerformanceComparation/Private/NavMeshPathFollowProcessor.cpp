// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshPathFollowProcessor.h"

#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "MassSignalSubsystem.h"
#include "MassSimulationLOD.h"

UE_DISABLE_OPTIMIZATION

FVector FNavMeshPathFragment::GetTargetPosition()
{
	if (PathPoints.IsValidIndex(CurrentPathIndex))
	{
		return PathPoints[CurrentPathIndex];
	}

	return FVector::ZeroVector;
}

void FNavMeshPathFragment::SetNewPathPoints(const TArray<FVector>& NewPathPoints)
{
	if (NewPathPoints.Num() < 1)
	{
		Status = ENavMeshPathFragmentStatus::Invalid;
		return;
	}
	
	PathPoints.Reset();
	PathPoints = NewPathPoints;
	Status = ENavMeshPathFragmentStatus::PathFound;
	CurrentPathIndex = 1;
}

void FNavMeshPathFragment::CreateNewMoveAction(FMassMoveTargetFragment& MoveTarget, const UWorld& World, const FVector& EntityPosition, const FMassMovementParameters& MovementParams)
{
	const FVector& LastPosition = GetTargetPosition();
	const FVector ToTarget = LastPosition - EntityPosition;
	
	MoveTarget.CreateNewAction(EMassMovementAction::Move, World);
	
	MoveTarget.Center = LastPosition;
	MoveTarget.Forward = ToTarget.GetSafeNormal();
	MoveTarget.DistanceToGoal = ToTarget.Length();;
	MoveTarget.bOffBoundaries = false;
	MoveTarget.DesiredSpeed.Set(MovementParams.MaxSpeed);

	Status = ENavMeshPathFragmentStatus::InProgress;
}

void FNavMeshPathFragment::Update(const float DeltaSeconds, const FVector& EntityPosition, const float MaxSpeed, FMassMoveTargetFragment& MoveTarget)
{
	const FVector ToTarget = GetTargetPosition() - EntityPosition;
	const float DeltaMovement =  DeltaSeconds* MaxSpeed;
	const float DistanceToTarget = ToTarget.Length();
	if (DistanceToTarget <= DeltaMovement)
	{
		CurrentPathIndex++;
		if (!PathPoints.IsValidIndex(CurrentPathIndex))
		{
			Status = ENavMeshPathFragmentStatus::Finished;
		}
	}
	
	MoveTarget.Center = GetTargetPosition();
	MoveTarget.Forward = ToTarget.GetSafeNormal();
	MoveTarget.DistanceToGoal = ToTarget.Length();;
}

void UNavMeshMovementTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);
	
	BuildContext.AddFragment<FNavMeshPathFragment>();
	BuildContext.AddFragment<FMassSimulationVariableTickFragment>();

	const FConstSharedStruct MovementSharedFragment = EntityManager.GetOrCreateConstSharedFragment(MovementFragment);
	BuildContext.AddConstSharedFragment(MovementSharedFragment);
}

UNavMeshPathFollowProcessor::UNavMeshPathFollowProcessor()
	: EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = (int32)(EProcessorExecutionFlags::Standalone | EProcessorExecutionFlags::Server);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Tasks);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
}

void UNavMeshPathFollowProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
	SignalSubsystem = UWorld::GetSubsystem<UMassSignalSubsystem>(Owner.GetWorld());
}

void UNavMeshPathFollowProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FNavMeshPathFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	
	EntityQuery.AddConstSharedRequirement<FMassMovementParameters>(EMassFragmentPresence::All);
}

void UNavMeshPathFollowProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context,  [this](FMassExecutionContext& Context)
	{
		const int32 NumEntities = Context.GetNumEntities();

		const TArrayView<FNavMeshPathFragment>& PathFragments = Context.GetMutableFragmentView<FNavMeshPathFragment>();
		const TArrayView<FMassMoveTargetFragment>& MoveTargetFragments = Context.GetMutableFragmentView<FMassMoveTargetFragment>();
		const TConstArrayView<FTransformFragment>& TransformFragments = Context.GetFragmentView<FTransformFragment>();

		for (int32 EntityIndex = 0; EntityIndex < NumEntities; ++EntityIndex)
		{
			FNavMeshPathFragment& PathFragment = PathFragments[EntityIndex];
			if (!PathFragment.IsValid() || PathFragment.GetStatus() == ENavMeshPathFragmentStatus::Finished)
			{
				continue;
			}
			
			const FTransformFragment& TransformFragment = TransformFragments[EntityIndex];
			FMassMoveTargetFragment& MoveTarget = MoveTargetFragments[EntityIndex];
			const FMassMovementParameters& MovementParams = Context.GetConstSharedFragment<FMassMovementParameters>();
		
			FVector EntityPosition = TransformFragment.GetTransform().GetLocation();
			
			if (PathFragment.GetStatus() == ENavMeshPathFragmentStatus::PathFound)
			{
				PathFragment.CreateNewMoveAction(MoveTarget, *Context.GetWorld(), EntityPosition, MovementParams);
			}
			else if (PathFragment.GetStatus() == ENavMeshPathFragmentStatus::InProgress)
			{
				PathFragment.Update(Context.GetDeltaTimeSeconds(), EntityPosition, MovementParams.MaxSpeed, MoveTarget);

				if (PathFragment.GetStatus() == ENavMeshPathFragmentStatus::Finished)
				{
					const FMassEntityHandle Entity = Context.GetEntity(EntityIndex);
					SignalSubsystem->SignalEntity("MyCustomName", Entity);
					MoveTarget.CreateNewAction(EMassMovementAction::Stand,  *Context.GetWorld());
				}
			}
		}
	});
}
UE_ENABLE_OPTIMIZATION