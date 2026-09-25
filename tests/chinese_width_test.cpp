#include "../festival/core/text/unicode_width.hpp"
#include <iostream>
#include <cassert>

using namespace festival::core;

int main() {
    std::string s1 = "举杯邀明月，对影成三人。";
    std::string s2 = "但愿人长久，千里共婵娟。";
    std::string s3 = "愿望：希望今年顺利保研";

    std::cout << "Testing Chinese UTF-8 string visual widths:" << std::endl;
    // s1: 12 characters (10 Chinese + 2 Chinese punctuation) = 24 columns
    int w1 = stringVisualWidth(s1);
    std::cout << s1 << " -> visual width: " << w1 << std::endl;
    assert(w1 == 24);

    // s2: 12 characters = 24 columns
    int w2 = stringVisualWidth(s2);
    std::cout << s2 << " -> visual width: " << w2 << std::endl;
    assert(w2 == 24);

    // s3: 11 characters (10 Chinese + 1 Chinese fullwidth colon) = 22 columns
    int w3 = stringVisualWidth(s3);
    std::cout << s3 << " -> visual width: " << w3 << std::endl;
    assert(w3 == 22);

    std::string b1 = "祝【大家】中秋快乐";
    int wb1 = stringVisualWidth(b1);
    std::cout << b1 << " -> visual width: " << wb1 << std::endl;
    assert(wb1 == 18);

    std::string b2 = "祝【淳阳项目组】中秋快乐";
    int wb2 = stringVisualWidth(b2);
    std::cout << b2 << " -> visual width: " << wb2 << std::endl;
    assert(wb2 == 24);

    std::cout << "All Chinese width tests PASSED!" << std::endl;
    return 0;
}
