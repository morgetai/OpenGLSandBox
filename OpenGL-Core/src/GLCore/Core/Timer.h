#pragma once

#include <chrono>

class Timer
{
public:
    Timer()
    {
       // Reset();
    }

    void Reset()
    {
       // m_Start = std::chrono::system_clock::now().time_since_epoch();
    }

    float Elapsed()
    {
        //const auto now = std::chrono::system_clock::now().time_since_epoch();
        //const auto now = std::chrono::system_clock::now().time_since_epoch();
        return {};
       // return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
    }

    float ElapsedMillis()
    {
        return {};
        //return Elapsed() * 1000.0f;
    }

private:
    //std::chrono::system_clock::duration m_Start;
};