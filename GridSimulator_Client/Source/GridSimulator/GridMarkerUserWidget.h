// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GridMarkerUserWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class GRIDSIMULATOR_API UGridMarkerUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SendStartEndDateTimesToServer();

	void SetMarkerID(int32 InTargetId);
    int32 GetMarkerID() { return MarkerIndex; }
    void SetTargetPosition(FVector2D InTargetPosition) { TargetPosition = FVector(InTargetPosition, 0.0f); }
    void SetCurrentPosition(FVector2D InCurrentPosition) { CurrentPosition = FVector(InCurrentPosition, 0.0f); }
    void UpdatePosition(float InDeltaTime);
    void SetEndTargetRowColumn(int32 InRow, int32 InColumn);
    void SetTargetRowColumn(int32 InRow, int32 InColumn);
    int32 GetTargetRow() { return TargetRow; }
    int32 GetTargetColumn() { return TargetColumn; }
    void SetCurrentRowColumn(int32 InRow, int32 InColumn);
    int32 GetCurrentRow() { return CurrentRow; }
    int32 GetCurrentColumn() { return CurrentColumn; }
    void SetNextPosition();
    void SetPath(TArray<TPair<int32, int32>> InPath);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Marker", meta = (BindWidget))
	UTextBlock* MarkerID = nullptr;

    int32 MarkerIndex = 0;
    FVector CurrentPosition = FVector(0.0f, 0.0f, 0.0f);
    FVector TargetPosition = FVector(0.0f, 0.0f, 0.0f);

    float MaxSpeed = 100.0f;  // 최대 속도 (cm/s)
    float Acceleration = 10.0f;  // 가속도 (cm/s^2)
    float MaxRotationSpeed = 10.0f;  // 최대 회전 각속도 (도/s)

    FVector CurrentVelocity = FVector(0.0f, 0.0f, 0.0f);
    FRotator CurrentRotation;

    int32 CurrentRow = 0;
    int32 CurrentColumn = 0;
    int32 TargetRow;
    int32 TargetColumn;
    int32 EndTargetRow;
    int32 EndTargetColumn;
    int32 CurrentPathIndex = 1;

    TArray<TPair<int32, int32>> Path;
    bool NextFlag = false;
    bool IsEnd = false;

    UPROPERTY()
    FDateTime MoveStartTime;

    UPROPERTY()
    FDateTime MoveEndTime;
};
