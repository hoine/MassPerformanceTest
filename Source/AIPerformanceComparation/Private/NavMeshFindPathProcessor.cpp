// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshFindPathProcessor.h"

#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MassNavigationFragments.h"
#include "NavMeshPathFollowProcessor.h"
UE_DISABLE_OPTIMIZATION
UNavMeshFindPathProcessor::UNavMeshFindPathProcessor()
	: EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = (int32)(EProcessorExecutionFlags::Standalone | EProcessorExecutionFlags::Server);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Tasks);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
}

void UNavMeshFindPathProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
	MassNavSubsystem = UWorld::GetSubsystem<UNavMeshPathSubsystem>(Owner.GetWorld());
}

void UNavMeshFindPathProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FNavMeshPathFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
}

void UNavMeshFindPathProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context,  [this](FMassExecutionContext& Context)
	{
		const int32 NumEntities = Context.GetNumEntities();

		const TArrayView<FNavMeshPathFragment> PathFragments = Context.GetMutableFragmentView<FNavMeshPathFragment>();
		const TConstArrayView<FTransformFragment> TransformFragments = Context.GetFragmentView<FTransformFragment>();

		for (int32 EntityIndex = 0; EntityIndex < NumEntities; ++EntityIndex)
		{
			const FVector& StartLocation = TransformFragments[EntityIndex].GetTransform().GetLocation();
			FNavMeshPathFragment& PathFragment = PathFragments[EntityIndex];
			if (PathFragment.bCalculated == false)
			{
				TArray<FVector> OutPoints;
				if (MassNavSubsystem->FindPath(StartLocation, PathFragment.DestinationPosition, OutPoints))
				{
					PathFragment.CurrentPath = OutPoints;
					PathFragment.CurrentPathIndex = 0;
					PathFragment.bCalculated = true;
				}
			}
		}
		
	});
}
UE_ENABLE_OPTIMIZATION
