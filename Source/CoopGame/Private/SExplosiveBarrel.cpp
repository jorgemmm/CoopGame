// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/SExplosiveBarrel.h"

#include "Public/SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

#include "CoopGame.h"

#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);

	BarrelMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelMeshComp"));
	BarrelMeshComp->SetSimulatePhysics(true);
	// Set to physics body to let radial component affect us(eg.when a nearby barrel explodes)
	BarrelMeshComp->SetCollisionObjectType(ECC_PhysicsBody);


	RootComponent = BarrelMeshComp;

	//BarrelMeshComp->SetCollisionResponseToChannel(COLLISION_MYWEAPONCHANNEL, ECR_Ignore);

	
	
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("ForceComp"));
	RadialForceComp->SetupAttachment(BarrelMeshComp);
	RadialForceComp->Radius = 250;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false; // Prevent component from ticking, and only use FireImpulse() instead
	RadialForceComp->bIgnoreOwningActor = true; // ignore self

	ExplosionImpulse = 400;
	
	BarrelBaseDamage = 200;
	
	BarrelDamageRadius = 4000;

}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();


	

	
	
}



void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if(bExploted)
	{
		return;
	}
	if(Health<=0.0f)
	{
		bExploted = true;

		// Boost the barrel upwards
		FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
		BarrelMeshComp->AddImpulse(BoostIntensity, NAME_None, true);


		//Play FX Explosion
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
		//Override material
		BarrelMeshComp->SetMaterial(0, ExplodedMaterial);

		//Blast nearby Physics actors
		RadialForceComp->FireImpulse();

		

		TArray<AActor*> IgnoreActors;

		UGameplayStatics::ApplyRadialDamage(GetWorld(), BarrelBaseDamage, GetActorLocation(), BarrelDamageRadius, BarrelDamageType, IgnoreActors,this, GetInstigatorController());
	}

}
