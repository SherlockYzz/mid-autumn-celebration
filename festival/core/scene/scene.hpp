#pragma once

#include "../renderer/renderer.hpp"
#include "../platform/input_event.hpp"

namespace festival::core {

class Scene {
public:
    virtual ~Scene() = default;

    virtual void enter() {}
    virtual void update(float dt) = 0;
    virtual void render(Renderer& renderer) = 0;
    virtual void handleInput(const InputEvent& event) { (void)event; }
    virtual void exit() {}
};

} // namespace festival::core
