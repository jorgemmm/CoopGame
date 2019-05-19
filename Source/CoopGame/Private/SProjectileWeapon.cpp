// Fill out your copyright notice in the Description page of Project Settings.

//#include "SProjectileWeapon.h"
#include "Public/SProjectileWeapon.h"

#include "Public/SCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Public/SWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"



void ASProjectileWeapon::Fire()
{
	//Super::Fire();

	RangeSkill = FMath::Clamp(RangeSkill,0.0f, 90.0f);
	float Dispersion = 180.0f - RangeSkill;
	

	AActor* MyOwner = GetOwner();
	if (MyOwner!=nullptr && ProjectileClass)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);	
		
		
		FVector MuzzleLocation = Meshcomp->GetSocketLocation(MuzzleSockeName);
		
		// Get a random rotation for the spawned item
		FRotator SpawnRotation = EyeRotation;
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		//Without dispersion
		/*		
		SpawnRotation.Pitch += 10.0f;
		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, SpawnRotation, SpawnParams);
		*/


		//With dispersion
		
		// Multiply Dispersion by 0.1 - 0.0 ( = 18º- 9º)
		SpawnRotation.Pitch =  (FMath::FRand() * 0.1f ) *  Dispersion;
		SpawnRotation.Yaw +=   (FMath::FRand() / 10.0f ) * Dispersion;   //FMath::FRand() * 15.0f;
		SpawnRotation.Roll +=  (FMath::FRand() / 10.0f ) * Dispersion;   //FMath::FRand() *15.0f;
	
		
		
		//SpawnParams.Owner = this;
		//SpawnParams.Instigator = Instigator;
		

		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, SpawnRotation, SpawnParams);
	
		Recoil();

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon doesn´t found Owner or is none"))
		UE_LOG(LogTemp, Error, TEXT("Or Weapon doesn´t found ProjectileClass or is none"))
	}


}
