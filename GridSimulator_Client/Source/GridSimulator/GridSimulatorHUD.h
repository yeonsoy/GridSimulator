// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GridSimulatorHUD.generated.h"

class UButton;
class UTextBlock;
class UEditableTextBox;
class UGridMapUserWidget;
class UGridTargetUserWidget;
class UGridMarkerUserWidget;

/**
 * 
 */
UCLASS()
class GRIDSIMULATOR_API UGridSimulatorHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void ParseJsonResponse(const FString& JsonString, const FString& FieldName, int32& OutValue);
	void SendGridSizeRequestToServer();
	void SendTargetRouteRequestToServer(int32 InTargetId);

	void OnCreateMarkerWidget();
	void ResetMap();

	void SetGridSize(int32 InGridSize);
	void OnMakeGrid();
	void OnSetMarker();
	void UpdateMarker(UGridMarkerUserWidget* InMarker);

	UFUNCTION(BlueprintCallable)
	void UpdateMarkerMove(float InDeltaTime);

	UFUNCTION()
	void OnSetTarget();

	UFUNCTION(BlueprintCallable)
	void UpdateTimeDisplay();
	
	UFUNCTION()
	void OnStartButtonClicked();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSubclassOf<UGridTargetUserWidget> TargetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSubclassOf<UGridMarkerUserWidget> MarkerClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Simulator HUD", meta = (BindWidget))
	UButton* Start = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Simulator HUD", meta = (BindWidget))
	UButton* SetTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Simulator HUD", meta = (BindWidget))
	UGridMapUserWidget* GridMap;

	UPROPERTY()
	TArray<UGridTargetUserWidget*> TargetWidgets;

	UPROPERTY()
	TArray<UGridMarkerUserWidget*> MarkerWidgets;

	UPROPERTY()
	TMap<int32, UGridTargetUserWidget*> TargetIndexMap;

	UPROPERTY()
	TMap<int32, UGridMarkerUserWidget*> MarkerIndexMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Simulator HUD", meta = (BindWidget))
	UTextBlock* TextTime = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Simulator HUD", meta = (BindWidget))
	UTextBlock* TextStartButton = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Simulator HUD")
	float ElapsedTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Simulator HUD")
	bool IsStart = false;

	UPROPERTY()
	FDateTime CurrentTime;

	UPROPERTY()
	TArray<int32> TargetIndex;

	UPROPERTY()
	TArray<int32> MarkerIndex;

private:
	int32 GridSize = 5;
	int32 CurrentCreationIndex = 0;
	bool IsSimulating = false;
};
