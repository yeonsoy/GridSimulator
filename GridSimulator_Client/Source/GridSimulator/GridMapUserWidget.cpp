// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMapUserWidget.h"
#include "GridHorizontalListUserWidget.h"
#include "GridUserWidget.h"
#include "Components/DynamicEntryBox.h"

void UGridMapUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (GridMap)
	{
		for (int i = 0; i < Row; ++i)
		{
			GridMap->CreateEntry();
		}
	}
}

void UGridMapUserWidget::SetRow(int32 InRow)
{
	Row = InRow;

	if (GridMap)
	{
		GridMap->Reset();

		for (int i = 0; i < Row; ++i)
		{
			if (UGridHorizontalListUserWidget* EntryWidget = Cast<UGridHorizontalListUserWidget>(GridMap->CreateEntry()))
			{
				EntryWidget->SetRow(i);
				EntryWidget->SetColumn(InRow);
			}
		}
	}
}

UGridUserWidget* UGridMapUserWidget::GetGrid(int32 InRow, int32 InColumn)
{
	if (GridMap)
	{
		for (UUserWidget* GridMapEntryWidget : GridMap->GetAllEntries())
		{
			if (UGridHorizontalListUserWidget* GridRowWidget = Cast<UGridHorizontalListUserWidget>(GridMapEntryWidget))
			{
				if (UDynamicEntryBox* GridRow = GridRowWidget->GridHorizontal)
				{
					for (UUserWidget* RowEntryWidget : GridRow->GetAllEntries())
					{
						if (UGridUserWidget* ColumnWidget = Cast<UGridUserWidget>(RowEntryWidget))
						{
							if (ColumnWidget->GetRow() == InRow && ColumnWidget->GetColumn() == InColumn)
							{
								return ColumnWidget;
							}
						}
					}
				}
			}
		}
	}

	return nullptr;
}

