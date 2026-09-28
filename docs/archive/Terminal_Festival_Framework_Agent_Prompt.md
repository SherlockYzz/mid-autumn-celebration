# Terminal Festival Celebration Framework
## Agent Engineering Prompt · 2026 中秋节首版

> **核心目标：** 构建一个可长期复用的轻量级终端节日庆祝框架。  
> 第一版实现 `Festival Core + Mid-Autumn Theme`。  
> 核心思想：**Core 负责“怎么庆祝”，Theme 负责“庆祝什么”。**

---

## 1. 任务目标

你现在是一名高级 C++ 工程师、Terminal UI 工程师、动画系统设计师，同时具备中国传统节日视觉设计能力。

请直接检查当前项目并开始实现。

我要做的不是一个一次性的中秋 Demo，而是一个可以长期扩展的：

**Terminal Festival Celebration Framework**

未来可以在完全复用 Core 的情况下继续增加：

- Chinese New Year
- Lantern Festival
- Dragon Boat Festival
- National Day
- Christmas
- Halloween
- Birthday
- Anniversary
- 其他节日/庆祝主题

本次先实现：

```text
Festival Core
+
Mid-Autumn Theme
```

---

# 2. FIRST：先检查当前项目

在写代码之前：

- 检查当前项目结构
- 判断当前使用的语言、标准和构建方式
- 优先遵循当前项目已有技术栈
- 如果适合，优先使用 C++17
- 检查现有 Terminal / ANSI / Animation / Utility 能力
- 不重复实现项目已经存在的功能
- 不破坏现有功能

不要先问我项目应该怎么改。

你自己分析并选择最合理的实现方案。

---

# 3. 架构：Core + Theme

整体采用：

```text
festival/
├── core/
│   ├── renderer/
│   ├── animation/
│   ├── particle/
│   ├── effects/
│   ├── scene/
│   ├── text/
│   └── platform/
│
├── themes/
│   └── mid_autumn/
│       ├── elements/
│       └── scenes/
│
└── main.cpp
```

具体文件结构可以根据当前项目调整，但架构边界必须保持。

---

# 4. Core / Theme 强边界

## Core 负责

“怎么渲染”

“怎么移动”

“怎么播放动画”

“怎么产生粒子”

“怎么产生烟花”

“怎么显示文字”

“怎么管理场景”

“怎么处理终端”

## Theme 负责

“什么时候出现”

“出现什么”

“使用什么颜色”

“表达什么节日文化”

“场景如何编排”

### Core 禁止知道任何具体节日

`core/` 中禁止出现任何类似：

```text
moon
rabbit
osmanthus
mid_autumn
lantern
moon_cake
chang_e
spring_festival
red_envelope
```

等节日专属概念。

验收时搜索 `core/` 中的节日专属关键词，原则上结果应为 0。

### Theme 不重复实现 Core

Theme 不应该重新实现：

- Renderer
- ParticleSystem
- Animation
- Terminal control
- Firework physics

所有通用能力必须调用 Core API。

---

# 5. Core：小而真正可复用

不要做游戏引擎。

不要 ECS。

不要几十层 interface。

不要为了“架构感”制造大量抽象类。

保持：

```text
small
clear
composable
reusable
```

但 Core 必须是真正可复用的能力，而不是简单的 `utils.cpp`。

---

# 6. Renderer

统一负责：

- Terminal Buffer
- ANSI Escape Sequence
- Cursor Control
- Character Rendering
- Text Rendering
- Color
- Terminal Resize
- Double Buffering

业务层禁止直接拼接 ANSI Escape Code。

建议提供类似：

```cpp
renderer.draw(x, y, ch, color);
renderer.drawText(x, y, text, color);
renderer.centerText(text, color);
renderer.clear();
renderer.present();
```

具体 API 根据项目实际情况设计。

---

# 7. Color

统一颜色表示。

支持：

- TrueColor
- ANSI 256
- ANSI 16

自动降级。

Theme 可以传 RGB，但不需要知道 ANSI 实现细节。

不要让 Theme 到处出现类似：

```cpp
"\033[38;2;255;200;100m"
```

这样的底层转义码。

---

# 8. Animation

建立统一时间驱动的轻量动画系统。

支持：

- from
- to
- duration
- easing
- loop
- callback

至少提供：

```text
linear
easeIn
easeOut
easeInOut
pulse
```

可用于：

- 位置
- 透明度
- 亮度
- 缩放
- 颜色过渡
- 呼吸效果

禁止各组件自己用：

```cpp
for (...) {
    sleep(...);
}
```

实现动画。

---

# 9. ParticleSystem

粒子至少支持：

```text
position
velocity
acceleration
life
maxLife
opacity
character
color
```

支持通用 emitter：

- directional
- radial
- gravity
- drift
- upward
- burst

