// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/DynamicEntryBox.h"
#include "GridHorizontalListUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class GRIDSIMULATOR_API UGridHorizontalListUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

public:
	void SetRow(int32 InRow);
	const int32 GetRow() { return Row; }
	void SetColumn(int32 InColumn);
	const int32 GetColumn() { return Column; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Horizontal", meta = (BindWidget))
	UDynamicEntryBox* GridHorizontal = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Horizontal")
	int32 Column = 5;

private:
	int32 Row = 0;
};
