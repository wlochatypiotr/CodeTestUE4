// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Stake.h"
#include "Enemy.h"
#include "Weapon.generated.h"

#define TRACE_WEAPON ECC_GameTraceChannel1

UENUM(BlueprintType, Meta = (Bitflags))
enum class EProjectileType : uint8
{
	Bullet,
	Spread,
	Projectile
};

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		int32 ShotCost;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
		float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
		float WeaponRange;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
		float WeaponSpread;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
		FString Name;

};

UCLASS()
class CODETEST_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FHitResult WeaponTrace(const FVector &TraceFrom, const FVector &TraceTo) const;

	void ProcessInstantHit(const FHitResult &Impact, const FVector &Origin, const FVector &ShootDir, int32 RandomSeed, float ReticleSpread);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void Fire();

	UFUNCTION()
		void InstantFire();

	UFUNCTION()
		virtual void FireProjectile();

	UPROPERTY(EditDefaultsOnly, Category = "Config")
		FWeaponData WeaponConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", Meta = (Bitmask, BitmaskEnum = "EProjectileType"))
		EProjectileType ProjectileType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
		UBoxComponent* CollisionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<class AStake> ProjectileClass;


};
