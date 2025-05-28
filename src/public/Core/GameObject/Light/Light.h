#pragma once
#include <iostream>
#include "Core/GameObject/GameObject.h"

class Light : public GameObject {
public:
    Light();

    void Init() override;
    void Update() override;
};