#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include "../../../core/particle/particle.hpp"
#include "../../../core/text/unicode_width.hpp"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

namespace festival::themes::mid_autumn {

/**
 * 月下题名系统 (Moonlight Inscription Blessing System)
 *
 * 功能：
 * 1. 维护受福之人称呼（默认“大家”，呈现“祝【大家】中秋快乐”）。
 * 2. 按 N 进入“月下题名”中文输入弹窗，支持全中文与退格。
 * 3. 题名动画：确认后文字化作流光金墨粒子，飘向月空再落入中央匾额。
 * 4. 联动触发玉兔抬头、满月光华、灯笼亮起与高空礼花。
 */
class BlessingSystem {
public:
    BlessingSystem() : m_targetName("大家") {}

    const std::string& targetName() const {
        return m_targetName;
    }

    std::string fullBlessing() const {
        return "祝【" + m_targetName + "】中秋快乐";
    }

    bool isInputting() const {
        return m_isInputting;
    }

    void startInput() {
        m_isInputting = true;
        m_inputText.clear();
        m_cursorTimer = 0.0f;
    }

    void cancelInput() {
        m_isInputting = false;
        m_inputText.clear();
    }

    void handleBackspace() {
        if (m_inputText.empty()) return;
        // 按照 UTF-8 字符边界安全回退
        while (!m_inputText.empty()) {
            unsigned char b = static_cast<unsigned char>(m_inputText.back());
            m_inputText.pop_back();
            if ((b & 0xC0) != 0x80) {
                break; // 弹出前导字节
            }
        }
    }

    void handleInputText(const std::string& text) {
        if (core::stringVisualWidth(m_inputText) >= 28) return; // 限制合理长度防溢出
        m_inputText += text;
    }

    // 提交题名并启动金色墨迹题名动画，返回是否成功
    bool submitInput(float startX, float startY) {
        if (!m_inputText.empty()) {
            m_targetName = m_inputText;
        }
        m_isInputting = false;

        // 启动题名动画
        m_animating = true;
        m_animTimer = 3.6f;
        m_highlightProgress = 1.0f;

        // 生成飘向明月的金色流光墨迹粒子
        m_inkParticles.clear();
        for (int i = 0; i < 28; ++i) {
            Particle p;
            p.position = {startX + (rand() % 16 - 8), startY + (rand() % 4 - 2)};
            float angle = -1.57f + (static_cast<float>(rand() % 80) - 40.0f) * 0.02f;
            float speed = 3.0f + (rand() % 15) * 0.2f;
            p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed * 0.6f};
            p.acceleration = {0.8f, -0.4f}; // 向右上月亮方向徐徐飘动
            p.life = 0.0f;
            p.maxLife = 2.5f + (rand() % 10) * 0.1f;
            p.character = (i % 3 == 0) ? "✦" : ((i % 3 == 1) ? "·" : "°");
            p.color = Palette::FireworkGold;
            p.opacity = 1.0f;
            m_inkParticles.push_back(p);
        }

        return true;
    }

    float highlightProgress() const {
        return m_highlightProgress;
    }

    bool isAnimating() const {
        return m_animating;
    }

    void update(float dt) {
        m_cursorTimer += dt;

        if (m_highlightProgress > 0.0f) {
            m_highlightProgress = std::max(0.0f, m_highlightProgress - dt * 0.45f);
        }

        if (m_animating) {
            m_animTimer -= dt;
            if (m_animTimer <= 0.0f) {
                m_animating = false;
            }

            // 更新金墨粒子
            for (auto& p : m_inkParticles) {
                p.life += dt;
                p.velocity += p.acceleration * dt;
                p.position += p.velocity * dt;
                p.opacity = std::max(0.0f, 1.0f - p.life / p.maxLife);
            }
            m_inkParticles.erase(
                std::remove_if(m_inkParticles.begin(), m_inkParticles.end(),
                    [](const Particle& p) { return !p.isAlive() || p.opacity <= 0.05f; }),
                m_inkParticles.end());
        }
    }

    void renderParticles(Renderer& renderer) {
        if (m_animating) {
            for (const auto& p : m_inkParticles) {
                int px = static_cast<int>(std::round(p.position.x));
                int py = static_cast<int>(std::round(p.position.y));
                renderer.drawChar(px, py, p.character, p.color.withOpacity(p.opacity));
            }
        }
    }

    void renderModal(Renderer& renderer, int screenWidth, int screenHeight) {
        if (m_isInputting) {
            renderInputModal(renderer, screenWidth, screenHeight);
        }
    }

    void render(Renderer& renderer, int screenWidth, int screenHeight) {
        renderParticles(renderer);
        renderModal(renderer, screenWidth, screenHeight);
    }

private:
    void renderInputModal(Renderer& renderer, int screenWidth, int screenHeight) {
        int modalW = 46;
        int modalH = 8;
        int mx = (screenWidth - modalW) / 2;
        int my = (screenHeight - modalH) / 2;

        Color borderCol  = Palette::FireworkGold;
        Color bgCol      = Palette::NightDeep;
        Color titleCol   = Palette::MoonWhite;
        Color labelCol   = Palette::OsmanthusLight;
        Color inputBg    = Palette::LanternDarkRed.withOpacity(0.85f);
        Color inputFg    = Palette::FireworkGold;
        Color hintCol    = Palette::PoetrySub;

        // 背景半透明遮罩
        renderer.fillRect(mx - 2, my - 1, modalW + 4, modalH + 2, bgCol, " ", borderCol);

        // 典雅金石边框
        renderer.drawText(mx, my + 0, "╭────────────────────────────────────────────╮", borderCol);
        renderer.drawText(mx, my + 1, "│       🌙 月下题名 · 谨以此宵明月赠佳人      │", titleCol, bgCol, true);
        renderer.drawText(mx, my + 2, "├────────────────────────────────────────────┤", borderCol);
        renderer.drawText(mx, my + 3, "│  受福对象 / 姓名（如：淳阳项目组、父母等）  │", labelCol);

        // 输入行框
        std::string displayInput = "▶ " + m_inputText;
        bool cursorOn = (std::sin(m_cursorTimer * 6.0f) > 0.0f);
        if (cursorOn) {
            displayInput += "█";
        } else {
            displayInput += " ";
        }

        int inWidth = core::stringVisualWidth(displayInput);
        int pad = std::max(0, 40 - inWidth);
        std::string fullLine = displayInput + std::string(pad, ' ');

        renderer.drawText(mx + 2, my + 4, fullLine, inputFg, inputBg, true);

        // 提示操作
        renderer.drawText(mx, my + 5, "│                                            │", borderCol);
        renderer.drawText(mx, my + 6, "│        [Enter] 题名落款     [ESC] 取消      │", hintCol);
        renderer.drawText(mx, my + 7, "╰────────────────────────────────────────────╯", borderCol);
    }

    std::string m_targetName;
    std::string m_inputText;
    bool m_isInputting = false;
    float m_cursorTimer = 0.0f;

    bool m_animating = false;
    float m_animTimer = 0.0f;
    float m_highlightProgress = 0.0f;
    std::vector<Particle> m_inkParticles;
};

} // namespace festival::themes::mid_autumn
