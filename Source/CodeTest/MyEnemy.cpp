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

	for (FBodyInstance* c : GetMesh()->Bodies)
	{
		int16 index = c->InstanceBoneIndex;
		BoneIndices.Push(index);
	}
}

// Called every frame
void AMyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ChooseGetUpAnimation();

	//to hold ragdoll rotation
	FVector PelvisLoc;
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

	if (!bIsImpaled && !currStake && vel.IsNearlyZero(1.5f) && bIsRagdoll && !bIsRecovering )
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, this->GetName() + " actor is starting recovery");
		PhysicsAlpha = 1.0f;

		RotationBoneAxis = GetMesh()->GetBoneAxis("pelvis", EAxis::X).Rotation(); //get ragdoll orientation
		RotationBoneAxis.Pitch = 0.0f;
		RotationBoneAxis.Roll = 0.0f;
		
		bIsRecovering = true;
	}
	
	if (bIsRecovering)
	{
		CurrentAnimPosition = GetMesh()->GetPosition();
		PhysicsAlpha -= DeltaTime / 1.5f;
		GetMesh()->PutAllRigidBodiesToSleep();

		//if true recovery is complete, restore values to initial ones
		if (PhysicsAlpha <= 0 && CurrentAnimPosition == CurrentGetUpAnim->GetPlayLength())
		{
			PhysicsAlpha = 0.0f;

			//disable physics simulation
			GetMesh()->SetAllBodiesBelowSimulatePhysics("root", false);

			//reattach capsule and it's properties
			GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
			GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			//enable character movement
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

			bIsRecovering = false;
			bIsRagdoll = false;
			bIsPlayingGetUpAnim = false;

			//play idle animation
			GetMesh()->PlayAnimation(AnimIdle, true);
		}
		//update blend weight
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight("pelvis",PhysicsAlpha);

		if (!bIsPlayingGetUpAnim && bIsRecovering)
		{
			GetMesh()->Stop();
			CurrentAnimPosition = 0.0f;
			bIsPlayingGetUpAnim = true;
			CurrentAnimPosition = 0.0f;

			//Here i would put part with choosing animation based on a ragdoll pose
			//1.Search for the most appropriate animation - ChooseGetUpAnimation();
			//2.Set CurrentGetUpAnim to found animation
			//3.Run the animation

			//This part is as it was before
			//////////////////////////////////////////////////////////////////////////////////////////
			CheckMeshOrientation();
			if (MeshOrientation == ECharacterOreintation::FRONT)
			{
				GetMesh()->PlayAnimation(AnimFront, false);
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "playing get up front");
				RotationBoneAxis.Yaw += 180.0f;

				GetMesh()->SetWorldRotation(RotationBoneAxis);
				CurrentGetUpAnim = AnimFront;

			}
			else if (MeshOrientation == ECharacterOreintation::BACK)
			{
				GetMesh()->PlayAnimation(AnimBack, false);
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "playing get up back");
				GetMesh()->SetWorldRotation(RotationBoneAxis);
				CurrentGetUpAnim = AnimBack;
			}
			else if (MeshOrientation == ECharacterOreintation::LEFT)
			{
				GetMesh()->PlayAnimation(AnimLeft, false);
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "playing get up left");
				RotationBoneAxis.Yaw += 270.0f;
				GetMesh()->SetWorldRotation(RotationBoneAxis);
				CurrentGetUpAnim = AnimLeft;
			}
			else if (MeshOrientation == ECharacterOreintation::RIGHT)
			{

				GetMesh()->PlayAnimation(AnimRight, false);
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "playing get up right");
				RotationBoneAxis.Yaw += 90.0f;
				GetMesh()->SetWorldRotation(RotationBoneAxis);
				CurrentGetUpAnim = AnimRight;
			}
			///////////////////////////////////////////////////////////////////////////////////////////////
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
	if (currStake)
	{
		//check if we hit a static mesh actor
		AStaticMeshActor* Wall = Cast<AStaticMeshActor>(OtherActor);

		//check angle
		FVector vel = currStake->GetActorForwardVector();
		FVector normal = -Hit.ImpactNormal;
		float res = FVector::DotProduct(vel, normal);

		//make sure if we hit a mesh and we are active for a new constraint creation
		if (currStake && Wall && !bIsInactive && currStake->KineticEnergy > KineticEnergyThreshold && res > currStake->Sharpness)
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

void AMyEnemy::ChooseGetUpAnimation()
{
	USkeletalMeshComponent * SkeletalMesh = GetMesh();
	UAnimSequence * CurrentAnim = AnimIdle;

	//iterate over all animations from which we are choosing best fitting one
	//for( every animation in the pool)
	{
		//obtain every bone transform or maybe just some key bones - performance should be checked
		for (int16 c : BoneIndices)
		{

			FName boneName = SkeletalMesh->GetBoneName(c);
			//root world transform
			FTransform rootWorld = SkeletalMesh->GetBoneTransform(SkeletalMesh->GetBoneIndex("root"));


			//this actually gives us world transform...
			FTransform ragdollTransform = SkeletalMesh->GetBoneTransform(c);

			//I tried to get relative transform but it's not consistent with the data that I get from animation sequence
			FTransform relativeT = ragdollTransform.GetRelativeTransform(rootWorld);

			FVector bleh = SkeletalMesh->GetBoneLocation(boneName, EBoneSpaces::ComponentSpace);

			/*FBodyInstance* bone = SkeletalMesh->GetBodyInstance(boneName);
			int32 shapes;
			physx::PxShape* currShape = *bone->GetAllShapes(shapes).GetData();
			FTransform relative = bone->GetRelativeBodyTransform(currShape);

			USceneComponent*  comp = SkeletalMesh->GetChildComponent(c);
			FTransform rel = comp->GetRelativeTransform();*/			

			//get transform from animation at time = 0.0f relative to the parent i.e. root bone
			FTransform animTransform;
			CurrentAnim->GetBoneTransform(animTransform, c, 0.0f, true);

			if (boneName == "head")
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, relativeT.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, animTransform.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, bleh.ToString());
			}

			//compare transforms and store the result
			//here we should compare how close to actual position from animation we are 
			//for example calculate the difference between every bone position
			//sum up the result
			//choose animation with the lowest difference
			/*bool result = ragdollTransform.Equals(animTransform, 0.9f);
			if (result)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, SkeletalMesh->GetBoneName(c).ToString() + " bone is almost in same place as in animation");
			}*/
		}
	}

}

FTransform AMyEnemy::GetBoneTransformAtTime(UAnimSequence* MyAnimSequence, float AnimTime, int16 BoneIdx, bool bUseRawDataOnly)
{
	const TArray<FTrackToSkeletonMap> & TrackToSkeletonMap = bUseRawDataOnly ? MyAnimSequence->GetRawTrackToSkeletonMapTable() : MyAnimSequence->GetCompressedTrackToSkeletonMapTable();

	if ((TrackToSkeletonMap.Num() > 0) && (TrackToSkeletonMap[0].BoneTreeIndex == 0))
	{
		FTransform BoneTransform;
		MyAnimSequence->GetBoneTransform(BoneTransform, BoneIdx, AnimTime, bUseRawDataOnly);
		return BoneTransform;
	}
	return FTransform::Identity;
}