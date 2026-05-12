#include "spatial_container_node.h"

void SpatialContainerNode::_bind_methods() {
	// No bindings yet — internal-only helpers for subclasses
}

MeshInstance3D *SpatialContainerNode::add_mesh_child(const Ref<Mesh> &p_mesh, const Transform3D &p_transform) {
	MeshInstance3D *instance = memnew(MeshInstance3D);
	instance->set_mesh(p_mesh);
	instance->set_transform(p_transform);
	add_child(instance);
	return instance;
}

void SpatialContainerNode::clear_mesh_children() {
	for (int i = get_child_count() - 1; i >= 0; --i) {
		Node *child = get_child(i);
		if (Object::cast_to<MeshInstance3D>(child)) {
			remove_child(child);
			memdelete(child);
		}
	}
}
