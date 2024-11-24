// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSimulatorHUD.h"
#include "GridSimulatorPlayerController.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "GridMapUserWidget.h"
#include "GridUserWidget.h"
#include "GridTargetUserWidget.h"
#include "GridMarkerUserWidget.h"
#include "Algo/RandomShuffle.h"
#include "TimerManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"

void UGridSimulatorHUD::NativeConstruct()
{
    Super::NativeConstruct();

    if (Start)
    {
        Start->OnClicked.AddDynamic(this, &UGridSimulatorHUD::OnStartButtonClicked);
    }

    if (SetTarget)
    {
        SetTarget->OnClicked.AddDynamic(this, &UGridSimulatorHUD::OnSetTarget);
    }

    AGridSimulatorPlayerController* PlayerController = Cast<AGridSimulatorPlayerController>(GetOwningPlayer());
    if (PlayerController)
    {
        PlayerController->OnTimerCompleted.BindUObject(this, &UGridSimulatorHUD::OnCreateMarkerWidget);
    }

    SendGridSizeRequestToServer();
    CurrentCreationIndex = 0;
}

void UGridSimulatorHUD::NativeDestruct()
{
    Super::NativeDestruct();

    if (Start)
    {
        Start->OnClicked.RemoveAll(this);
    }

    if (SetTarget)
    {
        SetTarget->OnClicked.RemoveAll(this);
    }

    AGridSimulatorPlayerController* PlayerController = Cast<AGridSimulatorPlayerController>(GetOwningPlayer());
    if (PlayerController)
    {
        PlayerController->OnTimerCompleted.Unbind();
    }
}

void UGridSimulatorHUD::ParseJsonResponse(const FString& JsonString, const FString& FieldName, int32& OutValue)
{
    TSharedPtr<FJsonObject> JsonObject;

    // FJsonReader를 사용하여 JSON 데이터를 읽기
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        OutValue = JsonObject->GetNumberField(FieldName);
        UE_LOG(LogTemp, Log, TEXT("Requested Value: %d"), OutValue);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON"));
    }
}


void UGridSimulatorHUD::SendGridSizeRequestToServer()
{
    FString Url = TEXT("http://127.0.0.1:8080/get_grid_size");

    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

    Request->SetURL(Url);
    Request->SetVerb(TEXT("GET"));

    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (bWasSuccessful)
        {
            FString ResponseContent = Response->GetContentAsString();
            UE_LOG(LogTemp, Log, TEXT("Response: %s"), *ResponseContent);
            int32 ResponseGridSize = 5;
            ParseJsonResponse(ResponseContent, "requested_value", ResponseGridSize);
            SetGridSize(ResponseGridSize);
            OnMakeGrid();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Request failed"));
        }
    });

    // 요청 실행
    Request->ProcessRequest();
}

