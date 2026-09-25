#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include <cmath>
#include <string>
#include <vector>

namespace festival::themes::mid_autumn {

/**
 * 赏灯筑阁 · 市井楼阁与水岸万家灯火
 *
 * 吸收《鸣潮》“赏灯筑阁”与《原神》“万家灯火与清辉相映”之精髓：
 * 支持 4 级节庆长街夜景繁华度切换 (按 K 键筑阁挑灯)：
 * 0. 幽夜清辉：素雅留白，月影空蒙
 * 1. 华灯初上：亭台微灯，画栋初显
 * 2. 宵月盛会：连珠灯串，画舫泛波
 * 3. 万家灯火：盛唐仙阙，万家灯火映彻澄江
 */
class MarketSilhouette {
public:
    MarketSilhouette() = default;

    int lightingTier() const { return m_tier; }

    std::string nextLightingTier() {
        m_tier = (m_tier + 1) % 4;
        switch (m_tier) {
            case 0: return "【幽夜清辉】素雅留白，水天一色";
            case 1: return "【华灯初上】飞檐挑灯，宵市微明";
            case 2: return "【宵月盛会】长街缀锦，画舫游波";
            case 3: return "【万家灯火】盛世长街，灯火辉煌照彻澄江";
        }
        return "【华灯初上】";
    }

    void setLightingTier(int tier) {
        m_tier = std::clamp(tier, 0, 3);
    }

    void render(Renderer& renderer, float moonPixelX, float time) {
        int pw = renderer.pixelWidth();
        int ph = renderer.pixelHeight();

        int waterLine = ph - 5;
        if (waterLine < 16) return;

        Color ridgeCol    = Palette::Mountain;
        Color lanternRed  = Palette::LanternRed;
        Color lanternGold = Palette::LanternLight;
        Color lanternGlow = Palette::OsmanthusGold;
        Color rippleCol   = Palette::WaterRipple;
        Color moonGlow    = Palette::MoonGlow;

        // 1. 玉兔静卧的月华石台剪影
        int terraceX1 = static_cast<int>(pw * 0.40f);
        int terraceX2 = static_cast<int>(pw * 0.68f);
        renderer.drawPixelLine(terraceX1, waterLine - 2, terraceX2, waterLine - 2, ridgeCol);
        renderer.drawPixelLine(terraceX1 + 2, waterLine - 1, terraceX2 - 2, waterLine - 1, Palette::NightWater);

        // 2. 右侧主景：望月仙阁飞檐剪影
        int rx = pw - 22;
        renderer.drawPixelLine(rx + 2, waterLine - 4, rx + 12, waterLine - 1, ridgeCol);
        renderer.drawPixel(rx + 1, waterLine - 5, ridgeCol); // 飞檐翘角
        renderer.drawPixelLine(rx + 12, waterLine - 1, rx + 20, waterLine - 4, ridgeCol);
        renderer.drawPixel(rx + 21, waterLine - 5, ridgeCol);

        // 阁楼二重檐 (筑阁效果)
        if (m_tier >= 2) {
            renderer.drawPixelLine(rx + 4, waterLine - 6, rx + 10, waterLine - 6, ridgeCol);
            renderer.drawPixel(rx + 3, waterLine - 7, ridgeCol);
            renderer.drawPixel(rx + 11, waterLine - 7, ridgeCol);
        }

        // 3. 左侧水乡民居市井剪影
        int lx = 6;
        renderer.drawPixelLine(lx, waterLine - 1, lx + 22, waterLine - 1, Palette::NightWater);
        if (m_tier >= 1) {
            renderer.drawPixelLine(lx + 2, waterLine - 3, lx + 8, waterLine - 1, ridgeCol);
            renderer.drawPixelLine(lx + 8, waterLine - 1, lx + 14, waterLine - 3, ridgeCol);
            renderer.drawPixel(lx + 1, waterLine - 4, ridgeCol);
            renderer.drawPixel(lx + 15, waterLine - 4, ridgeCol);
        }

        // 4. 节庆灯火渲染 (根据 m_tier 绽放人间烟火)
        float flicker = 0.85f + 0.15f * std::sin(time * 3.5f);

        // Tier 1: 华灯初上
        if (m_tier >= 1) {
            renderer.drawPixel(rx + 7, waterLine - 2, lanternGold.scaled(flicker));
            renderer.drawPixel(rx + 9, waterLine - 2, lanternRed);
            renderer.drawPixel(rx + 15, waterLine - 2, lanternGold.scaled(flicker * 0.9f));

            renderer.drawPixel(lx + 5, waterLine - 2, lanternRed);
            renderer.drawPixel(lx + 11, waterLine - 2, lanternGold.scaled(flicker));
        }

        // Tier 2: 宵月盛会（挂接连珠灯彩与水上画舫）
        if (m_tier >= 2) {
            // 右阁挂灯串
            renderer.drawPixel(rx + 3, waterLine - 4, lanternGold);
            renderer.drawPixel(rx + 11, waterLine - 4, lanternRed);
            renderer.drawPixel(rx + 19, waterLine - 4, lanternGold);

            // 水面迎仙画舫
            int boatX = static_cast<int>(pw * 0.22f);
            renderer.drawPixelLine(boatX, waterLine + 1, boatX + 7, waterLine + 1, ridgeCol);
            renderer.drawPixel(boatX + 6, waterLine, lanternGold.scaled(flicker)); // 船头暖灯
            renderer.drawPixel(boatX + 3, waterLine, lanternRed);
        }

        // Tier 3: 万家灯火（长街彻夜通明、灯火交织如繁星）
        if (m_tier >= 3) {
            // 阁楼顶部金顶光芒
            renderer.drawPixel(rx + 11, waterLine - 8, lanternGold);
            // 沿街密集红金灯笼
            for (int k = lx + 3; k < lx + 20; k += 3) {
                renderer.drawPixel(k, waterLine - 2, (k % 2 == 0) ? lanternRed : lanternGold.scaled(flicker));
            }
            // 右阁通明
            renderer.drawPixel(rx + 5, waterLine - 2, lanternGlow);
            renderer.drawPixel(rx + 13, waterLine - 2, lanternGlow);
        }

        // 5. 澄澈江波水韵与月华倒影
        for (int x = 0; x < pw; x += 2) {
            float dx = std::abs(static_cast<float>(x) - moonPixelX);
            if (dx < 14.0f) {
                // 满月正下方金色倒影波光
                if ((x + static_cast<int>(time * 3)) % 3 == 0) {
                    float alpha = (1.0f - dx / 14.0f) * 0.65f;
                    renderer.drawPixel(x, waterLine + 2, moonGlow.withOpacity(alpha));
                }
            } else if (m_tier >= 2 && x > rx && x < rx + 20) {
                // 右侧万家灯火在水中的赤金倒影
                if ((x + static_cast<int>(time * 4)) % 4 == 0) {
                    renderer.drawPixel(x, waterLine + 2, (m_tier == 3 ? lanternGlow : lanternRed).withOpacity(0.5f));
                }
            } else if ((x * 7 + static_cast<int>(time * 2)) % 17 == 0) {
                // 自然水波
                renderer.drawPixel(x, waterLine + 3, rippleCol);
            }
        }
    }

private:
    int m_tier = 1; // 默认：华灯初上
};

} // namespace festival::themes::mid_autumn
