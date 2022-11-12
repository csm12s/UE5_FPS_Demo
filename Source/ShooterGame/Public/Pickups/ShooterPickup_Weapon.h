// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterPickup.h"
#include "ShooterPickup_Weapon.generated.h"

class AShooterCharacter;
class AShooterWeapon;

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterPickup_Weapon : public AShooterPickup
{
	GENERATED_BODY()

	virtual bool CanBePickedUp(AShooterCharacter* TestPawn) const override;

protected:

	/** how much ammo does it give? */
	UPROPERTY(EditDefaultsOnly, Category=Pickup)
	int32 AmmoClips;

	UPROPERTY(EditDefaultsOnly, Category=Pickup)
	TSubclassOf<AShooterWeapon> WeaponType;

	/** give pickup */
	virtual void GivePickupTo(AShooterCharacter* Pawn) override;
};
