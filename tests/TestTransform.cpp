#include <gtest/gtest.h>

#include "macros.h"
#include "Transform.h"

TEST(TransformTests, ChangedSetsAndResets) {
	Transform<float> t({ 300,300 });

	EXPECT_TRUE(t.changed());  // initially set to true
	EXPECT_FALSE(t.changed()); // resets to false upon read

	t.set_position({ 1.f, 1.f });
	EXPECT_TRUE(t.changed());
}

TEST(TransformTests, TransformComposedWithInverseIsIdentity) {
	Transform<float> t({ 300,300 }); // 1 pixel -> 0.01 fractal coords

	Eigen::Vector2<float> a = { 1.0, 1.0 };
	Eigen::Vector2<float> b = { 0.0, 1.0 };

	constexpr float eps = 0.00001f;

	EXPECT_EIGEN_VECTOR2_NEAR(a, t.inverse_transform_point(t.transform_point(a)), eps);
	EXPECT_EIGEN_VECTOR2_NEAR(b, t.inverse_transform_point(t.transform_point(b)), eps);
}

TEST(TransformTests, TransformPoint) {
	Transform<float> t({ 300,300 });

	const Eigen::Vector4<float>& rect = t.get_rect();
	const Eigen::Vector2<int>& size = t.get_size();

	const Eigen::Vector2<float> f_min = { rect.x(), rect.z() };
	const Eigen::Vector2<float> f_max = { rect.y(), rect.w() };
	const Eigen::Vector2<float> f_mid = { 0.f, 0.f };

	const Eigen::Vector2<float> s_min = { 0, 0 };
	const Eigen::Vector2<float> s_max = size.cast<float>();
	const Eigen::Vector2<float> s_mid = { 200, 150 };

	EXPECT_EQ(t.transform_point(s_min), f_min);
	EXPECT_EQ(t.transform_point(s_mid), f_mid);
	EXPECT_EQ(t.transform_point(s_max), f_max);

}

TEST(TransformTests, InvserseTransformPoint) {
	Transform<float> t({ 300,300 });

	const Eigen::Vector4<float>& rect = t.get_rect();
	const Eigen::Vector2<int>& size = t.get_size();

	const Eigen::Vector2<float> f_min = { rect.x(), rect.z() };
	const Eigen::Vector2<float> f_max = { rect.y(), rect.w() };
	const Eigen::Vector2<float> f_mid = { 0.f, 0.f };

	const Eigen::Vector2<float> s_min = { 0, 0 };
	const Eigen::Vector2<float> s_max = size.cast<float>();
	const Eigen::Vector2<float> s_mid = { 200, 150 };

	EXPECT_EQ(t.inverse_transform_point(f_mid), s_mid);
	EXPECT_EQ(t.inverse_transform_point(f_max), s_max);
	EXPECT_EQ(t.inverse_transform_point(f_min), s_min);

}

TEST(TransformTests, TransformPositionInvariablyPosition) {
	Transform<float> t({ 300, 300 });

	EXPECT_EQ(t.centre(), Eigen::Vector2<float>({ -0.5, 0 }));
	EXPECT_EQ(t.centre(), t.get_position());

	// TODO some transformations

	t.set_size({ 3000, 3000 });
	EXPECT_EQ(t.centre(), t.get_position());

	t.set_zoom(32, {0,0});
	EXPECT_EQ(t.centre(), t.get_position());
}

TEST(TransformTests, SetZoomPreservesAnchorPosition) {
	// invariant: let a be an anchor in screen space, t be a transform and t' be the transform after scaling
	// t(a) = t'(a)
	constexpr float eps = 0.00001f;

	Transform<float> t1({ 300, 300 });
	const Eigen::Vector2i a1 = { 200, 150 }; // trivial centre anchor

	const Eigen::Vector2<float> a1_f0 = t1.transform_point(a1.cast<float>());
	
	t1.set_zoom(16, a1_f0);
	EXPECT_EIGEN_VECTOR2_NEAR(a1_f0, t1.transform_point(a1.cast<float>()), eps);
	t1.set_zoom(1, a1_f0);
	EXPECT_EIGEN_VECTOR2_NEAR(a1_f0, t1.transform_point(a1.cast<float>()), eps);
	t1.set_zoom(2048, a1_f0);
	EXPECT_EIGEN_VECTOR2_NEAR(a1_f0, t1.transform_point(a1.cast<float>()), eps);

	Transform<float> t2({ 300, 300 });
	const Eigen::Vector2i a2 = { 100, 50 };

	const Eigen::Vector2<float> a2_f0 = t2.transform_point(a2.cast<float>());

	t2.set_zoom(16, a2_f0);
	EXPECT_EIGEN_VECTOR2_NEAR(a2_f0, t2.transform_point(a2.cast<float>()), eps);
	t2.set_zoom(1, a2_f0);
	EXPECT_EIGEN_VECTOR2_NEAR(a2_f0, t2.transform_point(a2.cast<float>()), eps);
	t2.set_zoom(2048, a2_f0);
	EXPECT_EIGEN_VECTOR2_NEAR(a2_f0, t2.transform_point(a2.cast<float>()), eps);
}

TEST(TransformTests, SetSizePreservesCentrePosition) {
	Transform<float> t({ 300,300 });

	const Eigen::Vector2<float> c0 = t.get_position();

	t.set_size({ 30,30 });
	EXPECT_EQ(c0, t.get_position());

	t.set_size({ 30000, 30000 });
	EXPECT_EQ(c0, t.get_position());

	t.set_size({ 1000,1000 });
	EXPECT_EQ(c0, t.get_position());

	t.set_size({ 300, 300 });
	EXPECT_EQ(c0, t.get_position());
}