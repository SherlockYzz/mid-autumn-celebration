#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include <string>
#include <vector>

namespace festival::themes::mid_autumn {

struct Mooncake {
    std::string name;
    std::string flavor;
    std::string rabbitReaction;
};

class MooncakeFeast {
public:
    MooncakeFeast() {
        m_delicacies = {
            {
                "金桂冰皮月饼",
                "冰清软糯，桂魄凝香，宛若蟾宫仙品",
                "(*^▽^*) 仙气沁脾，周身飘落金桂！"
            },
            {
                "流心奶黄月饼",
                "金沙流蜜，奶香浓郁，一口甜入心坎",
                "(≧∇≦) 软糯爆浆，击打出流金星火！"
            },
            {
                "蛋黄白莲蓉月饼",
                "双黄映月，莲蓉温润，百年中秋至味",
                "(๑´ڡ`๑) 仰头拜月，祥云护体福泽长！"
            },
            {
                "苏式酥皮鲜肉月饼",
                "千层酥脆，鲜美多汁，江南人间烟火",
                "(`･ω･´) 酥香滚烫，长街灯火齐应和！"
            },
            {
                "传统金腿五仁月饼",
                "仁果醇厚，金腿回甘，岁月越久越醇香",
                "(´∀｀) 齿颊留芳，赐下灵露仙芝！"
            }
        };
    }

    const Mooncake& serveNext() {
        const auto& item = m_delicacies[m_currentIndex];
        m_currentIndex = (m_currentIndex + 1) % m_delicacies.size();
        m_servedCount++;
        return item;
    }

    int servedCount() const { return m_servedCount; }

private:
    std::vector<Mooncake> m_delicacies;
    size_t m_currentIndex = 0;
    int m_servedCount = 0;
};

} // namespace festival::themes::mid_autumn
