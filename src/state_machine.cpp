#include "gdlib/state_machine.h"

#include "gdlib/state.h"

void StateMachine::set_initial_state(const StringName &p_name) {
	initial_state = p_name;
}

StringName StateMachine::get_initial_state() const {
	return initial_state;
}

void StateMachine::set_enter_animation_player(const NodePath &p_path) {
	enter_animation_player_path = p_path;
	enter_anim_player_cache = nullptr;
}

NodePath StateMachine::get_enter_animation_player() const {
	return enter_animation_player_path;
}

void StateMachine::set_exit_animation_player(const NodePath &p_path) {
	exit_animation_player_path = p_path;
	exit_anim_player_cache = nullptr;
}

NodePath StateMachine::get_exit_animation_player() const {
	return exit_animation_player_path;
}

State *StateMachine::get_current_state() const {
	return current_state;
}

StringName StateMachine::get_current_state_name() const {
	return current_state ? current_state->get_state_name() : StringName();
}

bool StateMachine::is_transitioning() const {
	return transitioning;
}

State *StateMachine::get_state(const StringName &p_name) const {
	for (int i = 0; i < get_child_count(); i++) {
		State *s = Object::cast_to<State>(get_child(i));
		if (s && s->get_state_name() == p_name) {
			return s;
		}
	}
	return nullptr;
}

AnimationPlayer *StateMachine::_resolve_player(const NodePath &p_path) {
	if (p_path.is_empty()) {
		return nullptr;
	}
	Node *n = get_node_or_null(p_path);
	return Object::cast_to<AnimationPlayer>(n);
}

void StateMachine::_begin_exit(State *p_state) {
	if (!p_state) {
		return;
	}
	p_state->dispatch_exit();
	StringName anim = p_state->get_exit_animation();
	if (anim.is_empty()) {
		return;
	}
	if (!exit_anim_player_cache) {
		exit_anim_player_cache = _resolve_player(exit_animation_player_path);
	}
	if (exit_anim_player_cache && exit_anim_player_cache->has_animation(anim)) {
		exit_anim_player_cache->play(anim);
		exit_anim_running = true;
		tracked_exit_anim = anim;
	}
}

void StateMachine::_begin_enter(State *p_state) {
	if (!p_state) {
		return;
	}
	p_state->dispatch_enter();
	StringName anim = p_state->get_enter_animation();
	if (anim.is_empty()) {
		return;
	}
	if (!enter_anim_player_cache) {
		enter_anim_player_cache = _resolve_player(enter_animation_player_path);
	}
	if (enter_anim_player_cache && enter_anim_player_cache->has_animation(anim)) {
		enter_anim_player_cache->play(anim);
		enter_anim_running = true;
		tracked_enter_anim = anim;
	}
}

bool StateMachine::transition_to(const StringName &p_name, bool p_force) {
	StringName from_name = current_state ? current_state->get_state_name() : StringName();
	State *target = get_state(p_name);
	if (!target) {
		emit_signal("transition_rejected", from_name, p_name, "no_such_state");
		return false;
	}
	if (transitioning && !p_force) {
		emit_signal("transition_rejected", from_name, p_name, "already_transitioning");
		return false;
	}
	if (current_state && !p_force && !current_state->dispatch_can_transition_to(p_name)) {
		emit_signal("transition_rejected", from_name, p_name, "rejected_by_source");
		return false;
	}

	emit_signal("transition_started", from_name, p_name);

	State *outgoing = current_state;
	previous_state = outgoing;
	current_state = target;

	exit_anim_running = false;
	enter_anim_running = false;
	_begin_exit(outgoing);
	_begin_enter(target);

	transitioning = exit_anim_running || enter_anim_running;
	emit_signal("state_changed", from_name, p_name);
	if (!transitioning) {
		_finalize_transition();
	}
	return true;
}

