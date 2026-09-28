/**
 * John Tromp Lambda Diagrams Workstation - Internationalization (i18n) Engine
 * Bilingual support (Chinese & English) with zero external dependencies.
 */

let currentLang = 'zh'; // 'zh' | 'en'

const I18N = {
  zh: {
    brandTitle: "John Tromp Lambda 图动态演算工作台",
    badge: "Tromp Diagrams",
    brandSub: "纯黑白极简电路式几何拓扑 · 五阶段 β-规约动态演示 · 作用域分析器",
    langToggle: "English",
    themeToggleLight: "浅色模式",
    themeToggleDark: "深色模式",
    guideBtn: "图解指南",
    aboutBtn: "关于 Tromp",
    authBtn: "登录 / 注册",
    authGuest: "访客 (离线)",
    repoBtn: "公式仓库",
    saveRepoBtn: "存入仓库",
    inputSection: "表达式输入",
    statusReady: "就绪",
    statusParsed: "已解析",
    statusError: "语法错误",
    statusNormal: "已达范式",
    statusRedex: "红基待规约",
    statusLoop: "循环熔断 (>300步)",
    tabClassic: "经典记法 (λx.x)",
    tabDeBruijn: "De Bruijn 记法 (λλ1)",
    placeholderClassic: "输入表达式，例如 (\\x. x) y 或 (\\f. \\x. f (f x))",
    placeholderDeBruijn: "输入 De Bruijn 索引序列，例如 λλ1 或 \\ \\ 2 (2 1)",
    parseBtn: "解析绘制",
    stepperSection: "五阶段 β-规约演进器",
    stepCounterPrefix: "第",
    stepCounterSuffix: "步",
    phasePill1: "1. 锁定红基",
    phasePill2: "2. 关联形参",
    phasePill3: "3. 溶解横梁",
    phasePill4: "4. 代换接入",
    phasePill5: "5. 最终整流",
    microPrev: "上一阶段",
    microNext: "下一阶段",
    macroReset: "重置",
    macroPrev: "单步后退",
    macroNext: "单步规约",
    macroPlay: "自动演示",
    macroPause: "暂停演示",
    reduceNormal: "规约至范式",
    strategyLabel: "规约策略:",
    optNormal: "最左最外优先 (正常序)",
    optApplicative: "应用序 (最内优先)",
    speedLabel: "演示速率:",
    codeSection: "代码与项对照",
    copyCodeBtn: "复制表达式",
    copiedToast: "已复制",
    namedLabel: "经典变量记法",
    deBruijnLabel: "De Bruijn 索引记法",
    historySection: "规约历史",
    historyBadgeSteps: (n) => `${n} 步`,
    thHistRule: "规则",
    thHistTerm: "表达式",
    histRuleInit: "初始",
    histRuleBeta: "β-规约",
    histRuleNormal: "范式",
    initialTermLabel: "初始项",
    tabSvg: "SVG 矢量图",
    tabBoxChar: "Unicode 制表画",
    tabAscii: "ASCII 点阵网格",
    optStyleStd: "标准风格 (最左变量连接)",
    optStyleAlt: "替代风格 (最近深层连接)",
    showLabels: "显示标签",
    smoothAnim: "平滑动画",
    exportSvg: "导出 SVG",
    exportPng: "导出 PNG",
    copyBoxCharBtn: "复制字符画",
    boxCharTitle: "John Tromp Unicode 制表字符画 (boxChar)",
    asciiTitle: "John Tromp Raw ASCII 点阵网格 (_ 与 |)",
    readyNarrative: "就绪。可点击“单步规约”或“下一微阶段”查看演算步骤。",
    normalFormNarrative: "当前项已达范式（Normal Form），无可规约红基。",
    loopNarrative: "连续规约超过 300 步，可能存在循环或发散项（如 Omega 组合子）。",
    tooltipAbs: "λ-抽象函数",
    tooltipVar: "绑定变量",
    tooltipApp: "应用连接桥",
    tooltipRedex: "β-红基应用 (点击定向规约)",
    openPresetsBtn: "预设公式库...",
    modalPresetsTitle: "预设公式库",
    modalPresetsBadge: "23 个经典公式",
    searchPresetsPlaceholder: "快速筛选名称、组合子或表达式...",
    catAll: "全部",
    thSymbol: "名称 / 标识",
    thExpression: "Lambda 表达式",
    thCategory: "类别",
    thDescription: "说明",
    thAction: "操作",
    btnLoadPreset: "载入",
    txtModalFooterTip: "提示：点击列表任意行或点击右侧 [载入]，即可将该公式载入工作台并立即绘制 Tromp 图。",
    presetCountSuffix: "项",
    presetGroups: {
      core: "基础组合子",
      church: "Church 计数",
      arithmetic: "算术与逻辑",
      classics: "Tromp 经典项"
    },

    // Auth & Repository Strings
    modalAuthTitle: "用户身份认证",
    tabLogin: "账号登录",
    tabRegister: "新用户注册",
    lblUsername: "用户名",
    lblPassword: "密码",
    placeholderUser: "输入用户名 (3-32位字母/数字/下划线)",
    placeholderPass: "输入密码 (不少于6位)",
    btnSubmitLogin: "登录",
    btnSubmitRegister: "注册并登录",
    btnLogout: "退出登录",
    userProfileTitle: "个人资料",
    lblMemberSince: "注册时间",
    lblSavedRepos: "云端存储项",
    msgRegSuccess: "注册成功，已建立安全会话",
    msgLoginSuccess: "登录成功",
    msgLogoutSuccess: "已退出当前账号",
    modalRepoTitle: "表达式仓库",
    tabMyRepos: "我的云端仓库",
    tabPublicRepos: "公共广场",
    tabOfflineRepos: "本地离线存储",
    btnSaveCurrent: "存入当前项",
    btnExportJson: "导出备份 (JSON)",
    btnImportJson: "导入备份 (JSON)",
    searchRepoPlaceholder: "检索公式标题、代码或标签...",
    modalSaveTitle: "保存表达式入库",
    lblRepoTitle: "公式标题",
    placeholderRepoTitle: "例如：Church Numeral Two",
    lblRepoCode: "表达式代码",
    lblRepoDesc: "说明描述",
    placeholderRepoDesc: "可选说明或备注信息...",
    lblRepoTags: "标签 (逗号分隔)",
    placeholderRepoTags: "例如: church, math, tromp",
    lblRepoPublic: "在公共广场公开分享",
    btnConfirmSave: "保存入库",
    msgSavedSuccess: "公式已保存至仓库",
    msgDeleteConfirm: "确定要删除该公式吗？此操作不可逆。",
    msgDeleted: "公式已成功删除",
    emptyMyRepos: "您的云端仓库暂无公式。点击左侧“存入当前项”即可持久化保存。",
    emptyPublicRepos: "公共广场暂无公开公式。",
    emptyOfflineRepos: "本地离线仓库暂无内容。",
    authorPrefix: "贡献者:",
    serverOfflineNotice: "服务器离线中，当前已平滑无感降级至本地离线存储模式。",
    serverOnlineNotice: "已连接服务器，云端同步与多用户体系已就绪。",

    guideHtml: `
      <p>1997 年，荷兰计算机科学家 <strong>John Tromp</strong> 提出了 <em>Lambda Diagrams</em>（Lambda 图），这是一种针对无类型 Lambda 演算闭项的纯二维几何拓扑表示法。它完全抛弃了容易产生命名混淆的变量名，将计算过程抽象为直观的几何导线与电路。</p>
      <h3>1. 三大几何原语</h3>
      <ul>
        <li><strong>水平线 (─)</strong>：表示 <strong>λ-抽象（函数）</strong>。每当引入一个新的函数参数，就会在其整个作用域体上方绘制一条横向横梁。</li>
        <li><strong>垂直线 (│)</strong>：表示 <strong>绑定变量的出现</strong>。它从其所属的抽象横线上垂落至使用处。下垂的纵向落差精确对应其 De Bruijn 索引深度！</li>
        <li><strong>水平连接线 (─)</strong>：表示 <strong>函数应用（Application）</strong>。它如桥梁般水平连通函数的输出端与实参的输入端。</li>
      </ul>
      <h3>2. 标准风格与替代风格</h3>
      <ul>
        <li><strong>标准风格 (Standard Style)</strong>：应用横梁严格连向函数的<strong>最左侧变量</strong>。线条高度规范、形式完全统一。</li>
        <li><strong>替代风格 (Alternative Style)</strong>：应用横梁连向<strong>最近的深层变量</strong>。结构更紧凑，常用于高阶素数筛或大数表示。</li>
      </ul>
      <h3>3. 五阶段动态 β-规约流程</h3>
      <ul>
        <li><strong>阶段 1 (识别红基)</strong>：锁定待求值的 <code>(λx. M) N</code> 应用桥，虚线脉冲指示。</li>
        <li><strong>阶段 2 (关联变量与参数)</strong>：高亮函数体中所有待替换的形参 x 垂线及实参子图 N。</li>
        <li><strong>阶段 3 (溶解抽象横梁)</strong>：目标 λx 的水平线溶解消失，形参垂线端口开启接收插槽。</li>
        <li><strong>阶段 4 (执行代换与移位)</strong>：实参 N 复制并接入插槽，外部自由变量索引自动移位修正。</li>
        <li><strong>阶段 5 (最终项整流)</strong>：新形成的项整流归位至标准 Tromp 网格，进入下一轮。</li>
      </ul>
    `,
    aboutHtml: `
      <p><strong>John Tromp</strong> 是一位杰出的计算机科学家与数学家，因在<em>算法信息论 (AIT)</em>、完全解决<em>四子棋 (Connect Four)</em>博弈以及发明<strong>二进制 Lambda 演算 (Binary Lambda Calculus, BLC)</strong>而闻名。</p>
      <h3>二进制 Lambda 演算 (BLC)</h3>
      <p>在 BLC 中，Tromp 设计了世界上极其精简的图灵完备程序设计语言：</p>
      <ul>
        <li><code>00</code> = 抽象 (λ)</li>
        <li><code>01</code> = 应用</li>
        <li><code>1...10</code> = 变量 (一进制编码的 De Bruijn 索引)</li>
      </ul>
      <p>Tromp 凭借该体系构建了仅占 <strong>21 字节 (168 比特)</strong> 的通用自解释器以及 <strong>167 比特</strong> 的素数筛程序，两者均可通过 Lambda Diagrams 进行可视化呈现！</p>
    `
  },
  en: {
    brandTitle: "John Tromp's Lambda Diagrams Workstation",
    badge: "Tromp Diagrams",
    brandSub: "Monochrome Minimalist Geometric Topology · 5-Phase β-Reduction · Scope Analyzer",
    langToggle: "中文",
    themeToggleLight: "Light Theme",
    themeToggleDark: "Dark Theme",
    guideBtn: "Visual Guide",
    aboutBtn: "About Tromp",
    authBtn: "Login / Register",
    authGuest: "Guest (Offline)",
    repoBtn: "Repository",
    saveRepoBtn: "Save to Repo",
    inputSection: "Expression Input",
    statusReady: "Ready",
    statusParsed: "Parsed",
    statusError: "Syntax Error",
    statusNormal: "Normal Form",
    statusRedex: "Redex Ready",
    statusLoop: "Loop (>300 steps)",
    tabClassic: "Classic (λx.x)",
    tabDeBruijn: "De Bruijn (λλ1)",
    placeholderClassic: "Enter expression, e.g. (\\x. x) y or (\\f. \\x. f (f x))",
    placeholderDeBruijn: "Enter De Bruijn indices, e.g. λλ1 or \\ \\ 2 (2 1)",
    parseBtn: "Parse & Render",
    stepperSection: "5-Phase β-Reduction Stepper",
    stepCounterPrefix: "Step",
    stepCounterSuffix: "",
    phasePill1: "1. Redex",
    phasePill2: "2. Vars & Arg",
    phasePill3: "3. Dissolve λ",
    phasePill4: "4. Substitute",
    phasePill5: "5. Settle",
    microPrev: "Prev Phase",
    microNext: "Next Phase",
    macroReset: "Reset",
    macroPrev: "Step Back",
    macroNext: "Step Forward",
    macroPlay: "Auto Play",
    macroPause: "Pause",
    reduceNormal: "Reduce to Normal Form",
    strategyLabel: "Strategy:",
    optNormal: "Normal Order (Leftmost-Outermost)",
    optApplicative: "Applicative Order (Innermost-First)",
    speedLabel: "Speed:",
    codeSection: "Synchronized Code Inspector",
    copyCodeBtn: "Copy Expression",
    copiedToast: "Copied",
    namedLabel: "Classic Named Notation",
    deBruijnLabel: "De Bruijn Indices",
    historySection: "Reduction History",
    historyBadgeSteps: (n) => `${n} ${n === 1 ? 'step' : 'steps'}`,
    thHistRule: "Rule",
    thHistTerm: "Term",
    histRuleInit: "Init",
    histRuleBeta: "β-Redex",
    histRuleNormal: "Normal Form",
    initialTermLabel: "Initial Term",
    tabSvg: "SVG Vector",
    tabBoxChar: "Unicode Box-Art",
    tabAscii: "Raw ASCII Grid",
    optStyleStd: "Standard Style (Leftmost Vars)",
    optStyleAlt: "Alternative Style (Deepest Nearest)",
    showLabels: "Show Labels",
    smoothAnim: "Smooth Motion",
    exportSvg: "Export SVG",
    exportPng: "Export PNG",
    copyBoxCharBtn: "Copy Box-Art",
    boxCharTitle: "John Tromp Unicode Box-Drawing (boxChar)",
    asciiTitle: "John Tromp Raw ASCII Grid (_ and |)",
    readyNarrative: "Ready. Click 'Step Forward' or 'Next Phase' to view reduction transitions.",
    normalFormNarrative: "Current term is in Normal Form (no active redexes).",
    loopNarrative: "Reduction exceeded 300 steps. A cycle or divergence was detected (e.g. Omega).",
    tooltipAbs: "λ-Abstraction",
    tooltipVar: "Bound Variable",
    tooltipApp: "Application Bridge",
    tooltipRedex: "β-Redex Application (Click to reduce)",
    openPresetsBtn: "Preset Formulas Library...",
    modalPresetsTitle: "Preset Formulas Library",
    modalPresetsBadge: "23 Classics",
    searchPresetsPlaceholder: "Filter by combinator, name or expression...",
    catAll: "All",
    thSymbol: "Symbol / Name",
    thExpression: "Lambda Expression",
    thCategory: "Category",
    thDescription: "Description",
    thAction: "Action",
    btnLoadPreset: "Load",
    txtModalFooterTip: "Tip: Click any row or click [Load] to instantly load the formula into the workspace and render its Tromp diagram.",
    presetCountSuffix: "items",
    presetGroups: {
      core: "Core Combinators",
      church: "Church Numerals",
      arithmetic: "Arithmetic & Logic",
      classics: "Tromp Classics"
    },

    // Auth & Repository Strings
    modalAuthTitle: "User Authentication",
    tabLogin: "Sign In",
    tabRegister: "Sign Up",
    lblUsername: "Username",
    lblPassword: "Password",
    placeholderUser: "Enter username (3-32 chars)",
    placeholderPass: "Enter password (6+ chars)",
    btnSubmitLogin: "Sign In",
    btnSubmitRegister: "Create Account",
    btnLogout: "Sign Out",
    userProfileTitle: "User Profile",
    lblMemberSince: "Member Since",
    lblSavedRepos: "Cloud Formulas",
    msgRegSuccess: "Registered and authenticated successfully",
    msgLoginSuccess: "Logged in successfully",
    msgLogoutSuccess: "Signed out successfully",
    modalRepoTitle: "Expression Repository",
    tabMyRepos: "My Cloud Formulas",
    tabPublicRepos: "Public Square",
    tabOfflineRepos: "Offline Local Store",
    btnSaveCurrent: "Save Current Term",
    btnExportJson: "Export Backup (JSON)",
    btnImportJson: "Import Formulas (JSON)",
    searchRepoPlaceholder: "Search by title, code or tags...",
    modalSaveTitle: "Save Term to Repository",
    lblRepoTitle: "Formula Title",
    placeholderRepoTitle: "e.g. Church Numeral Two",
    lblRepoCode: "Expression Code",
    lblRepoDesc: "Description / Notes",
    placeholderRepoDesc: "Optional description or comments...",
    lblRepoTags: "Tags (comma separated)",
    placeholderRepoTags: "e.g. church, math, tromp",
    lblRepoPublic: "Share in Public Square",
    btnConfirmSave: "Save to Repository",
    msgSavedSuccess: "Formula successfully saved to repository",
    msgDeleteConfirm: "Are you sure you want to delete this formula? This cannot be undone.",
    msgDeleted: "Formula deleted successfully",
    emptyMyRepos: "No cloud formulas saved yet. Click 'Save Current Term' to store your expressions.",
    emptyPublicRepos: "No public formulas found in the square.",
    emptyOfflineRepos: "No offline local formulas stored.",
    authorPrefix: "Author:",
    serverOfflineNotice: "Server is offline; seamlessly switched to local offline storage mode.",
    serverOnlineNotice: "Server connected; cloud synchronization and user authentication active.",

    guideHtml: `
      <p>In 1997, Dutch computer scientist <strong>John Tromp</strong> designed <em>Lambda Diagrams</em> as a 2D geometric notation for closed lambda terms. Eliminating arbitrary variable names, computation is exposed directly as circuit routing.</p>
      <h3>1. The Three Primitives</h3>
      <ul>
        <li><strong>Horizontal Lines (─)</strong>: Represent <strong>λ-abstractions (functions)</strong>. Each parameter introduces a horizontal bar spanning across its body scope.</li>
        <li><strong>Vertical Lines (│)</strong>: Represent <strong>bound variables</strong>. Dropping downwards from their binding λ-bar to where they are evaluated. The height corresponds to its De Bruijn index!</li>
        <li><strong>Horizontal Links (─)</strong>: Represent <strong>function applications</strong>, bridging the function output stem with the argument input stem.</li>
      </ul>
      <h3>2. Standard vs. Alternative Style</h3>
      <ul>
        <li><strong>Standard Style</strong>: Applications link the <strong>leftmost variable</strong> of the function, providing a canonical, formal layout.</li>
        <li><strong>Alternative Style</strong>: Applications link the <strong>nearest deepest variable</strong>, yielding compact, organic structures.</li>
      </ul>
      <h3>3. The 5-Phase β-Reduction Process</h3>
      <ul>
        <li><strong>Phase 1 (Redex)</strong>: Target <code>(λx. M) N</code> application bridge is highlighted with a pulse.</li>
        <li><strong>Phase 2 (Variables & Arg)</strong>: Bound variable occurrences in M and argument N are highlighted.</li>
        <li><strong>Phase 3 (Dissolve λ)</strong>: The binding horizontal bar dissolves, opening reception sockets.</li>
        <li><strong>Phase 4 (Substitute)</strong>: Argument N is cloned and substituted into sockets; outer indices are shifted.</li>
        <li><strong>Phase 5 (Settle)</strong>: The new term settles into its canonical Tromp grid layout.</li>
      </ul>
    `,
    aboutHtml: `
      <p><strong>John Tromp</strong> is a renowned computer scientist and mathematician celebrated for work in <em>Algorithmic Information Theory (AIT)</em>, solving <em>Connect Four</em>, and inventing <strong>Binary Lambda Calculus (BLC)</strong>.</p>
      <h3>Binary Lambda Calculus (BLC)</h3>
      <p>BLC is an ultra-minimal universal programming framework:</p>
      <ul>
        <li><code>00</code> = Abstraction (λ)</li>
        <li><code>01</code> = Application</li>
        <li><code>1...10</code> = Variable (unary De Bruijn index)</li>
      </ul>
      <p>Tromp constructed a self-interpreter in only <strong>21 bytes (168 bits)</strong> and a prime sieve in <strong>167 bits</strong>, both visualizable via Lambda Diagrams!</p>
    `
  }
};

