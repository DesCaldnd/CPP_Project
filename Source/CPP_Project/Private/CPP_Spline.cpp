
#include "CPP_Project/Public/CPP_Spline.h"
#include "Engine/World.h"
#include <chrono>
#include "DrawDebugHelpers.h"

using namespace std::chrono;

ACPP_Spline::ACPP_Spline()
{
	PrimaryActorTick.bCanEverTick = true;

	BillboardComp = CreateDefaultSubobject<UBillboardComponent>(TEXT("RootComp"));
	Base_Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	if (Base_Spline && BillboardComp)
	{
		SetRootComponent(BillboardComp);
		Base_Spline->AttachToComponent(BillboardComp, FAttachmentTransformRules::KeepWorldTransform);
		Base_Spline->bShouldVisualizeScale = true;
		Base_Spline->ScaleVisualizationWidth = 70.0f;
	}
}

void ACPP_Spline::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACPP_Spline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPP_Spline::Start()
{
	if (Mesh.Num() > 0)
		FBuild_Spline();
}

void ACPP_Spline::FBuild_Spline()
{
	milliseconds StartMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	CurrentDistance = 0;
	Index = FMath::RandRange(0, (int32) (Mesh.Num() - 1));
	int32 ElementsNumber = 0;
	
	SplineLength = Base_Spline->GetSplineLength();

	MeshSize = BoundsSize();

	while (SplineLength > MeshSize && MeshSize > 0)
	{
		ElementsNumber++;

		NextDistance = CurrentDistance + MeshSize;
		float AvgDistance = (CurrentDistance + NextDistance) / 2;

		const FVector AvgPos = Base_Spline->GetLocationAtDistanceAlongSpline(AvgDistance, ESplineCoordinateSpace::World) + WorldOffset;
		const FVector StartPos = Base_Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::Local);
		const FVector EndPos = Base_Spline->GetLocationAtDistanceAlongSpline(NextDistance, ESplineCoordinateSpace::Local);
		const FVector StartTangent = Base_Spline->GetTangentAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::Local).GetClampedToSize(MeshSize, MeshSize);
		const FVector EndTangent = Base_Spline->GetTangentAtDistanceAlongSpline(NextDistance, ESplineCoordinateSpace::Local).GetClampedToSize(MeshSize, MeshSize);

		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		SplineMesh->AddLocalOffset(WorldOffset);
		float Distance = 0;
		if (AttachToFloor)
			Distance = Attach(AvgPos);
		SplineMesh->SetStaticMesh(Mesh[Index]);
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		SplineMesh->RegisterComponentWithWorld(GetWorld());
		SplineMesh->AttachToComponent(Base_Spline, FAttachmentTransformRules::KeepRelativeTransform);
		SplineMesh->SetForwardAxis(InForwardAxis, true);
		SplineMesh->SetCollisionEnabled(Collision);
		SplineMesh->AddRelativeLocation(FVector(0, 0, Distance));
		SplineMesh->AddLocalOffset(WorldOffset);
		if (UseTangent)
			SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
		else
			SplineMesh->SetStartAndEnd(StartPos, FVector::ZeroVector, EndPos, FVector::ZeroVector);
		CurrentDistance = CurrentDistance + MeshSize;
		Index = FMath::RandRange(0, (int32)(Mesh.Num() - 1));
		SplineLength = SplineLength - MeshSize;
		MeshSize = BoundsSize();
	}
	if (ShowStats)
	{
		milliseconds EndMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		int64 Time = EndMs.count() - StartMs.count();
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString::Printf(TEXT("Elements number = %d, time elapsed = %d"), ElementsNumber, Time));
	}
}

float ACPP_Spline::BoundsSize()
{
	float Size = 1;
	FVector Bounds = Mesh[Index]->GetBoundingBox().GetExtent();
	switch (InForwardAxis)
	{
	case ESplineMeshAxis::Z:
		Size = Bounds.Z;
		break;
	case ESplineMeshAxis::Y:
		Size = Bounds.Y;
		break;
	default:
		Size = Bounds.X;
		break;
	}
	Size *= 2;
	return Size;
}


float ACPP_Spline::Attach(const FVector& StartPos)
{
	float OutDistance = 0;
	FVector SecondPos = StartPos;
	SecondPos.Z -= 20000;
	FHitResult Out;
	if (GetWorld()->LineTraceSingleByChannel(Out, StartPos, SecondPos, ECollisionChannel::ECC_Visibility, FCollisionQueryParams(FName("Trace"), true)))
	{
		OutDistance = Out.Location.Z - StartPos.Z;
	}
	if (ShowStats)
		DrawDebugLine(GetWorld(), StartPos, SecondPos, FColor::Red, false, 3.0f, 0, 4);
	return OutDistance;
}

void ACPP_Spline::ResetSpline()
{
	for (int32 i = Base_Spline->GetNumberOfSplinePoints() - 1; i >= 2 ; i--)
		Base_Spline->RemoveSplinePoint(i);
	if (Base_Spline->GetNumberOfSplinePoints() >= 1)
	{
		Base_Spline->SetLocationAtSplinePoint(0, FVector::ZeroVector, ESplineCoordinateSpace::Local);
	}
	if (Base_Spline->GetNumberOfSplinePoints() >= 2)
	{
		Base_Spline->SetLocationAtSplinePoint(1, FVector(200, 0, 0), ESplineCoordinateSpace::Local);
	}
	RerunConstructionScripts();
}
