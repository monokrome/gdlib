#pragma once

#include "compat.h"

class State;

class StateMachine : public Node {
	GDCLASS(StateMachine, Node);

	StringName initial_state;
	NodePath enter_animation_player_path;
	NodePath exit_animation_player_path;

	State *current_state = nullptr;
	State *previous_state = nullptr;
	bool transitioning = false;
	bool exit_anim_running = false;
	bool enter_anim_running = false;
	AnimationPlayer *enter_anim_player_cache = nullptr;
	AnimationPlayer *exit_anim_player_cache = nullptr;
	StringName tracked_exit_anim;
	StringName tracked_enter_anim;

	AnimationPlayer *_resolve_player(const NodePath &p_path);
	void _begin_exit(State *p_state);
	void _begin_enter(State *p_state);
	void _check_anim_completion();
	void _finalize_transition();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void set_initial_state(const StringName &p_name);
	StringName get_initial_state() const;

	void set_enter_animation_player(const NodePath &p_path);
	NodePath get_enter_animation_player() const;

	void set_exit_animation_player(const NodePath &p_path);
	NodePath get_exit_animation_player() const;

	State *get_current_state() const;
	StringName get_current_state_name() const;
	bool is_transitioning() const;

	State *get_state(const StringName &p_name) const;
	bool transition_to(const StringName &p_name, bool p_force = false);

	virtual void input(const Ref<InputEvent> &p_event) override;
};
