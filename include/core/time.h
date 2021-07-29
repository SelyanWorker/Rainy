#pragma once

#include "core/core.h"

namespace Rainy
{
    class RAINY_API TimeStep
    {
    private:
        TimeStep(float seconds) : m_seconds(seconds) {}

    public:
        TimeStep(const TimeStep &other) { m_seconds = other.m_seconds; }

        float getMilli() const { return m_seconds * 1000; }

        float getMicro() const { return m_seconds * 1000000; }

        float getNano() const { return m_seconds * 1000000000; }

        float getSeconds() const { return m_seconds; }

        static TimeStep getTime();

    private:
        float m_seconds;
    };

}