function t(key, ...args) {
  const dict = I18N[currentLang] || I18N['zh'];
  const val = dict[key];
  if (typeof val === 'function') {
    return val(...args);
  }
  return val !== undefined ? val : key;
}

function setLanguage(lang) {
  currentLang = lang === 'en' ? 'en' : 'zh';
  document.documentElement.lang = currentLang === 'zh' ? 'zh-CN' : 'en';

  const dict = I18N[currentLang];

  // Header Brand & Controls
  const txtBrandTitle = document.getElementById('txtBrandTitle');
  if (txtBrandTitle) txtBrandTitle.textContent = dict.brandTitle;
  const txtBadge = document.getElementById('txtBadge');
  if (txtBadge) txtBadge.textContent = dict.badge;
  const txtBrandSub = document.getElementById('txtBrandSub');
  if (txtBrandSub) txtBrandSub.textContent = dict.brandSub;
  const langToggleBtn = document.getElementById('langToggleBtn');
  if (langToggleBtn) langToggleBtn.textContent = dict.langToggle;
  const guideModalBtn = document.getElementById('guideModalBtn');
  if (guideModalBtn) guideModalBtn.textContent = dict.guideBtn;
  const aboutModalBtn = document.getElementById('aboutModalBtn');
  if (aboutModalBtn) aboutModalBtn.textContent = dict.aboutBtn;
  const authOpenBtn = document.getElementById('authOpenBtn');
  if (authOpenBtn && !window.currentUser) authOpenBtn.textContent = dict.authBtn;
  const repoOpenBtn = document.getElementById('repoOpenBtn');
  if (repoOpenBtn) repoOpenBtn.textContent = dict.repoBtn;

  // Sidebar Controls
  const lblInputSection = document.getElementById('lblInputSection');
  if (lblInputSection) lblInputSection.textContent = dict.inputSection;
  const tabModeClassic = document.getElementById('tabModeClassic');
  if (tabModeClassic) tabModeClassic.textContent = dict.tabClassic;
  const tabModeDeBruijn = document.getElementById('tabModeDeBruijn');
  if (tabModeDeBruijn) tabModeDeBruijn.textContent = dict.tabDeBruijn;

  const expressionInput = document.getElementById('expressionInput');
  if (expressionInput) {
    const isClassic = tabModeClassic && tabModeClassic.classList.contains('active');
    expressionInput.placeholder = isClassic ? dict.placeholderClassic : dict.placeholderDeBruijn;
  }

  const lblOpenPresets = document.getElementById('lblOpenPresets');
  if (lblOpenPresets) lblOpenPresets.textContent = dict.openPresetsBtn;
  const lblPresetCount = document.getElementById('lblPresetCount');
  if (lblPresetCount) lblPresetCount.textContent = `23 ${dict.presetCountSuffix}`;
  const loadParseBtn = document.getElementById('loadParseBtn');
  if (loadParseBtn) loadParseBtn.textContent = dict.parseBtn;
  const saveToRepoBtn = document.getElementById('saveToRepoBtn');
  if (saveToRepoBtn) saveToRepoBtn.textContent = dict.saveRepoBtn;

  // Stepper Section
  const lblStepperSection = document.getElementById('lblStepperSection');
  if (lblStepperSection) lblStepperSection.textContent = dict.stepperSection;
  const phasePill1 = document.getElementById('phasePill1');
  if (phasePill1) phasePill1.textContent = dict.phasePill1;
  const phasePill2 = document.getElementById('phasePill2');
  if (phasePill2) phasePill2.textContent = dict.phasePill2;
  const phasePill3 = document.getElementById('phasePill3');
  if (phasePill3) phasePill3.textContent = dict.phasePill3;
  const phasePill4 = document.getElementById('phasePill4');
  if (phasePill4) phasePill4.textContent = dict.phasePill4;
  const phasePill5 = document.getElementById('phasePill5');
  if (phasePill5) phasePill5.textContent = dict.phasePill5;

  const lblStrategy = document.getElementById('lblStrategy');
  if (lblStrategy) lblStrategy.textContent = dict.strategyLabel;
  const optNormalOrder = document.getElementById('optNormalOrder');
  if (optNormalOrder) optNormalOrder.textContent = dict.optNormal;
  const optApplicativeOrder = document.getElementById('optApplicativeOrder');
  if (optApplicativeOrder) optApplicativeOrder.textContent = dict.optApplicative;
  const lblSpeed = document.getElementById('lblSpeed');
  if (lblSpeed) lblSpeed.textContent = dict.speedLabel;

  // Code Inspector & History
  const lblCodeSection = document.getElementById('lblCodeSection');
  if (lblCodeSection) lblCodeSection.textContent = dict.codeSection;
  const copyNamedTermBtn = document.getElementById('copyNamedTermBtn');
  if (copyNamedTermBtn) copyNamedTermBtn.textContent = dict.copyCodeBtn;
  const lblNamedNotation = document.getElementById('lblNamedNotation');
  if (lblNamedNotation) lblNamedNotation.textContent = dict.namedLabel;
  const lblDeBruijnNotation = document.getElementById('lblDeBruijnNotation');
  if (lblDeBruijnNotation) lblDeBruijnNotation.textContent = dict.deBruijnLabel;

  const lblHistorySection = document.getElementById('lblHistorySection');
  if (lblHistorySection) lblHistorySection.textContent = dict.historySection;
  const thHistRule = document.getElementById('thHistRule');
  if (thHistRule) thHistRule.textContent = dict.thHistRule;
  const thHistTerm = document.getElementById('thHistTerm');
  if (thHistTerm) thHistTerm.textContent = dict.thHistTerm;

  // Viewport Top Bar
  const tabSvgView = document.getElementById('tabSvgView');
  if (tabSvgView) tabSvgView.textContent = dict.tabSvg;
  const tabBoxCharView = document.getElementById('tabBoxCharView');
  if (tabBoxCharView) tabBoxCharView.textContent = dict.tabBoxChar;
  const tabAsciiView = document.getElementById('tabAsciiView');
  if (tabAsciiView) tabAsciiView.textContent = dict.tabAscii;
  const optStyleStd = document.getElementById('optStyleStd');
  if (optStyleStd) optStyleStd.textContent = dict.optStyleStd;
  const optStyleAlt = document.getElementById('optStyleAlt');
  if (optStyleAlt) optStyleAlt.textContent = dict.optStyleAlt;
  const exportSvgAction = document.getElementById('exportSvgAction');
  if (exportSvgAction) exportSvgAction.textContent = dict.exportSvg;
  const exportPngAction = document.getElementById('exportPngAction');
  if (exportPngAction) exportPngAction.textContent = dict.exportPng;
  const copyMonoTextBtn = document.getElementById('copyMonoTextBtn');
  if (copyMonoTextBtn) copyMonoTextBtn.textContent = dict.copyBoxCharBtn;

  // Presets Modal
  const modalPresetsTitle = document.getElementById('modalPresetsTitle');
  if (modalPresetsTitle) modalPresetsTitle.textContent = dict.modalPresetsTitle;
  const modalPresetsBadge = document.getElementById('modalPresetsBadge');
  if (modalPresetsBadge) modalPresetsBadge.textContent = dict.modalPresetsBadge;
  const presetSearchInput = document.getElementById('presetSearchInput');
  if (presetSearchInput) presetSearchInput.placeholder = dict.searchPresetsPlaceholder;
  const catTabAll = document.getElementById('catTabAll');
  if (catTabAll) catTabAll.textContent = dict.catAll;
  const catTabChurch = document.getElementById('catTabChurch');
  if (catTabChurch) catTabChurch.textContent = dict.presetGroups.church;
  const catTabCore = document.getElementById('catTabCore');
  if (catTabCore) catTabCore.textContent = dict.presetGroups.core;
  const catTabArith = document.getElementById('catTabArith');
  if (catTabArith) catTabArith.textContent = dict.presetGroups.arithmetic;
  const catTabClassics = document.getElementById('catTabClassics');
  if (catTabClassics) catTabClassics.textContent = dict.presetGroups.classics;

  const thSymbol = document.getElementById('thSymbol');
  if (thSymbol) thSymbol.textContent = dict.thSymbol;
  const thExpression = document.getElementById('thExpression');
  if (thExpression) thExpression.textContent = dict.thExpression;
  const thCategory = document.getElementById('thCategory');
  if (thCategory) thCategory.textContent = dict.thCategory;
  const thDescription = document.getElementById('thDescription');
  if (thDescription) thDescription.textContent = dict.thDescription;
  const thAction = document.getElementById('thAction');
  if (thAction) thAction.textContent = dict.thAction;
  const txtModalFooterTip = document.getElementById('txtModalFooterTip');
  if (txtModalFooterTip) txtModalFooterTip.textContent = dict.txtModalFooterTip;

  // Guides & About
  const modalGuideBody = document.getElementById('modalGuideBody');
  if (modalGuideBody) modalGuideBody.innerHTML = dict.guideHtml;
  const modalAboutBody = document.getElementById('modalAboutBody');
  if (modalAboutBody) modalAboutBody.innerHTML = dict.aboutHtml;

  // Notify any active views / tables to re-render localized strings
  window.dispatchEvent(new CustomEvent('languagechange', { detail: { lang: currentLang } }));
}
