#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include <vector>
#include <string>

namespace festival::themes::mid_autumn {

struct WispyCloud {
    float x = 0.0f;
    float y = 0.0f;
    float speed = 0.8f;
    std::vector<std::string> lines;
    float opacity = 0.6f;
};

class CloudLayer {
public:
    CloudLayer() = default;

    void initialize(int screenWidth, int screenHeight) {
        m_clouds.clear();

        // Cloud 1: High ethereal wisp near top/moon
        WispyCloud c1;
        c1.x = screenWidth * 0.15f;
        c1.y = screenHeight * 0.22f;
        c1.speed = 0.65f;
        c1.opacity = 0.55f;
        c1.lines = {
            "  .---.   _  ",
            " (     )-' )~",
            "  `---'---'  "
        };
        m_clouds.push_back(c1);

        // Cloud 2: Gentle layered cloud that can drift across the moon
        WispyCloud c2;
        c2.x = screenWidth * 0.50f;
        c2.y = screenHeight * 0.35f;
        c2.speed = 0.45f;
        c2.opacity = 0.65f;
        c2.lines = {
            "    .----.       _.._     ",
            "  (        )---'(     )~~ ",
            "   `------'      `---'    "
        };
        m_clouds.push_back(c2);

        // Cloud 3: Distant low wisp
        WispyCloud c3;
        c3.x = screenWidth * 0.80f;
        c3.y = screenHeight * 0.50f;
        c3.speed = 0.85f;
        c3.opacity = 0.45f;
        c3.lines = {
            "   _..- -.._   ",
            " ~(         )~ ",
            "   `-------'   "
        };
        m_clouds.push_back(c3);
    }

    void update(float dt, int screenWidth) {
        for (auto& c : m_clouds) {
            c.x += c.speed * dt;
            // Wrap around screen
            if (c.x > screenWidth + 10.0f) {
                c.x = -35.0f;
            }
        }
    }

    void render(Renderer& renderer) {
        for (const auto& c : m_clouds) {
            int bx = static_cast<int>(std::round(c.x));
            int by = static_cast<int>(std::round(c.y));

            Color bodyCol = Palette::CloudBody.withOpacity(c.opacity);
            Color edgeCol = Palette::CloudHighlight.withOpacity(c.opacity * 0.9f);

            for (size_t row = 0; row < c.lines.size(); ++row) {
                int curY = by + static_cast<int>(row);
                if (curY < 0 || curY >= renderer.height()) continue;

                const std::string& line = c.lines[row];
                for (size_t col = 0; col < line.size(); ++col) {
                    char ch = line[col];
                    if (ch == ' ') continue;

                    int curX = bx + static_cast<int>(col);
                    if (curX < 0 || curX >= renderer.width()) continue;

                    Color chColor = (row == 0 || ch == '~' || ch == '-') ? edgeCol : bodyCol;

                    // Blend cloud over existing moon/sky cells
                    const Cell& ex = renderer.backBuffer().get(curX, curY);
                    if (ex.bg.a > 0) {
                        // Softly veil the bright moon surface
                        Color blendedBg = Color::blend(ex.bg, bodyCol, 0.4f);
                        renderer.drawChar(curX, curY, std::string(1, ch), chColor, blendedBg);
                    } else {
                        renderer.drawChar(curX, curY, std::string(1, ch), chColor);
                    }
                }
            }
        }
    }

private:
    std::vector<WispyCloud> m_clouds;
};

} // namespace festival::themes::mid_autumn
