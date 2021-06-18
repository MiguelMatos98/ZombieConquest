#pragma once

#include "PlatformerGameInstance.generated.h"

UCLASS()
class PLATFORMER_API UPlatformerGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadOnly, Category = "Game Instance")
	float Health = -1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Game Instance")
	int32 Score;

public:

	void SetHealth(float InHealth) { Health = InHealth; }

	void AddScore(int32 InScore) { Score += InScore; }

	float GetHealth() const { return Health; }
};
