// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/TextReaderActorComponent.h"

#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"

UTextReaderActorComponent::UTextReaderActorComponent()
{
}

FString UTextReaderActorComponent::ReadFile(FString FilePath)
{
	FString DirectoryPath = FPaths::ProjectContentDir();
	FString FullPath = DirectoryPath + "/" + FilePath;
	FString Result;
	IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();

	if (File.FileExists(*FullPath))
	{
		FFileHelper::LoadFileToString(Result, *FullPath);
	}

	return Result;
}
