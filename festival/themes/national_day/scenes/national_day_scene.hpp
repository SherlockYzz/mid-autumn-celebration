#pragma once

#include "../../../core/scene/scene.hpp"
#include "../../../core/renderer/renderer.hpp"
#include "../../../core/platform/input_event.hpp"
#include "../../../core/effects/starfield.hpp"
#include "../../../core/color/color.hpp"
#include "../../mid_autumn/elements/firework_display.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

namespace festival::themes::national_day {

using namespace festival::core;

struct BalloonParticle {
    float x;
    float y;
    float vy;
    float vx;
    Color color;
    std::string glyph;
    bool active;
};

class NationalDayScene : public Scene {
public:
    NationalDayScene() : m_starfield(75, Color(255, 230, 160)) {}

    void enter() override {
        m_time = 0.0f;
        m_heatScore = 666;
        m_nextMilestone = 1000;
        m_climaxTimer = 0.0f;
        m_drumBeatTimer = 0.0f;
        m_toastTimer = 0.0f;
        m_toastText = "";
        m_balloons.clear();

        m_starfield.initialize(120, 30);
    }

    void handleInput(const InputEvent& event) override {
        if (event.code == KeyCode::Enter || event.isChar('a')) {
            triggerNationalCelebration();
        } else if (event.code == KeyCode::Space) {
            launchSalute();
        } else if (event.isChar('b')) {
            releaseBalloons();
        } else if (event.isChar('d')) {
            beatDrums();
        } else if (event.isChar('t')) {
            showTrivia();
        } else if (event.isChar('n')) {
            sendNationWish();
        }
    }

    void update(float dt) override {
        m_time += dt;

        if (m_climaxTimer > 0.0f) {
            m_climaxTimer -= dt;
            if (static_cast<int>(m_time * 10) % 3 == 0) {
                launchSalute();
            }
        }

        if (m_drumBeatTimer > 0.0f) {
            m_drumBeatTimer -= dt;
        }

        if (m_toastTimer > 0.0f) {
            m_toastTimer -= dt;
        }

        m_starfield.update(dt);
        m_fireworks.update(dt);

        for (auto& b : m_balloons) {
            if (!b.active) continue;
            b.y += b.vy * dt;
            b.x += b.vx * dt + std::sin(b.y * 0.2f + m_time * 2.0f) * 0.3f * dt;
            if (b.y < -2.0f) {
                b.active = false;
            }
        }
        m_balloons.erase(
            std::remove_if(m_balloons.begin(), m_balloons.end(), [](const BalloonParticle& b) { return !b.active; }),
            m_balloons.end()
        );

        if (static_cast<int>(m_time * 100) % 250 == 0) {
            spawnSingleBalloon();
        }
    }

    void render(Renderer& renderer) override {
        int w = renderer.width();
        int h = renderer.height();

        // 1. Festive Crimson & Imperial Gold Background Gradient
        for (int y = 0; y < h; ++y) {
            float t = static_cast<float>(y) / static_cast<float>(h);
            Color rowBg = Color::blend(Color(12, 16, 36), Color(50, 10, 14), t);
            for (int x = 0; x < w; ++x) {
                renderer.drawChar(x, y, " ", Color(255, 255, 255), rowBg);
            }
        }

        // 2. Stars
        m_starfield.render(renderer);

        // 3. Great Wall & Tiananmen Silhouette
        int silY = h - 10;
        drawMountainAndWallSilhouette(renderer, w, silY);

        // 4. Central Monument / Emblem & Calligraphy
        renderer.drawCenterText(2, "🇨🇳 盛 世 华 诞 · 举 国 同 庆 🇨🇳", Color(255, 230, 130), Color(120, 20, 20), true);
        renderer.drawCenterText(4, "「愿以寸心寄华夏，且将岁月赠山河」", Color(255, 215, 100), Color(0, 0, 0, 0), false);

        // 5. Drum Beats Resonance
        if (m_drumBeatTimer > 0.0f) {
            renderer.drawCenterText(6, "🥁【金鼓齐鸣 · 礼炮震天】山河锦绣，万民同欢！", Color(255, 240, 150), Color(160, 25, 25), true);
        }

        // 6. Balloons
        for (const auto& b : m_balloons) {
            int bx = static_cast<int>(b.x);
            int by = static_cast<int>(b.y);
            if (bx >= 0 && bx < w && by >= 0 && by < h) {
                renderer.drawChar(bx, by, b.glyph, b.color, Color(0, 0, 0, 0), true);
            }
        }

        // 7. Fireworks
        m_fireworks.render(renderer);

        // 8. Climax Banner
        if (m_climaxTimer > 0.0f) {
            renderer.drawCenterText(h - 5, "★【盛世华章】五星闪耀皆为信仰，万家灯火共庆华诞！★", Color(255, 255, 200), Color(180, 20, 20), true);
        }

        // 9. Toast Notification
        if (m_toastTimer > 0.0f) {
            renderer.drawCenterText(h - 3, m_toastText, Color(255, 235, 120), Color(0, 0, 0, 0), true);
        }

        // 10. Bottom HUD
        std::string hud = "[TAB]切换节日  热度:" + std::to_string(m_heatScore) + 
                          "  [ENTER]华彩盛典  [SPACE]礼炮齐鸣  [B]腾空气球  [D]盛世金鼓  [T]华夏答题  [N]祝愿祖国";
        renderer.drawCenterText(h - 1, hud, Color(255, 220, 100), Color(20, 24, 45), true);
    }

private:
    void drawMountainAndWallSilhouette(Renderer& renderer, int w, int silY) {
        for (int x = 0; x < w; ++x) {
            int wallHeight = static_cast<int>(std::sin(x * 0.08f) * 2.5f + std::cos(x * 0.04f) * 1.5f);
            int top = silY + wallHeight;
            
            std::string_view ch = (x % 3 == 0) ? " " : "#";
            renderer.drawChar(x, top, ch, Color(210, 160, 50), Color(60, 15, 15));
            for (int y = top + 1; y < silY + 8; ++y) {
                renderer.drawChar(x, y, "#", Color(120, 30, 30), Color(40, 10, 10));
            }
        }

        int gy = silY - 2;
        renderer.drawCenterText(gy, "╔═══════[ 天 安 门 ]═══════╗", Color(255, 215, 0), Color(140, 20, 20), true);
        renderer.drawCenterText(gy + 1, "║  ★  ★  ★  ★  ★  ★  ★  ║", Color(255, 230, 100), Color(100, 15, 15), true);
        renderer.drawCenterText(gy + 2, "╚══════════════════════════╝", Color(255, 215, 0), Color(80, 10, 10), true);
    }

