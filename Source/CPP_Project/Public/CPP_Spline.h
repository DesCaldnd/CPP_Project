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

	UFUNCTION(CallInEditor, Category = "Base")
	void ResetSpline();

	UPROPERTY()
	USplineComponent* Base_Spline;

	UFUNCTION(BlueprintCallable)
	void Start();

	void FBuild_Spline();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	TArray<UStaticMesh*> Mesh;
	
	UPROPERTY(EditAnywhere, Category = "Base")
	TEnumAsByte<ESplineMeshAxis::Type> InForwardAxis = ESplineMeshAxis::X;

	UPROPERTY(EditAnywhere, Category = "Base")
	TEnumAsByte<ECollisionEnabled::Type> Collision = ECollisionEnabled::NoCollision;

	UPROPERTY(EditAnywhere, Category = "Base")
	bool UseTangent = true;

	UPROPERTY(EditAnywhere, Category = "Base")
	bool Build = true;

	UPROPERTY()
	UBillboardComponent* BillboardComp;

	UPROPERTY(EditAnywhere, Category = "Feature")
	FVector WorldOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Feature")
	bool AttachToFloor = false;

	UPROPERTY(EditAnywhere, Category = "Feature")
	bool ShowStats = false;
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:

	int32 Index;
	float MeshSize;
	float SplineLength;
	float CurrentDistance = 0;
	float NextDistance = 0;
	float BoundsSize();
	float Attach(const FVector& StartPos);

};

