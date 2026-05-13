#ifndef GDLIB_EXTENSIBLE_PHYSICS_SERVER_3D_H
#define GDLIB_EXTENSIBLE_PHYSICS_SERVER_3D_H

#include "modules/godot_physics_3d/godot_physics_server_3d.h"

// A drop-in replacement for GodotPhysicsServer3D that wires up
// custom_shape_create() to a module-supplied factory. Lets external
// modules add new shape types (voxels, SDFs, anything implementing
// GodotShape3D) without further engine changes.
//
// Use: at module init, install a factory:
//
//     ExtensiblePhysicsServer3D::set_custom_shape_factory(&MyShape3D::create);
//
// Then register this server with PhysicsServer3DManager so users can
// pick it (or set it as the default) in Project Settings.
class ExtensiblePhysicsServer3D : public GodotPhysicsServer3D {
	GDCLASS(ExtensiblePhysicsServer3D, GodotPhysicsServer3D);

public:
	using ShapeFactory = GodotShape3D *(*)();

	// One factory per process — there is only one custom shape "slot"
	// in PhysicsServer3D::ShapeType. If you need multiple custom shapes,
	// have the factory dispatch internally or stack physics servers.
	// Used by the standard PhysicsServer3D::shape_create(SHAPE_CUSTOM) path.
	static void set_custom_shape_factory(ShapeFactory p_factory) { factory_ = p_factory; }
	static ShapeFactory get_custom_shape_factory() { return factory_; }

	RID custom_shape_create() override;

	// Register a pre-constructed and pre-configured custom shape. The
	// server takes ownership of the pointer. Use this when the shape
	// needs per-instance state (e.g., a pointer to an SVO) that the
	// no-arg factory can't supply.
	RID register_custom_shape(GodotShape3D *p_shape);

protected:
	static void _bind_methods() {}

private:
	static ShapeFactory factory_;
};

#endif