同一个系统未来可以表现：

- 桂花
- 雪花
- 烟花碎屑
- 彩屑
- 星尘
- 天灯
- 光点

但 Core 不知道这些粒子具体代表什么。

---

# 10. Effects

Core 提供真正通用的庆祝效果：

- Firework
- Spark
- Burst
- Glow
- Confetti
- FloatingParticles
- StarField

## Firework

至少支持：

```text
launch
rise
explode
particle spread
gravity
fade
```

颜色、数量、速度、范围等全部参数化。

不要把任何节日颜色写死。

例如：

春节可以使用红金色；

中秋可以使用金色、暖白；

其他节日可以自由传入自己的颜色。

---

# 11. Text

提供：

- center
- fadeIn
- fadeOut
- typewriter
- pulse
- banner

必须考虑中文终端显示宽度。

不要使用简单 `strlen()` 处理中文布局。

---

# 12. Scene

提供：

```text
enter()
update(dt)
render()
handleInput()
exit()
```

支持：

- scene transition
- fade transition
- scene lifecycle
- 输入分发

---

# 13. Platform

负责：

- Terminal Size
- Color Capability
- Unicode Capability
- Keyboard Input
- Terminal Mode
- Graceful Shutdown

---

# 14. Terminal 质量要求

这是 Terminal 项目，不是普通控制台输出。

必须：

- 双缓冲
- 避免闪烁
- 稳定帧率
- 动态适配终端大小
- 正确处理中文宽度
- 正确处理 Ctrl+C
- 正确恢复 terminal state

目标约：

```text
30 FPS
```

不需要为了 60 FPS 浪费 CPU。

粒子数量应受到控制。

退出后必须恢复：

- Cursor Visibility
- Terminal Colors
- Input Echo
- Terminal Mode

---

# 15. 终端兼容

优先支持：

- Linux Terminal
- macOS Terminal
- Windows Terminal
- VS Code Terminal

颜色自动降级：

```text
TrueColor
    ↓
256 Color
    ↓
16 Color
```

视觉字符自动降级：

```text
Unicode
    ↓
ASCII
```

如果终端能力不足：

**降低视觉质量，而不是崩溃。**

---

# 16. 2026 中秋主题

现在实现：

**2026 Mid-Autumn Festival**

核心审美：

> 静谧秋夜，月圆人圆。

不要做成：

- 春节式热闹
- RGB Cyberpunk
- 满屏烟花
- 元素堆砌
- 廉价 ASCII Art

应该是：

```text
东方
清雅
柔和
克制
温暖
诗意
```

视觉核心：

> **月光为魂，灯火为暖，桂香为动，诗词为意。**

---

# 17. 中秋配色

建议：

| 用途 | 颜色 |
|---|---|
| 夜空 | `#0A1020` |
| 星星 | `#E0E8F0` |
| 月亮 | `#F5F0E1` |
| 月晕 | `#D4B87A` |
| 桂花 | `#E6C878` |
| 灯笼 | `#C23A2B` |
| 灯光 | `#FFD080` |
| 诗词 | `#F0EBE0` |

允许根据终端实际效果微调。

但必须保持：

**深靛蓝夜空 + 月白月光 + 淡金桂花 + 暖红灯火**

的冷暖关系。

禁止随机彩虹色。

---

# 18. 中秋专属元素

实现：

```text
Moon
JadeRabbit
Osmanthus
Lantern
Cloud
Poetry
Moonlight
```

全部属于 Theme。

## Moon

必须是整个画面的视觉中心。

具有：

- 圆形月轮
- 简单月面纹理
- 月晕
- Glow Pulse
- Moonrise Animation

不要只打印一个：

```text
O
```

尽可能使用：

- Unicode
- Block Characters
- ANSI Shading

构建完整月轮。

---

## Jade Rabbit

极简、优雅。

不要卡通化。

可以表现：

- 静立
- 抬头
- 耳朵轻动
- 缓慢捣药

动作非常轻。

---

## Osmanthus

使用 ParticleSystem。

粒子数量少。

建议约：

```text
20~30
```

缓慢下落。

具有：

- gravity
- horizontal drift
- slight wind

字符可以使用：

```text
·
*
✽
```

并根据 Unicode 能力自动降级。

---

## Lantern

2~3 盏即可。

朱红灯身。

暖黄色灯光。

轻微摇摆。

光亮缓慢呼吸。

---

## Cloud

少量云层。

缓慢横向移动。

偶尔掠过月亮。

制造朦胧月色。

---

# 19. Scene Flow

不要所有元素同时出现。

设计完整视觉节奏：

```text
Night
  ↓
Moonrise
  ↓
Osmanthus
  ↓
Jade Rabbit
  ↓
Lantern
  ↓
Poetry
  ↓
Celebration
  ↓
Interactive
```

