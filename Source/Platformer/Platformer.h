// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"



#define LOG_SCREEN(Color, Duration, String) \
{ \
GEngine->AddOnScreenDebugMessage(-1, Duration, Color, FString::Printf(TEXT("%s"), TEXT( String))); \
}
