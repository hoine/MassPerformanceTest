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
	if (NavSys)
	{
		const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance();
		const FPathFindingQuery Query(nullptr, *NavData, StartLocation, Destination, UNavigationQueryFilter::GetQueryFilter(*NavData, nullptr, nullptr));
		const FPathFindingResult Result = NavSys->FindPathSync(Query, EPathFindingMode::Regular);
		if (Result.IsSuccessful())
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
	}
	else
	{
		NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	}
	
	return false;
}