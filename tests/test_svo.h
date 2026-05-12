#pragma once

#include "../src/svo.h"

#include "tests/test_macros.h"

namespace TestSVO {

struct Voxel {
	unsigned char density = 0;
	unsigned char material = 0;
	bool operator==(const Voxel &p_other) const {
		return density == p_other.density && material == p_other.material;
	}
};

struct VoxelEmpty {
	bool operator()(const Voxel &p_v) const { return p_v.density == 0; }
};

TEST_CASE("[SVO] default state returns default-constructed T") {
	SVO<int> svo(3);
	CHECK(svo.size() == 8);
	CHECK(svo.depth() == 3);
	for (int x = 0; x < 8; ++x) {
		for (int y = 0; y < 8; ++y) {
			for (int z = 0; z < 8; ++z) {
				CHECK(svo.get(x, y, z) == 0);
			}
		}
	}
}

TEST_CASE("[SVO] set and get roundtrip") {
	SVO<int> svo(4);
	svo.set(3, 5, 7, 42);
	CHECK(svo.get(3, 5, 7) == 42);
	// Other positions remain default.
	CHECK(svo.get(0, 0, 0) == 0);
	CHECK(svo.get(15, 15, 15) == 0);
	CHECK(svo.get(2, 5, 7) == 0);
}

TEST_CASE("[SVO] overwrite same position") {
	SVO<int> svo(3);
	svo.set(1, 2, 3, 10);
	svo.set(1, 2, 3, 20);
	CHECK(svo.get(1, 2, 3) == 20);
}

TEST_CASE("[SVO] setting to empty clears the value") {
	SVO<int> svo(3);
	svo.set(1, 2, 3, 7);
	CHECK(svo.get(1, 2, 3) == 7);
	svo.set(1, 2, 3, 0);
	CHECK(svo.get(1, 2, 3) == 0);
}

TEST_CASE("[SVO] traverse visits only non-empty leaves") {
	SVO<int> svo(3);
	svo.set(1, 1, 1, 5);
	svo.set(4, 4, 4, 9);
	svo.set(7, 0, 0, 3);

	int visit_count = 0;
	int sum = 0;
	svo.traverse([&](int x, int y, int z, int value) {
		(void)x;
		(void)y;
		(void)z;
		++visit_count;
		sum += value;
	});
	CHECK(visit_count == 3);
	CHECK(sum == 17);
}

TEST_CASE("[SVO] traverse reports correct positions") {
	SVO<int> svo(3);
	svo.set(2, 3, 5, 1);

	bool found = false;
	svo.traverse([&](int x, int y, int z, int value) {
		if (x == 2 && y == 3 && z == 5 && value == 1) {
			found = true;
		}
	});
	CHECK(found);
}

TEST_CASE("[SVO] setting then clearing leaves the tree fully empty") {
	SVO<int> svo(3);
	svo.set(0, 0, 0, 1);
	svo.set(7, 7, 7, 2);
	svo.set(0, 0, 0, 0);
	svo.set(7, 7, 7, 0);

	int visit_count = 0;
	svo.traverse([&](int x, int y, int z, int value) {
		(void)x;
		(void)y;
		(void)z;
		(void)value;
		++visit_count;
	});
	CHECK(visit_count == 0);
}

TEST_CASE("[SVO] set_region fills inclusive box") {
	SVO<int> svo(4);
	svo.set_region(2, 2, 2, 5, 5, 5, 7);
	for (int x = 0; x < 16; ++x) {
		for (int y = 0; y < 16; ++y) {
			for (int z = 0; z < 16; ++z) {
				bool inside = (x >= 2 && x <= 5 && y >= 2 && y <= 5 && z >= 2 && z <= 5);
				CHECK(svo.get(x, y, z) == (inside ? 7 : 0));
			}
		}
	}
}

TEST_CASE("[SVO] set_region with empty value clears") {
	SVO<int> svo(3);
	svo.set_region(0, 0, 0, 7, 7, 7, 4);
	svo.set_region(2, 2, 2, 5, 5, 5, 0);

	CHECK(svo.get(0, 0, 0) == 4);
	CHECK(svo.get(3, 3, 3) == 0);
	CHECK(svo.get(7, 7, 7) == 4);
}

TEST_CASE("[SVO] set_region whole-tree fill then clear") {
	SVO<int> svo(3);
	svo.set_region(0, 0, 0, 7, 7, 7, 9);
	CHECK(svo.get(0, 0, 0) == 9);
	CHECK(svo.get(7, 7, 7) == 9);

	svo.set_region(0, 0, 0, 7, 7, 7, 0);
	int visit_count = 0;
	svo.traverse([&](int x, int y, int z, int value) {
		(void)x;
		(void)y;
		(void)z;
		(void)value;
		++visit_count;
	});
	CHECK(visit_count == 0);
}

TEST_CASE("[SVO] custom empty predicate") {
	SVO<Voxel, VoxelEmpty> svo(3);

	Voxel solid_air;
	solid_air.density = 0;
	solid_air.material = 5; // non-default material, but density 0 = empty
	svo.set(1, 1, 1, solid_air);

	int visit_count = 0;
	svo.traverse([&](int x, int y, int z, const Voxel &v) {
		(void)x;
		(void)y;
		(void)z;
		(void)v;
		++visit_count;
	});
	CHECK(visit_count == 0); // empty predicate skipped it

	Voxel solid;
	solid.density = 255;
	solid.material = 5;
	svo.set(1, 1, 1, solid);

	svo.traverse([&](int x, int y, int z, const Voxel &v) {
		(void)x;
		(void)y;
		(void)z;
		(void)v;
		++visit_count;
	});
	CHECK(visit_count == 1);
}

} // namespace TestSVO
