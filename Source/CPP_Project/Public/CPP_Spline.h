// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "CPP_Spline.generated.h"

UCLASS()
class CPP_PROJECT_API ACPP_Spline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_Spline();

	UPROPERTY(EditAnywhere)
	USplineComponent* Base_Spline;

	UFUNCTION(BlueprintCallable)
	void FAsync_Build_Spline();

	void FBuild_Spline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
