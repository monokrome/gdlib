#include "register_types.h"

#include "core/object/class_db.h"

#include "primitives/state_machine/state.h"
#include "primitives/state_machine/state_machine.h"

void initialize_gdlib_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GDREGISTER_CLASS(State);
	GDREGISTER_CLASS(StateMachine);
}

void uninitialize_gdlib_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}
