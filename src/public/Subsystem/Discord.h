#pragma once
#include <iostream>
#include "spdlog/spdlog.h"
#include <thread>
#include <atomic>

class Discord {
private:
    static Discord* m_instance;
    uint64_t m_appId;
public:
    Discord();
    void Init();
    void ClientThread();
    static Discord* GetInstance();

};