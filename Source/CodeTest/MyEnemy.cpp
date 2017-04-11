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
	auto inst  = GetMesh()->GetAnimInstance();

	//IsLyingOnFace();
	//GetMesh()->RefreshBoneTransforms();
	GetMesh()->UpdateComponentToWorld();
	//make capsule follow the mesh
	if (bIsRagdoll /*&& !bIsRecovering*/)
	{
		FVector PelvisLoc = GetMesh()->GetBoneLocation("pelvis") + FVector(0.0f, 0.0f, 88.0f);
		GetCapsuleComponent()->SetWorldLocation(PelvisLoc);
	}

	FVector ragdollLoc;
	FRotator RotationBoneAxis;
	FVector vel = GetMesh()->GetPhysicsLinearVelocity("pelvis");
	if (!bIsImpaled && !bIsGrabbed && vel.IsNearlyZero(1.5f) && bIsRagdoll && !bIsRecovering )
	{
		/*
		ragdollLoc = GetMesh()->GetBoneLocation("pelvis") + FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) + FVector(0.0f, 0.0f, -88.0f);
		RotationBoneAxis = GetMesh()->GetBoneAxis("pelvis", EAxis::X).Rotation()+ FRotator(180.f, 0.0f, 0.0f);
		RotationBoneAxis.Pitch = 0;
		RotationBoneAxis.Roll = 0;
		RotationBoneAxis = RotationBoneAxis.Clamp();
		SetActorLocationAndRotation(ragdollLoc, RotationBoneAxis);
		GetCapsuleComponent()->SetWorldLocationAndRotation(ragdollLoc, RotationBoneAxis);
		GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);


		//GetMesh()->SetSimulatePhysics(false);
		//GetMesh()->AttachTo(GetCapsuleComponent());
		//bUseControllerRotationYaw = false;
		*/
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, this->GetName() + " actor is starting recovery");
		PhysicsAlpha = 1.0f;

		bIsRecovering = true;
	}

	if (bIsRecovering)
	{
		PhysicsAlpha -= DeltaTime / 2.0f;

		//if true recovery is complete, restore values to intial ones
		if (PhysicsAlpha <= 0)
		{
			PhysicsAlpha = 0.0f;

			//disable physics simulation
			GetMesh()->SetAllBodiesBelowSimulatePhysics("root", false);

			//reattach capsule and it's properties
			//GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
			//GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
			//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			//enable character movement
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

			bIsRecovering = false;
			bIsRagdoll = false;
			bIsPlayingGetUpAnim = false;

			GetMesh()->PlayAnimation(AnimIdle, true);
		}
		//update blend weigth
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight("root",PhysicsAlpha);
		if (!bIsPlayingGetUpAnim && bIsRecovering)
		{
			if (IsLyingOnFace())
			{
				GetMesh()->PlayAnimation(AnimFront, false);
				bIsPlayingGetUpAnim = true;

			}
			else
			{
				GetMesh()->PlayAnimation(AnimBack, false);
				bIsPlayingGetUpAnim = true;
			}
		}

	}
	/*

	//check if we stopped moving if yes we need to start getting up
	FVector vel = GetMesh()->GetPhysicsLinearVelocity("pelvis");
	if (!bIsImpaled && !bIsGrabbed && vel.IsNearlyZero(0.5f) && bIsRagdoll && !bIsRecovering)
	{
		bIsRecovering = true;
		PhysicsAlpha = 1.0f;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, this->GetName() + " actor is starting recovery");
	}

	///////////////////////////////	RECOVERY PART/////////////////////////////////////
	if (bIsRecovering)
	{
		//with the time change we need to change physics alpha
w		PhysicsAlpha -= DeltaTime /10.0f;

		//getting up is complete
		if (PhysicsAlpha < 0)
		{
			PhysicsAlpha = 0.0;

			//update blend weight
			GetMesh()->SetAllBodiesBelowPhysicsBlendWeight("pelvis", PhysicsAlpha);

			bIsRagdoll = false;
			bIsRecovering = false;
			GetMesh()->PutAllRigidBodiesToSleep();
			GetMesh()->SetAllBodiesSimulatePhysics(false);
			GetMesh()->SetComponentTickEnabled(true);

			//go back to idle animation
			GetMesh()->PlayAnimation(AnimIdle, true);
			//also it rotates mesh back to capsule rotation
			//this makes mesh pop up from ground
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
			GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));

			bIsPlayingGetUpAnim = false;

			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, this->GetName() + " actor has finished recovery - disabling physics simulation ");
		}
		
		//update blend weight
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight("pelvis", PhysicsAlpha);

		//choosing the right animation
		if (!bIsPlayingGetUpAnim && bIsRecovering)
		{
			if (IsLyingOnFace())
			{
				//GetMesh()->PlayAnimation(AnimFront, false);
				bIsPlayingGetUpAnim = true;

			}
			if (!IsLyingOnFace())
			{
				//GetMesh()->PlayAnimation(AnimBack, false);
				bIsPlayingGetUpAnim = true;
			}
		}	
	}
	*/
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
		if (/*OtherActor != this && HitComponent != OtherComponent && */bIsGrabbed && Wall && !bIsInactive)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, this->GetName() + "just hit an Actor:" + OtherActor->GetName());
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, HitComponent->GetName() + "just hit an component:" + OtherComponent->GetName());
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, this->GetName() + "just hit an StaticMeshActor:" + Wall->GetName());

			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue,  "Bone hit in this collision is:" + Hit.BoneName.ToString());

			//UPhysicsHandleComponent * PhysicsHandle = NewObject<UPhysicsHandleComponent>(this);

			//PhysicsHandle->RegisterComponent();

			//PhysicsHandle->GrabComponentAtLocation(GetMesh(), GrabbedBone, Hit.Location);

			//Create new constraint
			{
				UPhysicsConstraintComponent* ConstraintComp = NewObject<UPhysicsConstraintComponent>(this);
				if (!ConstraintComp)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Constraint could not be created");
				}
				else
				{
					ConstraintComp->RegisterComponent();
					ConstraintComp->SetWorldLocation(Hit.Location);
					ConstraintComp->SetConstrainedComponents(OtherComponent, NAME_None, HitComponent, GrabbedBone);

					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Constraint created");
					bIsInactive = true;
					bIsImpaled = true;
				}
			}
		}
		
	}
}

bool AMyEnemy::IsLyingOnFace()
{
	//helper - draws front facing vector
	FVector pos = GetMesh()->GetBoneLocation("pelvis");
	FVector forward = GetMesh()->GetForwardVector() * 100.0f;
	DrawDebugLine(GetWorld(), pos, pos - forward, FColor::Red, true, 10000, 10.0f);

	//checks what dir mesh is facing
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("enemy trace")), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);

	bool coll = GetWorld()->LineTraceSingleByChannel(Hit, pos, pos - forward, ECC_EngineTraceChannel2, TraceParams);
	if (coll)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, this->GetName() + " actor is lying on face");
		return coll;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, this->GetName() + " actor is on his back");
		return coll;
	}
}
