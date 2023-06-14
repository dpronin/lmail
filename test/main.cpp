#include <string>

#include <gtest/gtest.h>

#include "crypt.hpp"

using namespace lmail;
using namespace testing;

namespace lmail::test
{

TEST(Cyphering, EncryptDecrypt)
{
    auto key_pair  = generate_rsa_key_pair(RSA_KEY_SIZE_DEFAULT);
    std::string m  = "Fusce tincidunt at mauris ut molestie. Integer a purus egestas, porta sem at, "
                     "condimentum mauris. Fusce vitae quam gravida, imperdiet mauris quis, ullamcorper nisi. "
                     "Fusce dignissim neque vel nisl eleifend, at faucibus elit egestas. Pellentesque habitant morbi "
                     "tristique senectus et netus et malesuada fames ac turpis egestas. Class aptent taciti sociosqu "
                     "ad litora torquent per conubia nostra, per inceptos himenaeos. Duis at sapien ut ex sollicitudin "
                     "dapibus quis nec dui. Donec nec laoreet risus. Praesent vehicula non ex eget iaculis. "
                     "Aenean hendrerit sem fermentum quam venenatis, et ullamcorper odio aliquam. Aenean pharetra, "
                     "nisl sed interdum finibus, nulla libero viverra mi, vel mollis sem arcu sed metus. Nulla commodo "
                     "lectus id dictum varius. Curabitur non magna fringilla eros molestie sodales et sit amet neque. "
                     "Nunc ac aliquet felis, ac porta nisi. Donec ligula justo, lobortis vel nibh sed, pharetra tempor orci.";
    std::string cm = m;
    encrypt(cm, key_pair.second);
    decrypt(cm, key_pair.first);
    ASSERT_EQ(m, cm);
}

} // namespace lmail::test

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
