#pragma once

// gdlib supports building as a Godot custom module today, and is structured to
// support a GDExtension build target later. All translation units include this
// header instead of reaching into engine internals directly.

#ifdef GDLIB_GDEXTENSION
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/typed_array.hpp>
using namespace godot;
#else
#include "core/object/class_db.h"
#include "core/string/string_name.h"
#include "core/variant/typed_array.h"
#include "scene/animation/animation_player.h"
#include "scene/main/node.h"
#endif
