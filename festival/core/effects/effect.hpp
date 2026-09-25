#pragma once

#include "../renderer/renderer.hpp"

namespace festival::core {

class Effect {
public:
    virtual ~Effect() = default;
    virtual void update(float dt) = 0;
    virtual void render(Renderer& renderer) = 0;
    virtual bool isFinished() const = 0;
};

} // namespace festival::core
