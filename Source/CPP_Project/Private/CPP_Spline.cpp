
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
	Scale = FMath::Abs(Scale);
	if (Scale == 0)
		Scale = 1;
	if (Mesh.Num() > 0)
		FBuild_Spline();
}

void ACPP_Spline::FBuild_Spline()
{
	milliseconds StartMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	
	CurrentDistance = 0;
	Index = FMath::RandRange(0, (int32) (Mesh.Num() - 1));
	int32 ElementsNumber = 0;
	FPadding = 0;
	
	SplineLength = Base_Spline->GetSplineLength();
	MeshSize = BoundsSize();

	while (SplineLength > MeshSize && MeshSize > 0)
	{
		ElementsNumber++;

		FWorldOffset.X = Differ(WorldOffset.X, WorldOffsetVariation.X);
		FWorldOffset.Y = Differ(WorldOffset.Y, WorldOffsetVariation.Y);
		FWorldOffset.Z = Differ(WorldOffset.Z, WorldOffsetVariation.Z);

		FRotation = Differ(Rotation, RotationVariation);

		NextDistance = CurrentDistance + MeshSize;
		
		FVector StartPos = Base_Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::Local);
		FVector EndPos = Base_Spline->GetLocationAtDistanceAlongSpline(NextDistance, ESplineCoordinateSpace::Local);
		FVector StartTangent = Base_Spline->GetTangentAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::Local).GetClampedToSize(MeshSize, MeshSize);
		FVector EndTangent = Base_Spline->GetTangentAtDistanceAlongSpline(NextDistance, ESplineCoordinateSpace::Local).GetClampedToSize(MeshSize, MeshSize);

		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

		if (UseFeatures)
			Features(SplineMesh, StartPos, StartTangent, EndPos, EndTangent);

		SplineMesh->SetStaticMesh(Mesh[Index]);
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		SplineMesh->RegisterComponentWithWorld(GetWorld());
		SplineMesh->AttachToComponent(Base_Spline, FAttachmentTransformRules::KeepRelativeTransform);
		SplineMesh->SetForwardAxis(InForwardAxis, true);
		SplineMesh->SetCollisionEnabled(Collision);
		SplineMesh->SetStartScale(FVector2D(FScale, FScale));
		SplineMesh->SetEndScale(FVector2D(FScale, FScale));
		if (UseTangent)
			SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
		else
			SplineMesh->SetStartAndEnd(StartPos, FVector::ZeroVector, EndPos, FVector::ZeroVector);
		Index = FMath::RandRange(0, (int32)(Mesh.Num() - 1));
		FPadding = Differ(Padding, PaddingVariation);
		if (!CanNegativePadding)
			FPadding = FMath::Abs(FPadding);
		SplineLength = SplineLength - (MeshSize + FPadding);
		CurrentDistance = CurrentDistance + MeshSize + FPadding;
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

void ACPP_Spline::Features(USplineMeshComponent* SplineMeshComponent, FVector& StartPos, FVector& StartTangent, FVector& EndPos, FVector& EndTangent)
{
	if (AttachToFloor)
	{
		FVector StartAttachPos = Base_Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World) + FVector(FWorldOffset.X, FWorldOffset.Y, 0);
		FVector EndAttachPos = Base_Spline->GetLocationAtDistanceAlongSpline(NextDistance, ESplineCoordinateSpace::World) + FVector(FWorldOffset.X, FWorldOffset.Y, 0);

		StartPos.Z += Attach(StartAttachPos);
		EndPos.Z += Attach(EndAttachPos);
	}
	if (Plane)
		MakePlane(StartPos, StartTangent, EndPos, EndTangent);
	SplineMeshComponent->AddLocalOffset(FWorldOffset);
	SplineMeshComponent->SetStartRoll(FRotation / 57.32);
	SplineMeshComponent->SetEndRoll(FRotation / 57.32);
}


float ACPP_Spline::BoundsSize()
{
	FScale = Differ(Scale, ScaleVariation);
	if (FScale == 0)
		FScale = Scale;
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
	Size *= 2 * FScale;
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
		if (ShowStats)
			DrawDebugLine(GetWorld(), StartPos, SecondPos, FColor::Red, false, 3.0f, 0, 4);
	}
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

void ACPP_Spline::Regenerate()
{
	RerunConstructionScripts();
}

void ACPP_Spline::MakePlane(FVector& StartPos, FVector& StartTangent, FVector& EndPos, FVector& EndTangent)
{
	float TmpFloat = (StartPos.Z + EndPos.Z) / 2;
	StartPos.Z = TmpFloat;
	EndPos.Z = TmpFloat;
	StartTangent.Z = 0;
	EndTangent.Z = 0;
}

float ACPP_Spline::Differ(float Value, float Variation)
{
	Variation = FMath::Abs(Variation);
	return Value + FMath::RandRange(Variation * -1, Variation);
}
