#include "BaseRoom.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "PaperSpriteComponent.h"

ABaseRoom::ABaseRoom()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = root;

    exitPoints = CreateDefaultSubobject<USceneComponent>(TEXT("Exits"));
    exitPoints->SetupAttachment(RootComponent);
    exitPoints->SetRelativeLocation(FVector(1000.f, 0.f, 0.f));

    specialSpawnPoints = CreateDefaultSubobject<USceneComponent>(TEXT("Spacial Spawn Points"));
    specialSpawnPoints->SetupAttachment(RootComponent);
    specialSpawnPoints->SetRelativeLocation(FVector(1000.f, 0.f, 0.f));

    spawnBounds = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Bounds"));
    spawnBounds->SetupAttachment(RootComponent);
    spawnBounds->SetRelativeLocation(FVector(1000.f, 0.f, 0.f));

    entranceArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Entrace"));
    entranceArrow->SetupAttachment(RootComponent);
    entranceArrow->SetArrowSize(8.f);
    entranceArrow->SetArrowColor(FLinearColor::White);

    roomMeshes = CreateDefaultSubobject<USceneComponent>(TEXT("Meshes"));
    roomMeshes->SetupAttachment(RootComponent);
    roomMeshes->SetRelativeLocation(FVector(1000.f, 0.f, 0.f));

    overlappingBounds = CreateDefaultSubobject<USceneComponent>(TEXT("Overlapping Bounds"));
    overlappingBounds->SetupAttachment(RootComponent);
    overlappingBounds->SetRelativeLocation(FVector(1000.f, 0.f, 0.f));

    roomBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Bound"));
    roomBounds->SetupAttachment(overlappingBounds);
    roomBounds->SetBoxExtent(FVector(900.f, 900.f, 32.f));
    roomBounds->SetCollisionProfileName(TEXT("OverlapAll"));
    roomBounds->SetGenerateOverlapEvents(true);
    roomBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    roomBounds->ComponentTags.Add("RoomBound");

    spawnBound = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Bound"));
    spawnBound->SetupAttachment(spawnBounds);
    spawnBound->SetBoxExtent(FVector(900.f, 900.f, 50.f));
    spawnBound->SetCollisionProfileName(TEXT("NoCollision"));
    spawnBound->SetGenerateOverlapEvents(false);

    lighting = CreateDefaultSubobject<USceneComponent>(TEXT("Lighting"));
    lighting->SetupAttachment(RootComponent);
    lighting->SetRelativeLocation(FVector(1000.f, 0.f, 0.f));

    roomMap = CreateDefaultSubobject<USceneComponent>(TEXT("Room Map"));
    roomMap->SetupAttachment(RootComponent);
    roomMap->SetRelativeLocation(FVector(1000.f, 0.f, 400.f));

    roomMapSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Room Map Sprite"));
    roomMapSprite->SetupAttachment(roomMap);
    roomMapSprite->SetCollisionProfileName("NoCollision");
    roomMapSprite->bVisibleInRayTracing = false;
    roomMapSprite->bVisibleInRealTimeSkyCaptures = false;
    roomMapSprite->bVisibleInReflectionCaptures = false;
    roomMapSprite->bVisibleInSceneCaptureOnly = true;
}

void ABaseRoom::BeginPlay()
{
    Super::BeginPlay();
}

void ABaseRoom::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}