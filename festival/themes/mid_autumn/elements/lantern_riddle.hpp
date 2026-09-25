#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include "../../../core/platform/input_event.hpp"
#include <string>
#include <vector>
#include <algorithm>

namespace festival::themes::mid_autumn {

struct Riddle {
    std::string question;
    std::string hint;
    std::vector<std::string> options;
    int correctIndex;
    std::string answer;
    std::string explanation;
};

enum class RiddleReward {
    None,
    CorrectAnswer,
    Revealed
};

class LanternRiddleGame {
public:
    LanternRiddleGame() {
        m_riddles = {
            {
                "中秋大团圆",
                "打一数学名词",
                {"A. 圆周率", "B. 黄金分割", "C. 自然对数", "D. 虚数单位"},
                0,
                "圆周率",
                "月满中秋，万家团聚，周而复始，圆满无尽！"
            },
            {
                "月落日出天益明",
                "打一汉字",
                {"A. 旦", "B. 朋", "C. 明", "D. 昭"},
                1,
                "朋",
                "日月相伴，千里逢迎，天涯挚友共良辰。"
            },
            {
                "十五的月亮悬空照",
                "打一四字成语",
                {"A. 光明磊落", "B. 光明正大", "C. 月白风清", "D. 皓月千里"},
                1,
                "光明正大",
                "清辉朗朗洒人间，明镜高悬映乾坤。"
            },
            {
                "平分秋色一轮满",
                "打一传统节日",
                {"A. 重阳节", "B. 七夕节", "C. 中秋节", "D. 元宵节"},
                2,
                "中秋节",
                "三秋恰半，金风玉露，最是人间团圆时。"
            },
            {
                "明月照我还",
                "打一明代文学家",
                {"A. 归有光", "B. 汤显祖", "C. 冯梦龙", "D. 吴承恩"},
                0,
                "归有光",
                "月华如昼引归人，故土情深意韵长。"
            },
            {
                "中秋赏月，折桂殿前",
                "打一祝贺吉语",
                {"A. 步步高升", "B. 蟾宫折桂", "C. 金玉满堂", "D. 万事胜意"},
                1,
                "蟾宫折桂",
                "折桂蟾宫步青云，金榜题名捷报传！"
            },
            {
                "圆圆小饼香又甜，中秋月下寄团圆",
                "打一节日食品",
                {"A. 桂花糕", "B. 汤圆", "C. 月饼", "D. 糍粑"},
                2,
                "月饼",
                "小饼如嚼月，中有酥和饴，默品人间至味。"
            },
            {
                "白兔捣药秋复春",
                "打一中药名",
                {"A. 当归", "B. 苦参", "C. 仙茅", "D. 茯苓"},
                0,
                "当归",
                "灵兔捣药年复年，盼游子早日当归团聚。"
            },
            {
                "八月十五共婵娟",
                "打一古文篇名",
                {"A. 滕王阁序", "B. 赤壁赋", "C. 岳阳楼记", "D. 桃花源记"},
                1,
                "赤壁赋",
                "苏子泛舟赤壁，诵明月之诗，歌窈窕之章。"
            },
            {
                "中秋佳节结良缘",
                "打一我国直辖市",
                {"A. 北京", "B. 上海", "C. 天津", "D. 重庆"},
                3,
                "重庆",
                "佳节逢良缘，双喜临门，大庆重重！"
            },
            {
                "举头望明月",
                "打一中药名",
                {"A. 怀熟", "B. 细辛", "C. 当归", "D. 望月砂"},
                2,
                "当归",
                "举头望明月，低头思故乡，切切当归时。"
            },
            {
                "一轮明月挂中天",
                "打一四字成语",
                {"A. 顶天立地", "B. 高高在上", "C. 望子成龙", "D. 普天同庆"},
                1,
                "高高在上",
                "玉轮高悬于九重苍穹之上，清光如银。"
            }
        };
    }

    bool isActive() const { return m_active; }

    void open() {
        m_active = true;
        m_revealed = false;
        m_answered = false;
        m_isCorrect = false;
        m_lastReward = RiddleReward::None;
    }

    void close() {
        m_active = false;
        m_revealed = false;
        m_answered = false;
        m_lastReward = RiddleReward::None;
    }

    void nextRiddle() {
        m_currentIndex = (m_currentIndex + 1) % m_riddles.size();
        m_revealed = false;
        m_answered = false;
        m_isCorrect = false;
        m_lastReward = RiddleReward::None;
    }

    void reveal() {
        m_revealed = true;
        m_lastReward = RiddleReward::Revealed;
    }

    int score() const { return m_score; }
    int streak() const { return m_streak; }
    RiddleReward lastReward() const { return m_lastReward; }
    void clearReward() { m_lastReward = RiddleReward::None; }

