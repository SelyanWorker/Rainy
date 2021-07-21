#pragma once

#include "Rainy/Core/Core.h"
#include <cstdint>

namespace Rainy {

	class RAINY_API TimeStep 
	{
	private:
		TimeStep(float seconds)
		:	m_seconds(seconds) {}

	public:
		TimeStep(const TimeStep& other) { m_seconds = other.m_seconds; }

		float GetMilli() const { return m_seconds * 1000; }

		float GetMicro() const { return m_seconds * 1000000; }

		float GetNano() const { return m_seconds * 1000000000; }

		float GetSeconds() const { return m_seconds; }

		static TimeStep GetTime();

	private:
		float m_seconds;
	};

}
