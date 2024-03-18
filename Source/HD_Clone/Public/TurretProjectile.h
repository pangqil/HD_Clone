// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretProjectile.generated.h"

UCLASS()
class HD_CLONE_API ATurretProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurretProjectile();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skill")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skill")
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skill")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category="Effects")
	class UParticleSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	float Damage;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
