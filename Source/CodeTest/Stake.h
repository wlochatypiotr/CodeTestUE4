// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Enemy.h"
#include "MyEnemy.h"
#include "Stake.generated.h"

UCLASS()
class CODETEST_API AStake : public AActor
{
	GENERATED_BODY()

	TArray<UPhysicsHandleComponent *> PhysicsHandles;

	TArray<class AActor *> IgnoreActors;

public:	
	// Sets default values for this actor's properties
	AStake();

	UPROPERTY(VisibleDefaultsOnly,Category = "Projectile")
		USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
		UStaticMeshComponent* ProjectileMesh;

	UFUNCTION()
		virtual void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void LifeSpanExpired() override;

	AMyEnemy * PiercedEnemy;

	
	
};
