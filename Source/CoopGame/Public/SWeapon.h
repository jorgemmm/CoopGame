// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	
	UPROPERTY()
	FVector_NetQuantize TraceTo;

};

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	virtual void BeginPlay() override;
	
	//CameraArmComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Meshcomp;	

	

	//Damage Weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class UDamageType>  DamageType;
	
	//in tl course
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon") 
	FName MuzzleSockeName;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon") 
	FName TracerTargetName;
	
	/* EFuego en la bocacha*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* MuzzleEffect;

	/* FX Impact Fire */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* FleshImpactVulnerableEffect;

	/* FX Tracer Fire */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* TracerEffect;

	//Retroceso del Arma 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float BaseDamage;

	FTimerHandle TH_TimeBetweenShots;

	//Bullets per minuts
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float RateOfFire;

	float LastFireTime;

	float TimeBetweenShots;

	//Dispersión

	/**Un valor rntre 0 y 90 grados--- a value from 0 to 90 degrees  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float DispersionDegree;

	/**Un valor rntre 0 y 1--- a value from 0 to 1 1  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float DispersionFactor;
	
	float Dispersion;

	/**Fire Sound when shots  */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* FireSound;
	/**Weapon Sound whe Clip is empty  */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* NoBulletSound;
	/**Weapon Sound when is reloading  */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* ReloadSound;
	
	/** Play a FX Recoil when Fire the weapon */
	UFUNCTION()
	virtual void Recoil();

	//Play FX Fire Effect 
	UFUNCTION()
	void PlayFireEffects(FVector TracerEnd);
	
	//Play FX Fire Effect 
	UFUNCTION()
	void PlayImpactsEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);//, FRotator ImpactNormal);
	
	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();	

	//UFUNCTION(BlueprintCallable, Category = "Components")
	//UFUNCTION(Client, Unreliable)
	virtual void Fire();
	
	
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerFire();



	/* Como lo haría yo
		UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf <class UParticleSystem>  MuzzleEffect;*/
	
	//Quitarlo del constructor;
	/*UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName MuzzleSockeName = "Muzzle";
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName TracerTargetName = "Target"; */


	

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;
	//Ammo consumption

	UPROPERTY(Replicated,EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int Clip;

	UFUNCTION(BlueprintCallable, Category = "Components")
	virtual void Reload(int MaxAmmoLoadinClip);
	//Handled the fire Weapon
	UFUNCTION(BlueprintCallable, Category = "Components")
	virtual void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Components")
	virtual void StopFire();

	/** Return the mesh for the pickup */
	FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return Meshcomp; }
	
};