void StateMachine::_check_anim_completion() {
	if (exit_anim_running) {
		bool done = !exit_anim_player_cache || !exit_anim_player_cache->is_playing()
				|| exit_anim_player_cache->get_current_animation() != tracked_exit_anim;
		if (done) {
			exit_anim_running = false;
		}
	}
	if (enter_anim_running) {
		bool done = !enter_anim_player_cache || !enter_anim_player_cache->is_playing()
				|| enter_anim_player_cache->get_current_animation() != tracked_enter_anim;
		if (done) {
			enter_anim_running = false;
		}
	}
	if (!exit_anim_running && !enter_anim_running) {
		_finalize_transition();
	}
}

void StateMachine::_finalize_transition() {
	transitioning = false;
	previous_state = nullptr;
	emit_signal("transition_finished", get_current_state_name());
}

void StateMachine::input(const Ref<InputEvent> &p_event) {
	if (current_state && !transitioning) {
		current_state->dispatch_input(p_event);
	}
}

void StateMachine::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			set_process(true);
			set_physics_process(true);
			set_process_input(true);
			if (!initial_state.is_empty()) {
				State *s = get_state(initial_state);
				if (s) {
					current_state = s;
					s->dispatch_enter();
					emit_signal("state_changed", StringName(), initial_state);
					emit_signal("transition_finished", initial_state);
				}
			}
		} break;
		case NOTIFICATION_PROCESS: {
			if (transitioning) {
				_check_anim_completion();
			}
			if (current_state) {
				current_state->dispatch_process(get_process_delta_time());
			}
		} break;
		case NOTIFICATION_PHYSICS_PROCESS: {
			if (current_state) {
				current_state->dispatch_physics_process(get_physics_process_delta_time());
			}
		} break;
	}
}

void StateMachine::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_initial_state", "name"), &StateMachine::set_initial_state);
	ClassDB::bind_method(D_METHOD("get_initial_state"), &StateMachine::get_initial_state);
	ClassDB::bind_method(D_METHOD("set_enter_animation_player", "path"), &StateMachine::set_enter_animation_player);
	ClassDB::bind_method(D_METHOD("get_enter_animation_player"), &StateMachine::get_enter_animation_player);
	ClassDB::bind_method(D_METHOD("set_exit_animation_player", "path"), &StateMachine::set_exit_animation_player);
	ClassDB::bind_method(D_METHOD("get_exit_animation_player"), &StateMachine::get_exit_animation_player);
	ClassDB::bind_method(D_METHOD("get_current_state"), &StateMachine::get_current_state);
	ClassDB::bind_method(D_METHOD("get_current_state_name"), &StateMachine::get_current_state_name);
	ClassDB::bind_method(D_METHOD("is_transitioning"), &StateMachine::is_transitioning);
	ClassDB::bind_method(D_METHOD("get_state", "name"), &StateMachine::get_state);
	ClassDB::bind_method(D_METHOD("transition_to", "name", "force"), &StateMachine::transition_to, DEFVAL(false));

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "initial_state"), "set_initial_state", "get_initial_state");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "enter_animation_player"), "set_enter_animation_player", "get_enter_animation_player");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "exit_animation_player"), "set_exit_animation_player", "get_exit_animation_player");

	ADD_SIGNAL(MethodInfo("state_changed", PropertyInfo(Variant::STRING_NAME, "from"), PropertyInfo(Variant::STRING_NAME, "to")));
	ADD_SIGNAL(MethodInfo("transition_started", PropertyInfo(Variant::STRING_NAME, "from"), PropertyInfo(Variant::STRING_NAME, "to")));
	ADD_SIGNAL(MethodInfo("transition_finished", PropertyInfo(Variant::STRING_NAME, "to")));
	ADD_SIGNAL(MethodInfo("transition_rejected", PropertyInfo(Variant::STRING_NAME, "from"), PropertyInfo(Variant::STRING_NAME, "to"), PropertyInfo(Variant::STRING, "reason")));
}
