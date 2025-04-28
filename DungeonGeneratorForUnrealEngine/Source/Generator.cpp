#include "Generator.h"
#include "BaseRoom.h"
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "MySaveGame.h"

AGenerator::AGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = root;
}

AGenerator::~AGenerator()
{
	ResetGeneratorState();

	normalRoomTypes.Empty();
	specialRoomTypes.Empty();
	specialPropTypes.Empty();
	doorTypes.Empty();
	standardPropTypes.Empty();
}

void AGenerator::BeginPlay()
{
	Super::BeginPlay();

	GenerateLevel(initialSeed, initialRoomCount);
}

void AGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGenerator::InitializeRandomGenerator()
{
	if (initialSeed == -1) {
		randomGenerator.GenerateNewSeed();
	}
	else {
		randomGenerator.Initialize(initialSeed);
	}
}

void AGenerator::GenerateStandardProps()
{
	if (!standardPropTypes.IsEmpty()) {

		TArray<USceneComponent*> children;
		lastGeneratedRoom->spawnBounds->GetChildrenComponents(false, children);

		TArray<UBoxComponent*> boxChildren;
		for (USceneComponent* child : children) {

			if (UBoxComponent* boxChild = Cast<UBoxComponent>(child)) {

				boxChildren.Add(boxChild);
			}
		}

		if (!boxChildren.IsEmpty()) {

			int32 randomSpawnsAmount = randomGenerator.RandRange(lastGeneratedRoom->minSpawnsAmount, lastGeneratedRoom->maxSpawnsAmount);

			for (int32 i = 0; i < randomSpawnsAmount; i++) {

				UBoxComponent* spawnBound = boxChildren[randomGenerator.RandRange(0, boxChildren.Num() - 1)];

				if (spawnBound) {

					FVector extent = spawnBound->GetScaledBoxExtent();

					float x = randomGenerator.FRandRange(-extent.X, extent.X);
					float y = randomGenerator.FRandRange(-extent.Y, extent.Y);

					FVector spawnLocation = spawnBound->GetComponentTransform().TransformPosition(FVector(x, y, extent.Z));

					TSubclassOf<AActor> randomlySelectedActor = standardPropTypes[randomGenerator.RandRange(0, standardPropTypes.Num() - 1)];

					AActor* spawnedActor = SpawnDynamicActor(randomlySelectedActor, FTransform(FRotator::ZeroRotator, spawnLocation, FVector::OneVector));
				}
			}
		}
	}
}

void AGenerator::GenerateSpecialProps()
{
	if (!specialPropTypes.IsEmpty()) {

		TArray<USceneComponent*> children;
		lastGeneratedRoom->specialSpawnPoints->GetChildrenComponents(false, children);

		if (randomGenerator.GetFraction() < lastGeneratedRoom->chanceOfSpawnSpecialActor
			&& !children.IsEmpty()) {

			for (USceneComponent* child : children) {

				TSubclassOf<AActor> randomlySelectedSpecialActor = specialPropTypes[randomGenerator.RandRange(0, specialPropTypes.Num() - 1)];

				SpawnDynamicActor(randomlySelectedSpecialActor, child->GetComponentTransform());
			}
		}
	}
}

void AGenerator::BeginGenerationTimeout()
{
	if (generationTimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(generationTimeoutHandle);
	}

	GetWorld()->GetTimerManager().SetTimer(
		generationTimeoutHandle,
		this,
		&AGenerator::ValidateGenerationCompletion,
		generationTimeoutSeconds,
		false
	);
}

void AGenerator::ValidateGenerationCompletion()
{
	UE_LOG(LogTemp, Error, TEXT("Generation timed out!"));

	ResetGeneratorState();
	GenerateLevel(-1, initialRoomCount);
}

void AGenerator::ResetGeneratorState()
{
	TArray<AActor*> roomsToDestroy;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), generatedActorTag, roomsToDestroy);

	for (AActor* room : roomsToDestroy)
	{
		room->Destroy();
	}

	availableRoomTypes.Empty();
	doorTypes.Empty();
	roomExitPoints.Empty();
	lastGeneratedRoom = nullptr;
	selectedExitComponent = nullptr;

	if (generationTimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(generationTimeoutHandle);
		generationTimeoutHandle.Invalidate();
	}
}

void AGenerator::GenerateLevel(int32 newSeed, int32 roomCount)
{
	if (roomCount > 0) {

		BeginGenerationTimeout();
		generationStartTime = FDateTime::UtcNow();
		initialSeed = newSeed;

		InitializeRandomGenerator();
		availableRoomTypes = normalRoomTypes;

		initialRoomCount = roomCount;
		remainingRooms = initialRoomCount;
		GenerateStartingRoom();
		GenerateNextRoom();
	}
}

void AGenerator::FinalizeGeneration()
{
	GenerateExitRoom();
	SealRemainingExits();
	SpawnDoorways();

	if (OnGenerationComplete.IsBound()) {

		FTimespan Duration = FDateTime::UtcNow() - generationStartTime;
		lastGenerationTimeMs = Duration.GetTotalMilliseconds();

		OnGenerationComplete.Broadcast(lastGenerationTimeMs);
	}

	if (generationTimeoutHandle.IsValid()) {

		GetWorld()->GetTimerManager().ClearTimer(generationTimeoutHandle);
		generationTimeoutHandle.Invalidate();
	}
}

void AGenerator::GenerateStartingRoom()
{
	ABaseRoom* spawnedRoom = SpawnRoomActor(startingRoomType, root->GetComponentTransform());

	if (spawnedRoom && spawnedRoom->exitPoints) {
		TArray<USceneComponent*> children;
		spawnedRoom->exitPoints->GetChildrenComponents(false, children);
		roomExitPoints.Append(children);
	}
}

