#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

UCLASS()
class MYPROJECT_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadOnly, Category = "Save Game")
    int32 savedSeed; 

    UPROPERTY(BlueprintReadOnly, Category = "Save Game")
    int32 savedRoomAmount;

    UPROPERTY(BlueprintReadOnly, Category = "Save Game")
    TArray<FString> geenratedActors;
};
