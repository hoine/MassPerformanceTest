// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshPathFollowProcessor.h"

#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "MassSimulationLOD.h"

UE_DISABLE_OPTIMIZATION

bool FNavMeshPathFragment::IsNear(const FVector& InPosition)
{
	return IsValid() && FVector::DistSquared(InPosition, GetCurrentDestinationLocation()) < 1000.0;
}

FVector FNavMeshPathFragment::GetCurrentDestinationLocation()
{
	if (CurrentPath.IsValidIndex(CurrentPathIndex))
	{
		return CurrentPath[CurrentPathIndex];
	}

	return FVector::ZeroVector;
}

void FNavMeshPathFragment::Reset()
{
	CurrentPath.Empty();
	CurrentPathIndex = 0;
	DestinationPosition = FVector::ZeroVector;
	bInProgress = false;
}

void FNavMeshPathFragment::Next()
{
	CurrentPathIndex++;
	if (!CurrentPath.IsValidIndex(CurrentPathIndex))
	{
		Reset();
	}
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
			if (!PathFragment.IsValid())
			{
				continue;
			}
			
			const FTransformFragment& TransformFragment = TransformFragments[EntityIndex];
			FMassMoveTargetFragment& MoveTarget = MoveTargetFragments[EntityIndex];
			const FMassMovementParameters& MovementParams = Context.GetConstSharedFragment<FMassMovementParameters>();
		
			FVector EntityPosition = TransformFragment.GetTransform().GetLocation();
			const FVector ToTarget = PathFragment.GetCurrentDestinationLocation() - EntityPosition;
			
			const float DeltaMovement = Context.GetDeltaTimeSeconds() * MovementParams.MaxSpeed;
			const float DistanceToTarget = ToTarget.Length();
			if (DistanceToTarget <= DeltaMovement)
			{
				PathFragment.Next();
				continue;
			}
			
			if (!PathFragment.bInProgress)
			{
				MoveTarget.CreateNewAction(EMassMovementAction::Move, *Context.GetWorld());
				PathFragment.bInProgress = true;
			}

			MoveTarget.Center = PathFragment.GetCurrentDestinationLocation();
			MoveTarget.Forward = ToTarget.GetSafeNormal();
			MoveTarget.DistanceToGoal = DistanceToTarget;
			MoveTarget.bOffBoundaries = false;
			MoveTarget.DesiredSpeed.Set(MovementParams.MaxSpeed);
		}
	});
}
UE_ENABLE_OPTIMIZATION