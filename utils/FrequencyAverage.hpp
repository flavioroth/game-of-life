//
// Created by fla on 18.06.20.
//

#pragma once

#include "RollingAverage.hpp"

#include <chrono>

namespace utils {

template <size_t WindowSize, typename TReal = float, typename TClock = std::chrono::steady_clock>
class FrequencyAverage {
public:
	using TRollingAverage = RollingAverage<uint64_t, TReal, WindowSize>;

private:
	typename TClock::time_point _previousTimePoint;
	TRollingAverage _averageInterval;

public:
	explicit FrequencyAverage()
		: _previousTimePoint(TClock::now()) {}

	TReal averageFrequencyHz() const {
		return static_cast<TReal>(std::nano::den) / _averageInterval.currentAverage();
	}

	void update() {
		const auto now = TClock::now();
		uint64_t elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - _previousTimePoint).count();
		_averageInterval.push(elapsed);
		_previousTimePoint = now;
	}
};

}// namespace utils
