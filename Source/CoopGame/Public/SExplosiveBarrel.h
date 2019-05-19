// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"


class UStaticMeshComponent;
class USHealthComponent;
class URadialForceComponent;
class UMaterialInterface;
class UDamageType;

UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/** Static mesh to represent the Barrel in the level*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BarrelMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	URadialForceComponent* RadialForceComp;
	
	/**Damage Barrel*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelExplosion")
	TSubclassOf <UDamageType>  BarrelDamageType;
	


	/** Force Strength of the explesion */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelExplosion")
	float ExplosionImpulse;

	/**Solo para debug*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelExplosion")
	bool bExploted;

	//Apply Dammage
	/**Barrel Base Damage to affect to area */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelExplosion")
	float BarrelBaseDamage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelExplosion")
	float BarrelDamageRadius;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	/* Particle to play when health reached zero */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* ExplosionEffect;

	/* The material to replace the original on the mesh once exploded (a blackened version) */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UMaterialInterface* ExplodedMaterial;




public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	/** Return the mesh for the Mesh */
	
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return BarrelMeshComp; }
	
	

};
