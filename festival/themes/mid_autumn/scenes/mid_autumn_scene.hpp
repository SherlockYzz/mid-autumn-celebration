#pragma once

#include "../mid_autumn_palette.hpp"
#include "../elements/moon.hpp"
#include "../elements/jade_rabbit.hpp"
#include "../elements/osmanthus.hpp"
#include "../elements/lantern.hpp"
#include "../elements/cloud.hpp"
#include "../elements/poetry.hpp"
#include "../elements/market_silhouette.hpp"
#include "../elements/prayer.hpp"
#include "../elements/firework_display.hpp"
#include "../elements/lantern_riddle.hpp"
#include "../elements/moon_divination.hpp"
#include "../elements/mooncake_feast.hpp"
#include "../elements/festival_guide_modal.hpp"
#include "../elements/central_blessing.hpp"
#include "../elements/blessing_system.hpp"
#include "../../../core/scene/scene.hpp"
#include "../../../core/effects/starfield.hpp"
#include "../../../core/renderer/renderer.hpp"
#include "../../../core/platform/input_event.hpp"

#if __has_include("../../../addons/henan_university_114.hpp")
#include "../../../addons/henan_university_114.hpp"
#define FESTIVAL_HAS_HENU_114 1
#endif

#include <memory>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>

namespace festival::themes::mid_autumn {

enum class FestivalStage {
    Intro,
    Interactive
};

class MidAutumnScene : public Scene {
public:
    MidAutumnScene() = default;

    void enter() override {
        m_stage = FestivalStage::Intro;
        m_time = 0.0f;
        m_initialized = false;
        m_easterEggTimer = 0.0f;
    }

