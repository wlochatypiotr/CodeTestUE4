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
		//GetWorld()->SpawnActor(ProjectileClass, MFLoc, MFRot, SpawnParams);
		AStake *const Stake = GetWorld()->SpawnActor<AStake>(ProjectileClass, MFLoc, MFRot, SpawnParams);
		if (Stake)
		{
			//FVector const Launch = MFRot.Vector();
			//Stake->Init
		}
	}
}
