#include "extensible_physics_server_3d.h"

ExtensiblePhysicsServer3D::ShapeFactory ExtensiblePhysicsServer3D::factory_ = nullptr;

RID ExtensiblePhysicsServer3D::custom_shape_create() {
	if (!factory_) {
		return RID();
	}
	GodotShape3D *shape = factory_();
	if (!shape) {
		return RID();
	}
	return register_custom_shape(shape);
}

RID ExtensiblePhysicsServer3D::register_custom_shape(GodotShape3D *p_shape) {
	if (!p_shape) {
		return RID();
	}
	// shape_owner is `protected` (per the engine patch). Once registered, the
	// shape is tracked by the standard server's RID system, so every other
	// shape_* method (set_data, get_aabb, etc.) works through
	// GodotPhysicsServer3D's existing implementations via polymorphism on
	// the shape pointer.
	RID rid = shape_owner.make_rid(p_shape);
	p_shape->set_self(rid);
	return rid;
}
