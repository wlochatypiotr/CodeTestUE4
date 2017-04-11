// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeTest.h"
#include "StakeGun.h"




void AStakeGun::FireProjectile()
{
	Super::FireProjectile();

	if (ProjectileClass != nullptr)
	{
		const FVector MFLoc = WeaponMesh->GetSocketLocation("Muzzle");
		const FRotator MFRot = WeaponMesh->GetSocketRotation("Muzzle");
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		SpawnParams.Instigator = Instigator;
		AStake *const Stake = GetWorld()->SpawnActor<AStake>(ProjectileClass, MFLoc, MFRot, SpawnParams);
	}
}
