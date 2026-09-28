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

namespace festival::themes::spring_festival {

using namespace festival::core;

struct IngotParticle {
    float x;
    float y;
    float vy;
    std::string glyph;
    Color color;
    bool active;
};

class SpringFestivalScene : public Scene {
public:
    SpringFestivalScene() : m_starfield(70, Color(255, 230, 160)) {}

    void enter() override {
        m_time = 0.0f;
        m_heatScore = 666;
        m_nextMilestone = 1000;
        m_climaxTimer = 0.0f;
        m_lionDanceTimer = 0.0f;
        m_toastTimer = 0.0f;
        m_toastText = "";
        m_coupletIndex = 0;
        m_ingots.clear();

        m_starfield.initialize(120, 30);
    }

    void handleInput(const InputEvent& event) override {
        if (event.code == KeyCode::Enter || event.isChar('a')) {
            triggerSpringCelebration();
        } else if (event.code == KeyCode::Space) {
            launchFirecrackers();
        } else if (event.isChar('l')) {
            triggerLionDance();
        } else if (event.isChar('c')) {
            cycleCouplets();
        } else if (event.isChar('f')) {
            showerIngots();
        } else if (event.isChar('t')) {
            showSpringTrivia();
        } else if (event.isChar('n')) {
            sendSpringWish();
        }
    }

    void update(float dt) override {
        m_time += dt;

        if (m_climaxTimer > 0.0f) {
            m_climaxTimer -= dt;
            if (static_cast<int>(m_time * 10) % 3 == 0) {
                launchFirecrackers();
            }
        }

        if (m_lionDanceTimer > 0.0f) {
            m_lionDanceTimer -= dt;
        }

        if (m_toastTimer > 0.0f) {
            m_toastTimer -= dt;
        }

        m_starfield.update(dt);
        m_fireworks.update(dt);

        for (auto& ing : m_ingots) {
            if (!ing.active) continue;
            ing.y += ing.vy * dt;
            if (ing.y > 28.0f) {
                ing.active = false;
            }
        }
        m_ingots.erase(
            std::remove_if(m_ingots.begin(), m_ingots.end(), [](const IngotParticle& i) { return !i.active; }),
            m_ingots.end()
        );
    }

    void render(Renderer& renderer) override {
        int w = renderer.width();
        int h = renderer.height();

        // 1. Warm Festive Carmine Background Gradient
        for (int y = 0; y < h; ++y) {
            float t = static_cast<float>(y) / static_cast<float>(h);
            Color rowBg = Color::blend(Color(18, 8, 20), Color(60, 10, 15), t);
            for (int x = 0; x < w; ++x) {
                renderer.drawChar(x, y, " ", Color(255, 255, 255), rowBg);
            }
        }

        // 2. Stars
        m_starfield.render(renderer);

        // 3. Classical Red Chunlian Couplets
        drawSpringCouplets(renderer, w, h);

        // 4. Center Banner
        renderer.drawCenterText(2, "🏮 岁 序 更 新 · 九 州 同 春 🏮", Color(255, 230, 120), Color(130, 20, 20), true);
        renderer.drawCenterText(4, "「千门万户曈曈日，总把新桃换旧符」", Color(255, 215, 100), Color(0, 0, 0, 0), false);

        // 5. Pixel Dancing Lion / Dragon
        drawDancingLion(renderer, w, h);

        // 6. Falling Golden Ingots
        for (const auto& ing : m_ingots) {
            int ix = static_cast<int>(ing.x);
            int iy = static_cast<int>(ing.y);
            if (ix >= 0 && ix < w && iy >= 0 && iy < h) {
                renderer.drawChar(ix, iy, ing.glyph, ing.color, Color(0, 0, 0, 0), true);
            }
        }

        // 7. Fireworks & Crackers
        m_fireworks.render(renderer);

        // 8. Climax Banner
        if (m_climaxTimer > 0.0f) {
            renderer.drawCenterText(h - 5, "★【金龙献瑞】辞旧迎新除夕夜，万象呈祥贺新春！★", Color(255, 255, 220), Color(180, 20, 20), true);
        }

        // 9. Toast Notification
        if (m_toastTimer > 0.0f) {
            renderer.drawCenterText(h - 3, m_toastText, Color(255, 235, 120), Color(0, 0, 0, 0), true);
        }

        // 10. Bottom HUD
        std::string hud = "[TAB]切换节日  热度:" + std::to_string(m_heatScore) + 
                          "  [ENTER]同春盛典  [SPACE]爆竹辟邪  [L]祥龙瑞狮  [C]更换春联  [F]财神赐福  [T]年俗问答  [N]新春拜年";
        renderer.drawCenterText(h - 1, hud, Color(255, 220, 100), Color(20, 24, 45), true);
    }

private:
    void drawSpringCouplets(Renderer& renderer, int w, int h) {
        struct Couplet {
            const char* left;
            const char* right;
            const char* top;
        };
        static const Couplet couplets[] = {
            {"天增岁月人增寿", "春满乾坤福满门", "迎春接福"},
            {"一帆风顺年年好", "万事如意步步高", "吉星高照"},
            {"喜居宝地千年旺", "福照家门万事兴", "万象更新"}
        };

        const auto& cur = couplets[m_coupletIndex % 3];

        renderer.drawCenterText(6, cur.top, Color(255, 240, 150), Color(140, 20, 20), true);

        int leftX = 12;
        int topY = 7;
        drawVerticalString(renderer, leftX, topY, cur.left);

        int rightX = w - 16;
        drawVerticalString(renderer, rightX, topY, cur.right);
    }

