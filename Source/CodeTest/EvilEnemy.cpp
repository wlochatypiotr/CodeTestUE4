// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeTest.h"
#include "EvilEnemy.h"
#include "Engine.h"


// Sets default values
AEvilEnemy::AEvilEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetCapsuleRadius(34.0f);
	CollisionComponent->SetCapsuleHalfHeight(88.0f);
	RootComponent = CollisionComponent;

	EnemyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EnemyMesh"));
	EnemyMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}

// Called when the game starts or when spawned
void AEvilEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEvilEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEvilEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, Hit.BoneName.ToString() + "bone just hit smth");
}

