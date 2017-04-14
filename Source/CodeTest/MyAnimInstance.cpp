// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeTest.h"
#include "MyAnimInstance.h"




void UMyAnimInstance::UpdateAnimationProperties()
{
	APawn* Pawn = TryGetPawnOwner();

	if (Pawn)
	{
		//update properties
		AMyEnemy * Enemy = Cast<AMyEnemy>(Pawn);
		if (Enemy)
		{
			bIsRagdoll = Enemy->bIsRagdoll;
			bIsRecovering = Enemy->bIsRecovering;
			physicsBlendWeigth = Enemy->PhysicsAlpha;
		}

	}
}
