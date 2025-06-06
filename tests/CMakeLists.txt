add_executable(ImMandelUnitTest
	"TestTransform.cpp"
)

find_package(GTest CONFIG REQUIRED)

# link core static library for testing
target_link_libraries(ImMandelUnitTest PRIVATE
	ImMandelCore
    GTest::gtest_main
)

target_include_directories(ImMandelUnitTest PRIVATE ${PROJECT_SOURCE_DIR}/src)

include(GoogleTest)
gtest_discover_tests(ImMandelUnitTest)