#pragma once

#include "compat.h"

class StateMachine;

class State : public Node {
	GDCLASS(State, Node);

	StringName state_name;
	TypedArray<StringName> allowed_transitions;
	StringName enter_animation;
	StringName exit_animation;

protected:
	static void _bind_methods();

	GDVIRTUAL0(_enter)
	GDVIRTUAL0(_exit)
	GDVIRTUAL1(_state_process, double)
	GDVIRTUAL1(_state_physics_process, double)
	GDVIRTUAL1(_state_input, Ref<InputEvent>)
	GDVIRTUAL1RC(bool, _can_transition_to, StringName)

public:
	void set_state_name(const StringName &p_name);
	StringName get_state_name() const;

	void set_allowed_transitions(const TypedArray<StringName> &p_transitions);
	TypedArray<StringName> get_allowed_transitions() const;

	void set_enter_animation(const StringName &p_anim);
	StringName get_enter_animation() const;

	void set_exit_animation(const StringName &p_anim);
	StringName get_exit_animation() const;

	StateMachine *get_state_machine() const;

	void dispatch_enter();
	void dispatch_exit();
	void dispatch_process(double p_delta);
	void dispatch_physics_process(double p_delta);
	void dispatch_input(const Ref<InputEvent> &p_event);
	bool dispatch_can_transition_to(const StringName &p_target) const;
};
