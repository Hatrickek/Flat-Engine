#pragma once
#include "application.h"
#include "utils/log.h"
extern FlatEngine::Application* FlatEngine::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	FlatEngine::Log::Init();

	auto app = FlatEngine::CreateApplication({ argc, argv });

	app->Run();

	delete app;
}