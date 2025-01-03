// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshSimpleMovementProcessor.h"

#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MassNavigationFragments.h"
#include "NavMeshPathFollowProcessor.h"

UNavMeshSimpleMovementProcessor::UNavMeshSimpleMovementProcessor()
	: EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = (int32)(EProcessorExecutionFlags::Standalone | EProcessorExecutionFlags::Server);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Tasks);
}

void UNavMeshSimpleMovementProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FNavMeshPathFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FNaveMeshSimpleMovementFragment>();
}
UE_DISABLE_OPTIMIZATION
void UNavMeshSimpleMovementProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context,  [this](FMassExecutionContext& Context)
	{
		const TArrayView<FNavMeshPathFragment> PathFragments = Context.GetMutableFragmentView<FNavMeshPathFragment>();
		const TArrayView<FTransformFragment> TransformFragments = Context.GetMutableFragmentView<FTransformFragment>();
		const FNaveMeshSimpleMovementFragment NaveMeshSimpleMovementFragment = Context.GetConstSharedFragment<FNaveMeshSimpleMovementFragment>();
		
		const float DeltaTime = Context.GetDeltaTimeSeconds();
		const int32 NumEntities = Context.GetNumEntities();
		
		for (int32 EntityIndex = 0; EntityIndex < NumEntities; ++EntityIndex)
		{
			FNavMeshPathFragment& PathFragment = PathFragments[EntityIndex];
			if (PathFragment.IsValid())
			{
				FTransform& EntityTransform = TransformFragments[EntityIndex].GetMutableTransform();
				//PathFragment.Update(Context.GetDeltaTimeSeconds(), EntityTransform.GetLocation(), NaveMeshSimpleMovementFragment.MovementSpeed, TODO);

				if (PathFragment.GetStatus() == ENavMeshPathFragmentStatus::Finished)
				{
					continue;
				}
				
				FVector ToTarget = PathFragment.GetTargetPosition() - EntityTransform.GetLocation();
				FVector Acceleration = ToTarget.GetSafeNormal() * NaveMeshSimpleMovementFragment.MovementSpeed * DeltaTime;
				
				EntityTransform.SetLocation(EntityTransform.GetLocation() + Acceleration);
			}
		}
	});
}
UE_ENABLE_OPTIMIZATION