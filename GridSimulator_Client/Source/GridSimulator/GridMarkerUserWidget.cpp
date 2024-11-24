// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMarkerUserWidget.h"
#include "Components/TextBlock.h" 
#include "Kismet/KismetMathLibrary.h"
#include "Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Engine/Engine.h"
#include "Misc/DateTime.h"
#include "Misc/Timespan.h"

void UGridMarkerUserWidget::SetMarkerID(int32 InMarkerId)
{
    MarkerIndex = InMarkerId;
    if (MarkerID)
    {
        FString MText = FString::Printf(TEXT("M%d"), InMarkerId + 1);
        MarkerID->SetText(FText::FromString(MText));
    }
}

void UGridMarkerUserWidget::UpdatePosition(float InDeltaTime)
{
    if (Path.IsEmpty() && NextFlag == false)
    {
        return;
    }

    FVector Direction = TargetPosition - CurrentPosition;
    float Distance = Direction.Size();

    if (Distance < 1.0f)
    {
        CurrentVelocity = FVector(0.0f, 0.0f, 0.0f);
        NextFlag = true;

        if (CurrentRow == EndTargetRow && CurrentColumn == EndTargetColumn)
        {
            MoveEndTime = FDateTime::Now();
            SendStartEndDateTimesToServer();
            IsEnd = true;
        }
    }
    else
    {
        Direction.Normalize();
        FVector DesiredVelocity = Direction * MaxSpeed;
        CurrentVelocity = FMath::VInterpTo(CurrentVelocity, DesiredVelocity, InDeltaTime, Acceleration);
        CurrentPosition += CurrentVelocity * InDeltaTime;
    }

    FVector ForwardVector = CurrentPosition + Direction;
    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CurrentPosition, ForwardVector);
    CurrentRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, InDeltaTime, MaxRotationSpeed);
    // SetRenderTransformAngle(CurrentRotation.Yaw);
    SetPositionInViewport(FVector2D(CurrentPosition.X, CurrentPosition.Y));
}

void UGridMarkerUserWidget::SetEndTargetRowColumn(int32 InRow, int32 InColumn)
{
    EndTargetRow = InRow;
    EndTargetColumn = InColumn;
}

void UGridMarkerUserWidget::SetTargetRowColumn(int32 InRow, int32 InColumn)
{
    TargetRow = InRow;
    TargetColumn = InColumn;
}

void UGridMarkerUserWidget::SetCurrentRowColumn(int32 InRow, int32 InColumn)
{
    CurrentRow = InRow;
    CurrentColumn = InColumn;
}

void UGridMarkerUserWidget::SendStartEndDateTimesToServer()
{
    FString DateTimeString1 = MoveStartTime.ToIso8601();
    FString DateTimeString2 = MoveEndTime.ToIso8601();

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetNumberField(TEXT("marker_id"), MarkerIndex);
    JsonObject->SetStringField(TEXT("start_datetime"), DateTimeString1);
    JsonObject->SetStringField(TEXT("end_datetime"), DateTimeString2);

    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(TEXT("http://127.0.0.1:8080/receive_datetimes"));
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetContentAsString(JsonString);

    HttpRequest->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (bWasSuccessful)
        {
            FString ResponseString = Response->GetContentAsString();
            UE_LOG(LogTemp, Log, TEXT("Response: %s"), *ResponseString);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Request failed"));
        }
    });

    HttpRequest->ProcessRequest();
}

void UGridMarkerUserWidget::SetPath(TArray<TPair<int32, int32>> InPath)
{
    Path = InPath;
    if (InPath.IsValidIndex(CurrentPathIndex))
    {
        SetTargetRowColumn(InPath[CurrentPathIndex].Key, InPath[CurrentPathIndex].Value);
        CurrentPathIndex++;
    }

    MoveStartTime = FDateTime::Now();
}

void UGridMarkerUserWidget::SetNextPosition()
{
    SetCurrentRowColumn(TargetRow, TargetColumn);
    if (Path.IsValidIndex(CurrentPathIndex))
    {
        SetTargetRowColumn(Path[CurrentPathIndex].Key, Path[CurrentPathIndex].Value);
        CurrentPathIndex++;
    }

    NextFlag = false;
}
