// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeTest.h"
#include "Stake.h"
#include "MyEnemy.h"


// Sets default values


AMyEnemy::AMyEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->Activate(true);
	GetMesh()->OnComponentHit.AddDynamic(this, &AMyEnemy::OnHit);
}

// Called when the game starts or when spawned
void AMyEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->PlayAnimation(AnimIdle, true);
}

// Called every frame
void AMyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//to hold ragdoll rotation
	FVector PelvisLoc;
	////////////////////////////////////////DONE////////////////////////////////////////////
	//this timer prevents creation of multiple constraints on hit
	if (bIsInactive)
	{
		Timer += DeltaTime;
		if (Timer >= inactivationTime)
		{
			bIsInactive = false;
			Timer = 0.0f;
		}
	}
	////////////////////////////////////////DONE////////////////////////////////////////////
	GetMesh()->UpdateComponentToWorld();
	//make capsule follow the mesh
	if (bIsRagdoll && !bIsRecovering)
	{
		PelvisLoc = GetMesh()->GetBoneLocation("pelvis") + FVector(0.0f, 0.0f, 88.0f);
		GetCapsuleComponent()->SetWorldLocation(PelvisLoc);
	}

	FVector vel = GetMesh()->GetPhysicsLinearVelocity("pelvis");
	//make ragdoll still when it's impaled
	if (bIsImpaled && vel.IsNearlyZero(1.5f) && bShouldBeStill)
	{
		GetMesh()->Deactivate();
		this->SetActorTickEnabled(false);
	}
	if (!bIsImpaled && !bIsGrabbed && vel.IsNearlyZero(1.5f) && bIsRagdoll && !bIsRecovering )
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, this->GetName() + " actor is starting recovery");
		PhysicsAlpha = 1.0f;

		RotationBoneAxis = GetMesh()->GetBoneAxis("pelvis", EAxis::X).Rotation(); //get ragdoll orientation
		RotationBoneAxis.Pitch = 0.0f;
		RotationBoneAxis.Roll = 0.0f;
		
		//GetMesh()->PutAllRigidBodiesToSleep();
		//GetMesh()->SetAllBodiesSimulatePhysics(false);
		//GetMesh()->PlayAnimation(AnimFront, false);

		//bIsRagdoll = false;
		bIsRecovering = true;


		GetMesh()->GetAnimInstance()->Montage_Stop(0.0f);

	}
	
	if (bIsRecovering)
	{
		PhysicsAlpha -= DeltaTime / 2.0f;
		GetMesh()->PutAllRigidBodiesToSleep();
		//FVector BlendParams(100.0f *(1.0f-PhysicsAlpha), 0.0f, 0.0f);
		//GetMesh()->GetSingleNodeInstance()->SetBlendSpaceInput(BlendParams);
		//if true recovery is complete, restore values to intial ones
		if (PhysicsAlpha <= 0)
		{
			PhysicsAlpha = 0.0f;

			//disable physics simulation
			GetMesh()->SetAllBodiesBelowSimulatePhysics("root", false);

			//reattach capsule and it's properties
			//GetCapsuleComponent()->SetWorldRotation(RotationBoneAxis);
			//GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
			//GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
			//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			//enable character movement
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

			bIsRecovering = false;
			bIsRagdoll = false;
			bIsPlayingGetUpAnim = false;

			//GetMesh()->PlayAnimation(AnimIdle, true);
		}
		//update blend weight
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight("pelvis",PhysicsAlpha);

		if (!bIsPlayingGetUpAnim && bIsRecovering)
		{
			//GetMesh()->GetAnimInstance()->Montage_Stop(0.0f);
			GetMesh()->Stop();
			//GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimFront, "default");
			GetMesh()->PlayAnimation(AnimFront, false);
			bIsPlayingGetUpAnim = true;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "playing get up front");
			RotationBoneAxis.Yaw += 180.0f;
			GetMesh()->SetWorldRotation(RotationBoneAxis);

			//CheckMeshOrientation();
			//if (MeshOrientation == ECharacterOreintation::FRONT)
			//{
			//	//GetMesh()->PlayAnimation(AnimBlendSpaceFront, false);
			//	//GetMesh()->GetSingleNodeInstance()->SetBlendSpaceInput(BlendParams);
			//	//GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimFront, "default");
			//	GetMesh()->PlayAnimation(AnimFront, false);
			//	bIsPlayingGetUpAnim = true;
			//	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "playing get up front");
			//	RotationBoneAxis.Yaw += 180.0f;

			//	GetMesh()->SetWorldRotation(RotationBoneAxis);

			//}
			//else if (MeshOrientation == ECharacterOreintation::BACK)
			//{
			//	//GetMesh()->PlayAnimation(AnimBlendSpaceBack, false);
			//	//GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimBack, "default");
			//	GetMesh()->PlayAnimation(AnimBack, false);
			//	bIsPlayingGetUpAnim = true;
			//	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "playing get up back");
			//	GetMesh()->SetWorldRotation(RotationBoneAxis);
			//}
			//else if (MeshOrientation == ECharacterOreintation::LEFT)
			//{
			//	//getup left
			//	//GetMesh()->PlayAnimation(AnimBlendSpaceLeft, false);
			//	//GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimLeft, "default");
			//	GetMesh()->PlayAnimation(AnimLeft, false);

			//	bIsPlayingGetUpAnim = true;
			//	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "playing get up left");
			//	RotationBoneAxis.Yaw += 270.0f;
			//	GetMesh()->SetWorldRotation(RotationBoneAxis);
			//}
			//else if (MeshOrientation == ECharacterOreintation::RIGHT)
			//{
			//	//get up right
			//	//GetMesh()->PlayAnimation(AnimBlendSpaceRight, false);
			//	//GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimRight, "default");
			//	GetMesh()->PlayAnimation(AnimRight, false);
			//	bIsPlayingGetUpAnim = true;
			//	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "playing get up right");
			//	RotationBoneAxis.Yaw += 90.0f;
			//	GetMesh()->SetWorldRotation(RotationBoneAxis);
			//}
		}

	}
	
}

