// Fill out your copyright notice in the Description page of Project Settings.

#include "CodeTest.h"
#include "Stake.h"
#include "GameFramework/ProjectileMovementComponent.h"


void AStake::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	AMyEnemy * Character = Cast<AMyEnemy>(OtherActor);
	USkeletalMeshComponent * SkeletalMesh = Cast<USkeletalMeshComponent>(OtherComponent);

	if (Character == nullptr || SkeletalMesh == nullptr || (OtherActor != nullptr && IgnoreActors.Contains(OtherActor)))
		return;

	IgnoreActors.AddUnique(OtherActor);

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	AirDensity *= Character->StakeResistanceFactor;
	Mass += SkeletalMesh->GetMass();
	

	//setup mesh physics behavior
	float mas = SkeletalMesh->GetMass();
	SkeletalMesh->Activate();
	SkeletalMesh->SetSimulatePhysics(true);
	SkeletalMesh->WakeAllRigidBodies();
	SkeletalMesh->SetAllBodiesBelowSimulatePhysics("pelvis", true);
	SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("pelvis", 1.0f);
	SkeletalMesh->bCollideWithEnvironment = true;
	SkeletalMesh->bShowPrePhysBones = true;
	SkeletalMesh->bBlendPhysics = true;

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


	PhysicsHandle->GrabComponentAtLocation(SkeletalMesh, Hit.BoneName, SkeletalMesh->GetBoneLocation(Hit.BoneName));

	//update character properties
	Character->SetActorTickEnabled(true);
	Character->bIsRagdoll = true;
	Character->bIsRecovering = false;
	Character->bIsPlayingGetUpAnim = false;
	Character->bShouldBeStill = false;
	Character->GrabbedBone = Hit.BoneName;
	Character->currStake = this;
	PiercedEnemy = Character;

	SetLifeSpan(.35f);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, Hit.BoneName.ToString() + "was hit and is now dragged by Stake");
}

//Use Physics Handle to simulate movement with projectile
void AStake::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
 	AMyEnemy * Character = Cast<AMyEnemy>(OtherActor);
	USkeletalMeshComponent * SkeletalMesh = Cast<USkeletalMeshComponent>(OtherComp);

	if (Character == nullptr || SkeletalMesh == nullptr || (OtherActor != nullptr && IgnoreActors.Contains(OtherActor)))
		return;

	IgnoreActors.AddUnique(OtherActor);


	USkeletalMeshComponent * Mesh = Character->GetMesh();

	//setup mesh physics behavior
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
	
	PhysicsHandle->GrabComponentAtLocation(SkeletalMesh, SweepResult.BoneName, SkeletalMesh->GetBoneLocation(SweepResult.BoneName));

	//update character properties
	Character->SetActorTickEnabled(true);
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
	CollisionComponent->OnComponentHit.AddDynamic(this, &AStake::OnHit);
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

	ResistingArea = 3.141592 * FMath::Pow((CollisionComponent->GetScaledSphereRadius()), 2.0f);
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

	float velo = this->GetVelocity().Size();

	//for some reason after hit event speed goes to max so to suppress that I set max speed in every tick
	ProjectileMovement->MaxSpeed = velo;

	//compute drag force from drag force equation, divide by million to switch from meters to cm
	float drag = 0.5f * AirDensity * FMath::Pow(velo, 2.0) * DragCoefficent * ResistingArea / 1000000;

	//apply drag force
	CollisionComponent->AddForceAtLocation(-CollisionComponent->GetForwardVector() * drag, GetActorLocation());

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,"Projectile velocity is: " + FString::SanitizeFloat(velo) + "drag is: " + FString::SanitizeFloat(drag) );
	for (UPhysicsHandleComponent * PhysicsHandle : PhysicsHandles)
	{
			AMyEnemy* enemy = Cast<AMyEnemy>(PhysicsHandle->GetGrabbedComponent()->GetOwner());
			//this allows for only one impalement but prevent ragdoll twitching
			//ragdoll twitching is caused by ragdoll being grabbed after stake goes through the wall, this could be improved...
			if (enemy && !enemy->bIsImpaled)
			{
				PhysicsHandle->SetTargetLocation(CollisionComponent->GetComponentLocation());
				CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
	}
	KineticEnergy = Mass * FMath::Pow(velo, 2.0) * 0.5f / 10000;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, "Projectile kinetic energy is: " + FString::SanitizeFloat(KineticEnergy));

}

void AStake::LifeSpanExpired()
{
	if (PiercedEnemy)
	{
		PiercedEnemy->currStake = nullptr;
		PiercedEnemy = nullptr;
	}
	Super::LifeSpanExpired();
}
