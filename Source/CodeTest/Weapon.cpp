// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeTest.h"
#include "Weapon.h"
#include "Engine.h"


// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

FHitResult AWeapon::WeaponTrace(const FVector &TraceFrom, const FVector &TraceTo) const
{
	static FName WaponFireTag = FName(TEXT("WeaponTrace"));

	FCollisionQueryParams TraceParams(WaponFireTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);

	FHitResult Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, TRACE_WEAPON, TraceParams);

	return Hit;
}

void AWeapon::ProcessInstantHit(const FHitResult &Impact, const FVector &Origin, const FVector &ShootDir, int32 RandomSeed, float ReticleSpread)
{
	const FVector EndTrace = Origin + ShootDir * WeaponConfig.WeaponRange;
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
	DrawDebugLine(this->GetWorld(), Origin, Impact.TraceEnd, FColor::Black, true, 10000, 10.0f);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Timer += DeltaTime;

}

void AWeapon::Fire()
{

		Timer = 0.0f;
		if (ProjectileType == EProjectileType::Bullet)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, TEXT("Bullet"));
			InstantFire();
		}
		if (ProjectileType == EProjectileType::Spread)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, TEXT("Spread"));
			for (int32 i = 0; i <= WeaponConfig.WeaponSpread; ++i)
			{
				InstantFire();
			}
		}
		if (ProjectileType == EProjectileType::Projectile)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, TEXT("Projectile"));
			FireProjectile();
		}
	

}


void AWeapon::InstantFire()
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = WeaponConfig.WeaponSpread;
	const float SpreadCone = FMath::DegreesToRadians(WeaponConfig.WeaponSpread * 0.5f);
	const FVector AimDir = WeaponMesh->GetSocketRotation("Muzzle").Vector();
	const FVector StartTrace = WeaponMesh->GetSocketLocation("Muzzle");
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, SpreadCone, SpreadCone);
	const FVector EndTrace = StartTrace + ShootDir * WeaponConfig.WeaponRange;
	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);
}

void AWeapon::FireProjectile()
{

}

