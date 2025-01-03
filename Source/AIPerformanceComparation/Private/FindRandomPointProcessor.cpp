// Fill out your copyright notice in the Description page of Project Settings.


#include "FindRandomPointProcessor.h"

#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassExecutionContext.h"
#include "MassNavigationFragments.h"
#include "NavMeshPathFollowProcessor.h"
#include "NavMeshPathSubsystem.h"


DECLARE_LOG_CATEGORY_EXTERN(LogNavMeshNav, Log, All)

DEFINE_LOG_CATEGORY(LogNavMeshNav);
UE_DISABLE_OPTIMIZATION

void UFindRandomPointTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct RandomPointSharedFragment = EntityManager.GetOrCreateConstSharedFragment(RandomPointFragment);
	BuildContext.AddConstSharedFragment(RandomPointSharedFragment);
	
	BuildContext.AddFragment<FNavMeshPathFragment>();
}

UFindRandomPointProcessor::UFindRandomPointProcessor()
	: EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = (int32)(EProcessorExecutionFlags::Standalone | EProcessorExecutionFlags::Server);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Tasks);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
}

void UFindRandomPointProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
	MassNavSubsystem = UWorld::GetSubsystem<UNavMeshPathSubsystem>(Owner.GetWorld());
}

void UFindRandomPointProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FNavMeshPathFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FFindRandomPointFragment>();
	EntityQuery.AddTagRequirement<FMassFindRandomTag>(EMassFragmentPresence::All);
}

void UFindRandomPointProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context,  [this](FMassExecutionContext& Context)
	{
		const TConstArrayView<FTransformFragment> TransformFragments = Context.GetFragmentView<FTransformFragment>();
		const TArrayView<FNavMeshPathFragment> PathFragments = Context.GetMutableFragmentView<FNavMeshPathFragment>();
		const FFindRandomPointFragment& RandomPointFragment = Context.GetConstSharedFragment<FFindRandomPointFragment>();
		
		const int32 NumEntities = Context.GetNumEntities();
		for (int32 EntityIndex = 0; EntityIndex < NumEntities; ++EntityIndex)
		{
			FNavMeshPathFragment& NavMeshPathFragment = PathFragments[EntityIndex];
			if (NavMeshPathFragment.DestinationPosition.IsZero())
			{
				const FVector& EntityPosition = TransformFragments[EntityIndex].GetTransform().GetLocation();
				MassNavSubsystem->GetRandomPointInNavigableRadius(EntityPosition, RandomPointFragment.Radius, NavMeshPathFragment.DestinationPosition);
				UE_VLOG_SPHERE(this, LogNavMeshNav, Log, NavMeshPathFragment.DestinationPosition, 10, FColor::Red, TEXT("NavMeshRandomPoint"));
			}
		}
	});
}
UE_ENABLE_OPTIMIZATION