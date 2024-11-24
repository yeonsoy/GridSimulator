// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GridUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class GRIDSIMULATOR_API UGridUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRowColumn(int32 InRow, int32 InColumn);
	int32 GetRow() { return Row; }
	int32 GetColumn() { return Column; }

private:
	int32 Row;
	int32 Column;
};
