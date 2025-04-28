#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseRoom.generated.h"

class USceneComponent;
class UBoxComponent;
class UArrowComponent;
class UPaperSpriteComponent;

UCLASS()
class MYPROJECT_API ABaseRoom : public AActor
{
    GENERATED_BODY()

public:
    ABaseRoom();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;


    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Generation")
    int32 minSpawnsAmount = 1;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Generation")
    int32 maxSpawnsAmount = 10;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Generation")
    float chanceOfSpawnSpecialActor = 0.7f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> exitPoints;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> roomMeshes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UArrowComponent> entranceArrow;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> overlappingBounds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> spawnBounds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> specialSpawnPoints;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> lighting;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UBoxComponent> roomBounds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UBoxComponent> spawnBound;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> roomMap;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UPaperSpriteComponent> roomMapSprite;
};