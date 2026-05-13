#ifndef GDLIB_SPATIAL_CONTAINER_NODE_H
#define GDLIB_SPATIAL_CONTAINER_NODE_H

#include "scene/3d/node_3d.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/resources/mesh.h"

class SpatialContainerNode : public Node3D {
	GDCLASS(SpatialContainerNode, Node3D);

protected:
	static void _bind_methods();

	// Override to react to content changes (regenerate meshes, etc.)
	virtual void _content_changed() {}

public:
	MeshInstance3D *add_mesh_child(const Ref<Mesh> &p_mesh, const Transform3D &p_transform = Transform3D());
	void clear_mesh_children();
};

#endif