    void handleInput(const InputEvent& event) override {
        if (event.isQuit()) {
            if (m_blessingSystem.isInputting()) {
                m_blessingSystem.cancelInput();
                return;
            }
            if (m_prayer.isInputting()) {
                m_prayer.cancelInput();
                return;
            }
            if (m_riddleGame.isActive()) {
                m_riddleGame.close();
                return;
            }
            if (m_divination.isActive()) {
                m_divination.close();
                return;
            }
            if (m_guideModal.isActive()) {
                m_guideModal.close();
                return;
            }
            Platform::instance().requestStop();
            return;
        }

        // 1. If in Blessing Inscription modal ('N')
        if (m_blessingSystem.isInputting()) {
            if (event.code == KeyCode::Escape) {
                m_blessingSystem.cancelInput();
            } else if (event.code == KeyCode::Enter) {
                float inputX = m_cachedWidth * 0.5f;
                float inputY = m_cachedHeight * 0.5f;
                m_blessingSystem.submitInput(inputX, inputY);
                m_centralBlessing.setTargetName(m_blessingSystem.targetName());
                m_centralBlessing.triggerHighlight(3.5f);
                // 题名联动：玉兔抬头望月送祝福、灯笼亮起、满月辉映、金色烟花绽放！
                m_rabbit.triggerHeadTilt(6.0f);
                m_lanterns.ignite();
                m_moon.setMoonlightBoost(1.3f);
                m_moon.triggerMoonlightWave();
                m_fireworks.launchSingle(m_cachedWidth * 0.48f, static_cast<float>(m_cachedHeight - 3), m_cachedHeight * 0.20f);
                addMerriment(60);
            } else if (event.code == KeyCode::Backspace) {
                m_blessingSystem.handleBackspace();
            } else if (!event.text.empty()) {
                m_blessingSystem.handleInputText(event.text);
            } else if (event.code == KeyCode::Char && event.ch != 0) {
                m_blessingSystem.handleInputText(std::string(1, event.ch));
            }
            return;
        }

        // 2. If in Lantern Riddle game ('T')
        if (m_riddleGame.isActive()) {
            if (m_riddleGame.handleInput(event)) {
                if (m_riddleGame.lastReward() == RiddleReward::CorrectAnswer) {
                    m_riddleGame.clearReward();
                    addMerriment(50);
                    m_fireworks.launchSingle(m_cachedWidth * 0.5f, static_cast<float>(m_cachedHeight - 3), m_cachedHeight * 0.22f);
                    m_rabbit.triggerInteraction();
                }
            }
            return;
        }

        // 3. If in Moon Divination / 卜问月 modal ('J')
        if (m_divination.isActive()) {
            DivinationEffect eff = DivinationEffect::None;
            if (m_divination.handleInput(event, eff)) {
                if (eff != DivinationEffect::None) {
                    applyDivinationEffect(eff);
                    addMerriment(30);
                }
            }
            return;
        }

        // 4. If in Festival Guide modal ('H')
        if (m_guideModal.isActive()) {
            if (event.code == KeyCode::Escape || event.isChar('h')) {
                m_guideModal.close();
                return;
            }
            return;
        }

        // 5. If in Prayer text input mode ('W')
        if (m_prayer.isInputting()) {
            if (event.code == KeyCode::Escape) {
                m_prayer.cancelInput();
            } else if (event.code == KeyCode::Enter) {
                m_prayer.submitInput(m_cachedWidth * 0.25f, static_cast<float>(m_cachedHeight - 4));
                addMerriment(40);
            } else if (event.code == KeyCode::Backspace) {
                m_prayer.handleBackspace();
            } else if (!event.text.empty()) {
                m_prayer.handleInputText(event.text);
            } else if (event.code == KeyCode::Char && event.ch != 0) {
                m_prayer.handleInputText(std::string(1, event.ch));
            }
            return;
        }

        // Fast skip to interactive mode on any key
        if (m_stage == FestivalStage::Intro) {
            skipIntro();
        }

        // Track Easter egg typing buffer ("MOON")
        if (event.code == KeyCode::Char) {
            char upper = static_cast<char>(std::toupper(event.ch));
            m_keyBuffer.push_back(upper);
            if (m_keyBuffer.size() > 4) {
                m_keyBuffer.erase(m_keyBuffer.begin());
            }
            if (m_keyBuffer == "MOON") {
                triggerFullMoonEasterEgg();
                m_keyBuffer.clear();
                return;
            }
        }

        // 6. Direct Interactive Key Bindings
        if (event.code == KeyCode::Enter || event.isChar('a') || event.isChar('x')) {
            // [ENTER] / [A] / [X]: 盛典齐鸣 · 全部中秋庆祝效果同框爆发！
            triggerGrandCelebrationAll();
        } else if (event.code == KeyCode::Space) {
            // [SPACE]: Instant golden celebration firework
            float fwX = m_cachedWidth * (0.35f + (rand() % 30) * 0.01f);
            m_fireworks.launchSingle(fwX, static_cast<float>(m_cachedHeight - 3), m_cachedHeight * 0.22f);
            addMerriment(15);
        } else if (event.isChar('t')) {
            // [T]: 宵月游园 · 猜灯谜 (互动选择题、积分、连胜与焰火奖励)
            m_riddleGame.open();
        } else if (event.isChar('j')) {
            // [J]: 追月灵签 · 卜问月 (抽取中秋上上签，触发专属祥瑞Buff)
            DivinationEffect eff = m_divination.open();
            applyDivinationEffect(eff);
            addMerriment(30);
        } else if (event.isChar('b')) {
            // [B]: 中秋食韵 · 玉兔品名饼 (奉献五方名饼，玉兔咀嚼萌态与灵露反馈)
            const auto& cake = m_mooncakeFeast.serveNext();
            m_rabbit.feedMooncake(cake.name, cake.rabbitReaction);
            addMerriment(30);
        } else if (event.isChar('k')) {
            // [K]: 赏灯筑阁 · 万家灯火 (四级长街夜市繁华度与水波灯火倒影)
            std::string tierDesc = m_market.nextLightingTier();
            m_marketToastText = " 🏮【赏灯筑阁】市井夜色升至：" + tierDesc + " 🏮 ";
            m_marketToastTimer = 7.0f;
            addMerriment(30);
        } else if (event.isChar('h')) {
            // [H]: 展开 / 收起 宵月会游园锦囊
            m_guideModal.toggle();
        } else if (event.isChar('n')) {
            // [N]: 月下题名（输入受福之人，如：淳阳项目组、妈妈）
            m_blessingSystem.startInput();
        } else if (event.isChar('c')) {
            // [C]: 复制当前最终祝福「祝【XXX】中秋快乐」至剪贴板
            Platform::instance().copyToClipboard(m_centralBlessing.fullBlessing());
            m_copyToastTimer = 4.0f;
        } else if (event.isChar('r')) {
            // [R]: 灵兔捣仙芝互动、耳朵欢动、金光四溢
            m_rabbit.triggerInteraction();
            m_moon.setMoonlightBoost(1.3f);
            addMerriment(20);
        } else if (event.isChar('l')) {
            // [L]: 点亮 / 熄灭朱红画舫灯笼
            m_lanterns.toggle();
            addMerriment(10);
        } else if (event.isChar('g')) {
            // [G]: 摇落金桂花雨
            m_osmanthus.shakeShower();
            addMerriment(20);
        } else if (event.isChar('p')) {
            // [P]: 轮换唐宋中秋经典诗句与朱砂印章
            m_poetry.nextVerse();
            addMerriment(20);
        } else if (event.isChar('w')) {
            // [W]: 亲题心愿，放飞祈愿天灯
            m_prayer.startInput();
        } else if (event.isChar('f')) {
            // [F]: 盛典连珠烟火
            m_fireworks.launchGrandCelebration(m_cachedWidth, m_cachedHeight);
            addMerriment(40);
        } else if (event.isChar('m')) {
            // [M]: 月华潮涌，全场清辉泛波
            m_moon.triggerMoonlightWave();
            addMerriment(15);
        }
    }

