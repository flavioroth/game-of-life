//
// Created by fla on 23.06.20.
//

#pragma once

#include "Events.hpp"

#include <deque>
#include <mutex>
#include <memory>

namespace engine {

class EventQueue {
private:
	std::deque<std::unique_ptr<InputEvent>> _queue;
	std::mutex _mutex;

public:
	void pushEvent(std::unique_ptr <InputEvent> event) {
		std::lock_guard <std::mutex> lock(_mutex);
		_queue.push_back(std::move(event));
	}

	std::unique_ptr <InputEvent> popEvent() {
		std::lock_guard <std::mutex> lock(_mutex);
		if(_queue.empty()) {
			return nullptr;
		} else {
			auto val = std::move(_queue.front());
			_queue.pop_front();
			return val;
		}
	}
};
}