// Called to bind functionality to input
void AMyEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMyEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bIsGrabbed)
	{
		//check if we hit a static mesh actor
		AStaticMeshActor* Wall = Cast<AStaticMeshActor>(OtherActor);
		//make sure if we hit a mesh and we are active for a new constraint creation
		if (bIsGrabbed && Wall && !bIsInactive)
		{
			//Create new constraint
			{
				UPhysicsConstraintComponent* ConstraintComp = NewObject<UPhysicsConstraintComponent>(this);
				if (!ConstraintComp)
				{
					return;
				}
				else
				{
					ConstraintComp->RegisterComponent();
					ConstraintComp->SetWorldLocation(Hit.Location);
					ConstraintComp->SetConstrainedComponents(OtherComponent, NAME_None, HitComponent, GrabbedBone);

					bIsInactive = true;
					bIsImpaled = true;
					bShouldBeStill = true;
				}
			}
		}
		
	}
}

void AMyEnemy::CheckMeshOrientation()
{
	float FrontBack;
	bool isOnFace;
	float LeftRight;
	bool isOnRight;

	isOnFace = IsLyingOnFace(FrontBack);
	isOnRight = IsLyingOnRightSide(LeftRight);

		//we know that we are lying on either left or right
	if (FrontBack == 1000.f)
	{
		if (isOnRight)
		{
			MeshOrientation = ECharacterOreintation::RIGHT;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on right side");
			return;
		}
		else
		{
			MeshOrientation = ECharacterOreintation::LEFT;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on left side");
			return;
		}
	}
	//we cant be lying on left or right
	else if (LeftRight == 1000.f)
	{
		if (isOnFace)
		{
			MeshOrientation = ECharacterOreintation::FRONT;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on face");
			return;
		}
		else
		{
			MeshOrientation = ECharacterOreintation::BACK;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on back");
			return;
		}
	}
	if (isOnFace && isOnRight)
	{
		if (FrontBack <= LeftRight)
		{
			MeshOrientation = ECharacterOreintation::FRONT;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on face");
			return;
		}
		else
		{
			MeshOrientation = ECharacterOreintation::RIGHT;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on right side");
			return;
		}
	}
	if (isOnFace && !isOnRight)
	{
		if (FrontBack <= LeftRight)
		{
			MeshOrientation = ECharacterOreintation::FRONT;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on face");
			return;
		}
		else
		{
			MeshOrientation = ECharacterOreintation::LEFT;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on left side");
			return;
		}
	}
	if (!isOnFace && isOnRight)
	{
		if (FrontBack <= LeftRight)
		{
			MeshOrientation = ECharacterOreintation::BACK;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on back");
			return;
		}
		else
		{
			MeshOrientation = ECharacterOreintation::RIGHT;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on right side");
			return;
		}
	}
	if (!isOnFace && !isOnRight)
	{
		if (FrontBack <= LeftRight)
		{
			MeshOrientation = ECharacterOreintation::BACK;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on back");
			return;
		}
		else
		{
			MeshOrientation = ECharacterOreintation::LEFT;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on left side");
			return;
		}
	}
}

