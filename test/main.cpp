#include <gtest/gtest.h>

using namespace testing;

namespace lmail::test
{

} // namespace lmail::test

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
