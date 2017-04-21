// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeTest.h"
#include "Stake.h"
#include "GameFramework/ProjectileMovementComponent.h"


//void AStake::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
//{
//	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComponent != nullptr))
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("'OnHit' called at Stake site"));
//		OtherComponent->AddImpulseAtLocation(GetVelocity() * 100.f, GetActorLocation());
//		Destroy();
//		AEnemy * Enemy = Cast<AEnemy>(OtherActor);
//		if (Enemy)
//		{
//			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, TEXT("You just hit ana enemy"));
//		}
//	}
//}

//Use Physics Handle to simulate movement with projectile
void AStake::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
 	AMyEnemy * Character = Cast<AMyEnemy>(OtherActor);
	USkeletalMeshComponent * SkeletalMesh = Cast<USkeletalMeshComponent>(OtherComp);

	if (Character == nullptr || SkeletalMesh == nullptr || (OtherActor != nullptr && IgnoreActors.Contains(OtherActor)))
		return;

	IgnoreActors.AddUnique(OtherActor);

	USkeletalMeshComponent * Mesh = Character->GetMesh();

	//setup mesh physics behaviour
	Mesh->Activate();
	Mesh->SetSimulatePhysics(true);
	Mesh->WakeAllRigidBodies();
	Mesh->SetAllBodiesBelowSimulatePhysics("pelvis", true);
	Mesh->SetAllBodiesBelowPhysicsBlendWeight("pelvis", 1.0f);
	Mesh->bCollideWithEnvironment = true;
	Mesh->bShowPrePhysBones = true;
	Mesh->bBlendPhysics = true;

	//setup character movement
	UCharacterMovementComponent * CharacterMovement = Character->GetCharacterMovement();

	CharacterMovement->StopMovementImmediately();
	CharacterMovement->DisableMovement();
	CharacterMovement->SetComponentTickEnabled(false);
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//create physics handle
	UPhysicsHandleComponent * PhysicsHandle = NewObject<UPhysicsHandleComponent>(this);

	PhysicsHandle->RegisterComponent();
	PhysicsHandles.AddUnique(PhysicsHandle);
	PhysicsHandle->SetLinearStiffness(1000.f);
	
	PhysicsHandle->GrabComponentAtLocation(SkeletalMesh, SweepResult.BoneName, SkeletalMesh->GetBoneLocation(SweepResult.BoneName));

	//update character propoerties
	Character->SetActorTickEnabled(true);
	Character->bIsGrabbed = true;
	Character->bIsRagdoll = true;
	Character->bIsRecovering = false;
	Character->bIsPlayingGetUpAnim = false;
	Character->bShouldBeStill = false;
	Character->GrabbedBone = SweepResult.BoneName;
	PiercedEnemy = Character;

	SetLifeSpan(.35f);

	//helper
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, SweepResult.BoneName.ToString() + "was hit and is now dragged by Stake");
}

// Sets default values
AStake::AStake()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->GetBodyInstance()->SetCollisionProfileName("Projectile");
	//CollisionComponent->OnComponentHit.AddDynamic(this, &AStake::OnHit);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AStake::OnActorBeginOverlap);

	CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComponent->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComponent;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;


	InitialLifeSpan = 5.f;
}

// Called when the game starts or when spawned
void AStake::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStake::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float  vel = ProjectileMovement->Velocity.Size();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,"Projectile velocity is: " + FString::SanitizeFloat(vel) );
	for (UPhysicsHandleComponent * PhysicsHandle : PhysicsHandles)
	{
			PhysicsHandle->SetTargetLocation(CollisionComponent->GetComponentLocation());
	}
	CollisionComponent->AddForceAtLocation(FVector(1000.0f, 10.f, 10.0f), GetActorLocation());
	
}

void AStake::LifeSpanExpired()
{
	if (PiercedEnemy)
	{
		PiercedEnemy->bIsGrabbed = false;
		PiercedEnemy = nullptr;
	}
	Super::LifeSpanExpired();
}