    void launchSalute() {
        float rx = static_cast<float>(rand() % 100 + 10);
        float ry = static_cast<float>(rand() % 12 + 4);
        m_fireworks.launchSingle(rx, 26.0f, ry);
        addHeat(15);
    }

    void releaseBalloons() {
        for (int i = 0; i < 20; ++i) {
            spawnSingleBalloon();
        }
        addHeat(25);
        showToast("🎈 五彩气球腾空！万鸽齐飞，祥和满人间！");
    }

    void spawnSingleBalloon() {
        Color colors[] = {Color(255, 70, 70), Color(255, 215, 0), Color(80, 170, 255), Color(255, 140, 0), Color(160, 90, 240)};
        BalloonParticle b;
        b.x = static_cast<float>(rand() % 110 + 5);
        b.y = 28.0f;
        b.vy = - (static_cast<float>(rand() % 20 + 15) / 10.0f);
        b.vx = static_cast<float>((rand() % 20) - 10) / 20.0f;
        b.color = colors[rand() % 5];
        b.glyph = (rand() % 2 == 0) ? "o" : "@";
        b.active = true;
        m_balloons.push_back(b);
    }

    void beatDrums() {
        m_drumBeatTimer = 2.5f;
        addHeat(30);
        showToast("🥁 击鼓鸣钟！盛世华夏，气壮山河！");
    }

    void showTrivia() {
        const char* trivias[] = {
            "【华夏问答】国庆华诞迎盛典，神州万里庆安康！(+40热度)",
            "【华夏问答】大好河山美如画，五星红旗迎风扬！(+40热度)",
            "【华夏问答】同心筑梦向复兴，盛世繁华锦绣春！(+40热度)"
        };
        showToast(trivias[rand() % 3]);
        addHeat(40);
    }

    void sendNationWish() {
        showToast("🚩 愿以吾辈之青春，护卫这盛世之中华！祝祖国繁荣昌盛！(+35热度)");
        addHeat(35);
    }

    void triggerNationalCelebration() {
        m_climaxTimer = 8.0f;
        m_fireworks.launchGrandCelebration(120, 30);
        releaseBalloons();
        addHeat(334);
        showToast("🎉【举国同庆】万象华彩！盛世华诞，普天同乐！");
    }

    void addHeat(int amount) {
        m_heatScore += amount;
        if (m_heatScore >= m_nextMilestone) {
            triggerNationalCelebration();
            m_nextMilestone += 2000;
        }
    }

    void showToast(const std::string& msg) {
        m_toastText = msg;
        m_toastTimer = 4.0f;
    }

private:
    float m_time = 0.0f;
    int m_heatScore = 666;
    int m_nextMilestone = 1000;
    float m_climaxTimer = 0.0f;
    float m_drumBeatTimer = 0.0f;
    float m_toastTimer = 0.0f;
    std::string m_toastText;

    StarField m_starfield;
    mid_autumn::FestivalFireworks m_fireworks;
    std::vector<BalloonParticle> m_balloons;
};

} // namespace festival::themes::national_day
