/**
 * John Tromp Lambda Diagrams Workstation - Application Coordinator
 * State machine, reduction runner, keyboard navigation, and event wiring.
 */
const appState = {
      initialTerm: null,
      currentTerm: null,
      stepHistory: [],
      currentStepIndex: 0,
      currentPhase: 1,
      currentReductionMeta: null,
      reductionStrategy: 'normal',
      inputMode: 'classic',
      diagramStyle: 'standard',
      showLabels: true,
      smoothAnimation: true,
      activeViewTab: 'svg',
      isPlaying: false,
      playTimer: null,
      playSpeed: 600,
      theme: 'dark'
    };

    // DOM Elements
    const exprInput = document.getElementById('expressionInput');
    const openPresetsModalBtn = document.getElementById('openPresetsModalBtn');
    const presetsModal = document.getElementById('presetsModal');
    const loadParseBtn = document.getElementById('loadParseBtn');
    const syntaxStatus = document.getElementById('syntaxStatus');
    const stepCounterLabel = document.getElementById('stepCounterLabel');
    const narrativeText = document.getElementById('narrativeText');
    const codeNamedContainer = document.getElementById('codeNamedContainer');
    const codeDeBruijnContainer = document.getElementById('codeDeBruijnContainer');
    const stepHistoryTableBody = document.getElementById('stepHistoryTableBody');
    const historyBadgeCount = document.getElementById('historyBadgeCount');
    const strategyDropdown = document.getElementById('strategyDropdown');
    const animSpeedSlider = document.getElementById('animSpeedSlider');
    const speedValueLabel = document.getElementById('speedValueLabel');
    const diagramStyleSelect = document.getElementById('diagramStyleSelect');
    const toggleDiagramLabels = document.getElementById('toggleDiagramLabels');
    const toggleSmoothAnim = document.getElementById('toggleSmoothAnim');
    const monoTitleLabel = document.getElementById('monoTitleLabel');
    const monoDiagramDisplay = document.getElementById('monoDiagramDisplay');
    const svgCanvasContainer = document.getElementById('svgCanvasContainer');
    const monoViewContainer = document.getElementById('monoViewContainer');

    let currentPresetFilter = { search: '', category: 'all' };

    function renderPresetsTable() {
      const tbody = document.getElementById('presetsTableBody');
      if (!tbody) return;

      const q = (currentPresetFilter.search || '').toLowerCase().trim();
      const cat = currentPresetFilter.category;
      const t = I18N[currentLang];

      const filtered = PRESET_DEFINITIONS.filter(p => {
        if (cat !== 'all' && p.group !== cat) return false;
        if (!q) return true;
        const n = (p.name || '').toLowerCase();
        const c = (p.code || '').toLowerCase();
        const dZh = (p.zh_desc || '').toLowerCase();
        const dEn = (p.en_desc || '').toLowerCase();
        return n.includes(q) || c.includes(q) || dZh.includes(q) || dEn.includes(q);
      });

      tbody.innerHTML = '';
      if (filtered.length === 0) {
        const tr = document.createElement('tr');
        tr.innerHTML = `<td colspan="5" style="text-align: center; color: var(--text-dim); padding: 24px;">${currentLang === 'zh' ? '无匹配的预设公式' : 'No matching preset formulas'}</td>`;
        tbody.appendChild(tr);
        return;
      }

      filtered.forEach(preset => {
        const tr = document.createElement('tr');
        tr.className = 'preset-row';
        tr.setAttribute('data-id', preset.id);

        const groupLabel = (t.presetGroups && t.presetGroups[preset.group]) ? t.presetGroups[preset.group] : preset.group;
        const desc = currentLang === 'zh' ? preset.zh_desc : preset.en_desc;

        tr.innerHTML = `
          <td class="preset-cell-name">${preset.name}</td>
          <td class="preset-cell-code"><code>${preset.code}</code></td>
          <td class="preset-cell-cat">${groupLabel}</td>
          <td class="preset-cell-desc">${desc}</td>
          <td style="text-align: right;">
            <button class="btn btn-solid load-preset-action-btn" data-id="${preset.id}" style="font-size: 11px; padding: 2px 8px;">${t.btnLoadPreset}</button>
          </td>
        `;

        tr.addEventListener('click', (e) => {
          selectAndLoadPreset(preset.id);
        });

        tbody.appendChild(tr);
      });
    }

    function selectAndLoadPreset(presetId) {
      const found = PRESET_DEFINITIONS.find(p => p.id === presetId);
      if (found) {
        if (appState.inputMode !== 'classic') {
          appState.inputMode = 'classic';
          document.querySelectorAll('.segmented-tab').forEach(b => {
            b.classList.toggle('active', b.getAttribute('data-input-mode') === 'classic');
          });
        }
        exprInput.value = found.code;
        loadAndParse(found.code);
        document.getElementById('presetsModal').style.display = 'none';
      }
    }

    function updateLanguageUI() {
      const t = I18N[currentLang];
      document.getElementById('txtBrandTitle').textContent = t.brandTitle;
      document.getElementById('txtBadge').textContent = t.badge;
      document.getElementById('txtBrandSub').textContent = t.brandSub;
      document.getElementById('langToggleBtn').textContent = t.langToggle;
      document.getElementById('themeToggleBtn').textContent = appState.theme === 'dark' ? t.themeToggleLight : t.themeToggleDark;
      document.getElementById('guideModalBtn').textContent = t.guideBtn;
      document.getElementById('aboutModalBtn').textContent = t.aboutBtn;
      const repoOpenBtn = document.getElementById('repoOpenBtn');
      if (repoOpenBtn) repoOpenBtn.textContent = t.repoBtn;
      const saveToRepoBtn = document.getElementById('saveToRepoBtn');
      if (saveToRepoBtn) saveToRepoBtn.textContent = t.saveRepoBtn;
      const authOpenBtn = document.getElementById('authOpenBtn');
      if (authOpenBtn && !window.currentUser) authOpenBtn.textContent = t.authBtn;
      document.getElementById('lblInputSection').textContent = t.inputSection;
      document.getElementById('tabModeClassic').textContent = t.tabClassic;
      document.getElementById('tabModeDeBruijn').textContent = t.tabDeBruijn;
      document.getElementById('lblOpenPresets').textContent = t.openPresetsBtn;
      document.getElementById('lblPresetCount').textContent = `${PRESET_DEFINITIONS.length} ${t.presetCountSuffix}`;
      document.getElementById('modalPresetsTitle').textContent = t.modalPresetsTitle;
      document.getElementById('modalPresetsBadge').textContent = t.modalPresetsBadge;
      document.getElementById('presetSearchInput').placeholder = t.searchPresetsPlaceholder;
      document.getElementById('catTabAll').textContent = t.catAll;
      document.getElementById('catTabChurch').textContent = t.presetGroups.church;
      document.getElementById('catTabCore').textContent = t.presetGroups.core;
      document.getElementById('catTabArith').textContent = t.presetGroups.arithmetic;
      document.getElementById('catTabClassics').textContent = t.presetGroups.classics;
      document.getElementById('thSymbol').textContent = t.thSymbol;
      document.getElementById('thExpression').textContent = t.thExpression;
      document.getElementById('thCategory').textContent = t.thCategory;
      document.getElementById('thDescription').textContent = t.thDescription;
      document.getElementById('thAction').textContent = t.thAction;
      document.getElementById('txtModalFooterTip').textContent = t.txtModalFooterTip;
      document.getElementById('loadParseBtn').textContent = t.parseBtn;
      document.getElementById('lblStepperSection').textContent = t.stepperSection;
      document.getElementById('phasePill1').textContent = t.phasePill1;
      document.getElementById('phasePill2').textContent = t.phasePill2;
      document.getElementById('phasePill3').textContent = t.phasePill3;
      document.getElementById('phasePill4').textContent = t.phasePill4;
      document.getElementById('phasePill5').textContent = t.phasePill5;
      document.getElementById('macroResetBtn').title = `${t.macroReset} (R)`;
      document.getElementById('macroResetBtn').setAttribute('aria-label', t.macroReset);
      document.getElementById('macroPrevBtn').title = `${t.macroPrev} (←)`;
      document.getElementById('macroPrevBtn').setAttribute('aria-label', t.macroPrev);
      document.getElementById('microPrevBtn').title = `${t.microPrev}`;
      document.getElementById('microPrevBtn').setAttribute('aria-label', t.microPrev);
      document.getElementById('playPauseBtn').title = `${appState.isPlaying ? t.macroPause : t.macroPlay} (Space)`;
      document.getElementById('playPauseBtn').setAttribute('aria-label', appState.isPlaying ? t.macroPause : t.macroPlay);
      document.getElementById('microNextBtn').title = `${t.microNext} (Shift+→)`;
      document.getElementById('microNextBtn').setAttribute('aria-label', t.microNext);
      document.getElementById('macroNextBtn').title = `${t.macroNext} (→)`;
      document.getElementById('macroNextBtn').setAttribute('aria-label', t.macroNext);
      document.getElementById('reduceNormalFormBtn').title = `${t.reduceNormal}`;
      document.getElementById('reduceNormalFormBtn').setAttribute('aria-label', t.reduceNormal);
      document.getElementById('lblStrategy').textContent = t.strategyLabel;
      document.getElementById('optNormalOrder').textContent = t.optNormal;
      document.getElementById('optApplicativeOrder').textContent = t.optApplicative;
      document.getElementById('lblSpeed').textContent = t.speedLabel;
      document.getElementById('lblCodeSection').textContent = t.codeSection;
      document.getElementById('copyNamedTermBtn').textContent = t.copyCodeBtn;
      document.getElementById('lblNamedNotation').textContent = t.namedLabel;
      document.getElementById('lblDeBruijnNotation').textContent = t.deBruijnLabel;
      document.getElementById('lblHistorySection').textContent = t.historySection;
      const thHistRule = document.getElementById('thHistRule');
      if (thHistRule) thHistRule.textContent = t.thHistRule;
      const thHistTerm = document.getElementById('thHistTerm');
      if (thHistTerm) thHistTerm.textContent = t.thHistTerm;
      document.getElementById('tabSvgView').textContent = t.tabSvg;
      document.getElementById('tabBoxCharView').textContent = t.tabBoxChar;
      document.getElementById('tabAsciiView').textContent = t.tabAscii;
      document.getElementById('optStyleStd').textContent = t.optStyleStd;
      document.getElementById('optStyleAlt').textContent = t.optStyleAlt;
      const lblShowLabels = document.getElementById('lblShowLabels');
      if (lblShowLabels) lblShowLabels.textContent = t.showLabels;
      const lblSmoothAnim = document.getElementById('lblSmoothAnim');
      if (lblSmoothAnim) lblSmoothAnim.textContent = t.smoothAnim;
      document.getElementById('exportSvgAction').textContent = t.exportSvg;
      document.getElementById('exportPngAction').textContent = t.exportPng;
      document.getElementById('copyMonoTextBtn').textContent = t.copyBoxCharBtn;
      document.getElementById('modalGuideTitle').textContent = t.guideBtn;
      document.getElementById('modalGuideBody').innerHTML = t.guideHtml;
      document.getElementById('modalAboutTitle').textContent = t.aboutBtn;
      document.getElementById('modalAboutBody').innerHTML = t.aboutHtml;

      if (appState.inputMode === 'debruijn') {
        exprInput.placeholder = t.placeholderDeBruijn;
      } else {
        exprInput.placeholder = t.placeholderClassic;
      }

      renderPresetsTable();
      syncFullUI();
    }

    function toggleLanguage() {
      currentLang = currentLang === 'zh' ? 'en' : 'zh';
      setLanguage(currentLang);
      updateLanguageUI();
    }

    function toggleTheme() {
      appState.theme = appState.theme === 'dark' ? 'light' : 'dark';
      document.body.setAttribute('data-theme', appState.theme);
      document.getElementById('themeToggleBtn').textContent = appState.theme === 'dark'
        ? I18N[currentLang].themeToggleLight
        : I18N[currentLang].themeToggleDark;
    }

    function loadAndParse(inputStr) {
      const t = I18N[currentLang];
      try {
        let dbTerm;
        if (appState.inputMode === 'debruijn') {
          dbTerm = parseDeBruijnString(inputStr);
        } else {
          const namedAst = parseNamedLambda(inputStr);
          dbTerm = convertNamedToDB(namedAst);
        }

        appState.initialTerm = dbTerm;
        appState.currentTerm = dbTerm;
        appState.stepHistory = [{ term: dbTerm, rule: 'init' }];
        appState.currentStepIndex = 0;
        appState.currentPhase = 1;
        appState.currentReductionMeta = prepareReductionStep(dbTerm, appState.reductionStrategy);

        pausePlayback();
        syntaxStatus.className = 'status-badge status-active';
        syntaxStatus.textContent = t.statusParsed;
        syncFullUI();
      } catch (err) {
        syntaxStatus.className = 'status-badge';
        syntaxStatus.textContent = t.statusError;
        narrativeText.textContent = `${t.statusError}: ${err.message}`;
        codeNamedContainer.innerHTML = `<span style="color: var(--text-dim);">${err.message}</span>`;
        codeDeBruijnContainer.textContent = '─';
      }
    }

    function advanceMicroPhase() {
      const t = I18N[currentLang];
      if (!appState.currentReductionMeta) {
        narrativeText.textContent = t.normalFormNarrative;
        return;
      }

      if (appState.currentPhase < 5) {
        appState.currentPhase++;
        syncFullUI();
      } else {
        advanceMacroStep();
      }
    }

    function rewindMicroPhase() {
      if (appState.currentPhase > 1) {
        appState.currentPhase--;
        syncFullUI();
      } else if (appState.currentStepIndex > 0) {
        rewindMacroStep();
      }
    }

    function advanceMacroStep(onComplete) {
      const cb = (typeof onComplete === 'function') ? onComplete : null;
      if (!appState.currentReductionMeta) {
        if (cb) cb();
        return;
      }

      const prevTerm = appState.currentTerm;
      const meta = appState.currentReductionMeta;
      const nextTerm = meta.termAfter;

      if (!appState.smoothAnimation || appState.activeViewTab !== 'svg') {
        commitCurrentStep();
        if (cb) cb();
        return;
      }

      // Smooth motion animation
      narrativeText.textContent = meta.narratives[currentLang][4];
      animateDiagramTransition(prevTerm, nextTerm, meta, () => {
        commitCurrentStep();
        if (cb) cb();
      });
    }

    function commitCurrentStep() {
      if (!appState.currentReductionMeta) return;

      const nextTerm = appState.currentReductionMeta.termAfter;
      const t = I18N[currentLang];
      appState.stepHistory = appState.stepHistory.slice(0, appState.currentStepIndex + 1);
      appState.stepHistory.push({
        term: nextTerm,
        rule: 'beta'
      });
      appState.currentStepIndex = appState.stepHistory.length - 1;
      appState.currentTerm = nextTerm;
      appState.currentPhase = 1;
      appState.currentReductionMeta = prepareReductionStep(nextTerm, appState.reductionStrategy);

      syncFullUI();
    }

    function rewindMacroStep() {
      if (appState.currentStepIndex <= 0) return;

      const targetIdx = appState.currentStepIndex - 1;
      const prevTerm = appState.stepHistory[targetIdx].term;
      const currTerm = appState.currentTerm;

      if (!appState.smoothAnimation || appState.activeViewTab !== 'svg') {
        appState.currentStepIndex = targetIdx;
        appState.currentTerm = prevTerm;
        appState.currentPhase = 1;
        appState.currentReductionMeta = prepareReductionStep(prevTerm, appState.reductionStrategy);
        syncFullUI();
        return;
      }

      animateDiagramTransition(currTerm, prevTerm, null, () => {
        appState.currentStepIndex = targetIdx;
        appState.currentTerm = prevTerm;
        appState.currentPhase = 1;
        appState.currentReductionMeta = prepareReductionStep(prevTerm, appState.reductionStrategy);
        syncFullUI();
      });
    }

    function resetToInitial() {
      if (appState.stepHistory.length > 0) {
        pausePlayback();
        const initial = appState.stepHistory[0].term;
        const current = appState.currentTerm;

        if (appState.smoothAnimation && appState.activeViewTab === 'svg' && appState.currentStepIndex > 0) {
          animateDiagramTransition(current, initial, null, () => {
            appState.currentStepIndex = 0;
            appState.currentTerm = initial;
            appState.currentPhase = 1;
            appState.currentReductionMeta = prepareReductionStep(initial, appState.reductionStrategy);
            syncFullUI();
          });
        } else {
          appState.currentStepIndex = 0;
          appState.currentTerm = initial;
          appState.currentPhase = 1;
          appState.currentReductionMeta = prepareReductionStep(initial, appState.reductionStrategy);
          syncFullUI();
        }
      }
    }

    function triggerSpecificRedex(redexId) {
      pausePlayback();
      const meta = prepareReductionStep(appState.currentTerm, appState.reductionStrategy, redexId);
      if (!meta) return;
      appState.currentReductionMeta = meta;

      if (!appState.smoothAnimation || appState.activeViewTab !== 'svg') {
        commitCurrentStep();
      } else {
        animateDiagramTransition(appState.currentTerm, meta.termAfter, meta, () => {
          commitCurrentStep();
        });
      }
    }

    function togglePlayback() {
      if (appState.isPlaying) pausePlayback();
      else startPlayback();
    }

    const ICON_PLAY_SVG = `<svg width="15" height="15" viewBox="0 0 16 16" fill="currentColor"><polygon points="4.5,3 13,8 4.5,13"/></svg>`;
    const ICON_PAUSE_SVG = `<svg width="15" height="15" viewBox="0 0 16 16" fill="currentColor"><rect x="4" y="3" width="2.5" height="10" rx="0.5"/><rect x="9.5" y="3" width="2.5" height="10" rx="0.5"/></svg>`;

    function startPlayback() {
      appState.isPlaying = true;
      const playBtn = document.getElementById('playPauseBtn');
      playBtn.innerHTML = ICON_PAUSE_SVG;
      playBtn.title = `${I18N[currentLang].macroPause} (Space)`;
      playBtn.setAttribute('aria-label', I18N[currentLang].macroPause);
      playBtn.classList.add('btn-active');
      runPlaybackLoop();
    }

    function pausePlayback() {
      appState.isPlaying = false;
      const playBtn = document.getElementById('playPauseBtn');
      playBtn.innerHTML = ICON_PLAY_SVG;
      playBtn.title = `${I18N[currentLang].macroPlay} (Space)`;
      playBtn.setAttribute('aria-label', I18N[currentLang].macroPlay);
      playBtn.classList.remove('btn-active');
      if (appState.playTimer) {
        clearTimeout(appState.playTimer);
        appState.playTimer = null;
      }
      syncFullUI();
    }

    function runPlaybackLoop() {
      if (!appState.isPlaying) return;

      if (!appState.currentReductionMeta) {
        pausePlayback();
        narrativeText.textContent = I18N[currentLang].normalFormNarrative;
        return;
      }

      advanceMacroStep(() => {
        if (!appState.isPlaying) return;
        if (!appState.currentReductionMeta) {
          pausePlayback();
          narrativeText.textContent = I18N[currentLang].normalFormNarrative;
          return;
        }
        // Settled pause after animation settles before next step begins
        const pauseDelay = Math.max(120, Math.floor((appState.playSpeed || 600) * 0.3));
        appState.playTimer = setTimeout(runPlaybackLoop, pauseDelay);
      });
    }

    function reduceToNormalForm() {
      pausePlayback();
      let steps = 0;
      const maxSteps = 300;
      const t = I18N[currentLang];

      while (steps < maxSteps) {
        const meta = prepareReductionStep(appState.currentTerm, appState.reductionStrategy);
        if (!meta) break;
        appState.stepHistory.push({
          term: meta.termAfter,
          rule: 'beta'
        });
        appState.currentTerm = meta.termAfter;
        steps++;
      }

      appState.currentStepIndex = appState.stepHistory.length - 1;
      appState.currentPhase = 1;
      appState.currentReductionMeta = prepareReductionStep(appState.currentTerm, appState.reductionStrategy);

      if (steps >= maxSteps) {
        syntaxStatus.className = 'status-badge';
        syntaxStatus.textContent = t.statusLoop;
        narrativeText.textContent = t.loopNarrative;
      }
      syncFullUI();
    }

    function syncFullUI() {
      const t = I18N[currentLang];
      const term = (appState.currentPhase === 5 && appState.currentReductionMeta)
        ? appState.currentReductionMeta.termAfter
        : appState.currentTerm;

      if (!term) return;

      const meta = appState.currentReductionMeta;
      const isAlt = (appState.diagramStyle === 'alt');

      document.querySelectorAll('.phase-pill').forEach(pill => {
        const pNum = parseInt(pill.getAttribute('data-phase'), 10);
        pill.classList.remove('active', 'passed');
        if (pNum === appState.currentPhase) pill.classList.add('active');
        else if (pNum < appState.currentPhase) pill.classList.add('passed');
      });

      if (meta && meta.narratives && meta.narratives[currentLang]) {
        narrativeText.textContent = meta.narratives[currentLang][appState.currentPhase];
      } else {
        narrativeText.textContent = t.normalFormNarrative;
      }

      if (currentLang === 'zh') {
        stepCounterLabel.textContent = `第 ${appState.currentStepIndex} 步 (微阶段 ${appState.currentPhase}/5)`;
      } else {
        stepCounterLabel.textContent = `Step ${appState.currentStepIndex} (Phase ${appState.currentPhase}/5)`;
      }

      if (!meta) {
        syntaxStatus.className = 'status-badge status-active';
        syntaxStatus.textContent = t.statusNormal;
      } else {
        syntaxStatus.className = 'status-badge status-active';
        syntaxStatus.textContent = t.statusRedex;
      }

      const highlights = {
        redexId: (meta && appState.currentPhase <= 3) ? meta.redexId : null,
        targetVarIds: (meta && appState.currentPhase >= 2 && appState.currentPhase <= 4) ? meta.targetVarIds : [],
        argId: (meta && appState.currentPhase >= 2 && appState.currentPhase <= 4) ? meta.argId : null
      };

      const namedAst = dbToNamedAST(term);
      codeNamedContainer.innerHTML = renderNamedHtml(namedAst, 0, highlights);
      codeDeBruijnContainer.innerHTML = renderDeBruijnHtml(term, 0, highlights);

      codeNamedContainer.querySelectorAll('.highlight-redex').forEach(el => {
        el.addEventListener('click', () => {
          const id = parseInt(el.getAttribute('data-node-id'), 10);
          triggerSpecificRedex(id);
        });
      });

      historyBadgeCount.textContent = t.historyBadgeSteps(appState.stepHistory.length);
      if (stepHistoryTableBody) {
        stepHistoryTableBody.innerHTML = '';
        appState.stepHistory.forEach((h, idx) => {
          const tr = document.createElement('tr');
          tr.className = (idx === appState.currentStepIndex) ? 'active' : '';
          const ruleLabel = idx === 0
            ? t.histRuleInit
            : (idx === appState.stepHistory.length - 1 && !appState.currentReductionMeta ? t.histRuleNormal : t.histRuleBeta);
          const termStr = termToPlainString(h.term);

          tr.innerHTML = `
            <td style="text-align: center; color: var(--text-dim);">${idx}</td>
            <td style="font-weight: 500;">${ruleLabel}</td>
            <td class="col-term" title="${termStr}">${termStr}</td>
          `;

          tr.addEventListener('click', () => {
            pausePlayback();
            appState.currentStepIndex = idx;
            appState.currentTerm = appState.stepHistory[idx].term;
            appState.currentPhase = 1;
            appState.currentReductionMeta = prepareReductionStep(appState.currentTerm, appState.reductionStrategy);
            syncFullUI();
          });
          stepHistoryTableBody.appendChild(tr);
        });

        const activeRow = stepHistoryTableBody.querySelector('tr.active');
        if (activeRow) activeRow.scrollIntoView({ block: 'nearest' });
      }

      if (appState.activeViewTab === 'svg') {
        svgCanvasContainer.style.display = 'block';
        monoViewContainer.style.display = 'none';
        renderTrompDiagram(term, {
          alt: isAlt,
          showLabels: appState.showLabels,
          phase: appState.currentPhase,
          reductionMeta: meta
        });
      } else if (appState.activeViewTab === 'boxchar') {
        svgCanvasContainer.style.display = 'none';
        monoViewContainer.style.display = 'flex';
        monoTitleLabel.textContent = t.boxCharTitle;
        monoDiagramDisplay.textContent = generateBoxChar(term, isAlt);
      } else if (appState.activeViewTab === 'ascii') {
        svgCanvasContainer.style.display = 'none';
        monoViewContainer.style.display = 'flex';
        monoTitleLabel.textContent = t.asciiTitle;
        monoDiagramDisplay.textContent = generateAsciiGrid(term, isAlt);
      }
    }
    /**
     * =========================================================================
     * SECTION 8: Viewport Controls (Pan & Zoom)
     * =========================================================================
     */
    const svgArea = document.getElementById('svgCanvasContainer');

    svgArea.addEventListener('mousedown', (e) => {
      if (e.target.closest('.floating-viewport-ctrls') || e.target.closest('.diag-redex-bridge')) return;
      draggingPan = true;
      panAnchor = { x: e.clientX - panZoom.x, y: e.clientY - panZoom.y };
    });

    window.addEventListener('mousemove', (e) => {
      if (!draggingPan) return;
      panZoom.x = e.clientX - panAnchor.x;
      panZoom.y = e.clientY - panAnchor.y;
      updateSceneTransform();
    });

    window.addEventListener('mouseup', () => {
      draggingPan = false;
    });

    svgArea.addEventListener('wheel', (e) => {
      e.preventDefault();
      const zoomFactor = e.deltaY < 0 ? 1.12 : 0.88;
      const newScale = Math.max(0.15, Math.min(6.0, panZoom.scale * zoomFactor));

      const rect = svgArea.getBoundingClientRect();
      const mouseX = e.clientX - rect.left;
      const mouseY = e.clientY - rect.top;

      panZoom.x = mouseX - (mouseX - panZoom.x) * (newScale / panZoom.scale);
      panZoom.y = mouseY - (mouseY - panZoom.y) * (newScale / panZoom.scale);
      panZoom.scale = newScale;
      updateSceneTransform();
    }, { passive: false });

    document.getElementById('ctrlZoomIn').addEventListener('click', () => {
      panZoom.scale = Math.min(6.0, panZoom.scale * 1.25);
      updateSceneTransform();
    });

    document.getElementById('ctrlZoomOut').addEventListener('click', () => {
      panZoom.scale = Math.max(0.15, panZoom.scale * 0.8);
      updateSceneTransform();
    });

    document.getElementById('ctrlResetZoom').addEventListener('click', () => {
      panZoom = { x: 60, y: 50, scale: 1.0 };
      updateSceneTransform();
    });

    document.getElementById('ctrlFitDiagram').addEventListener('click', () => {
      const svg = document.getElementById('primarySvgDiagram');
      const scene = document.getElementById('diagramSceneGroup');
      const bbox = scene.getBBox();
      if (!bbox || bbox.width === 0 || bbox.height === 0) return;

      const containerW = svg.clientWidth || 800;
      const containerH = svg.clientHeight || 600;
      const pad = 60;

      const scaleX = (containerW - pad * 2) / bbox.width;
      const scaleY = (containerH - pad * 2) / bbox.height;
      const fitScale = Math.max(0.2, Math.min(2.5, Math.min(scaleX, scaleY)));

      panZoom.scale = fitScale;
      panZoom.x = pad - bbox.x * fitScale + (containerW - pad * 2 - bbox.width * fitScale) / 2;
      panZoom.y = pad - bbox.y * fitScale + (containerH - pad * 2 - bbox.height * fitScale) / 2;
      updateSceneTransform();
    });

    /**
     * =========================================================================
     * SECTION 9: Export Handlers
     * =========================================================================
     */
    document.getElementById('exportSvgAction').addEventListener('click', () => {
      const svg = document.getElementById('primarySvgDiagram');
      const serializer = new XMLSerializer();
      let source = serializer.serializeToString(svg);

      if (!source.match(/^<svg[^>]+xmlns="http\:\/\/www\.w3\.org\/2000\/svg"/)) {
        source = source.replace(/^<svg/, '<svg xmlns="http://www.w3.org/2000/svg"');
      }

      const blob = new Blob([source], { type: 'image/svg+xml;charset=utf-8' });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = `tromp_diagram_${Date.now()}.svg`;
      a.click();
      URL.revokeObjectURL(url);
    });

    document.getElementById('exportPngAction').addEventListener('click', () => {
      const svg = document.getElementById('primarySvgDiagram');
      const serializer = new XMLSerializer();
      const source = serializer.serializeToString(svg);
      const blob = new Blob([source], { type: 'image/svg+xml;charset=utf-8' });
      const url = URL.createObjectURL(blob);
      const img = new Image();

      img.onload = () => {
        const canvas = document.createElement('canvas');
        const scale = 2;
        canvas.width = (svg.clientWidth || 1000) * scale;
        canvas.height = (svg.clientHeight || 700) * scale;
        const ctx = canvas.getContext('2d');
        ctx.fillStyle = appState.theme === 'dark' ? '#000000' : '#ffffff';
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        ctx.scale(scale, scale);
        ctx.drawImage(img, 0, 0);
        URL.revokeObjectURL(url);

        const a = document.createElement('a');
        a.href = canvas.toDataURL('image/png');
        a.download = `tromp_diagram_${Date.now()}.png`;
        a.click();
      };
      img.src = url;
    });

    document.getElementById('copyMonoTextBtn').addEventListener('click', () => {
      const txt = monoDiagramDisplay.textContent;
      navigator.clipboard.writeText(txt).then(() => {
        const btn = document.getElementById('copyMonoTextBtn');
        const orig = btn.textContent;
        btn.textContent = I18N[currentLang].copiedToast;
        setTimeout(() => { btn.textContent = orig; }, 1500);
      });
    });

    document.getElementById('copyNamedTermBtn').addEventListener('click', () => {
      const txt = codeNamedContainer.textContent;
      navigator.clipboard.writeText(txt).then(() => {
        const btn = document.getElementById('copyNamedTermBtn');
        const orig = btn.textContent;
        btn.textContent = I18N[currentLang].copiedToast;
        setTimeout(() => { btn.textContent = orig; }, 1500);
      });
    });

    /**
     * =========================================================================
     * SECTION 10: Event Bindings & Initializer
     * =========================================================================
     */
    loadParseBtn.addEventListener('click', () => loadAndParse(exprInput.value));

    exprInput.addEventListener('keydown', (e) => {
      if (e.key === 'Enter' && (e.ctrlKey || e.metaKey)) {
        loadAndParse(exprInput.value);
      }
    });

    openPresetsModalBtn.addEventListener('click', () => {
      presetsModal.style.display = 'flex';
      renderPresetsTable();
      const input = document.getElementById('presetSearchInput');
      if (input) {
        input.focus();
        input.select();
      }
    });

    const presetSearchInput = document.getElementById('presetSearchInput');
    if (presetSearchInput) {
      presetSearchInput.addEventListener('input', (e) => {
        currentPresetFilter.search = e.target.value;
        renderPresetsTable();
      });
    }

    document.querySelectorAll('.preset-cat-tab').forEach(tab => {
      tab.addEventListener('click', () => {
        document.querySelectorAll('.preset-cat-tab').forEach(t => t.classList.remove('active'));
        tab.classList.add('active');
        currentPresetFilter.category = tab.getAttribute('data-category');
        renderPresetsTable();
      });
    });

    document.querySelectorAll('.segmented-tab').forEach(btn => {
      btn.addEventListener('click', () => {
        document.querySelectorAll('.segmented-tab').forEach(b => b.classList.remove('active'));
        btn.classList.add('active');
        appState.inputMode = btn.getAttribute('data-input-mode');
        const t = I18N[currentLang];
        exprInput.placeholder = appState.inputMode === 'debruijn' ? t.placeholderDeBruijn : t.placeholderClassic;
      });
    });

    document.querySelectorAll('.phase-pill').forEach(pill => {
      pill.addEventListener('click', () => {
        appState.currentPhase = parseInt(pill.getAttribute('data-phase'), 10);
        syncFullUI();
      });
    });

    document.getElementById('microNextBtn').addEventListener('click', advanceMicroPhase);
    document.getElementById('microPrevBtn').addEventListener('click', rewindMicroPhase);
    document.getElementById('macroNextBtn').addEventListener('click', advanceMacroStep);
    document.getElementById('macroPrevBtn').addEventListener('click', rewindMacroStep);
    document.getElementById('macroResetBtn').addEventListener('click', resetToInitial);
    document.getElementById('playPauseBtn').addEventListener('click', togglePlayback);
    document.getElementById('reduceNormalFormBtn').addEventListener('click', reduceToNormalForm);

    animSpeedSlider.addEventListener('input', (e) => {
      appState.playSpeed = parseInt(e.target.value, 10);
      speedValueLabel.textContent = `${appState.playSpeed}ms`;
    });

    strategyDropdown.addEventListener('change', (e) => {
      appState.reductionStrategy = e.target.value;
      appState.currentReductionMeta = prepareReductionStep(appState.currentTerm, appState.reductionStrategy);
      syncFullUI();
    });

    diagramStyleSelect.addEventListener('change', (e) => {
      appState.diagramStyle = e.target.value;
      syncFullUI();
    });

    if (toggleDiagramLabels) {
      toggleDiagramLabels.addEventListener('change', (e) => {
        appState.showLabels = e.target.checked;
        syncFullUI();
      });
    }

    if (toggleSmoothAnim) {
      toggleSmoothAnim.addEventListener('change', (e) => {
        appState.smoothAnimation = e.target.checked;
      });
    }

    document.getElementById('langToggleBtn').addEventListener('click', toggleLanguage);
    document.getElementById('themeToggleBtn').addEventListener('click', toggleTheme);

    document.querySelectorAll('.view-tab').forEach(tab => {
      tab.addEventListener('click', () => {
        document.querySelectorAll('.view-tab').forEach(t => t.classList.remove('active'));
        tab.classList.add('active');
        appState.activeViewTab = tab.getAttribute('data-tab');
        syncFullUI();
      });
    });

    document.querySelectorAll('.symbol-key').forEach(key => {
      key.addEventListener('click', () => {
        const sym = key.getAttribute('data-sym');
        const start = exprInput.selectionStart;
        const end = exprInput.selectionEnd;
        exprInput.value = exprInput.value.slice(0, start) + sym + exprInput.value.slice(end);
        exprInput.focus();
        exprInput.selectionStart = exprInput.selectionEnd = start + sym.length;
      });
    });

    document.getElementById('guideModalBtn').addEventListener('click', () => {
      document.getElementById('visualGuideModal').style.display = 'flex';
    });
    document.getElementById('aboutModalBtn').addEventListener('click', () => {
      document.getElementById('aboutTrompModal').style.display = 'flex';
    });
    document.querySelectorAll('.close-modal-btn, .modal-backdrop').forEach(el => {
      el.addEventListener('click', (e) => {
        if (e.target === el || el.classList.contains('close-modal-btn')) {
          document.querySelectorAll('.modal-backdrop').forEach(m => m.style.display = 'none');
        }
      });
    });

    window.addEventListener('keydown', (e) => {
      if (e.key === 'Escape') {
        document.querySelectorAll('.modal-backdrop').forEach(m => m.style.display = 'none');
        return;
      }
      if (document.activeElement === exprInput || document.activeElement === document.getElementById('presetSearchInput')) return;
      if (e.key === ' ') {
        e.preventDefault();
        togglePlayback();
      } else if (e.key === 'ArrowRight') {
        if (e.shiftKey) advanceMicroPhase();
        else advanceMacroStep();
      } else if (e.key === 'ArrowLeft') {
        if (e.shiftKey) rewindMicroPhase();
        else rewindMacroStep();
      } else if (e.key === 'r' || e.key === 'R') {
        resetToInitial();
      } else if (e.key === 'f' || e.key === 'F') {
        document.getElementById('ctrlFitDiagram').click();
      }
    });

    // Boot & Initialize
    window.addEventListener('DOMContentLoaded', async () => {
      // Test server connectivity
      await api.ping();

      // Initialize Auth and Repository Controllers
      if (typeof authCtrl !== 'undefined') {
        authCtrl.init();
      }
      if (typeof repoCtrl !== 'undefined') {
        repoCtrl.init();
      }

      updateLanguageUI();
      const defaultPreset = PRESET_DEFINITIONS.find(p => p.id === 'TWO');
      if (defaultPreset) {
        exprInput.value = defaultPreset.code;
        loadAndParse(defaultPreset.code);
      }
      updateSceneTransform();
    });