    void update(float dt) override {
        m_time += dt;

        // Fast pacing: Intro completes within 3.5 seconds
        if (m_stage == FestivalStage::Intro) {
            if (m_time > 3.5f) {
                m_stage = FestivalStage::Interactive;
                m_poetry.startDisplay();
            }
        }

        if (m_easterEggTimer > 0.0f) {
            m_easterEggTimer -= dt;
        }

        if (m_marketToastTimer > 0.0f) {
            m_marketToastTimer -= dt;
        }

        if (m_climaxBannerTimer > 0.0f) {
            m_climaxBannerTimer -= dt;
        }

        // Update all components
        m_starField.update(dt);
        m_clouds.update(dt, m_cachedWidth);
        m_centralBlessing.update(dt);
        m_blessingSystem.update(dt);
        m_moon.update(dt);
        m_osmanthus.update(dt, m_cachedWidth, m_cachedHeight);
        m_rabbit.update(dt);
        m_lanterns.update(dt);
        m_poetry.update(dt);
        m_prayer.update(dt);
        m_fireworks.update(dt);
        m_divination.update(dt);

        if (m_copyToastTimer > 0.0f) {
            m_copyToastTimer -= dt;
        }
    }

    void render(Renderer& renderer) override {
        if (!m_initialized || m_cachedWidth != renderer.width() || m_cachedHeight != renderer.height()) {
            m_cachedWidth = renderer.width();
            m_cachedHeight = renderer.height();
            initializeElements(m_cachedWidth, m_cachedHeight);
            m_initialized = true;
        }

        // 1. Clear background to Deep Indigo Night Sky
        renderer.clear(Palette::NightSky);

        // 2. Star field (Sparse, peaceful starlight)
        m_starField.render(renderer);

        // 3. Pixel Full Moon (Visual Center)
        m_moon.render(renderer);

        // 4. Wispy drifting clouds
        m_clouds.render(renderer);

        // 5. Oriental Market & Pavilion Silhouette + Water Reflections
        m_market.render(renderer, m_moon.position().x, m_time);

        // 6. Jade Rabbit on stone terrace (Prominent Oriental Pixel Art)
        m_rabbit.render(renderer);

        // 7. Osmanthus branches & falling blossoms
        m_osmanthus.render(renderer);

        // 8. Cinnabar Red Lanterns
        m_lanterns.render(renderer);

        // 9. Classical Poetry & Red Seal
        m_poetry.render(renderer);

        // 10. Moonlight Ink Particles (floating towards moon)
        m_blessingSystem.renderParticles(renderer);

        // 11. Grand Central Blessing: “祝【XXX】中秋快乐” (Oriental Palace Plaque Centerpiece)
        m_centralBlessing.render(renderer, m_cachedWidth, m_cachedHeight);

        // 12. Flying Sky Lanterns
        m_prayer.render(renderer);

        // 13. Fireworks
        m_fireworks.render(renderer);

        // 14. Optional Anniversary Easter Egg (Removable Addon)
#if defined(FESTIVAL_HAS_HENU_114)
        festival::addons::HenanUniversityAnniversary::render(renderer);
#endif

        // 15. Market Lighting Level Toast Notification
        if (m_marketToastTimer > 0.0f) {
            float fade = std::clamp(m_marketToastTimer / 1.0f, 0.0f, 1.0f);
            renderer.drawCenterText(m_cachedHeight - 3, m_marketToastText, Palette::LanternLight.withOpacity(fade), Palette::NightDeep, true);
        }

        // 16. Climax Celebration Banner
        if (m_climaxBannerTimer > 0.0f) {
            float fade = std::clamp(m_climaxBannerTimer / 1.5f, 0.0f, 1.0f);
            std::string banner = m_climaxBannerText.empty() ? " 🎊 ✨ 宵月游园热度达成！【月华如愿 · 人间团圆】万邦同庆 ✨ 🎊 " : m_climaxBannerText;
            renderer.drawCenterText(2, banner, Palette::FireworkGold.withOpacity(fade), Palette::LanternDarkRed, true);
        }

        // 17. Full Moon Easter Egg Banner
        if (m_easterEggTimer > 0.0f) {
            float fade = std::clamp(m_easterEggTimer / 1.5f, 0.0f, 1.0f);
            std::string eggBanner = " 🌕 " + m_centralBlessing.fullBlessing() + " · 月圆人圆事事圆 🌕 ";
            renderer.drawCenterText(2, eggBanner, Palette::FireworkGold.withOpacity(fade), Palette::LanternDarkRed, true);
        }

        // 18. Clipboard Copy Toast Notification
        if (m_copyToastTimer > 0.0f) {
            float fade = std::clamp(m_copyToastTimer / 1.0f, 0.0f, 1.0f);
            std::string toast = " 📋 已复制「" + m_centralBlessing.fullBlessing() + "」至剪贴板，可直接发送好友！ ";
            renderer.drawCenterText(m_cachedHeight - 2, toast, Palette::MoonWhite.withOpacity(fade), Palette::SealRed.withOpacity(fade), true);
        }

        // 19. Controls HUD (Always prominent, shows key activities)
        renderControlsHUD(renderer);

        // 20. Modals (Drawn on top of everything when active)
        if (m_blessingSystem.isInputting()) {
            m_blessingSystem.renderModal(renderer, m_cachedWidth, m_cachedHeight);
        }

        if (m_riddleGame.isActive()) {
            m_riddleGame.render(renderer);
        }

        if (m_divination.isActive()) {
            m_divination.render(renderer);
        }

        if (m_guideModal.isActive()) {
            m_guideModal.render(renderer);
        }
    }

private:
    void applyDivinationEffect(DivinationEffect eff) {
        switch (eff) {
            case DivinationEffect::MoonlightWave:
                m_moon.setMoonlightBoost(1.5f);
                m_moon.triggerMoonlightWave();
                break;
            case DivinationEffect::OsmanthusFlurry:
                m_osmanthus.shakeShower();
                m_rabbit.triggerHeadTilt(6.0f);
                break;
            case DivinationEffect::FireworksCelebration:
                m_fireworks.launchGrandCelebration(m_cachedWidth, m_cachedHeight);
                m_market.setLightingTier(3);
                break;
            case DivinationEffect::JadeRabbitJoy:
                m_rabbit.triggerInteraction();
                break;
            case DivinationEffect::SkyLanternAscent:
                m_prayer.launchLantern(m_cachedWidth * 0.45f, static_cast<float>(m_cachedHeight - 4), "但愿人长久 千里共婵娟");
                break;
            default:
                break;
        }
    }