    bool handleInput(const core::InputEvent& event) {
        if (!m_active) return false;

        if (event.code == core::KeyCode::Escape) {
            close();
            return true;
        }

        if (event.code == core::KeyCode::Enter) {
            reveal();
            return true;
        }

        if (event.isChar('n')) {
            nextRiddle();
            return true;
        }

        // Choice keys: A, B, C, D or 1, 2, 3, 4
        int selected = -1;
        if (event.isChar('a') || event.isChar('1')) selected = 0;
        else if (event.isChar('b') || event.isChar('2')) selected = 1;
        else if (event.isChar('c') || event.isChar('3')) selected = 2;
        else if (event.isChar('d') || event.isChar('4')) selected = 3;

        if (selected >= 0) {
            const auto& r = m_riddles[m_currentIndex];
            m_answered = true;
            m_selectedOption = selected;
            if (selected == r.correctIndex) {
                m_isCorrect = true;
                m_revealed = true;
                m_score += 50;
                m_streak++;
                m_lastReward = RiddleReward::CorrectAnswer;
            } else {
                m_isCorrect = false;
                m_streak = 0;
            }
            return true;
        }

        return false;
    }

    void render(Renderer& renderer) {
        if (!m_active) return;

        const auto& r = m_riddles[m_currentIndex];

        int w = std::min(68, renderer.width() - 4);
        int h = 11;
        int x = (renderer.width() - w) / 2;
        int y = (renderer.height() - h) / 2;

        Color cardBg    = Palette::NightWater;
        Color borderCol = Palette::LanternRed;
        Color titleCol  = Palette::LanternLight;
        Color textCol   = Palette::PoetryText;
        Color optCol    = Palette::MoonWhite;
        Color correctCol = Palette::OsmanthusGold;
        Color wrongCol   = Palette::LanternDarkRed;
        Color hintCol   = Palette::PoetrySub;

        // Card frame
        renderer.fillRect(x, y, w, h, cardBg);
        renderer.drawBorder(x, y, w, h, borderCol);

        // Header Title & Score
        std::string title = " 🏮【宵月会游园 · 雅趣猜灯谜】🏮 ";
        renderer.drawCenterText(y, title, titleCol, cardBg, true);

        std::string scoreStr = " 🏆 连对: " + std::to_string(m_streak) + " 题 │ 累计得分: " + std::to_string(m_score) + " 分 ";
        renderer.drawText(x + w - 24, y + 1, scoreStr, Palette::FireworkGold, cardBg);

        // Riddle question
        std::string qLine = "谜面：" + r.question + "  (" + r.hint + ")";
        renderer.drawText(x + 3, y + 2, qLine, textCol, cardBg, true);

        // 4 Options (2 rows of 2 options)
        if (r.options.size() >= 4) {
            std::string opt0 = r.options[0];
            std::string opt1 = r.options[1];
            std::string opt2 = r.options[2];
            std::string opt3 = r.options[3];

            // Row 1: A and B
            renderer.drawText(x + 4, y + 4, opt0, (m_answered && m_selectedOption == 0) ? (m_isCorrect ? correctCol : wrongCol) : optCol, cardBg);
            renderer.drawText(x + 34, y + 4, opt1, (m_answered && m_selectedOption == 1) ? (m_isCorrect ? correctCol : wrongCol) : optCol, cardBg);

            // Row 2: C and D
            renderer.drawText(x + 4, y + 5, opt2, (m_answered && m_selectedOption == 2) ? (m_isCorrect ? correctCol : wrongCol) : optCol, cardBg);
            renderer.drawText(x + 34, y + 5, opt3, (m_answered && m_selectedOption == 3) ? (m_isCorrect ? correctCol : wrongCol) : optCol, cardBg);
        }

        // Status or Explanation
        if (m_answered && m_isCorrect) {
            std::string banner = "🎉 聪颖绝伦！回答正确！获得宵月热度 +50！(按 [N] 键挑战下一题)";
            renderer.drawText(x + 3, y + 7, banner, correctCol, cardBg, true);
            std::string expLine = "解曰：" + r.explanation;
            renderer.drawText(x + 3, y + 8, expLine, textCol, cardBg);
        } else if (m_answered && !m_isCorrect) {
            std::string banner = "🏮 差一点点，再凝神细思~ (按 [ENTER] 揭晓谜底，或按 [N] 下一题)";
            renderer.drawText(x + 3, y + 7, banner, Palette::LanternLight, cardBg);
        } else if (m_revealed) {
            std::string ansLine = "💡 谜底：【 " + r.answer + " 】  " + r.explanation;
            renderer.drawText(x + 3, y + 7, ansLine, correctCol, cardBg, true);
        } else {
            std::string prompt = "请按键 [A / B / C / D] 选择答案，或按 [ENTER] 查阅秘解";
            renderer.drawText(x + 3, y + 7, prompt, hintCol, cardBg);
        }

        // Footer hint
        std::string footer = " [A-D] 作答  [ENTER] 揭晓  [N] 下一题  [ESC] 退出游园 ";
        renderer.drawCenterText(y + h - 1, footer, hintCol, cardBg);
    }

private:
    std::vector<Riddle> m_riddles;
    size_t m_currentIndex = 0;
    bool m_active = false;
    bool m_revealed = false;
    bool m_answered = false;
    bool m_isCorrect = false;
    int m_selectedOption = -1;
    int m_score = 0;
    int m_streak = 0;
    RiddleReward m_lastReward = RiddleReward::None;
};

} // namespace festival::themes::mid_autumn
