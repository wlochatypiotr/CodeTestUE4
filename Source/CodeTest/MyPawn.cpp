// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeTest.h"
#include "MyPawn.h"


// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//GetMesh()->OnComponentBeginOverlap.AddDynamic(this, &AMyEnemy::OnHit);

}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//void AMyPawn::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (bIsGrabbed)
//	{
//		GetMesh()->GetBoneLocation(GrabbedBone);
//		UStaticMeshComponent * staticMesh = Cast<UStaticMeshComponent>(OtherComp);
//		AStake* stake = Cast<AStake>(OtherActor);
//		if (!stake  && staticMesh)
//		{
//			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, GrabbedBone.ToString() + "is currently staked and hit a static mesh");
//		}
//	}
//}