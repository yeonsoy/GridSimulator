// Fill out your copyright notice in the Description page of Project Settings.


#include "GridHorizontalListUserWidget.h"
#include "GridUserWidget.h"

void UGridHorizontalListUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (GridHorizontal)
	{
		for (int i = 0; i < Column; ++ i)
		{
			GridHorizontal->CreateEntry();
		}
	}
}


void UGridHorizontalListUserWidget::SetRow(int32 InRow)
{
	Row = InRow;
}

void UGridHorizontalListUserWidget::SetColumn(int32 InColumn)
{
	Column = InColumn;

	if (GridHorizontal)
	{
		GridHorizontal->Reset();

		for (int i = 0; i < Column; ++i)
		{
			if (UGridUserWidget* EntryWidget = Cast<UGridUserWidget>(GridHorizontal->CreateEntry()))
			{
				EntryWidget->SetRowColumn(Row, i);
			}
		}
	}
}
