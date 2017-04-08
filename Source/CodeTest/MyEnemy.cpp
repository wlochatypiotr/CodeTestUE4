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
}

// Called every frame
void AMyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRagdoll && !bIsRecovering)
	{
		FVector PelvisLoc = GetMesh()->GetBoneLocation("pelvis") + FVector(0.0f, 0.0f, 88.0f);
		GetCapsuleComponent()->SetWorldLocation(PelvisLoc);
	}

	//check if we stopped moving if yes we need to start recovering
	FVector vel = GetMesh()->GetPhysicsLinearVelocity("pelvis");
	if (!bIsImpaled && !bIsGrabbed && vel.IsNearlyZero(2.0f) && bIsRagdoll && !bIsRecovering)
	{
		//check what direction is ragdoll facing
		CheckFacingDirection();

		bIsRecovering = true;
		PhysicsAlpha = 1.0f;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, this->GetName() + " actor is starting recovery");
	}

	///////////////////////////////	RECOVERY PART/////////////////////////////////////
	if (bIsRecovering)
	{
		//with the time change we need to change physics alpha
		PhysicsAlpha -= DeltaTime / 2.0f;
		if (PhysicsAlpha < 0)
		{
			PhysicsAlpha = 0.0;
			bIsRagdoll = false;
			bIsRecovering = false;
			GetMesh()->PutAllRigidBodiesToSleep();
			GetMesh()->SetAllBodiesSimulatePhysics(false);
			GetMesh()->SetComponentTickEnabled(true);

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
			//GetMesh()->AttachTo(GetCapsuleComponent(), NAME_None, EAttachLocation::SnapToTargetIncludingScale, true);
			GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, this->GetName() + " actor has finished recovery - disabling physics simulation ");
		}
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight("pelvis", PhysicsAlpha);
		//GetMesh()->PlayAnimation(AnimFront, false);
		
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
		UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(OtherComponent);
		if (Wall && StaticMesh)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "We know that we hit a StaticMeshActor with StaticMeshComponent");
			auto BoneLocation  = GetMesh()->GetBoneLocation(GrabbedBone);
			
		}
		auto  ourbone = HitComponent->GetBodyInstance(GrabbedBone);
		//make sure we grab hit bone at correct location
		if (OtherActor != this && HitComponent != OtherComponent && Wall)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, this->GetName() + "just hit an Actor:" + OtherActor->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, HitComponent->GetName() + "just hit an component:" + OtherComponent->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, this->GetName() + "just hit an StaticMeshActor:" + Wall->GetName());

			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue,  "Bone hit in this collision is:" + Hit.BoneName.ToString());

			UPhysicsHandleComponent * PhysicsHandle = NewObject<UPhysicsHandleComponent>(this);

			PhysicsHandle->RegisterComponent();

			PhysicsHandle->GrabComponentAtLocation(GetMesh(), GrabbedBone, Hit.Location);

			bIsImpaled = true;
		}
		//UStaticMeshComponent * staticMesh = Cast<UStaticMeshComponent>(OtherComponent);
		//USceneComponent * target = Cast<USceneComponent>(OtherComponent);
		//AActor* test = OtherActor;
		//AStake* stake = Cast<AStake>(OtherActor);
		//if (!stake  && staticMesh && target)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, GrabbedBone.ToString() + "is currently staked and hit a static mesh" + OtherActor->GetName());
		//	UPhysicsConstraintComponent* ConstraintComp = NewObject<UPhysicsConstraintComponent>(target);
		//	if (!ConstraintComp)
		//	{
		//		//UE_LOG constraint UObject could not be created!
		//		return;
		//	}

		//	//Set World Location
		//	ConstraintComp->SetWorldLocation(Hit.Location);

		//	//Attach to Root!
		//	ConstraintComp->AttachTo(test->GetRootComponent(), NAME_None, EAttachLocation::KeepWorldPosition);

		//	//~~~ Init Constraint ~~~
		//	ConstraintComp->SetConstrainedComponents(OtherComponent, NAME_None, Hit.GetComponent(), Hit.BoneName);
		//}
	}
}

void AMyEnemy::CheckFacingDirection()
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
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, this->GetName() + " actor is lying on face");
	else
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, this->GetName() + " actor is on his back");
}

//void AMyEnemy::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if(GrabbedBone !=  NAME_None)
//		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, GrabbedBone.ToString() + "is currently overlapping ");
//	else
//		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, OverlappedComp->GetName() + "is currently overlapping " + OtherComp->GetName()) ;
//}
