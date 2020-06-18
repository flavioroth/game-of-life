//
// Created by fla on 18.06.20.
//

#pragma once

#include <algorithm>
#include <numeric>

namespace utils {

template <typename TSample, typename TReal, size_t WindowSize>
class RollingAverage {
public:
	using SamplesArray = std::array<TSample, WindowSize>;

private:
	SamplesArray _samples;
	size_t _index;
	TSample _sum;
	TReal _average;
	bool _isFull;

public:
	RollingAverage()
		: _index(0)
		, _sum(std::move(TSample(0)))
		, _average(std::move(TReal(0)))
		, _isFull(false) {}

	TReal currentAverage() const {
		return _average;
	}

	const SamplesArray& samples() const {
		return _samples;
	}

	void push(TSample sample) {

		_sum -= _samples[_index];
		_sum += sample;
		_samples[_index] = sample;

		_index++;

		if(_index >= WindowSize) {
			_isFull = true;
			_index = 0;
			// recompute sum to avoid accumulating rounding error
			_sum = std::accumulate(_samples.begin(), _samples.end(), 0);
		}

		_average = _sum / (_isFull ? WindowSize : _index);
	}
};

}// namespace utils
