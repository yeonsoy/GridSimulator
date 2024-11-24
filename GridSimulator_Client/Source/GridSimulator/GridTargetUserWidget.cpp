// Fill out your copyright notice in the Description page of Project Settings.


#include "GridTargetUserWidget.h"
#include "Components/TextBlock.h" 

void UGridTargetUserWidget::SetTargetID(int32 InTargetId)
{
    TargetIndex = InTargetId;
    if (TargetID)
    {
        FString TText = FString::Printf(TEXT("T%d"), InTargetId + 1);
        TargetID->SetText(FText::FromString(TText));
    }
}

void UGridTargetUserWidget::SetRowColumn(int32 InRow, int32 InColumn)
{
    Row = InRow;
    Column = InColumn;
}