#ifndef GDLIB_SVO_H
#define GDLIB_SVO_H

// Generic sparse voxel octree.
//
// Storage: each Node has either 8 child pointers (internal) or a value
// (leaf at level 0). Null subtrees represent empty regions, so sparseness is
// preserved as a tree of present nodes only.
//
// Contract: T() (default-constructed T) must satisfy the empty predicate.
// Setting a position to a value that satisfies the empty predicate prunes
// the subtree; siblings being empty propagates collapse upward.

template <typename T>
struct DefaultEmptyPredicate {
	bool operator()(const T &p_value) const { return p_value == T(); }
};

template <typename T, typename EmptyPredicate = DefaultEmptyPredicate<T>>
class SVO {
public:
	explicit SVO(int p_depth, EmptyPredicate p_empty = EmptyPredicate())
			: depth_(p_depth), root_(nullptr), empty_(p_empty) {}

	~SVO() { destroy(root_); }

	SVO(const SVO &) = delete;
	SVO &operator=(const SVO &) = delete;

	T get(int x, int y, int z) const {
		return get_recursive(root_, depth_, x, y, z);
	}

	void set(int x, int y, int z, const T &p_value) {
		root_ = set_recursive(root_, depth_, x, y, z, p_value);
	}

	// Fill an inclusive box [x0..x1] × [y0..y1] × [z0..z1] with p_value.
	// Short-circuits whole subtrees that lie fully inside the box.
	void set_region(int x0, int y0, int z0, int x1, int y1, int z1, const T &p_value) {
		if (x0 > x1 || y0 > y1 || z0 > z1) {
			return;
		}
		root_ = set_region_recursive(root_, depth_, 0, 0, 0, x0, y0, z0, x1, y1, z1, p_value);
	}

	// Visit every non-empty leaf as visitor(x, y, z, value).
	template <typename Visitor>
	void traverse(Visitor &&visitor) const {
		traverse_recursive(root_, depth_, 0, 0, 0, visitor);
	}

	int depth() const { return depth_; }
	int size() const { return 1 << depth_; }

private:
	struct Node {
		Node *children[8] = { nullptr };
		T value{};
	};

	int depth_;
	Node *root_;
	EmptyPredicate empty_;

	bool is_empty_value(const T &p_value) const { return empty_(p_value); }

	static int child_index(int x, int y, int z, int level) {
		int bit = level - 1;
		int idx = 0;
		if ((x >> bit) & 1) idx |= 1;
		if ((y >> bit) & 1) idx |= 2;
		if ((z >> bit) & 1) idx |= 4;
		return idx;
	}

	void destroy(Node *node) {
		if (!node) return;
		for (int i = 0; i < 8; ++i) {
			destroy(node->children[i]);
		}
		delete node;
	}

	bool node_is_empty(const Node *node) const {
		if (!node) return true;
		for (int i = 0; i < 8; ++i) {
			if (node->children[i]) return false;
		}
		return is_empty_value(node->value);
	}

	T get_recursive(const Node *node, int level, int x, int y, int z) const {
		if (!node) return T();
		if (level == 0) return node->value;
		int idx = child_index(x, y, z, level);
		return get_recursive(node->children[idx], level - 1, x, y, z);
	}

	// Returns the (possibly new or null) node after the set. Null means
	// the subtree is now entirely empty and the caller should treat its
	// pointer as cleared.
	Node *set_recursive(Node *node, int level, int x, int y, int z, const T &p_value) {
		if (level == 0) {
			if (is_empty_value(p_value)) {
				delete node;
				return nullptr;
			}
			if (!node) node = new Node();
			node->value = p_value;
			return node;
		}

		if (!node) {
			if (is_empty_value(p_value)) return nullptr;
			node = new Node();
		}

		int idx = child_index(x, y, z, level);
		node->children[idx] = set_recursive(node->children[idx], level - 1, x, y, z, p_value);

		for (int i = 0; i < 8; ++i) {
			if (node->children[i]) return node;
		}
		// All children empty — collapse this node.
		delete node;
		return nullptr;
	}

	// Returns the (possibly new or null) node after the region set.
	Node *set_region_recursive(Node *node, int level, int ox, int oy, int oz,
			int x0, int y0, int z0, int x1, int y1, int z1, const T &p_value) {
		int sz = 1 << level;
		// Bail if this node region is fully outside the target box.
		if (ox + sz - 1 < x0 || oy + sz - 1 < y0 || oz + sz - 1 < z0 ||
				ox > x1 || oy > y1 || oz > z1) {
			return node;
		}

		// Fully inside — replace this subtree with the fill value.
		if (ox >= x0 && oy >= y0 && oz >= z0 &&
				ox + sz - 1 <= x1 && oy + sz - 1 <= y1 && oz + sz - 1 <= z1) {
			destroy(node);
			if (is_empty_value(p_value)) {
				return nullptr;
			}
			if (level == 0) {
				Node *leaf = new Node();
				leaf->value = p_value;
				return leaf;
			}
			// Build a uniformly-filled subtree.
			return build_uniform(level, p_value);
		}

		// Partial overlap — descend.
		if (!node) {
			if (is_empty_value(p_value)) return nullptr;
			node = new Node();
		}

		int half = 1 << (level - 1);
		for (int i = 0; i < 8; ++i) {
			int cx = ox + ((i & 1) ? half : 0);
			int cy = oy + ((i & 2) ? half : 0);
			int cz = oz + ((i & 4) ? half : 0);
			node->children[i] = set_region_recursive(node->children[i], level - 1, cx, cy, cz,
					x0, y0, z0, x1, y1, z1, p_value);
		}

		// Collapse if all children empty.
		for (int i = 0; i < 8; ++i) {
			if (node->children[i]) return node;
		}
		delete node;
		return nullptr;
	}

	Node *build_uniform(int level, const T &p_value) {
		if (level == 0) {
			Node *leaf = new Node();
			leaf->value = p_value;
			return leaf;
		}
		Node *internal = new Node();
		for (int i = 0; i < 8; ++i) {
			internal->children[i] = build_uniform(level - 1, p_value);
		}
		return internal;
	}

	template <typename Visitor>
	void traverse_recursive(const Node *node, int level, int ox, int oy, int oz, Visitor &visitor) const {
		if (!node) return;
		if (level == 0) {
			if (!is_empty_value(node->value)) {
				visitor(ox, oy, oz, node->value);
			}
			return;
		}
		int half = 1 << (level - 1);
		for (int i = 0; i < 8; ++i) {
			int cx = ox + ((i & 1) ? half : 0);
			int cy = oy + ((i & 2) ? half : 0);
			int cz = oz + ((i & 4) ? half : 0);
			traverse_recursive(node->children[i], level - 1, cx, cy, cz, visitor);
		}
	}
};

#endif