void AGenerator::GenerateExitRoom()
{
	if (roomExitPoints.Num() == 0) return;

	const int32 maxAttempts = FMath::Min(10, roomExitPoints.Num());
	int32 attempts = 0;
	bool bSuccess = false;

	while (attempts < maxAttempts && !bSuccess)
	{
		selectedExitComponent = roomExitPoints[randomGenerator.RandRange(0, roomExitPoints.Num() - 1)];
		ABaseRoom* finish = SpawnRoomActor(exitRoomType, selectedExitComponent->GetComponentTransform());

		if (CheckRoomOverlap(finish)) {

			finish->Destroy();
		}
		else {

			roomExitPoints.Remove(selectedExitComponent);
			doorSpawnPoints.Add(selectedExitComponent);
			bSuccess = true;
		}

		attempts++;
	}
}

bool AGenerator::CheckRoomOverlap(ABaseRoom* room)
{
	if (!room || !room->roomBounds) return false;

	TArray<UPrimitiveComponent*> overlappingComponents;
	room->roomBounds->GetOverlappingComponents(overlappingComponents);

	for (UPrimitiveComponent* comp : overlappingComponents)
	{
		if (comp && comp->ComponentHasTag(roomCollisionTag) && comp->GetOwner() != room)
		{
			return true;
		}
	}
	return false;
}

void AGenerator::GenerateNextRoom()
{
	const int32 MAX_RETRIES = 50;
	if (currentGenerationAttempts >= MAX_RETRIES)
	{
		ValidateGenerationCompletion();
		return;
	}

	if (roomExitPoints.Num() == 0)
	{
		ValidateGenerationCompletion();
		return;
	}

	selectedExitComponent = roomExitPoints[randomGenerator.RandRange(0, roomExitPoints.Num() - 1)];

	if (availableRoomTypes.Num() == 0)
	{
		availableRoomTypes = normalRoomTypes;
	}

	TSubclassOf<ABaseRoom> randomlySelectedRoom = availableRoomTypes[randomGenerator.RandRange(0, availableRoomTypes.Num() - 1)];
	lastGeneratedRoom = SpawnRoomActor(randomlySelectedRoom, selectedExitComponent->GetComponentTransform());

	if (lastGeneratedRoom)
	{
		ValidateRoomPlacement();
	}
	else
	{
		currentGenerationAttempts++;
		GenerateNextRoom();
	}
}

void AGenerator::ValidateRoomPlacement()
{
	if (CheckRoomOverlap(lastGeneratedRoom))
	{
		currentGenerationAttempts++;

		if (lastGeneratedRoom)
		{
			lastGeneratedRoom->Destroy();
		}

		GenerateNextRoom();
	}
	else
	{
		currentGenerationAttempts = 0;
		remainingRooms -= 1;
		roomExitPoints.Remove(selectedExitComponent);
		doorSpawnPoints.Add(selectedExitComponent);

		if (lastGeneratedRoom)
		{
			TArray<USceneComponent*> exitChildren;
			lastGeneratedRoom->exitPoints->GetChildrenComponents(false, exitChildren);
			roomExitPoints.Append(exitChildren);

			GenerateStandardProps();
			GenerateSpecialProps();
		}

		if (remainingRooms > 0) {

			availableRoomTypes = (remainingRooms == 10) ? specialRoomTypes : normalRoomTypes;
			GenerateNextRoom();
		}
		else {

			FinalizeGeneration();
		}
	}
}

void AGenerator::SealRemainingExits()
{
	for (USceneComponent* exit : roomExitPoints)
	{
		SpawnDynamicActor(wallClosureType, exit->GetComponentTransform());
	}
}

void AGenerator::SpawnDoorways()
{
	for (USceneComponent* door : doorSpawnPoints)
	{
		if (doorTypes.Num() == 0) continue;

		TSubclassOf<AActor> randomlySelectedDoor = doorTypes[randomGenerator.RandRange(0, doorTypes.Num() - 1)];
		SpawnDynamicActor(randomlySelectedDoor, door->GetComponentTransform());
	}
}

void AGenerator::SaveLevel(FString saveName)
{
	UMySaveGame* saveGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

	saveGame->savedRoomAmount = initialRoomCount;
	saveGame->savedSeed = initialSeed;

	UGameplayStatics::SaveGameToSlot(saveGame, saveName, 0);
}

void AGenerator::LoadLevel(FString loadName)
{
	UMySaveGame* saveGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(loadName, 0));

	if (saveGame) {

		GenerateLevel(saveGame->savedSeed, saveGame->savedRoomAmount);
	}
}

AActor* AGenerator::SpawnDynamicActor(TSubclassOf<AActor> actorToSpawn, FTransform transform)
{
	if (!actorToSpawn) return nullptr;

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	spawnParams.Owner = this;
	spawnParams.Instigator = GetInstigator();

	AActor* spawnedActor = GetWorld()->SpawnActor<AActor>(actorToSpawn, transform, spawnParams);
	spawnedActor->Tags.Add(generatedActorTag);
	return spawnedActor;
}

ABaseRoom* AGenerator::SpawnRoomActor(TSubclassOf<ABaseRoom> roomToSpawn, FTransform transform)
{
	if (!roomToSpawn) return nullptr;

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParams.Owner = this;
	spawnParams.Instigator = GetInstigator();

	ABaseRoom* spawnedRoom = GetWorld()->SpawnActor<ABaseRoom>(roomToSpawn, transform, spawnParams);
	spawnedRoom->Tags.Add(generatedActorTag);
	return spawnedRoom;
}