bool AMyEnemy::IsLyingOnFace(float& distance)
{
	//helper - draws front facing vector
	FVector pos = GetMesh()->GetBoneLocation("pelvis");
	FVector forward = GetMesh()->GetForwardVector() * 30.0f;
	DrawDebugLine(GetWorld(), pos, pos -forward, FColor::Red, true, 10000, 10.0f);

	//checks what dir mesh is facing
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("enemy trace")), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);

	FHitResult HitFront;
	FHitResult HitBack;

	bool frontCollision = GetWorld()->LineTraceSingleByChannel(HitFront, pos, pos - forward, ECC_EngineTraceChannel2, TraceParams);
	bool backCollision = GetWorld()->LineTraceSingleByChannel(HitBack, pos, pos + forward, ECC_EngineTraceChannel2, TraceParams);

	bool res;

	if (backCollision && frontCollision)
	{
		//we are lying on the back
		if (HitBack.Distance <= HitFront.Distance)
		{
			res = false;
			distance = HitBack.Distance;
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on back");
		}
		//we are lying on the right side
		else
		{
			res = true;
			distance = HitFront.Distance;
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on face");
		}
	}
	else if (!backCollision && !frontCollision)
	{
		res = false;
		distance = 1000.f;
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is not lying on face or back");
	}
	//we are lying on the right side
	else if (frontCollision)
	{
		res = true;
		distance = HitFront.Distance;
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on face");
	}
	//we are kying on the left sude
	else if (backCollision)
	{
		res = false;
		distance = HitBack.Distance;
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Mesh is lying on back");
	}

	return res;
}

//true - right side
//false - left side
//false and distance == 1000.f neither left or rigth
bool AMyEnemy::IsLyingOnRightSide(float &distance)
{
	FVector pos = GetMesh()->GetBoneLocation("pelvis");
	FVector right = GetMesh()->GetRightVector() * 30.f;
	DrawDebugLine(GetWorld(), pos, pos - right, FColor::Red, true, 10000, 10.0f);

	//checks what dir mesh is facing
	FCollisionQueryParams TraceParams(FName(TEXT("enemy trace")), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);

	FHitResult HitLeft;
	FHitResult HitRight;

	bool leftCollision = GetWorld()->LineTraceSingleByChannel(HitLeft, pos, pos + right, ECC_EngineTraceChannel2, TraceParams);
	bool rightCollision = GetWorld()->LineTraceSingleByChannel(HitRight, pos, pos - right, ECC_EngineTraceChannel2, TraceParams);
	bool res;

	if (leftCollision && rightCollision)
	{
		//we are lying on the left side
		if (HitLeft.Distance <= HitRight.Distance)
		{
			res = false;
			distance = HitLeft.Distance;
		}
		//we are lying on the right side
		else
		{
			res = true;
			distance = HitRight.Distance;
		}
	}
	else if (!leftCollision && !rightCollision)
	{
		res = false;
		distance = 1000.f;
	}
	//we are lying on the right side
	else if(rightCollision)
	{
		res = true;
		distance = HitRight.Distance;
	}
	//we are lying on the left side
	else if(leftCollision)
	{
		res = false;
		distance = HitLeft.Distance;
	}
	return res;
}