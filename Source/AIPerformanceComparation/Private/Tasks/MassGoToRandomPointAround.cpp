// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/MassGoToRandomPointAround.h"

#include "NavMeshPathSubsystem.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

UE_DISABLE_OPTIMIZATION
bool FMassGoToRandomPointAround::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(NavMeshPathSubsystem);
	Linker.LinkExternalData(EntityTransformHandle);
	Linker.LinkExternalData(PathFragmentHandle);
	
	return true;
}

EStateTreeRunStatus FMassGoToRandomPointAround::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const UNavMeshPathSubsystem& PathSubsystem = Context.GetExternalData(NavMeshPathSubsystem);
	const FTransformFragment& TransformFragment = Context.GetExternalData(EntityTransformHandle);

	FVector NewRandomLocation = FVector::ZeroVector;
	if (PathSubsystem.GetRandomPointInNavigableRadius(TransformFragment.GetTransform().GetLocation(), InstanceData.Radius, NewRandomLocation))
	{
		FNavMeshPathFragment& PathFragment = Context.GetExternalData(PathFragmentHandle);
		const bool PathFounded = PathSubsystem.FindPath(TransformFragment.GetTransform().GetLocation(), NewRandomLocation, PathFragment);
		if (!PathFounded)
		{
			return EStateTreeRunStatus::Failed;
		}
	}
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FMassGoToRandomPointAround::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FNavMeshPathFragment& PathFragment = Context.GetExternalData(PathFragmentHandle);
	
	if (PathFragment.GetStatus() == ENavMeshPathFragmentStatus::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return EStateTreeRunStatus::Running;
}

UE_ENABLE_OPTIMIZATION
