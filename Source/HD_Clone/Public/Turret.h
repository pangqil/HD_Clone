// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

class USkeletalMeshComponent;
class USphereComponent;
class UArrowComponent;
class ATurretProjectile;
class USoundWave;

UCLASS()
class HD_CLONE_API ATurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	 ATurret();
	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* TurretSM=nullptr;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* SphereComponent=nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* ArrowComponent=nullptr;

	UFUNCTION()
	void FindTarget();

	UPROPERTY(EditInstanceOnly, Category="Turret", meta=(ClampMin="1.0",ClampMax="2000.0",UIMin="1.0",UIMax="2000.0"))
	float SenseRange=1000.f;
	
	UPROPERTY(EditInstanceOnly, Category="Turret")
	bool EnableSphere=false;
	
	UPROPERTY(EditAnywhere, Category = "Turret")
	USoundBase* RotationSoundCue = nullptr;

	UPROPERTY(EditAnywhere, Category = "Turret")
	float InterpolationSpeed = 10.f;

	UPROPERTY(EditInstanceOnly, Category = "Fire")
	float FireRate = 1.f;

	
	
	UPROPERTY(EditInstanceOnly, Category = "Fire")
	TSubclassOf<ATurretProjectile> ProjectileActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Fire")
	USoundWave* FireSoundEffect = nullptr;
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret")
	// USkeletalMesh* DefaultMesh;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	bool bIsInDelayTime = false;
	bool bIsRotating = false;
	float RandValue = 0.f;
	
	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;
	
	UPROPERTY()
	TArray<AActor*> ActorIgnoreSphereOverlap;

	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	
	UPROPERTY()
	AActor* BestTarget = nullptr;

	UPROPERTY()
	UAudioComponent* RotationAC = nullptr;
	
	UPROPERTY()
	UAudioComponent* FireAC = nullptr;

	FTimerHandle TimerHandle;
	FTimerHandle FireTimerHandle;

	void RotateToTarget();

	void PlayRotateSound();
	void PlayFireSound();

	void IdleRotate(const float DeltaSecond);
	//void FireProjectile();

	float RotateValue = 0;

	FHitResult SightHitResult;
};