## Scene 1 — Night

黑色 / 深靛蓝夜空。

少量星星逐渐出现。

云层缓慢移动。

## Scene 2 — Moonrise

满月缓缓升起。

使用 `easeOut`。

月晕逐渐增强。

月光逐渐扩散。

## Scene 3 — Osmanthus

桂树 / 桂花意象出现。

少量金色桂花粒子缓慢飘落。

## Scene 4 — Jade Rabbit

玉兔静静出现在月轮附近。

只做轻微动作。

## Scene 5 — Lantern

2~3 盏灯笼逐渐亮起。

形成：

```text
冷月 + 暖灯
```

的视觉关系。

## Scene 6 — Poetry

画面中央保持留白。

诗句淡入：

> 海上生明月，天涯共此时

停留后淡出。

第二句可通过交互切换：

> 但愿人长久，千里共婵娟

## Scene 7 — Celebration

最终只出现一次非常克制的金色烟花。

不要抢走月亮的视觉中心。

然后进入：

```text
Interactive Mode
```

---

# 20. 轻量交互

动画结束后进入待机状态。

建议：

```text
M → Moonlight
R → Rabbit animation
L → Lantern toggle
P → Next poetry
F → Firework
Q / ESC → Quit
```

具体实现可以自行优化。

交互的目标是：

**惊喜，而不是游戏。**

---

# 21. 可选创意元素

如果实现成本低且不会破坏整体审美，可以加入：

- 少量孔明灯
- 远山 / 亭台剪影
- 印章式“中秋”落款
- 非侵入式灯谜
- 隐藏彩蛋

但遵循：

> 少即是多。

如果一个元素会让画面变乱：

**不要加。**

---

# 22. 文化方向

中秋不要简单等于：

```text
Moon + Rabbit + Firework
```

真正的主题应该是：

```text
月圆
团圆
赏月
桂香
灯火
诗意
```

烟花只是庆祝效果。

不能让烟花成为中秋主题本身。

**月亮始终是视觉中心。**

---

# 23. 无外部素材

默认：

- 不使用图片
- 不使用网络
- 不使用在线 API
- 不下载资源
- 不依赖外部素材

所有视觉效果尽量由：

```text
字符
颜色
粒子
动画
ANSI
程序绘制
```

生成。

这样整个 Framework 才真正可移植。

---

# 24. 工程要求

代码必须：

- 可编译
- 可运行
- 模块化
- 可维护
- 无明显重复
- 无明显内存泄漏
- 无无意义抽象

不要留下：

```text
TODO
以后实现
这里需要你补充
```

等未完成内容。

如果需要取舍：

优先保证：

```text
架构正确
+
运行稳定
+
视觉完整
```

而不是堆功能。

---

# 25. 自测与迭代

不要只写代码然后告诉我“应该可以”。

必须：

```text
Inspect
  ↓
Implement
  ↓
Build
  ↓
Run
  ↓
Observe
  ↓
Fix
  ↓
Build Again
  ↓
Run Again
```

主动检查：

- 闪烁
- 错位
- 中文宽度
- 月亮比例
- 颜色
- 粒子数量
- 动画速度
- CPU 占用
- Terminal Restore
- 小窗口布局

如果发现问题：

**自己修改，不要把调参工作交给我。**

---

# 26. 架构验收

完成后必须验证：

Core 可以脱离 Mid-Autumn Theme 独立存在。

删除：

```text
themes/mid_autumn/
```

之后：

Core 仍然应该可以编译。

未来新增：

```text
themes/chinese_new_year/
```

不允许复制：

```text
renderer
animation
particle
firework
scene
terminal
```

等 Core 实现。

最终必须满足：

```text
Core  = Reusable Mechanisms

Theme = Cultural Expression
```

---

# 27. 交付

完成后只需要汇报：

1. 当前 Core 包含哪些模块
2. Mid-Autumn Theme 包含哪些模块
3. 如何编译
4. 如何运行
5. 当前支持哪些交互
6. 未来新增一个节日需要新增什么

不要给我长篇理论。

最重要的是：

**直接把可以运行的工程做好。**

---

# FINAL DESIGN PRINCIPLE

请始终记住：

这是一个：

**Terminal Festival Framework**

不是一个一次性的中秋 Demo。

Core 应该让未来的节日开发越来越简单。

Theme 应该让每一个节日都拥有自己的灵魂。

本次中秋的灵魂是：

> **静谧秋夜，月圆人圆。**

让用户打开终端后，不是看到一堆特效，

而是看到：

一轮月亮升起，

桂花轻落，

玉兔静卧月中，

灯火在夜色里微微摇曳，

然后出现：

> **海上生明月，天涯共此时。**

这就是最终的视觉与工程目标。

**现在直接开始实现。**
