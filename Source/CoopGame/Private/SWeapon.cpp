// Fill out your copyright notice in the Description page of Project Settings.

//#include "SWeapon.h"

#include "Public/SWeapon.h"
#include "Public/SCharacter.h"

#include "CoopGame.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#include "Kismet/GameplayStatics.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/SpringArmComponent.h"

#include "GameFramework/Pawn.h"
#include "Camera/CameraShake.h"

#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/EngineTypes.h"

#include "TimerManager.h"

#include "Net/UnrealNetwork.h"


int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef   CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Dbug Lines for weapons"), 
	ECVF_Cheat);


// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  
	//You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Meshcomp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Meshcomp"));
	
	//Meshcomp->SetSimulatePhysics(true);
	
	RootComponent = Meshcomp;

	MuzzleSockeName = "Muzzle";//"MuzzleSocket";// mejor "Muzzle";
	TracerTargetName="BeamEnd";

	BaseDamage = 20.0f;
	RateOfFire = 600.0f;
	DispersionFactor = 0.5;
	Clip = 45;

	UE_LOG(LogTemp, Warning, TEXT("Weapon Initial Clip Load Up 45"));

	SetReplicates(true);

	//To Prevent lag or latency between Server and clients
	/*NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;*/


}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();


	TimeBetweenShots = (60 / RateOfFire);

	DispersionFactor = FMath::Clamp(DispersionFactor, 0.0f, 1.0f);
	DispersionDegree = FMath::Clamp(DispersionDegree, 0.0f, 90.0f);
	Dispersion = 180.0f - DispersionDegree;

}



void ASWeapon::OnRep_HitScanTrace()
{

	//Play cosmeic Effects

	PlayFireEffects(HitScanTrace.TraceTo);

	PlayImpactsEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}



void ASWeapon::Fire()
{
	
	//if(!HasAuthority())
	/*if (Role < ROLE_Authority) //provoca un crash
	{
		ServerFire();
	
	}*/
	

	//Trace the World, from pawn eyes to crooshair location
	if(Clip>0)
	{
		


		AActor* MyOwner = GetOwner();

		if (MyOwner != nullptr) 
		{


			FVector EyeLocation;
			FRotator EyeRotation;

			MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

			//FRotator SpawnRotation = EyeRotation;
			DispersionFactor /= 10;

			EyeRotation.Pitch += (FMath::FRandRange(-1, 1) * DispersionFactor) *  Dispersion;
			EyeRotation.Yaw +=  (FMath::FRandRange(-1, 1) *  DispersionFactor)  *  Dispersion;   //FMath::FRand() * 15.0f;
			EyeRotation.Roll += (FMath::FRandRange(-1, 1) * DispersionFactor)  *  Dispersion;   //FMath::FRand() *15.0f;


			FVector ShotDirection = EyeRotation.Vector();
			FVector TraceEnd = EyeLocation + (ShotDirection * 2000);

			FCollisionQueryParams QueryParams;

			QueryParams.AddIgnoredActor(MyOwner);
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = true;

			//Particle "Target" Parameter
			FVector TracerEndPoint = TraceEnd;

			EPhysicalSurface SurfaceType = SurfaceType_Default;

			FHitResult Hit;
			if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd,
				//ECC_Visibility, 
				//ECC_GameTraceChannel1,
				COLLISION_MYWEAPONCHANNEL,  //Defining en CoopGame.h
				QueryParams))
			{
				//Blocking hit!! Process Dammage and Impàct Effects

				AActor* HicActor = Hit.GetActor();

				//UGameplayStatics::ApplyDamage(, , , );//Para aplicar daño normal o con veneno e.g.
				SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

				if (SurfaceType < 0) UE_LOG(LogTemp, Error, TEXT("SurfaceType is not Got it"));

				float ActualDamage = BaseDamage;
				//A critical Shot
				if ((SurfaceType == SURFACE_FLESHVULNERABLE) || (SurfaceType == SURFACE_FLESHSEMIVULNERABLE))
				{
					ActualDamage *= 2.0f;
					//A head ccritical shot
					if (SurfaceType == SURFACE_FLESHVULNERABLE) ActualDamage *= 2.0f;
				}

				//in TL Course
			//switch (SurfaceType)
			//	{
			//	case  SURFACE_FLESHDEFAULT:
			//		//FleashDefault: Body
			//		/*SelectedEffect = FleshImpactEffect;
			//		break;*/
			//	case   SURFACE_FLESHVULNERABLE:
			//		//FleshVulnerable: Head-> Dead
			//		SelectedEffect = FleshImpactEffect;
			//		break;
			//	case  SURFACE_FLESHSEMIVULNERABLE:
			//		//FleshSemiVulnerable -> Arms and tous 
			//		SelectedEffect = FleshImpactEffect;
			//		break;
			//	default:
			//		SelectedEffect = DefaultImpactEffect;
			//		break;
			//	}


				UGameplayStatics::ApplyPointDamage(HicActor, ActualDamage, ShotDirection, Hit,
					MyOwner->GetInstigatorController(),
					MyOwner,//this,
					DamageType);

				/*if (FireSound != nullptr)
					UGameplayStatics::PlaySound2D(this, FireSound);
				else UE_LOG(LogTemp, Error, TEXT("FireSound is null or none"));*/

				
				PlayImpactsEffects(SurfaceType, Hit.ImpactPoint);//, Hit.ImpactNormal.Rotation());

				TracerEndPoint = Hit.ImpactPoint;
			}

			PlayFireEffects(TracerEndPoint);

			if(Role == ROLE_Authority)
			{

				HitScanTrace.TraceTo = TracerEndPoint;
				
				HitScanTrace.SurfaceType = SurfaceType;
			}

			Clip -= 1;
			
			if (DebugWeaponDrawing > 0)
			{
				DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
			}

			LastFireTime = GetWorld()->GetTimeSeconds();
			//Recoil();

			
		}
		
       else
		{
			UE_LOG(LogTemp, Error, TEXT("Weapon doesn´t found Owner or is none"));
		}

		


	}
	else
	{
	  //Sonido sin balas
	  UGameplayStatics::PlaySound2D(this, NoBulletSound);
	}

	



}



