// Fill out your copyright notice in the Description page of Project Settings.

#include "NavMeshPathSubsystem.h"
#include "NavFilters/NavigationQueryFilter.h"

bool UNavMeshPathSubsystem::FindPath(const FVector& StartLocation, const FVector& Destination, TArray<FVector>& OutPoints) const
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem == nullptr)
	{
		return false;
	}
	
	const ANavigationData* NavData = NavSystem->GetDefaultNavDataInstance();
	FPathFindingQuery Query(nullptr, *NavData, StartLocation, Destination, UNavigationQueryFilter::GetQueryFilter(*NavData, nullptr, nullptr));
	FNavAgentProperties Properties;
	Properties.AgentHeight = 170.f;
	Properties.AgentRadius = 60.f;
	
	Query.SetNavAgentProperties(Properties);
	const FPathFindingResult Result = NavSystem->FindPathSync(Query, EPathFindingMode::Regular);
#if WITH_MASSGAMEPLAY_DEBUG
	if (!Result.IsSuccessful())
	{
		UE_VLOG_SPHERE(this, "LogNavMeshNav", Log, StartLocation, 10, FColor::Red, TEXT(""));
		UE_VLOG_SPHERE(this, "LogNavMeshNav", Log, Destination, 10, FColor::Red, TEXT("Can't find path to this point"));
		UE_VLOG_SEGMENT_THICK(this, "LogNavMeshNav", Log, StartLocation, Destination, FColor::Red, 3, TEXT(""));
	}
#endif
	
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
			FString LocationCoords = PathPoint.Location.ToString();
			UE_VLOG_SEGMENT_THICK(this, LogNavMeshNav, Log, PreviousLocation, PathPoint.Location, FColor::Blue, 3, TEXT(""));
			UE_VLOG_SPHERE(this, LogNavMeshNav, Log, PathPoint.Location, 10, FColor::Blue, TEXT("%s"), *LocationCoords);
			PreviousLocation = PathPoint.Location;
		}
		UE_VLOG_SPHERE(this, LogNavMeshNav, Log, Destination, 10, FColor::Yellow, TEXT(""));
#endif

		return true;
	}
	
	return false;
}

bool UNavMeshPathSubsystem::FindPath(const FVector& StartLocation, const FVector& Destination, FNavMeshPathFragment& OutPathFragment) const
{
	TArray<FVector> OutPoints;
	bool Result = FindPath(StartLocation, Destination, OutPoints);
	OutPathFragment.SetNewPathPoints(OutPoints);

	return Result;
}

bool UNavMeshPathSubsystem::GetRandomPointInNavigableRadius(const FVector& StartLocation, const float Radius, FVector& OutRandomLocation) const
{
	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem == nullptr)
	{
		return false;
	}
	
	FNavLocation ResultLocation;
	NavSystem->GetRandomPointInNavigableRadius(StartLocation, Radius, ResultLocation);
	OutRandomLocation = ResultLocation.Location;
	return true;
}
