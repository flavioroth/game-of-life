//
// Created by fla on 23.06.20.
//

#pragma once

#include <glm/glm.hpp>

namespace engine {

class InputEvent {
public:
	enum Type {
		MousePress,
		MouseRelease,
		MouseWheel
	};

	virtual ~InputEvent() = default;

private:
	const Type _type;

protected:
	InputEvent(Type type)
			: _type(type) {}

public:
	Type type() const {
		return _type;
	}
};

struct MouseCursorInput {
	const glm::dvec2 windowCoordinates;

	MouseCursorInput(glm::dvec2 pos)
			: windowCoordinates(std::move(pos)) {}
};


struct MouseWheelInput {
	const enum Direction {
		Down,
		Up
	} direction;

	MouseWheelInput(Direction dir)
			: direction(dir) {}
};


struct MouseButtonInput {
	const enum Button {
		Left,
		Right
	} button;

	MouseButtonInput(Button b)
			: button(b) {}
};

struct MousePressEvent : public MouseCursorInput, public MouseButtonInput, public InputEvent {
	MousePressEvent(MouseButtonInput::Button button, glm::dvec2 pos)
			: MouseCursorInput(std::move(pos)), MouseButtonInput(button), InputEvent(InputEvent::MousePress) {}
};

struct MouseReleaseEvent : public MouseCursorInput, public MouseButtonInput, public InputEvent {
	MouseReleaseEvent(MouseButtonInput::Button button, glm::dvec2 pos)
			: MouseCursorInput(std::move(pos)), MouseButtonInput(button), InputEvent(InputEvent::MouseRelease) {}
};

struct MouseWheelEvent : public MouseWheelInput, public InputEvent {
	explicit MouseWheelEvent(MouseWheelInput::Direction direction)
			: MouseWheelInput(direction),
			  InputEvent(InputEvent::MouseWheel) {}
};

}
