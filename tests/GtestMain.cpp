#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    std::cout << std::endl << std::endl;
    ::testing::InitGoogleTest(&argc, argv); 
    auto retVal = RUN_ALL_TESTS();
    std::cout << std::endl << std::endl;
    return retVal;
}