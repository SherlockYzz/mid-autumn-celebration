#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include "../../../core/particle/particle.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

namespace festival::themes::mid_autumn {

/**
 * 东方精致像素玉兔 (第二视觉中心 · 沐于月华之下)
 *
 * 位置：严格居于满月正下方、中央祝福偏右的月华露台之上，与满月呼应。
 * 特性：
 * 1. 结构清晰：优雅长耳、粉红耳瓣、赤朱玉眸、高光晶莹、白毫桃腮、温润身躯、棉球软尾、前爪抱杵、青玉仙臼。
 * 2. R 键肉眼可见互动：抬爪持杵起落捣药、双耳轻快摆动、臼内溅射金色仙芝星芒，并伴随桂花纷纷飘落。
 */
class PixelJadeRabbit {
public:
    PixelJadeRabbit() = default;

    void initialize(float px, float py) {
        m_baseX = px;
        m_baseY = py;
        m_poundTimer = 0.0f;
        m_interactTimer = 0.0f;
        m_earTwitchTimer = 0.0f;
        m_visible = true;
    }

    void setPosition(float px, float py) {
        m_baseX = px;
        m_baseY = py;
    }

    void fadeIn() {
        m_visible = true;
    }

    // 按 R 键触发肉眼可见的捣药抬杵、耳朵轻动与仙草星芒
    void triggerInteraction() {
        m_interactTimer = 10.0f; // 持续 10 秒满载互动
        m_earTwitchTimer = 8.0f;

        // 臼中激荡迸发 32 颗金色仙药星芒
        for (int i = 0; i < 32; ++i) {
            Particle p;
            p.position = {m_baseX + 16.0f, m_baseY + 9.0f};
            float angle = -1.57f + (static_cast<float>(rand() % 140) - 70.0f) * 0.02f;
            float speed = 2.8f + (rand() % 16) * 0.25f;
            p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed * 0.7f};
            p.acceleration = {0.0f, 0.4f};
            p.life = 0.0f;
            p.maxLife = 5.0f + (rand() % 10) * 0.3f;
            p.character = (i % 4 == 0) ? "✨" : ((i % 4 == 1) ? "✦" : ((i % 4 == 2) ? "★" : "·"));
            p.color = (i % 2 == 0) ? Palette::FireworkGold : Palette::OsmanthusLight;
            p.opacity = 1.0f;
            m_sparkles.push_back(p);
        }

        // 周围飘落 16 朵金桂花瓣
        for (int i = 0; i < 16; ++i) {
            Particle p;
            p.position = {m_baseX + (rand() % 36 - 8), m_baseY - 12.0f - (rand() % 10)};
            p.velocity = {(static_cast<float>(rand() % 14) - 7.0f) * 0.1f, 1.0f + (rand() % 8) * 0.15f};
            p.acceleration = {0.0f, 0.2f};
            p.life = 0.0f;
            p.maxLife = 7.0f + (rand() % 6) * 0.4f;
            p.character = (i % 2 == 0) ? "✽" : "❀";
            p.color = Palette::OsmanthusGold;
            p.opacity = 1.0f;
            m_sparkles.push_back(p);
        }
    }

    void triggerHeadTilt(float duration = 8.0f) {
        m_headTiltTimer = duration;
        m_interactTimer = std::max(m_interactTimer, duration);
        m_earTwitchTimer = duration * 0.8f;
    }

