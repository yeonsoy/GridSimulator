// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GridTargetUserWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class GRIDSIMULATOR_API UGridTargetUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTargetID(int32 InTargetId);
	int32 GetTargetID() { return TargetIndex; }
	void SetRowColumn(int32 InRow, int32 InColumn);
	int32 GetRow() { return Row; }
	int32 GetColumn() { return Column; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid Target", meta = (BindWidget))
	UTextBlock* TargetID = nullptr;

	int32 TargetIndex = 0;
private:
	int32 Row;
	int32 Column;
};
