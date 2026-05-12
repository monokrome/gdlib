#include "state.h"

#include "state_machine.h"

void State::set_state_name(const StringName &p_name) {
	state_name = p_name;
}

StringName State::get_state_name() const {
	return state_name;
}

void State::set_allowed_transitions(const TypedArray<StringName> &p_transitions) {
	allowed_transitions = p_transitions;
}

TypedArray<StringName> State::get_allowed_transitions() const {
	return allowed_transitions;
}

void State::set_enter_animation(const StringName &p_anim) {
	enter_animation = p_anim;
}

StringName State::get_enter_animation() const {
	return enter_animation;
}

void State::set_exit_animation(const StringName &p_anim) {
	exit_animation = p_anim;
}

StringName State::get_exit_animation() const {
	return exit_animation;
}

StateMachine *State::get_state_machine() const {
	return Object::cast_to<StateMachine>(get_parent());
}

void State::dispatch_enter() {
	GDVIRTUAL_CALL(_enter);
}

void State::dispatch_exit() {
	GDVIRTUAL_CALL(_exit);
}

void State::dispatch_process(double p_delta) {
	GDVIRTUAL_CALL(_state_process, p_delta);
}

void State::dispatch_physics_process(double p_delta) {
	GDVIRTUAL_CALL(_state_physics_process, p_delta);
}

void State::dispatch_input(const Ref<InputEvent> &p_event) {
	GDVIRTUAL_CALL(_state_input, p_event);
}

bool State::dispatch_can_transition_to(const StringName &p_target) const {
	bool result = false;
	if (GDVIRTUAL_CALL(_can_transition_to, p_target, result)) {
		return result;
	}
	if (allowed_transitions.is_empty()) {
		return true;
	}
	return allowed_transitions.has(p_target);
}

void State::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_state_name", "name"), &State::set_state_name);
	ClassDB::bind_method(D_METHOD("get_state_name"), &State::get_state_name);
	ClassDB::bind_method(D_METHOD("set_allowed_transitions", "transitions"), &State::set_allowed_transitions);
	ClassDB::bind_method(D_METHOD("get_allowed_transitions"), &State::get_allowed_transitions);
	ClassDB::bind_method(D_METHOD("set_enter_animation", "name"), &State::set_enter_animation);
	ClassDB::bind_method(D_METHOD("get_enter_animation"), &State::get_enter_animation);
	ClassDB::bind_method(D_METHOD("set_exit_animation", "name"), &State::set_exit_animation);
	ClassDB::bind_method(D_METHOD("get_exit_animation"), &State::get_exit_animation);
	ClassDB::bind_method(D_METHOD("get_state_machine"), &State::get_state_machine);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "state_name"), "set_state_name", "get_state_name");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "allowed_transitions", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_allowed_transitions", "get_allowed_transitions");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "enter_animation"), "set_enter_animation", "get_enter_animation");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "exit_animation"), "set_exit_animation", "get_exit_animation");

	GDVIRTUAL_BIND(_enter);
	GDVIRTUAL_BIND(_exit);
	GDVIRTUAL_BIND(_state_process, "delta");
	GDVIRTUAL_BIND(_state_physics_process, "delta");
	GDVIRTUAL_BIND(_state_input, "event");
	GDVIRTUAL_BIND(_can_transition_to, "target");
}