    void drawVerticalString(Renderer& renderer, int x, int startY, const std::string& str) {
        for (int y = startY - 1; y < startY + 16; ++y) {
            renderer.drawChar(x - 1, y, " ", Color(255, 255, 255), Color(120, 15, 15));
            renderer.drawChar(x, y, " ", Color(255, 255, 255), Color(120, 15, 15));
            renderer.drawChar(x + 1, y, " ", Color(255, 255, 255), Color(120, 15, 15));
            renderer.drawChar(x + 2, y, " ", Color(255, 255, 255), Color(120, 15, 15));
        }

        std::vector<std::string> chars;
        for (size_t i = 0; i < str.size(); ) {
            unsigned char byte = static_cast<unsigned char>(str[i]);
            size_t len = 1;
            if (byte >= 0xF0) len = 4;
            else if (byte >= 0xE0) len = 3;
            else if (byte >= 0xC0) len = 2;
            chars.push_back(str.substr(i, len));
            i += len;
        }

        for (size_t i = 0; i < chars.size(); ++i) {
            renderer.drawText(x, startY + static_cast<int>(i) * 2, chars[i], Color(255, 230, 120), Color(120, 15, 15), true);
        }
    }

    void drawDancingLion(Renderer& renderer, int w, int h) {
        int cy = h - 11;
        float bob = (m_lionDanceTimer > 0.0f) ? std::sin(m_time * 8.0f) * 2.0f : std::sin(m_time * 3.0f);

        int ly = cy + static_cast<int>(bob);

        renderer.drawCenterText(ly - 2, "   /\\___/\\   ", Color(255, 215, 0), Color(0, 0, 0, 0), true);
        renderer.drawCenterText(ly - 1, "  ( ◕   ◕ )  ", Color(255, 60, 60), Color(140, 20, 20), true);
        renderer.drawCenterText(ly,     " ╰─── ▽ ───╯ ", Color(255, 215, 0), Color(160, 30, 30), true);
        renderer.drawCenterText(ly + 1, "   [ 祥龙瑞狮 ]  ", Color(255, 240, 150), Color(120, 15, 15), true);
    }

    void launchFirecrackers() {
        for (int i = 0; i < 3; ++i) {
            float rx = static_cast<float>(rand() % 100 + 10);
            float ry = static_cast<float>(rand() % 10 + 5);
            m_fireworks.launchSingle(rx, 26.0f, ry);
        }
        addHeat(15);
    }

    void triggerLionDance() {
        m_lionDanceTimer = 3.5f;
        addHeat(30);
        showToast("🦁 祥龙腾跃，瑞狮欢腾！步步高升，迎春大吉！");
    }

    void cycleCouplets() {
        m_coupletIndex++;
        addHeat(25);
        showToast("📜 换新桃符！春回大地千峰秀，日暖神州万木荣！");
    }

    void showerIngots() {
        for (int i = 0; i < 25; ++i) {
            IngotParticle ing;
            ing.x = static_cast<float>(rand() % 110 + 5);
            ing.y = 0.0f;
            ing.vy = static_cast<float>(rand() % 15 + 10) / 10.0f;
            ing.glyph = (rand() % 2 == 0) ? "$" : "*";
            ing.color = Color(255, 215, 0);
            ing.active = true;
            m_ingots.push_back(ing);
        }
        addHeat(35);
        showToast("💰 财神进宝，金玉满堂！恭喜发财，红包拿来！");
    }

    void showSpringTrivia() {
        const char* trivias[] = {
            "【新春年俗】除夕守岁灯火明，初一拜年道吉祥！(+40热度)",
            "【新春年俗】包饺子迎福气，吃年糕步步高！(+40热度)",
            "【新春年俗】贴福字纳百祥，放爆竹迎新春！(+40热度)"
        };
        showToast(trivias[rand() % 3]);
        addHeat(40);
    }

    void sendSpringWish() {
        showToast("🧧 岁岁平安，吉祥如意！祝大家新春愉快，万事大吉！(+35热度)");
        addHeat(35);
    }

    void triggerSpringCelebration() {
        m_climaxTimer = 8.0f;
        m_fireworks.launchGrandCelebration(120, 30);
        triggerLionDance();
        showerIngots();
        addHeat(334);
        showToast("🎉【九州同春】万象更新！辞旧迎新，岁序万福！");
    }

    void addHeat(int amount) {
        m_heatScore += amount;
        if (m_heatScore >= m_nextMilestone) {
            triggerSpringCelebration();
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
    float m_lionDanceTimer = 0.0f;
    float m_toastTimer = 0.0f;
    int m_coupletIndex = 0;
    std::string m_toastText;

    StarField m_starfield;
    mid_autumn::FestivalFireworks m_fireworks;
    std::vector<IngotParticle> m_ingots;
};

} // namespace festival::themes::spring_festival
