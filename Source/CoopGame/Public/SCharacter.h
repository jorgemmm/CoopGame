// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USHealthComponent;
class ASWeapon;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Handles input for moving forward and backward.
	UFUNCTION()
	void MoveForward(float Value);

	// Handles input for moving right and left.
	UFUNCTION()
	void MoveRight(float Value);

	// Sets jump flag when key is pressed.
	UFUNCTION()
	void StartJump();

	// Clears jump flag when key is released.
	UFUNCTION()
	void StopJump();

	UFUNCTION()
	void BeginCrouch();

	UFUNCTION()
	void EndCrouch();



	//CameraComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UCameraComponent* CameraComp;


	//CameraArmComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USHealthComponent* HealthComp;
	   	

	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player", meta=(ClampMin=0.1, ClampMax=100))
	float InterpSpeed;

	/*Default  FOV set during begin play */
	float DefaultFOV;

	UFUNCTION()
	void StartZoom();
	
	UFUNCTION()
	void StopZoom();

	//Handled the fire Weapon
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	UPROPERTY(Replicated)
    ASWeapon* CurrentWeapon;

	//Default Weapon load in beginPlay
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	FName WeaponSocketName;
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();
	/*UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EndFire();*/

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();
	
	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/* Pawn died previously */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bDied;

	//Ammo consumption
	UPROPERTY(Replicated,VisibleAnyWhere, BlueprintReadOnly, Category = "Player")
	int Ammo;	
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	void BeginReload();

	//UFUNCTION()
	//void EndReload();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	int TimeReloadClip;

	

	

	//FTimerHandle TH_TimeToReload;

	

	

public:	

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player", Meta = (AllowPrivateAccess = "true"))
	float Count;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player", Meta = (AllowPrivateAccess = "true"))
	bool bIsvisibleReloadBar;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	virtual FVector GetPawnViewLocation() const override;



	UFUNCTION(BlueprintCallable, Category = "Player")
	virtual void AddAmmo(int inAmmo);

	UFUNCTION(BlueprintPure, Category = "Player")
	virtual	int GetAmmo();

	UFUNCTION(BlueprintPure, Category = "Player")
	virtual	int GetClip();

	/*UFUNCTION(BlueprintPure, Category = "Player")
	virtual	int GetTimeReloadClipBar();*/

};
