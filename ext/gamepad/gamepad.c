#include <gamepad.h>

VALUE m_game_controller;

void Init_gamepad() {
	m_game_controller = rb_define_module("GameController");
	Init_xbox_1(m_game_controller);
}
