// Fill out your copyright notice in the Description page of Project Settings.

#include "NavMeshPathSubsystem.h"
#include "NavFilters/NavigationQueryFilter.h"


void UNavMeshPathSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
}

void UNavMeshPathSubsystem::Deinitialize()
{
	NavSys.Reset();
	
	Super::Deinitialize();
}

bool UNavMeshPathSubsystem::FindPath(const FVector& StartLocation, const FVector& Destination, TArray<FVector>& OutPoints)
{
	if (NavSys == nullptr)
	{
		NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
		return false;
	}
	
	const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance();
	FPathFindingQuery Query(nullptr, *NavData, StartLocation, Destination, UNavigationQueryFilter::GetQueryFilter(*NavData, nullptr, nullptr));
	FNavAgentProperties Properties;
	Properties.AgentHeight = 170.f;
	Properties.AgentRadius = 60.f;
	
	Query.SetNavAgentProperties(Properties);
	const FPathFindingResult Result = NavSys->FindPathSync(Query, EPathFindingMode::Regular);
	if (Result.IsSuccessful() && !Result.IsPartial())
	{
		for (FNavPathPoint PathPoint : Result.Path->GetPathPoints())
		{
			if (PathPoint.Location != StartLocation)
			{
				OutPoints.Add(PathPoint.Location);
			}
		}
#if WITH_MASSGAMEPLAY_DEBUG

		FVector PreviousLocation = StartLocation;
		if (!OutPoints.IsEmpty())
		{
			PreviousLocation.Z = OutPoints[0].Z;
		}
		for (FNavPathPoint PathPoint : Result.Path->GetPathPoints())
		{
			UE_VLOG_SEGMENT_THICK(this, LogNavMeshNav, Log, PreviousLocation, PathPoint.Location, FColor::Red, 3, TEXT("NavMeshPath"));
			UE_VLOG_SPHERE(this, LogNavMeshNav, Log, PathPoint.Location, 10, FColor::Red, TEXT("NavMeshPath"));
			PreviousLocation = PathPoint.Location;
		}
		UE_VLOG_SPHERE(this, LogNavMeshNav, Log, Destination, 10, FColor::Yellow, TEXT("NavMeshPath"));
#endif

		return true;
	}
	
	return false;
}

FVector UNavMeshPathSubsystem::GetRandomPointInNavigableRadius(const FVector& StartLocation, const float Radius)
{
	if (NavSys == nullptr)
	{
		NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	}
	
	FNavLocation ResultLocation;
	NavSys->GetRandomPointInNavigableRadius(StartLocation, Radius, ResultLocation);
	return ResultLocation.Location;
}
