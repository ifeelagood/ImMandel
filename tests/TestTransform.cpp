#include <gtest/gtest.h>

#include "macros.h"
#include "Transform.h"

TEST(TransformTests, ChangedSetsAndResets) {
	Transform<float> t({ 10,10 });

	EXPECT_TRUE(t.changed());  // initially set to true
	EXPECT_FALSE(t.changed()); // resets to false upon read

	t.set_position({ 1.f, 1.f });
	EXPECT_TRUE(t.changed());
}

TEST(TransformTests, TransformComposedWithInverseIsIdentity) {
	Transform<float> t({ 10,10 });

	Eigen::Vector2<float> a = { 1.0, 1.0 };
	Eigen::Vector2<float> b = { 0.0, 1.0 };

	constexpr float eps = 0.000001f;

	EXPECT_EIGEN_VECTOR2_NEAR(a, t.inverse_transform_point(t.transform_point(a)), eps);
	EXPECT_EIGEN_VECTOR2_NEAR(b, t.inverse_transform_point(t.transform_point(b)), eps);
}