    // 奉献中秋佳肴月饼：玉兔品尝、甩耳欢跃、喷吐红心与灵露星芒
    void feedMooncake(const std::string& name, const std::string& reaction) {
        m_feedName = name;
        m_feedReaction = reaction;
        m_feedTimer = 10.0f;
        m_interactTimer = 10.0f;
        m_earTwitchTimer = 8.0f;
        m_headTiltTimer = 8.0f;

        // 臼中激荡迸发美味红心与金星
        for (int i = 0; i < 28; ++i) {
            Particle p;
            p.position = {m_baseX + 16.0f, m_baseY + 9.0f};
            float angle = -1.57f + (static_cast<float>(rand() % 140) - 70.0f) * 0.02f;
            float speed = 2.5f + (rand() % 14) * 0.25f;
            p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed * 0.7f};
            p.acceleration = {0.0f, 0.35f};
            p.life = 0.0f;
            p.maxLife = 5.0f + (rand() % 8) * 0.3f;
            p.character = (i % 3 == 0) ? "♥" : ((i % 3 == 1) ? "✨" : "✿");
            p.color = (i % 3 == 0) ? Palette::LanternLight : Palette::OsmanthusGold;
            p.opacity = 1.0f;
            m_sparkles.push_back(p);
        }
    }

    void update(float dt) {
        m_time += dt;

        if (m_headTiltTimer > 0.0f) {
            m_headTiltTimer -= dt;
        }

        if (m_feedTimer > 0.0f) {
            m_feedTimer -= dt;
        }

        if (m_interactTimer > 0.0f) {
            m_interactTimer -= dt;
            // 欢快生动的捣药冲程
            m_poundTimer += dt * 4.6f;

            // 持续从臼内喷涌灵芝金光
            if (m_sparkles.size() < 40 && (rand() % 2 == 0)) {
                Particle p;
                p.position = {m_baseX + 16.0f, m_baseY + 9.0f};
                float angle = -1.57f + (static_cast<float>(rand() % 80) - 40.0f) * 0.02f;
                float speed = 2.2f + (rand() % 8) * 0.2f;
                p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed * 0.6f};
                p.acceleration = {0.0f, 0.35f};
                p.life = 0.0f;
                p.maxLife = 4.0f;
                p.character = (rand() % 2 == 0) ? "✨" : "✦";
                p.color = Palette::FireworkGold;
                p.opacity = 1.0f;
                m_sparkles.push_back(p);
            }
        } else {
            // 自然平缓的呼吸式捣药
            m_poundTimer += dt * 1.4f;
        }

        if (m_earTwitchTimer > 0.0f) {
            m_earTwitchTimer -= dt;
        }

        // 自然眨眼
        if (std::sin(m_time * 0.7f) > 0.98f && m_earTwitchTimer <= 0.0f) {
            m_earTwitchTimer = 0.5f;
        }

        // 更新星芒与桂花粒子
        for (auto& sp : m_sparkles) {
            sp.life += dt;
            sp.velocity += sp.acceleration * dt;
            sp.position += sp.velocity * dt;
            sp.opacity = std::max(0.0f, 1.0f - sp.life / sp.maxLife);
        }
        m_sparkles.erase(
            std::remove_if(m_sparkles.begin(), m_sparkles.end(),
                [](const Particle& p) { return !p.isAlive() || p.opacity <= 0.04f; }),
            m_sparkles.end());
    }

    void render(Renderer& renderer) {
        if (!m_visible) return;

        int bx = static_cast<int>(std::round(m_baseX));
        int by = static_cast<int>(std::round(m_baseY));

        Color furWhite   = Color::fromHex(0xFFFEFA); // 纯净皎月白毛
        Color furShade   = Color::fromHex(0xE5DFC9); // 柔美背阴暖灰
        Color earInner   = Color::fromHex(0xF5B7B1); // 柔嫩粉瓣内耳
        Color earDeep    = Color::fromHex(0xE5989B); // 耳心深粉
        Color rubyEye    = Color::fromHex(0xD32F2F); // 赤朱宝石红眸
        Color eyeGleam   = Color::fromHex(0xFFFFFF); // 眸中皓月高光
        Color nosePink   = Color::fromHex(0xF7A8B8); // 琼鼻
        Color cheekBlush = Color::fromHex(0xFCE4EC); // 桃腮
        Color pestleGold = Color::fromHex(0xFFC837); // 金光宝杵
        Color pestleDeep = Color::fromHex(0xE67E22); // 杵柄暗金
        Color mortarRim  = Color::fromHex(0x81C784); // 青玉臼外撇沿
        Color mortarBody = Color::fromHex(0x388E3C); // 碧玉臼身
        Color mortarDark = Color::fromHex(0x1B5E20); // 臼底暗影
        Color elixirGold = Color::fromHex(0xFFE082); // 臼内灵液金光

        // 呼吸起伏
        int breath = (std::sin(m_time * 2.2f) > 0.0f) ? 0 : 1;

        // 肉眼可见的抬手捣药行程 (大幅起落动态)
        float poundPhase = std::sin(m_poundTimer * 4.2f);
        int pestleLift = 0;
        if (poundPhase > 0.2f) {
            pestleLift = -4; // 玉杵高高举过头顶！
        } else if (poundPhase < -0.2f) {
            pestleLift = 2;  // 重重捣入灵臼，金光四射！
        }

        // 1. 双耳 (长耳优雅，粉红内廓，具生动摆动)
        bool twitch = (m_earTwitchTimer > 0.0f);
        if (twitch) {
            // 警觉微动长耳
            for (int y = 0; y <= 5; ++y) {
                renderer.drawPixel(bx + 4, by + y, furWhite);
                renderer.drawPixel(bx + 5, by + y, (y >= 1 && y <= 4) ? earInner : furWhite);
                renderer.drawPixel(bx + 6, by + y, furWhite);
            }
            renderer.drawPixel(bx + 5, by + 2, earDeep);

            // 后耳微翘
            for (int y = 1; y <= 5; ++y) {
                renderer.drawPixel(bx + 8, by + y, furShade);
                renderer.drawPixel(bx + 9, by + y, (y >= 2 && y <= 4) ? earInner : furShade);
                renderer.drawPixel(bx + 10, by + y, furWhite);
            }
        } else {
            // 自然倾斜温柔长耳
            for (int y = 1; y <= 5; ++y) {
                renderer.drawPixel(bx + 4, by + y, furWhite);
                renderer.drawPixel(bx + 5, by + y, (y >= 2 && y <= 4) ? earInner : furWhite);
                renderer.drawPixel(bx + 6, by + y, furWhite);
            }
            renderer.drawPixel(bx + 5, by + 3, earDeep);

            // 后耳向后优雅后掠
            renderer.drawPixel(bx + 8,  by + 2, furShade);
            renderer.drawPixel(bx + 9,  by + 2, furShade);
            renderer.drawPixel(bx + 8,  by + 3, furShade);
            renderer.drawPixel(bx + 9,  by + 3, earInner);
            renderer.drawPixel(bx + 10, by + 3, furWhite);
            renderer.drawPixel(bx + 9,  by + 4, earInner);
            renderer.drawPixel(bx + 10, by + 4, furWhite);
        }

        // 2. 玉兔头部轮廓与灵动五官 (Row 6 ~ 9)
        int headOffset = (m_headTiltTimer > 0.0f) ? -1 : 0; // 抬头姿态
        for (int y = 6; y <= 9; ++y) {
            int xStart = (y == 6) ? 4 : ((y == 9) ? 3 : 2);
            int xEnd   = (y == 8) ? 11 : 10;
            for (int x = xStart; x <= xEnd; ++x) {
                renderer.drawPixel(bx + x, by + y + headOffset, furWhite);
            }
        }
        // 琼鼻
        renderer.drawPixel(bx + 11, by + 8 + headOffset, nosePink);

        // 赤朱宝珠明眸
        bool blink = (std::sin(m_time * 3.2f) > 0.985f && m_earTwitchTimer <= 0.0f);
        if (!blink) {
            renderer.drawPixel(bx + 7, by + 7 + headOffset, eyeGleam); // 眸中月光倒影
            renderer.drawPixel(bx + 8, by + 7 + headOffset, rubyEye);
            renderer.drawPixel(bx + 7, by + 8 + headOffset, rubyEye);
            renderer.drawPixel(bx + 8, by + 8 + headOffset, Color::fromHex(0x8B0000));
        } else {
            renderer.drawPixel(bx + 7, by + 7 + headOffset, Color::fromHex(0x9E2A2B));
            renderer.drawPixel(bx + 8, by + 7 + headOffset, Color::fromHex(0x9E2A2B));
        }

        // 桃腮
        renderer.drawPixel(bx + 9,  by + 8 + headOffset, cheekBlush);
        renderer.drawPixel(bx + 10, by + 8 + headOffset, cheekBlush);

        // 3. 丰润白玉身躯与软萌尾巴 (Row 10 ~ 14)
        for (int y = 10; y <= 14; ++y) {
            int xStart = (y <= 11) ? 2 : 3;
            int xEnd   = (y >= 13) ? 12 : 11;
            for (int x = xStart; x <= xEnd; ++x) {
                Color c = (y >= 13 || x <= 3) ? furShade : furWhite;
                renderer.drawPixel(bx + x, by + y + breath, c);
            }
        }
        // 棉球小圆尾
        renderer.drawPixel(bx + 0, by + 12 + breath, furWhite);
        renderer.drawPixel(bx + 1, by + 11 + breath, furWhite);
        renderer.drawPixel(bx + 1, by + 12 + breath, furWhite);
        renderer.drawPixel(bx + 1, by + 13 + breath, furShade);

        // 4. 前爪持金杵 (明确抬手起落动态)
        renderer.drawPixel(bx + 11, by + 10 + breath, furWhite);
        renderer.drawPixel(bx + 12, by + 10 + breath, furWhite);
        renderer.drawPixel(bx + 12, by + 11 + breath, furShade);

        // 赤金灵杵 (随 pestleLift 起伏)
        int pyP = by + 6 + pestleLift;
        renderer.drawPixel(bx + 14, pyP + 0, pestleGold);
        renderer.drawPixel(bx + 14, pyP + 1, pestleGold);
        renderer.drawPixel(bx + 15, pyP + 2, pestleDeep);
        renderer.drawPixel(bx + 15, pyP + 3, pestleGold);
        renderer.drawPixel(bx + 15, pyP + 4, pestleGold);
        renderer.drawPixel(bx + 16, pyP + 5, pestleDeep);
        renderer.drawPixel(bx + 16, pyP + 6, pestleGold); // 击打端

        // 5. 青玉仙臼 (包含外撇沿与臼内金色灵液)
        for (int x = 14; x <= 20; ++x) {
            renderer.drawPixel(bx + x, by + 11, mortarRim);
        }
        // 灵液微光
        renderer.drawPixel(bx + 16, by + 12, elixirGold);
        renderer.drawPixel(bx + 17, by + 12, elixirGold);
        renderer.drawPixel(bx + 18, by + 12, elixirGold);

        for (int y = 12; y <= 15; ++y) {
            int xs = (y == 15) ? 15 : 14;
            int xe = (y == 15) ? 19 : 20;
            for (int x = xs; x <= xe; ++x) {
                if (y == 12 && x >= 16 && x <= 18) continue;
                Color mc = (x == xs || x == xe) ? mortarDark : mortarBody;
                renderer.drawPixel(bx + x, by + y, mc);
            }
        }

        // 6. 灵气星芒 (以真实字符栅格绘制，璀璨可见)
        for (const auto& sp : m_sparkles) {
            int cx = static_cast<int>(std::round(sp.position.x));
            int cy = static_cast<int>(std::round(sp.position.y / 2.0f));
            if (cy >= 0 && cy < renderer.height() && cx >= 0 && cx < renderer.width()) {
                renderer.drawChar(cx, cy, sp.character, sp.color.withOpacity(sp.opacity));
            }
        }

        // 7. 灵兔专属醒目祈福/品饼金匾 (位于玉兔露台上方第 13 行，绝不遮挡)
        if (m_feedTimer > 0.0f) {
            float alpha = std::clamp(m_feedTimer / 1.5f, 0.0f, 1.0f);
            int textX = std::max(2, bx - 10);
            int textY = 13;
            std::string badge = " 🥮【食韵】" + m_feedName + " · " + m_feedReaction + " ";
            renderer.drawText(textX, textY, badge, Palette::OsmanthusGold.withOpacity(alpha), Palette::NightDeep, true);
        } else if (m_interactTimer > 0.0f) {
            float alpha = std::clamp(m_interactTimer / 1.5f, 0.0f, 1.0f);
            int textX = std::max(2, bx - 6);
            int textY = 13;
            std::string badge = " ❖ ✨ 灵兔捣仙芝 · 岁岁享康宁 ✨ ❖ ";
            renderer.drawText(textX, textY, badge, Palette::FireworkGold.withOpacity(alpha), Palette::NightDeep, true);
        }
    }

private:
    float m_baseX = 52.0f;
    float m_baseY = 26.0f;
    float m_time = 0.0f;
    float m_poundTimer = 0.0f;
    float m_interactTimer = 0.0f;
    float m_earTwitchTimer = 0.0f;
    float m_headTiltTimer = 0.0f;
    float m_feedTimer = 0.0f;
    std::string m_feedName;
    std::string m_feedReaction;
    bool m_visible = true;

    std::vector<Particle> m_sparkles;
};

} // namespace festival::themes::mid_autumn
