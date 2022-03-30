// Fill out your copyright notice in the Description page of Project Settings.


//#include "CPP_Spline.h"
#include "CPP_Project/Public/CPP_Spline.h"

#include "Engine/Engine.h"

// Sets default values
ACPP_Spline::ACPP_Spline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void ACPP_Spline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPP_Spline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("Works"));

}

void ACPP_Spline::FAsync_Build_Spline()
{
	
}

void ACPP_Spline::FBuild_Spline()
{

}

