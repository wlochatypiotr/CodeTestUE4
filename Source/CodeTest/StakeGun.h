// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon.h"
#include "StakeGun.generated.h"

/**
 * 
 */
UCLASS()
class CODETEST_API AStakeGun : public AWeapon
{
	GENERATED_BODY()
	
		virtual void FireProjectile() override;
	
	
};
