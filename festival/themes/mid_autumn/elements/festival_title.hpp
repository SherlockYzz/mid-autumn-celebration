#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

namespace festival::themes::mid_autumn {

/**
 * 核心标题：东方节庆感像素书法艺术字「中秋快乐」
 *
 * 采用 Sub-pixel Half-Block 2x 垂直分辨率像素矩阵雕琢。
 * 具备流光渐变（描金汉白到暖琥珀金）、柔和月晕光辉与流动光斑。
 */
class FestivalTitle {
public:
    FestivalTitle() = default;

    void update(float dt) {
        m_time += dt;
    }

    void render(Renderer& renderer, int screenWidth, const std::string& targetName, float highlightProgress = 0.0f) {
        // 4 个汉字的 9x9 / 10x9 精确像素矩阵
        static const int ZHONG_W = 9;
        static const int ZHONG_H = 9;
        static const char* ZHONG[9] = {
            "....#....",
            "....#....",
            ".#######.",
            ".#..#..#.",
            ".#..#..#.",
            ".#######.",
            "....#....",
            "....#....",
            "....#....",
        };

        static const int QIU_W = 10;
        static const int QIU_H = 9;
        static const char* QIU[9] = {
            ".#.......#",
            "#####..#..",
            "..#...####",
            "..#...#..#",
            ".###..####",
            "#.#.#.#..#",
            "#.#.#.#..#",
            "..#...#..#",
            "......#..#",
        };

        static const int KUAI_W = 10;
        static const int KUAI_H = 9;
        static const char* KUAI[9] = {
            ".#...####.",
            "##...#..#.",
            ".#...####.",
            ".#...#....",
            ".#...####.",
            ".#...#..#.",
            ".#..#....#",
            ".#..#....#",
            ".#.#......",
        };

        static const int LE_W = 9;
        static const int LE_H = 9;
        static const char* LE[9] = {
            "...###...",
            ".######..",
            ".#...#...",
            ".....#...",
            "..#####..",
            ".#...#..#",
            "#....#..#",
            ".....#...",
            "....##...",
        };

        const int gap = (screenWidth >= 90) ? 3 : 2;
        const int totalWidth = ZHONG_W + gap + QIU_W + gap + KUAI_W + gap + LE_W;

        // 居中或稍偏左（留出右上角满月展示空间）
        int startX = (screenWidth - totalWidth) / 2;
        if (screenWidth >= 80) {
            startX = std::max(4, startX - 4);
        } else {
            startX = std::max(1, startX);
        }

        // 位于顶部天穹（大字像素行 5 ~ 14，留出上方字符行 1 放置题名匾额）
        int startY = 5;

        // 0. 绘制顶上题名匾额：「✦ 祝【XXX】✦」
        {
            std::string prefix = "✦ 祝【";
            std::string suffix = "】✦";
            std::string fullHeader = prefix + targetName + suffix;
            int bannerW = core::stringVisualWidth(fullHeader);
            int bannerX = startX + (totalWidth - bannerW) / 2;
            int bannerY = 1; // 字符第 1 行

            float pulse = 0.85f + 0.15f * std::sin(m_time * 2.5f);
            Color wingCol   = Palette::FireworkGold.withOpacity(pulse);
            Color zhucol    = Palette::MoonWhite;
            Color bracketCol= Palette::SealRed;
            Color nameCol   = (highlightProgress > 0.05f) ?
                Color::fromHex(0xFFFFFF) :
                Color::fromHex(0xFFE28A);

            // 装饰托底横线
            if (bannerX >= 4 && bannerW < screenWidth - 8) {
                renderer.drawChar(bannerX - 2, bannerY, "─", Palette::OsmanthusDark.withOpacity(0.6f));
                renderer.drawChar(bannerX + bannerW + 1, bannerY, "─", Palette::OsmanthusDark.withOpacity(0.6f));
            }

            // 绘制完整题名
            renderer.drawText(bannerX, bannerY, fullHeader, nameCol, Palette::NightSky, highlightProgress > 0.05f);
        }

        struct CharDef {
            const char** bitmap;
            int w;
            int h;
        };

        CharDef chars[4] = {
            {ZHONG, ZHONG_W, ZHONG_H},
            {QIU,   QIU_W,   QIU_H},
            {KUAI,  KUAI_W,  KUAI_H},
            {LE,    LE_W,    LE_H}
        };

        // 1. 第一遍绘制：柔和金色月华光晕 (Outer Shimmer Glow)
        Color haloBase = Palette::MoonGlow;
        float pulse = 0.28f + 0.12f * std::sin(m_time * 2.2f) + highlightProgress * 0.4f;

        int curX = startX;
        for (int c = 0; c < 4; ++c) {
            for (int y = 0; y < chars[c].h; ++y) {
                for (int x = 0; x < chars[c].w; ++x) {
                    if (chars[c].bitmap[y][x] == '#') {
                        // 在周围 4 邻域绘制淡雅光晕
                        int px = curX + x;
                        int py = startY + y;
                        Color haloCol = haloBase.withOpacity(pulse);
                        renderer.drawPixel(px - 1, py, haloCol);
                        renderer.drawPixel(px + 1, py, haloCol);
                        renderer.drawPixel(px, py - 1, haloCol);
                        renderer.drawPixel(px, py + 1, haloCol);
                    }
                }
            }
            curX += chars[c].w + gap;
        }

        // 2. 第二遍绘制：本体东方像素书法笔画（金箔流光渐变）
        curX = startX;
        Color goldTop = Color::fromHex(0xFFF9E6); // 皓白月光金
        Color goldMid = Color::fromHex(0xFFD269); // 暖金
        Color goldBot = Color::fromHex(0xF59E0B); // 琥珀深金
        Color goldShine = Color::fromHex(0xFFFFFF); // 流光亮斑

        for (int c = 0; c < 4; ++c) {
            for (int y = 0; y < chars[c].h; ++y) {
                float vertRatio = static_cast<float>(y) / static_cast<float>(chars[c].h - 1);
                Color baseColor = (vertRatio < 0.45f) ?
                    Color::lerp(goldTop, goldMid, vertRatio / 0.45f) :
                    Color::lerp(goldMid, goldBot, (vertRatio - 0.45f) / 0.55f);

                for (int x = 0; x < chars[c].w; ++x) {
                    if (chars[c].bitmap[y][x] == '#') {
                        int px = curX + x;
                        int py = startY + y;

                        // 斜向流光扫描波
                        float wave = std::sin((px * 0.35f - py * 0.25f) - m_time * 2.8f);
                        Color finalColor = baseColor;
                        if (wave > 0.82f) {
                            float shineAlpha = (wave - 0.82f) / 0.18f;
                            finalColor = Color::lerp(baseColor, goldShine, shineAlpha * 0.85f);
                        }

                        renderer.drawPixel(px, py, finalColor);
                    }
                }
            }
            curX += chars[c].w + gap;
        }

        // 3. 标题左右两侧的祥云星纹点缀（古典对称装饰）
        if (startX >= 8) {
            Color cloudCol = Palette::CloudHighlight.withOpacity(0.6f);
            Color starCol = Palette::FireworkGold.withOpacity(0.75f);
            // 左侧纹饰
            renderer.drawPixel(startX - 4, startY + 4, starCol);
            renderer.drawPixel(startX - 5, startY + 4, cloudCol);
            renderer.drawPixel(startX - 3, startY + 4, cloudCol);
            // 右侧纹饰
            renderer.drawPixel(curX + 2, startY + 4, starCol);
            renderer.drawPixel(curX + 1, startY + 4, cloudCol);
            renderer.drawPixel(curX + 3, startY + 4, cloudCol);
        }
    }

private:
    float m_time = 0.0f;
};

} // namespace festival::themes::mid_autumn