void UGridSimulatorHUD::SendTargetRouteRequestToServer(int32 InTargetId)
{
    if (MarkerIndexMap.Contains(InTargetId) && TargetIndexMap.Contains(InTargetId))
    {
        if (MarkerIndexMap[InTargetId] && TargetIndexMap[InTargetId])
        {
            auto CurrentMarker = MarkerIndexMap[InTargetId];
            int32 MarkerRow = MarkerIndexMap[InTargetId]->GetCurrentRow();
            int32 MarkerColumn = MarkerIndexMap[InTargetId]->GetCurrentColumn();

            int32 TargetRow = TargetIndexMap[InTargetId]->GetRow();
            int32 TargetColumn = TargetIndexMap[InTargetId]->GetColumn();

            TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

            TArray<TSharedPtr<FJsonValue>> CoordinatesArray;
            TSharedPtr<FJsonObject> Coordinate1 = MakeShareable(new FJsonObject);
            Coordinate1->SetNumberField("MarkerRow", MarkerRow);
            Coordinate1->SetNumberField("MarkerColumn", MarkerColumn);

            TSharedPtr<FJsonObject> Coordinate2 = MakeShareable(new FJsonObject);
            Coordinate2->SetNumberField("TargetRow", TargetRow);
            Coordinate2->SetNumberField("TargetColumn", TargetColumn);

            CoordinatesArray.Add(MakeShareable(new FJsonValueObject(Coordinate1)));
            CoordinatesArray.Add(MakeShareable(new FJsonValueObject(Coordinate2)));

            JsonObject->SetNumberField("marker_id", InTargetId);
            JsonObject->SetArrayField("coordinates", CoordinatesArray);

            FString OutputString;
            TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
            FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

            FString Url = TEXT("http://127.0.0.1:8080/receive_coordinates");
            TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
            Request->SetURL(Url);
            Request->SetVerb(TEXT("POST"));
            Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
            Request->SetContentAsString(OutputString);

            Request->OnProcessRequestComplete().BindLambda([this, CurrentMarker](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
            {
                if (bWasSuccessful)
                {
                    UE_LOG(LogTemp, Log, TEXT("Server response: %s"), *Response->GetContentAsString());
                    FString ResponseString = Response->GetContentAsString();
                    TSharedPtr<FJsonObject> JsonResponse;
                    TArray<TPair<int32, int32>> PathArray;

                    // JSON 파싱
                    TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(ResponseString);
                    if (FJsonSerializer::Deserialize(Reader, JsonResponse))
                    {
                        const TArray<TSharedPtr<FJsonValue>> PathJsonArray = JsonResponse->GetArrayField("path");

                        // JSON 배열을 TArray<TPair<int32, int32>>로 변환
                        for (const TSharedPtr<FJsonValue>& Value : PathJsonArray)
                        {
                            const TArray<TSharedPtr<FJsonValue>> Coordinate = Value->AsArray();
                            int32 Row = Coordinate[0]->AsNumber();
                            int32 Column = Coordinate[1]->AsNumber();
                            PathArray.Add(TPair<int32, int32>(Row, Column));
                        }
                    }

                    CurrentMarker->SetPath(PathArray);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Request failed"));
                }
            });

            Request->ProcessRequest();
        }
    }
}


void UGridSimulatorHUD::OnCreateMarkerWidget()
{
    if (MarkerWidgets.IsValidIndex(CurrentCreationIndex) == false)
    {
        return;
    }

    if (UGridMarkerUserWidget* Marker = MarkerWidgets[CurrentCreationIndex])
    {
        if (Marker->IsInViewport() == false)
        {
            Marker->AddToViewport();
            SendTargetRouteRequestToServer(Marker->GetMarkerID());
        }
    }
    CurrentCreationIndex++;
}

void UGridSimulatorHUD::UpdateTimeDisplay()
{
    if (TextTime)
    {
        int Minutes = FMath::FloorToInt(ElapsedTime / 60); // 분 계산
        int Seconds = FMath::FloorToInt(ElapsedTime) % 60; // 초 계산

        FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
        TextTime->SetText(FText::FromString(TimeString));
    }
}

void UGridSimulatorHUD::ResetMap()
{
    TargetIndex.Reset();
    MarkerIndex.Reset();

    for (UGridTargetUserWidget* Target : TargetWidgets)
    {
        if (Target && Target->IsInViewport())
        {
            Target->RemoveFromParent();
        }
    }

    for (UGridMarkerUserWidget* Marker : MarkerWidgets)
    {
        if (Marker && Marker->IsInViewport())
        {
            Marker->RemoveFromParent();
        }
    }

    TargetWidgets.Reset();
    MarkerWidgets.Reset();

    TargetIndexMap.Reset();
    MarkerIndexMap.Reset();
    CurrentCreationIndex = 0;
}

void UGridSimulatorHUD::SetGridSize(int32 InGridSize)
{
    GridSize = InGridSize;
}

void UGridSimulatorHUD::OnSetMarker()
{
    if (GridMap)
    {
        for (int32 i = 0; i < GridSize; ++i)
        {
            if (UGridUserWidget* TargetGrid = GridMap->GetGrid(GridSize - 1, i))
            {
                if (UGridMarkerUserWidget* Marker = MarkerWidgets.IsValidIndex(i) ? MarkerWidgets[i] : nullptr)
                {
                    FVector2D AbsolutePosition = TargetGrid->GetCachedGeometry().GetAbsolutePosition() - GridMap->GetCachedGeometry().GetAbsolutePosition();
                    Marker->SetPositionInViewport(AbsolutePosition);
                }
            }
        }
    }
}

void UGridSimulatorHUD::OnSetTarget()
{
    if (GridMap)
    {
        for (int32 i = 0; i < GridSize; ++i)
        {
            if (UGridUserWidget* TargetGrid = GridMap->GetGrid(0, i))
            {
                if (UGridTargetUserWidget* Target = TargetWidgets.IsValidIndex(i) ? TargetWidgets[i] : nullptr)
                {
                    FVector2D AbsolutePosition = TargetGrid->GetCachedGeometry().GetAbsolutePosition() - GridMap->GetCachedGeometry().GetAbsolutePosition();
                    Target->SetPositionInViewport(AbsolutePosition);

                    if (Target->IsInViewport() == false)
                    {
                        Target->AddToViewport();
                    }
                }
            }
        }
    }
}

void UGridSimulatorHUD::OnStartButtonClicked()
{
    IsStart = !IsStart;

    if (IsStart)
    {
        ElapsedTime = 0;

        OnSetMarker();

        if (TextStartButton)
        {
            TextStartButton->SetText(FText::FromString("Stop"));
        }
    }
    else
    {
        if (TextStartButton)
        {
            TextStartButton->SetText(FText::FromString("Start"));
        }
    }
}

void UGridSimulatorHUD::UpdateMarker(UGridMarkerUserWidget* InMarker)
{
    if (UGridMarkerUserWidget* CurrentMarker = InMarker)
    {
        if (GridMap)
        {
            if (UGridUserWidget* TargetGrid = GridMap->GetGrid(CurrentMarker->GetTargetRow(), CurrentMarker->GetTargetColumn()))
            {
                FVector2D AbsolutePosition = TargetGrid->GetCachedGeometry().GetAbsolutePosition() - GridMap->GetCachedGeometry().GetAbsolutePosition();
                
                if (UGridUserWidget* CurrentGrid = GridMap->GetGrid(CurrentMarker->GetCurrentRow(), CurrentMarker->GetCurrentColumn()))
                {
                    FVector2D CurrentPosition = CurrentGrid->GetCachedGeometry().GetAbsolutePosition() - GridMap->GetCachedGeometry().GetAbsolutePosition();
                    CurrentMarker->SetCurrentPosition(CurrentPosition);
                }
                CurrentMarker->SetTargetPosition(AbsolutePosition);
            }
        }
    }
}

void UGridSimulatorHUD::UpdateMarkerMove(float InDeltaTime)
{
    bool IsAllEnd = true;
    for (auto MarkerWidget : MarkerWidgets)
    {
        if (MarkerWidget->IsEnd == false)
        {
            if (MarkerWidget->IsInViewport())
            {
                MarkerWidget->UpdatePosition(InDeltaTime);

                if (MarkerWidget->NextFlag)
                {
                    UpdateMarker(MarkerWidget);
                    MarkerWidget->SetNextPosition();
                }
                IsAllEnd = false;
            }
        }
        else
        {
            if (MarkerWidget->IsInViewport())
            {
                MarkerWidget->RemoveFromParent();
            }

            if (TargetIndexMap.Contains(MarkerWidget->GetMarkerID()))
            {
                UGridTargetUserWidget* TargetWidget = TargetIndexMap[MarkerWidget->GetMarkerID()];
                if (TargetWidget->IsInViewport())
                {
                    TargetWidget->RemoveFromParent();
                }
            }
        }
    }

    if (IsAllEnd)
    {
        IsStart = false;
    }
}

void UGridSimulatorHUD::OnMakeGrid()
{
    if (GridMap)
    {
        ResetMap();

        for (int32 i = 0; i < GridSize; i++)
        {
            TargetIndex.Emplace(i);
            MarkerIndex.Emplace(i);
            TargetWidgets.Emplace(CreateWidget<UGridTargetUserWidget>(this, TargetClass));
            MarkerWidgets.Emplace(CreateWidget<UGridMarkerUserWidget>(this, MarkerClass));
        }

        Algo::RandomShuffle(TargetIndex);
        Algo::RandomShuffle(MarkerIndex);

        for (int32 i = 0; i < GridSize; i++)
        {
            TargetWidgets[i]->SetRowColumn(0, i);
            MarkerWidgets[i]->SetCurrentRowColumn(GridSize - 1, i);
            TargetIndexMap.Emplace(TargetIndex[i], TargetWidgets[i]);
            MarkerIndexMap.Emplace(MarkerIndex[i], MarkerWidgets[i]);
            TargetWidgets[i]->SetTargetID(TargetIndex[i]);
            MarkerWidgets[i]->SetMarkerID(MarkerIndex[i]);
        }

        for (int32 i = 0; i < GridSize; i++)
        {
            MarkerIndexMap[i]->SetEndTargetRowColumn(TargetIndexMap[i]->GetRow(), TargetIndexMap[i]->GetColumn());
        }

        GridMap->SetRow(GridSize);
    }
}
