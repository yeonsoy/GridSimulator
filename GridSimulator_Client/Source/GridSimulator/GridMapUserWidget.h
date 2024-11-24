// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GridMapUserWidget.generated.h"

class UDynamicEntryBox;
class UGridUserWidget;

/**
 * 
 */
UCLASS()
class GRIDSIMULATOR_API UGridMapUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	void SetRow(int32 InRow);
	const int32 GetRow() { return Row; }
	UGridUserWidget* GetGrid(int32 InRow, int32 InColumn);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Map", meta = (BindWidget))
	UDynamicEntryBox* GridMap = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Map")
	int32 Row = 5;
};
