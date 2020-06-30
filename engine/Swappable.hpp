//
// Created by fla on 19.06.20.
//

#pragma once

#include <utility>

namespace engine {

template <typename T>
class Swappable {
private:
	using TRef = std::reference_wrapper<T>;
	std::pair<TRef, TRef> _pair;

public:
	template <typename... Args>
	Swappable(Args&&... args)
		: _pair(std::forward<Args>(args)...)
	{}

	T& first() {
		return _pair.first;
	}

	T& second() {
		return _pair.second;
	}

	void swap() {
		std::swap(_pair.first, _pair.second);
	}
};
}
