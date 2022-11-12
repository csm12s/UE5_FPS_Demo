// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Pickups/ShooterPickup_Weapon.h"
#include "Weapons/ShooterWeapon.h"
#include "OnlineSubsystemUtils.h"


bool AShooterPickup_Weapon::CanBePickedUp(AShooterCharacter* TestPawn) const
{
	AShooterWeapon* TestWeapon = (TestPawn ? TestPawn->FindWeapon(WeaponType) : NULL);
	if (TestWeapon == NULL)
	{
		return true;
	}

	return false;
}

void AShooterPickup_Weapon::GivePickupTo(class AShooterCharacter* Pawn)
{
	AShooterWeapon* findWeapon = (Pawn ? Pawn->FindWeapon(WeaponType) : NULL);
	if (!findWeapon)
	{
		// Fire event for collected ammo
		if (Pawn)
		{
			//AShooterWeapon* newWeapon = Cast<AShooterWeapon>(WeaponType);
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AShooterWeapon* newWeapon = GetWorld()->SpawnActor<AShooterWeapon>(WeaponType, SpawnInfo);
			int32 Qty = AmmoClips * newWeapon->GetAmmoPerClip();
			newWeapon->GiveAmmo(Qty);

			Pawn->AddWeapon(newWeapon);


			// Server items:
			const UWorld* World = GetWorld();
			const IOnlineEventsPtr Events = Online::GetEventsInterface(World);
			const IOnlineIdentityPtr Identity = Online::GetIdentityInterface(World);

			if (Events.IsValid() && Identity.IsValid())
			{							
				AShooterPlayerController* PC = Cast<AShooterPlayerController>(Pawn->Controller);
				if (PC)
				{
					ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);

					if (LocalPlayer)
					{
						const int32 UserIndex = LocalPlayer->GetControllerId();
						TSharedPtr<const FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);			
						if (UniqueID.IsValid())
						{
							FVector Location = Pawn->GetActorLocation();

							FOnlineEventParms Params;		

							// Weapon
							Params.Add( TEXT( "SectionId" ), FVariantData( (int32)0 ) ); // unused
							Params.Add( TEXT( "GameplayModeId" ), FVariantData( (int32)1 ) ); // @todo determine game mode (ffa v tdm)
							Params.Add( TEXT( "DifficultyLevelId" ), FVariantData( (int32)0 ) ); // unused

							Params.Add( TEXT( "ItemId" ), FVariantData( (int32)newWeapon->GetAmmoType() + 1000 ) ); // @todo come up with a better way to determine item id, currently health is 0 and ammo counts from 1
							Params.Add( TEXT( "AcquisitionMethodId" ), FVariantData( (int32)0 ) ); // unused
							Params.Add( TEXT( "LocationX" ), FVariantData( Location.X ) );
							Params.Add( TEXT( "LocationY" ), FVariantData( Location.Y ) );
							Params.Add( TEXT( "LocationZ" ), FVariantData( Location.Z ) );
							Params.Add( TEXT( "ItemQty" ), FVariantData( 1 ) );	

							// Ammo
							Params.Add( TEXT( "SectionId" ), FVariantData( (int32)0 ) ); // unused
							Params.Add( TEXT( "GameplayModeId" ), FVariantData( (int32)1 ) ); // @todo determine game mode (ffa v tdm)
							Params.Add( TEXT( "DifficultyLevelId" ), FVariantData( (int32)0 ) ); // unused

							Params.Add( TEXT( "ItemId" ), FVariantData( (int32)newWeapon->GetAmmoType() + 1 ) ); // @todo come up with a better way to determine item id, currently health is 0 and ammo counts from 1
							Params.Add( TEXT( "AcquisitionMethodId" ), FVariantData( (int32)0 ) ); // unused
							Params.Add( TEXT( "LocationX" ), FVariantData( Location.X ) );
							Params.Add( TEXT( "LocationY" ), FVariantData( Location.Y ) );
							Params.Add( TEXT( "LocationZ" ), FVariantData( Location.Z ) );
							Params.Add( TEXT( "ItemQty" ), FVariantData( (int32)Qty ) );		

							Events->TriggerEvent(*UniqueID, TEXT("CollectPowerup"), Params);
						}
					}
				}
			}
		}
	}
}