    void addMerriment(int pts) {
        m_festivalMerriment += pts; // 不设上限，持续积累

        // 1000、3000、5000……每增加 2000 热度值自动触发一次全景大盛典！
        if (!m_isTriggeringCelebration && m_festivalMerriment >= m_nextMilestone) {
            int currentMilestone = m_nextMilestone;
            while (m_nextMilestone <= m_festivalMerriment) {
                m_nextMilestone += 2000;
            }
            triggerGrandCelebrationAll(currentMilestone);
        }
    }

    void triggerGrandClimax() {
        triggerGrandCelebrationAll(1000);
    }

    // 一键全部盛典同时发生：放烟花、赏明月、摇金桂、灵兔捣仙芝、万家灯火通明、天灯祈愿、金匾长明！
    void triggerGrandCelebrationAll(int milestone = 0) {
        m_isTriggeringCelebration = true;
        m_climaxBannerTimer = 16.0f;
        std::string milestoneTag = (milestone >= 1000) ? 
            ("【热度达 " + std::to_string(milestone) + " · 华彩同庆】") : 
            ((m_festivalMerriment >= 1000) ? 
                ("【热度达 " + std::to_string((m_festivalMerriment / 1000) * 1000) + " · 华彩齐鸣】") : 
                "【盛世良宵 · 华彩齐鸣】");
        m_climaxBannerText = " 🎊 ❖ 🌕 " + milestoneTag + " 祝大家中秋快乐 🌕 ❖ 🎊 ";

        // 1. 赏明月：满月清辉暴涨，月华脉冲潮涌
        m_moon.setMoonlightBoost(1.8f);
        m_moon.triggerMoonlightWave();

        // 2. 灵兔捣仙芝：玉兔起跃捣药、双耳欢动、臼中喷涌璀璨星芒
        m_rabbit.triggerInteraction();
        m_rabbit.triggerHeadTilt(12.0f);

        // 3. 摇落金桂：月桂震颤，上百枚金瓣漫天飞扬
        m_osmanthus.shakeShower();

        // 4. 放烟花：连珠高空盛典烟花绽放
        m_fireworks.launchGrandCelebration(m_cachedWidth, m_cachedHeight);
        m_fireworks.launchSingle(m_cachedWidth * 0.32f, static_cast<float>(m_cachedHeight - 3), m_cachedHeight * 0.18f);
        m_fireworks.launchSingle(m_cachedWidth * 0.68f, static_cast<float>(m_cachedHeight - 3), m_cachedHeight * 0.22f);

        // 5. 赏灯筑阁与万家灯火：长街市井升至最高级别「万家灯火 · 盛唐仙阙」
        m_market.setLightingTier(3);
        m_marketToastText = " 🏮【万象齐鸣】放烟花、赏明月、摇金桂、灵兔捣药、万家灯火齐明！ 🏮 ";
        m_marketToastTimer = 10.0f;

        // 6. 画舫燃灯：朱红流苏灯笼全部点亮
        m_lanterns.ignite();

        // 7. 祈愿天灯：两盏孔明灯携愿乘风而起
        m_prayer.launchLantern(m_cachedWidth * 0.40f, static_cast<float>(m_cachedHeight - 4), "但愿人长久 千里共婵娟");
        m_prayer.launchLantern(m_cachedWidth * 0.60f, static_cast<float>(m_cachedHeight - 5), "岁岁常相见 人月两团圆");

        // 8. 诗词渐显
        m_poetry.startDisplay();

        // 9. 中央祝福金匾：高亮闪耀金光波纹
        m_centralBlessing.triggerHighlight(10.0f);

        // 10. 如果是玩家主动按键触发，加很多热度（+334）
        if (milestone == 0) {
            m_festivalMerriment += 334;
            while (m_nextMilestone <= m_festivalMerriment) {
                m_nextMilestone += 2000;
            }
        }
        m_isTriggeringCelebration = false;
    }

