#pragma once
#include <iostream>
#include <Windows.h>
#include <wincodec.h>

class ResourceManager {
private:
	static ResourceManager* m_instance;

	ComPtr<IWICImagingFactory> m_factory;
public:
	ResourceManager();

	ResourceManager* GetInstance();
};