void ASWeapon::ServerFire_Implementation()
{
	Fire(); 
}

bool ASWeapon::ServerFire_Validate()
{
	return true;

}



void ASWeapon::Recoil()
{
	/*const float ForceAmount = 20000.0f;
	Meshcomp->AddForce(FVector(0.0f, 0.0f, ForceAmount));*/
	//bIsRecoil = true;
}

void ASWeapon::Reload(int MaxAmmoLoadinClip)
{
	//1 player
	ASCharacter* MyPlayer = Cast<ASCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

		if ( MyPlayer != nullptr )
		{   
			//Reload
			UGameplayStatics::PlaySound2D(this, ReloadSound);
			 //No recargamos hast que la recámara tenga un hueco
			if(Clip< MaxAmmoLoadinClip)
			{
			   
				//Does the Ammo player fill the clip?
				//Tenemos munición para rellenar el cargador?
				if(MyPlayer->GetAmmo() >= MaxAmmoLoadinClip)
				{			
				   //Tenemos mucho más munición que la recámara ó cargador (clip)
					//Player->GetAmmo() >> Clip
					for (size_t i = Clip; i < MaxAmmoLoadinClip; i++)
					{
						Clip += 1;
						MyPlayer->AddAmmo(-1);

					}

				}
				else
				{					
					//Tenemos menos munición que  los huecos de la recámara o cargador
					//Player->GetAmmo() << MaxAmmoLoadinClip
					for (size_t i = Clip ; i < MyPlayer->GetAmmo(); i++)
					{
						Clip += 1;
						MyPlayer->AddAmmo(-1);
					}

				}

			}
			
			//Clip always positive or zero
			if (Clip <= 0) {
				Clip = 0;
			}



		}
		else  UE_LOG(LogTemp, Error, TEXT("Players doesn´t found Owner or is none"));
		

}

void ASWeapon::StartFire()
{

	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->GetTimeSeconds(), 0.0f);
		//GetWorld()->GetTimeSeconds(), 		
		
	
	GetWorldTimerManager().SetTimer(TH_TimeBetweenShots,this, &ASWeapon::Fire, TimeBetweenShots,true, FirstDelay); //in TL Course

	//GetWorldTimerManager().SetTimer(TH_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true);



}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TH_TimeBetweenShots);

}


void ASWeapon::PlayFireEffects(FVector TracerEnd)
{


	if (FireSound != nullptr)  UGameplayStatics::PlaySound2D(this, FireSound);
	else UE_LOG(LogTemp, Error, TEXT("FireSound is null or none"));
		 


	//Fuego en la bocacha
	if (MuzzleEffect!=nullptr)   UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, Meshcomp, MuzzleSockeName);		
	else  UE_LOG(LogTemp, Error, TEXT("Muzzle Particle System is null or none"));		

	//Trazo del disparo
	if (TracerEffect!=nullptr)
	{
		FVector MuzzleLocation = Meshcomp->GetSocketLocation(MuzzleSockeName);
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);

		if (TracerComp!=nullptr )   TracerComp->SetVectorParameter(TracerTargetName, TracerEnd);
		else   UE_LOG(LogTemp, Error, TEXT("Tracer Particle System Not Spwaned"));
	}
	else  UE_LOG(LogTemp, Error, TEXT("Tracer Particle System is null or none"));
	
		

	

	
		APawn* MyOwner = Cast<APawn>(GetOwner());
		if (MyOwner != nullptr)
		{

			APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		

			if (PC != nullptr)
			{
				if (FireCamShake) PC->ClientPlayCameraShake(FireCamShake);
				else UE_LOG(LogTemp, Error, TEXT("FireCamShake is null or none"));
			}
			else  UE_LOG(LogTemp, Error, TEXT("Player Controller is null or none"));

		}
		else   UE_LOG(LogTemp, Error, TEXT("Pawn Owner is null or none"));
	
	
	
	

}

void ASWeapon::PlayImpactsEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)//, FRotator ImpactNormal)
{
		UParticleSystem* SelectedEffect = DefaultImpactEffect; //nullptr;



					   //Defining en CoopGame.h
		if ((SurfaceType == SURFACE_FLESHDEFAULT) ||			 
			(SurfaceType == SURFACE_FLESHSEMIVULNERABLE)
			)
		{
			SelectedEffect = FleshImpactEffect;
		}
		if(SurfaceType == SURFACE_FLESHVULNERABLE)
		{
			SelectedEffect = FleshImpactVulnerableEffect;
		}

		if (SelectedEffect != nullptr)
		{
			FVector MuzzleLocation = Meshcomp->GetSocketLocation(MuzzleSockeName);
			FVector ShotDirection = ImpactPoint - MuzzleLocation;
			ShotDirection.Normalize();

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint,
				ShotDirection.Rotation());
				//ImpactNormal.Rotation()); Pasaríamos un parámetro más
				


		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ImpactEffect Particle System is null or none"));
		}

		

}



void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	

	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
	DOREPLIFETIME(ASWeapon, Clip);
}