    void skipIntro() {
        m_stage = FestivalStage::Interactive;
        m_moon.skipToTop();
        m_osmanthus.startDrifting();
        m_rabbit.fadeIn();
        m_lanterns.ignite();
        m_poetry.startDisplay();
    }

    void triggerFullMoonEasterEgg() {
        m_easterEggTimer = 6.0f;
        m_moon.setMoonlightBoost(1.6f);
        m_moon.triggerMoonlightWave();
        m_rabbit.triggerHeadTilt(6.0f);
        m_rabbit.triggerInteraction();
        m_lanterns.ignite();
        m_osmanthus.shakeShower();
        m_prayer.launchLantern(m_cachedWidth * 0.45f, static_cast<float>(m_cachedHeight - 4), "但愿人长久 千里共婵娟");
        m_fireworks.launchGrandCelebration(m_cachedWidth, m_cachedHeight);
        addMerriment(100);
    }

    void initializeElements(int width, int height) {
        m_starField.initialize(width, height);
        m_clouds.initialize(width, height);
        m_moon.initialize(width, height);
        m_osmanthus.initialize(width, height);

        // Place Jade Rabbit firmly on the moonlit stone terrace under the full moon
        float rabbitPx = static_cast<float>(width) * 0.44f;
        float rabbitPy = static_cast<float>(height * 2) - 22.0f;
        m_rabbit.setPosition(rabbitPx, rabbitPy);
        m_rabbit.fadeIn();

        m_lanterns.initialize(width, height);
        m_lanterns.ignite();
        m_poetry.initialize(width, height);
        m_osmanthus.startDrifting();
        m_moon.startMoonrise();
    }

