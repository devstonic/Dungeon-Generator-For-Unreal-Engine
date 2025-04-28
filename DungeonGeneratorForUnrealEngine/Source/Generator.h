#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Generator.generated.h"

class ABaseRoom;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGenerationComplete,float,generationTimeMS);

UCLASS()
class MYPROJECT_API AGenerator : public AActor
{
    GENERATED_BODY()

public:

    AGenerator();
    ~AGenerator();

protected:

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Generation|Configuration")
    FName roomCollisionTag = "RoomBound";

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Generation|Configuration")
    FName generatedActorTag = "GeneratedActor";


    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Generation|Initialization")
    int32 initialSeed = -1;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Generation|Initialization")
    int32 initialRoomCount = 10;


    UPROPERTY(BlueprintReadOnly, Category = "Runtime|State")
    int32 remainingRooms = -1;


    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Development|Debug")
    FTimerHandle generationTimeoutHandle;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Development|Debug")
    float generationTimeoutSeconds = 20.f;


    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Assets|Rooms")
    TArray<TSubclassOf<ABaseRoom>> normalRoomTypes;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Assets|Rooms")
    TArray<TSubclassOf<ABaseRoom>> specialRoomTypes;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Assets|Rooms")
    TSubclassOf<ABaseRoom> startingRoomType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Assets|Rooms")
    TSubclassOf<ABaseRoom> exitRoomType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Assets|Props")
    TArray<TSubclassOf<AActor>> specialPropTypes;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Assets|Props")
    TSubclassOf<AActor> wallClosureType;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Assets|Props")
    TArray<TSubclassOf<AActor>> doorTypes;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Assets|Props")
    TArray<TSubclassOf<AActor>> standardPropTypes;


    UPROPERTY(BlueprintReadOnly, Category = "Runtime|State")
    TArray<TObjectPtr<USceneComponent>> roomExitPoints;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime|State")
    TArray<TObjectPtr<USceneComponent>> doorSpawnPoints;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime|State")
    TArray<TSubclassOf<ABaseRoom>> availableRoomTypes;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime|State")
    TObjectPtr<ABaseRoom> lastGeneratedRoom;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime|State")
    TObjectPtr<USceneComponent> selectedExitComponent;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime|State")
    FRandomStream randomGenerator;


    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USceneComponent> root;


    UPROPERTY(Transient)
    FDateTime generationStartTime;

    UPROPERTY(Transient)
    int32 currentGenerationAttempts = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime|Statistics")
    float lastGenerationTimeMs = 0.f;

public:

    UFUNCTION(BlueprintCallable, Category = "Generation|Initialization")
    void InitializeRandomGenerator();

    UFUNCTION(BlueprintCallable, Category = "Generation|Props")
    void GenerateStandardProps();

    UFUNCTION(BlueprintCallable, Category = "Generation|Props")
    void GenerateSpecialProps();

    UFUNCTION(BlueprintCallable, Category = "Generation")
    void BeginGenerationTimeout();

    UFUNCTION(BlueprintCallable, Category = "Generation|Validation")
    void ValidateGenerationCompletion();

    UFUNCTION(BlueprintCallable, Category = "Generation|Management")
    void ResetGeneratorState();

    UFUNCTION(BlueprintCallable, Category = "Generation|Management")
    void GenerateLevel(int32 newSeed, int32 roomCount);

    UFUNCTION(BlueprintCallable, Category = "Generation|Rooms")
    void GenerateStartingRoom();

    UFUNCTION(BlueprintCallable, Category = "Generation|Rooms")
    void GenerateExitRoom();

    UFUNCTION(BlueprintCallable, Category = "Generation|Collision")
    bool CheckRoomOverlap(ABaseRoom* room);

    UFUNCTION(BlueprintCallable, Category = "Generation|Rooms")
    void GenerateNextRoom();

    UFUNCTION(BlueprintCallable, Category = "Generation|Validation")
    void ValidateRoomPlacement();

    UFUNCTION(BlueprintCallable, Category = "Generation|Finalization")
    void FinalizeGeneration();

    UFUNCTION(BlueprintCallable, Category = "Generation|Finalization")
    void SealRemainingExits();

    UFUNCTION(BlueprintCallable, Category = "Generation|Finalization")
    void SpawnDoorways();

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    void SaveLevel(FString saveName);

    UFUNCTION(BlueprintCallable, Category = "Persistence")
    void LoadLevel(FString loadName);

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGenerationComplete OnGenerationComplete;

private:

    AActor* SpawnDynamicActor(TSubclassOf<AActor> actorToSpawn, FTransform transform);
    ABaseRoom* SpawnRoomActor(TSubclassOf<ABaseRoom> roomToSpawn, FTransform transform);
};