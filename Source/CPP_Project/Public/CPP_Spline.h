#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/BillboardComponent.h"
#include "Runtime/Engine/Public/EngineGlobals.h"
#include "CPP_Spline.generated.h"


UCLASS(ClassGroup=(Custom), meta = (BlueprintSpawnableComponent))
class CPP_PROJECT_API ACPP_Spline : public AActor
{
	GENERATED_BODY()
	
public:	
	ACPP_Spline();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	USplineComponent* Base_Spline;

	UPROPERTY()
	UBillboardComponent* BillboardComp;

	void FBuild_Spline();

	UFUNCTION(BlueprintCallable)
	void Start();



	UFUNCTION(CallInEditor, Category = "Base")
	void ResetSpline();

	UFUNCTION(CallInEditor, Category = "Base")
	void Regenerate();

	


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TArray<UStaticMesh*> Mesh;
	
	UPROPERTY(EditAnywhere, Category = "Base")
	TEnumAsByte<ESplineMeshAxis::Type> InForwardAxis = ESplineMeshAxis::X;

	UPROPERTY(EditAnywhere, Category = "Base")
	TEnumAsByte<ECollisionEnabled::Type> Collision = ECollisionEnabled::NoCollision;

	UPROPERTY(EditAnywhere, Category = "Base")
	float Padding = 0;

	UPROPERTY(EditAnywhere, Category = "Base")
	float PaddingVariation = 0;

	UPROPERTY(EditAnywhere, Category = "Base")
	bool CanNegativePadding = false;

	UPROPERTY(EditAnywhere, Category = "Base")
	float Scale = 1;

	UPROPERTY(EditAnywhere, Category = "Base")
	float ScaleVariation = 0;

	UPROPERTY(EditAnywhere, Category = "Base")
	bool UseTangent = true;

	UPROPERTY(EditAnywhere, Category = "Base")
	bool Build = true;

	UPROPERTY(EditAnywhere, Category = "Base")
	bool UseFeatures = false;

	UPROPERTY(EditAnywhere, Category = "Base")
	bool ShowStats = false;

	




	UPROPERTY(EditAnywhere, Category = "Feature")
	FVector WorldOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Feature")
	FVector WorldOffsetVariation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Feature")
	float Rotation = 0;

	UPROPERTY(EditAnywhere, Category = "Feature")
	float RotationVariation = 0;

	UPROPERTY(EditAnywhere, Category = "Feature")
	bool AttachToFloor = false;

	UPROPERTY(EditAnywhere, Category = "Feature")
	bool Plane = false;

protected:
	virtual void BeginPlay() override;
	
private:

	int32 Index;
	float MeshSize;
	float SplineLength;
	float CurrentDistance = 0;
	float NextDistance = 0;
	float FScale;
	float FRotation;
	float FPadding;
	FVector FWorldOffset;
	float BoundsSize();
	float Attach(const FVector& StartPos);
	void MakePlane(FVector& StartPos, FVector& StartTangent, FVector& EndPos, FVector& EndTangent);
	float Differ(float Value, float Variation);
	void Features(USplineMeshComponent* SplineMeshComponent, FVector& StartPos, FVector& StartTangent, FVector& EndPos, FVector& EndTangent);
};