    void renderControlsHUD(Renderer& renderer) {
        if (m_prayer.isInputting() || m_riddleGame.isActive() || m_blessingSystem.isInputting() || m_divination.isActive() || m_guideModal.isActive()) return;

        int hudY = m_cachedHeight - 1;

        // 底部常驻核心视觉效果与求签、猜灯谜、筑阁、吟诗，其余收纳于锦囊[H]
        std::string hud = "🏮 热度:" + std::to_string(m_festivalMerriment) + "  [ENTER]盛典  [SPACE]烟花  [R]玉兔  [G]摇桂  [J]求签  [T]灯谜  [K]筑阁  [P]吟诗  [N]题名  [H]锦囊";
        if (renderer.width() < 100) {
            hud = "🏮" + std::to_string(m_festivalMerriment) + " [ENTER]盛典 [SPACE]烟花 [R]玉兔 [G]摇桂 [J]求签 [T]灯谜 [K]筑阁 [P]吟诗 [N]题名 [H]锦囊";
        }
        renderer.drawCenterText(hudY, hud, Palette::PoetrySub, Palette::NightDeep);
    }

    FestivalStage m_stage = FestivalStage::Intro;
    float m_time = 0.0f;
    int m_cachedWidth = 80;
    int m_cachedHeight = 24;
    bool m_initialized = false;

    // Elements
    StarField m_starField{30, Palette::Stars};
    CloudLayer m_clouds;
    PixelMoon m_moon;
    CentralBlessing m_centralBlessing;
    BlessingSystem m_blessingSystem;
    PixelJadeRabbit m_rabbit;
    MarketSilhouette m_market;
    Osmanthus m_osmanthus;
    LanternGroup m_lanterns;
    PoetryDisplay m_poetry;
    PrayerSystem m_prayer;
    LanternRiddleGame m_riddleGame;
    MoonDivination m_divination;
    MooncakeFeast m_mooncakeFeast;
    FestivalGuideModal m_guideModal;
    FestivalFireworks m_fireworks;

    int m_festivalMerriment = 666; // 基础热度值从 666 开始，不设上限
    int m_nextMilestone = 1000;    // 1000、3000、5000……每增加2000触发一次全景大盛典
    bool m_isTriggeringCelebration = false;
    float m_marketToastTimer = 0.0f;
    std::string m_marketToastText;
    float m_climaxBannerTimer = 0.0f;
    std::string m_climaxBannerText;

    std::string m_keyBuffer;
    float m_easterEggTimer = 0.0f;
    float m_copyToastTimer = 0.0f;
};

} // namespace festival::themes::mid_autumn
