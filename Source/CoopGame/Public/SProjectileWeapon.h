// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASProjectileWeapon : public ASWeapon
{
	GENERATED_BODY()




protected:

	//Handled the fire Weapon
	//UFUNCTION(BlueprintCallable, Category = "Components")
	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectileWeapon")
	TSubclassOf<AActor> ProjectileClass;

	/**Un valor rntre 0 y 90 grados--- a value from 0 to 90 degrees  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectileWeapon")
	float RangeSkill;


};
