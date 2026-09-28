/**
 * John Tromp Lambda Diagrams Workstation - SVG Diagram Renderer & Motion Morphing Engine
 * Minimalist geometric SVG rendering, axis-aligned growth/retraction tweens, and hover tooltips.
 */
let panZoom = { x: 70, y: 60, scale: 1.0 };
    let draggingPan = false;
    let panAnchor = { x: 0, y: 0 };
    let activeAnimationRaf = null;

    const DIAG_STYLE = {
      cellW: 46,
      cellH: 44,
      offsetX: 60,
      offsetY: 60,
      serif: 14
    };

    function getElementCoords(el) {
      const { cellW, cellH, offsetX, offsetY, serif } = DIAG_STYLE;
      if (el.type === 'abstraction') {
        const yPx = offsetY + el.y * cellH;
        return {
          x1: offsetX + el.x1 * cellW - serif,
          y1: yPx,
          x2: offsetX + el.x2 * cellW + serif,
          y2: yPx
        };
      }
      if (el.type === 'variable') {
        const xPx = offsetX + el.col * cellW;
        return {
          x1: xPx,
          y1: offsetY + el.y1 * cellH,
          x2: xPx,
          y2: offsetY + el.y2 * cellH
        };
      }
      if (el.type === 'application') {
        const yPx = offsetY + el.y * cellH;
        return {
          x1: offsetX + el.x1 * cellW,
          y1: yPx,
          x2: offsetX + el.x2 * cellW,
          y2: yPx
        };
      }
      return { x1: 0, y1: 0, x2: 0, y2: 0 };
    }

    function renderTrompDiagram(term, options = {}) {
      if (activeAnimationRaf) {
        cancelAnimationFrame(activeAnimationRaf);
        activeAnimationRaf = null;
      }

      const {
        alt = false,
        showLabels = true,
        phase = 1,
        reductionMeta = null,
        hoverNodeId = null,
        isAutoPlaying = appState.isPlaying
      } = options;

      const scene = document.getElementById('diagramSceneGroup');
      scene.innerHTML = '';
      if (!term) return;

      const layout = computeTrompLayout(term, alt);
      const linkLayer = document.createElementNS('http://www.w3.org/2000/svg', 'g');
      const varLayer = document.createElementNS('http://www.w3.org/2000/svg', 'g');
      const absLayer = document.createElementNS('http://www.w3.org/2000/svg', 'g');
      const effectLayer = document.createElementNS('http://www.w3.org/2000/svg', 'g');
      const labelLayer = document.createElementNS('http://www.w3.org/2000/svg', 'g');

      scene.appendChild(linkLayer);
      scene.appendChild(varLayer);
      scene.appendChild(absLayer);
      scene.appendChild(effectLayer);
      scene.appendChild(labelLayer);

      const redexId = reductionMeta ? reductionMeta.redexId : null;
      const lamId = reductionMeta ? reductionMeta.lamId : null;
      const targetVarIds = reductionMeta ? (reductionMeta.targetVarIds || []) : [];
      const argNodeIds = reductionMeta ? (reductionMeta.argNodeIds || []) : [];
      const redexNodeIds = reductionMeta ? (reductionMeta.redexNodeIds || []) : [];
      const paramName = reductionMeta ? (reductionMeta.paramName || 'x') : 'x';
      const argSummary = reductionMeta ? (reductionMeta.argSummary || 'N') : 'N';

      function getBoundingBox(els) {
        if (!els || els.length === 0) return null;
        let minX = Infinity, minY = Infinity, maxX = -Infinity, maxY = -Infinity;
        els.forEach(el => {
          const c = getElementCoords(el);
          minX = Math.min(minX, c.x1, c.x2);
          maxX = Math.max(maxX, c.x1, c.x2);
          minY = Math.min(minY, c.y1, c.y2);
          maxY = Math.max(maxY, c.y1, c.y2);
        });
        if (minX === Infinity) return null;
        return { minX, minY, maxX, maxY, width: maxX - minX, height: maxY - minY };
      }

      const redexEls = redexNodeIds.length > 0 ? layout.elements.filter(e => redexNodeIds.includes(e.nodeId)) : [];
      const argEls = argNodeIds.length > 0 ? layout.elements.filter(e => argNodeIds.includes(e.nodeId)) : [];
      const redexBox = getBoundingBox(redexEls);
      const argBox = getBoundingBox(argEls);

      // 1. Abstractions
      layout.elements.filter(e => e.type === 'abstraction').forEach(abs => {
        const coords = getElementCoords(abs);
        let cls = 'diag-abs';
        if (hoverNodeId === abs.id) cls += ' diag-highlighted';
        if (hoverNodeId != null && hoverNodeId !== abs.id) cls += ' diag-dimmed';

        if (reductionMeta && !isAutoPlaying) {
          if (phase === 1 && !redexNodeIds.includes(abs.nodeId)) cls += ' diag-dimmed';
          if (phase === 2 && !redexNodeIds.includes(abs.nodeId)) cls += ' diag-dimmed';
          if (phase === 3 && abs.id === lamId) cls += ' diag-dissolving';
          if (phase === 4 && abs.id === lamId) return; // Completely dissolved in Phase 4
        }

        const line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
        line.setAttribute('x1', coords.x1);
        line.setAttribute('y1', coords.y1);
        line.setAttribute('x2', coords.x2);
        line.setAttribute('y2', coords.y2);
        line.setAttribute('class', cls);
        line.setAttribute('data-id', abs.id);
        line.setAttribute('data-type', 'abs');
        line.setAttribute('data-info', `λ${abs.param} (Depth ${abs.y})`);
        absLayer.appendChild(line);

        if (showLabels && !(reductionMeta && !isAutoPlaying && phase === 4 && abs.id === lamId)) {
          const txt = document.createElementNS('http://www.w3.org/2000/svg', 'text');
          txt.setAttribute('x', coords.x1 - 6);
          txt.setAttribute('y', coords.y1 + 4);
          txt.setAttribute('text-anchor', 'end');
          txt.setAttribute('class', 'diag-label');
          txt.textContent = `λ${abs.param}`;
          labelLayer.appendChild(txt);
        }
      });

      // 2. Variables
      layout.elements.filter(e => e.type === 'variable').forEach(v => {
        const coords = getElementCoords(v);
        let cls = 'diag-var';
        const isTargetVar = targetVarIds.includes(v.id);
        const isArgVar = argNodeIds.includes(v.nodeId);

        if (reductionMeta && !isAutoPlaying) {
          if (phase === 1 && !redexNodeIds.includes(v.nodeId)) cls += ' diag-dimmed';
          if (phase === 2) {
            if (isTargetVar) cls += ' diag-target-var';
            else if (isArgVar) cls += ' diag-highlighted';
            else if (!redexNodeIds.includes(v.nodeId)) cls += ' diag-dimmed';
          }
          if (phase === 3) {
            if (isTargetVar) cls += ' diag-target-var';
            else if (!redexNodeIds.includes(v.nodeId)) cls += ' diag-dimmed';
          }
          if (phase === 4) {
            if (isTargetVar) cls += ' diag-target-var';
          }
        }

        if (hoverNodeId === v.id) cls += ' diag-highlighted';
        else if (hoverNodeId != null && hoverNodeId !== v.id) cls += ' diag-dimmed';

        const line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
        line.setAttribute('x1', coords.x1);
        line.setAttribute('y1', coords.y1);
        line.setAttribute('x2', coords.x2);
        line.setAttribute('y2', coords.y2);
        line.setAttribute('class', cls);
        line.setAttribute('data-id', v.id);
        line.setAttribute('data-type', 'var');
        line.setAttribute('data-info', `${v.name} (Index ${v.index + 1})`);
        varLayer.appendChild(line);

        // Junction dot at binding abstraction (detached in Phase 3 & 4 for target variables)
        const showDot = !(reductionMeta && !isAutoPlaying && (phase === 3 || phase === 4) && isTargetVar);
        if (showDot) {
          const dot = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
          dot.setAttribute('cx', coords.x1);
          dot.setAttribute('cy', coords.y1);
          dot.setAttribute('r', 4.5);
          dot.setAttribute('class', 'diag-dot');
          varLayer.appendChild(dot);
        }

        if (showLabels) {
          const lbl = document.createElementNS('http://www.w3.org/2000/svg', 'text');
          lbl.setAttribute('x', coords.x1);
          lbl.setAttribute('y', coords.y2 + 15);
          lbl.setAttribute('text-anchor', 'middle');
          lbl.setAttribute('class', 'diag-label');
          lbl.setAttribute('font-size', '10px');
          lbl.textContent = v.name;
          labelLayer.appendChild(lbl);
        }
      });

      // 3. Applications
      layout.elements.filter(e => e.type === 'application').forEach(app => {
        const coords = getElementCoords(app);
        const isCurrentRedex = (app.id === redexId);
        const isArgApp = argNodeIds.includes(app.nodeId);

        let bridgeCls = 'diag-app';
        if (reductionMeta) {
          if (isCurrentRedex) {
            bridgeCls += ' diag-redex-bridge';
          } else if (!isAutoPlaying) {
            if (phase === 1 || phase === 2 || phase === 3) {
              if (isArgApp) bridgeCls += ' diag-highlighted';
              else if (!redexNodeIds.includes(app.nodeId)) bridgeCls += ' diag-dimmed';
            } else if (phase === 4) {
              // Nothing extra
            }
          }
        }

        if (hoverNodeId === app.id) bridgeCls += ' diag-highlighted';
        else if (hoverNodeId != null && hoverNodeId !== app.id) bridgeCls += ' diag-dimmed';

        const bridge = document.createElementNS('http://www.w3.org/2000/svg', 'line');
        bridge.setAttribute('x1', coords.x1);
        bridge.setAttribute('y1', coords.y1);
        bridge.setAttribute('x2', coords.x2);
        bridge.setAttribute('y2', coords.y2);
        bridge.setAttribute('class', bridgeCls);
        bridge.setAttribute('data-id', app.id);
        bridge.setAttribute('data-is-redex', app.isRedex);
        bridge.setAttribute('data-type', 'app');
        bridge.setAttribute('data-info', app.isRedex ? I18N[currentLang].tooltipRedex : I18N[currentLang].tooltipApp);
        linkLayer.appendChild(bridge);
      });

      // 4. Distinct 5-Phase Visual System (suppressed during continuous auto-play to avoid strobe flashing)
      if (reductionMeta && !isAutoPlaying) {
        // --- PHASE 1: 锁定红基 ---
        if (phase === 1 && redexBox) {
          const frame = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
          frame.setAttribute('x', redexBox.minX - 12);
          frame.setAttribute('y', redexBox.minY - 14);
          frame.setAttribute('width', redexBox.width + 24);
          frame.setAttribute('height', redexBox.height + 26);
          frame.setAttribute('class', 'phase-spotlight-frame');
          effectLayer.appendChild(frame);

          const redexEl = layout.elements.find(e => e.id === redexId);
          if (redexEl) {
            const rCoords = getElementCoords(redexEl);
            const badgeW = 160;
            const badgeH = 18;
            const bx = (rCoords.x1 + rCoords.x2) / 2 - badgeW / 2;
            const by = rCoords.y1 - 22;

            const bgRect = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
            bgRect.setAttribute('x', bx);
            bgRect.setAttribute('y', by);
            bgRect.setAttribute('width', badgeW);
            bgRect.setAttribute('height', badgeH);
            bgRect.setAttribute('fill', 'var(--text)');
            bgRect.setAttribute('rx', '3');
            effectLayer.appendChild(bgRect);

            const bTxt = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            bTxt.setAttribute('x', bx + badgeW / 2);
            bTxt.setAttribute('y', by + 13);
            bTxt.setAttribute('text-anchor', 'middle');
            bTxt.setAttribute('fill', 'var(--bg-base)');
            bTxt.setAttribute('font-family', 'var(--font-code)');
            bTxt.setAttribute('font-size', '10px');
            bTxt.setAttribute('font-weight', '600');
            bTxt.textContent = `REDEX: ((λ${paramName}. M) ${argSummary})`;
            effectLayer.appendChild(bTxt);
          }
        }

        // --- PHASE 2: 关联形参与实参 ---
        else if (phase === 2) {
          if (argBox) {
            const argFrame = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
            argFrame.setAttribute('x', argBox.minX - 8);
            argFrame.setAttribute('y', argBox.minY - 12);
            argFrame.setAttribute('width', Math.max(argBox.width + 16, 80));
            argFrame.setAttribute('height', argBox.height + 22);
            argFrame.setAttribute('class', 'phase-arg-box');
            effectLayer.appendChild(argFrame);

            const tagW = 110;
            const tagH = 18;
            const tx = argBox.minX - 6;
            const ty = argBox.minY - 26;
            const tagRect = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
            tagRect.setAttribute('x', tx);
            tagRect.setAttribute('y', ty);
            tagRect.setAttribute('width', tagW);
            tagRect.setAttribute('height', tagH);
            tagRect.setAttribute('fill', 'var(--text)');
            tagRect.setAttribute('rx', '3');
            effectLayer.appendChild(tagRect);

            const tagTxt = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            tagTxt.setAttribute('x', tx + tagW / 2);
            tagTxt.setAttribute('y', ty + 12);
            tagTxt.setAttribute('text-anchor', 'middle');
            tagTxt.setAttribute('fill', 'var(--bg-base)');
            tagTxt.setAttribute('font-family', 'var(--font-code)');
            tagTxt.setAttribute('font-size', '10px');
            tagTxt.setAttribute('font-weight', '600');
            tagTxt.textContent = `实参: [${argSummary}]`;
            effectLayer.appendChild(tagTxt);

            const targetEls = layout.elements.filter(e => targetVarIds.includes(e.id));
            if (targetEls.length > 0) {
              targetEls.forEach(tv => {
                const c = getElementCoords(tv);
                const startX = argBox.minX;
                const startY = (argBox.minY + argBox.maxY) / 2;
                const endX = c.x1;
                const endY = c.y2;
                const midX = (startX + endX) / 2;

                const path = document.createElementNS('http://www.w3.org/2000/svg', 'path');
                path.setAttribute('d', `M ${startX} ${startY} C ${midX} ${startY - 15}, ${midX} ${endY}, ${endX} ${endY}`);
                path.setAttribute('class', 'phase-flow-trace');
                effectLayer.appendChild(path);

                const beacon = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
                beacon.setAttribute('cx', endX);
                beacon.setAttribute('cy', endY);
                beacon.setAttribute('r', 5);
                beacon.setAttribute('class', 'phase-socket-outer');
                effectLayer.appendChild(beacon);
              });
            } else {
              const warnTag = document.createElementNS('http://www.w3.org/2000/svg', 'text');
              warnTag.setAttribute('x', argBox.minX + 4);
              warnTag.setAttribute('y', argBox.maxY + 16);
              warnTag.setAttribute('fill', 'var(--text-dim)');
              warnTag.setAttribute('font-family', 'var(--font-code)');
              warnTag.setAttribute('font-size', '10px');
              warnTag.textContent = `(0 处形参引用 · 实参将直接丢弃)`;
              effectLayer.appendChild(warnTag);
            }
          }
        }

        // --- PHASE 3: 溶解抽象与开启插槽 ---
        else if (phase === 3) {
          const lamEl = layout.elements.find(e => e.id === lamId);
          if (lamEl) {
            const lCoords = getElementCoords(lamEl);
            const midX = (lCoords.x1 + lCoords.x2) / 2;
            const tagW = 120;
            const tagH = 18;
            const rx = midX - tagW / 2;
            const ry = lCoords.y1 - 10;

            const dRect = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
            dRect.setAttribute('x', rx);
            dRect.setAttribute('y', ry);
            dRect.setAttribute('width', tagW);
            dRect.setAttribute('height', tagH);
            dRect.setAttribute('fill', 'var(--bg-base)');
            dRect.setAttribute('stroke', 'var(--text)');
            dRect.setAttribute('stroke-dasharray', '2 2');
            dRect.setAttribute('rx', '3');
            effectLayer.appendChild(dRect);

            const dTxt = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            dTxt.setAttribute('x', midX);
            dTxt.setAttribute('y', ry + 12);
            dTxt.setAttribute('text-anchor', 'middle');
            dTxt.setAttribute('fill', 'var(--text)');
            dTxt.setAttribute('font-family', 'var(--font-code)');
            dTxt.setAttribute('font-size', '10px');
            dTxt.textContent = `λ${paramName} 横梁溶解`;
            effectLayer.appendChild(dTxt);
          }

          const targetEls = layout.elements.filter(e => targetVarIds.includes(e.id));
          targetEls.forEach(tv => {
            const c = getElementCoords(tv);

            const outerCircle = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
            outerCircle.setAttribute('cx', c.x2);
            outerCircle.setAttribute('cy', c.y2);
            outerCircle.setAttribute('r', 8);
            outerCircle.setAttribute('class', 'phase-socket-outer');
            effectLayer.appendChild(outerCircle);

            const innerCircle = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
            innerCircle.setAttribute('cx', c.x2);
            innerCircle.setAttribute('cy', c.y2);
            innerCircle.setAttribute('r', 3);
            innerCircle.setAttribute('class', 'phase-socket-inner');
            effectLayer.appendChild(innerCircle);

            const socketLbl = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            socketLbl.setAttribute('x', c.x2 + 12);
            socketLbl.setAttribute('y', c.y2 + 4);
            socketLbl.setAttribute('fill', 'var(--text)');
            socketLbl.setAttribute('font-family', 'var(--font-code)');
            socketLbl.setAttribute('font-size', '10px');
            socketLbl.textContent = `[插槽: ${paramName}]`;
            effectLayer.appendChild(socketLbl);
          });
        }

        // --- PHASE 4: 执行代换接入 ---
        else if (phase === 4) {
          const targetEls = layout.elements.filter(e => targetVarIds.includes(e.id));
          if (targetEls.length > 0) {
            targetEls.forEach(tv => {
              const c = getElementCoords(tv);
              const g = document.createElementNS('http://www.w3.org/2000/svg', 'g');

              const stub = document.createElementNS('http://www.w3.org/2000/svg', 'line');
              stub.setAttribute('x1', c.x2);
              stub.setAttribute('y1', c.y2);
              stub.setAttribute('x2', c.x2);
              stub.setAttribute('y2', c.y2 + 10);
              stub.setAttribute('stroke', 'var(--text)');
              stub.setAttribute('stroke-width', '2.5');
              g.appendChild(stub);

              const blockW = 100;
              const blockH = 22;
              const bx = c.x2 - blockW / 2;
              const by = c.y2 + 10;

              const block = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
              block.setAttribute('x', bx);
              block.setAttribute('y', by);
              block.setAttribute('width', blockW);
              block.setAttribute('height', blockH);
              block.setAttribute('class', 'phase-docked-block');
              g.appendChild(block);

              const bTxt = document.createElementNS('http://www.w3.org/2000/svg', 'text');
              bTxt.setAttribute('x', c.x2);
              bTxt.setAttribute('y', by + 15);
              bTxt.setAttribute('text-anchor', 'middle');
              bTxt.setAttribute('fill', 'var(--text)');
              bTxt.setAttribute('font-family', 'var(--font-code)');
              bTxt.setAttribute('font-size', '10px');
              bTxt.setAttribute('font-weight', '600');
              bTxt.textContent = `代入 [${argSummary}]`;
              g.appendChild(bTxt);

              effectLayer.appendChild(g);
            });
          } else if (argBox) {
            const g = document.createElementNS('http://www.w3.org/2000/svg', 'g');

            const l1 = document.createElementNS('http://www.w3.org/2000/svg', 'line');
            l1.setAttribute('x1', argBox.minX - 4);
            l1.setAttribute('y1', argBox.minY - 4);
            l1.setAttribute('x2', argBox.maxX + 4);
            l1.setAttribute('y2', argBox.maxY + 4);
            l1.setAttribute('stroke', 'var(--text)');
            l1.setAttribute('stroke-width', '2');
            g.appendChild(l1);

            const l2 = document.createElementNS('http://www.w3.org/2000/svg', 'line');
            l2.setAttribute('x1', argBox.maxX + 4);
            l2.setAttribute('y1', argBox.minY - 4);
            l2.setAttribute('x2', argBox.minX - 4);
            l2.setAttribute('y2', argBox.maxY + 4);
            l2.setAttribute('stroke', 'var(--text)');
            l2.setAttribute('stroke-width', '2');
            g.appendChild(l2);

            const midX = (argBox.minX + argBox.maxX) / 2;
            const midY = (argBox.minY + argBox.maxY) / 2;
            const tagW = 140;
            const tagH = 20;

            const dRect = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
            dRect.setAttribute('x', midX - tagW / 2);
            dRect.setAttribute('y', midY - tagH / 2);
            dRect.setAttribute('width', tagW);
            dRect.setAttribute('height', tagH);
            dRect.setAttribute('fill', 'var(--text)');
            dRect.setAttribute('rx', '3');
            g.appendChild(dRect);

            const dTxt = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            dTxt.setAttribute('x', midX);
            dTxt.setAttribute('y', midY + 4);
            dTxt.setAttribute('text-anchor', 'middle');
            dTxt.setAttribute('fill', 'var(--bg-base)');
            dTxt.setAttribute('font-family', 'var(--font-code)');
            dTxt.setAttribute('font-size', '10px');
            dTxt.setAttribute('font-weight', '600');
            dTxt.textContent = `实参已丢弃 (K 组合子)`;
            g.appendChild(dTxt);

            effectLayer.appendChild(g);
          }
        }

        // --- PHASE 5: 最终项整流 ---
        else if (phase === 5) {
          const settleBadge = document.createElementNS('http://www.w3.org/2000/svg', 'g');
          const bw = 170;
          const bh = 22;
          const bx = 16;
          const by = 16;

          const sRect = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
          sRect.setAttribute('x', bx);
          sRect.setAttribute('y', by);
          sRect.setAttribute('width', bw);
          sRect.setAttribute('height', bh);
          sRect.setAttribute('fill', 'var(--bg-subpanel)');
          sRect.setAttribute('stroke', 'var(--border)');
          sRect.setAttribute('rx', '3');
          settleBadge.appendChild(sRect);

          const sTxt = document.createElementNS('http://www.w3.org/2000/svg', 'text');
          sTxt.setAttribute('x', bx + bw / 2);
          sTxt.setAttribute('y', by + 15);
          sTxt.setAttribute('text-anchor', 'middle');
          sTxt.setAttribute('fill', 'var(--text)');
          sTxt.setAttribute('font-family', 'var(--font-code)');
          sTxt.setAttribute('font-size', '10px');
          sTxt.setAttribute('font-weight', '600');
          sTxt.textContent = `β-规约完成 · 几何整流归位`;
          settleBadge.appendChild(sTxt);

          effectLayer.appendChild(settleBadge);
        }
      }

      attachSVGInteractiveEvents();
    }

    function animateDiagramTransition(termBefore, termAfter, meta, onComplete) {
      if (activeAnimationRaf) {
        cancelAnimationFrame(activeAnimationRaf);
        activeAnimationRaf = null;
      }

      const alt = (appState.diagramStyle === 'alt');
      const layoutBefore = computeTrompLayout(termBefore, alt);
      const layoutAfter = computeTrompLayout(termAfter, alt);

      const scene = document.getElementById('diagramSceneGroup');
      scene.innerHTML = '';

      const animLinkLayer = document.createElementNS('http://www.w3.org/2000/svg', 'g');
      const animVarLayer = document.createElementNS('http://www.w3.org/2000/svg', 'g');
      const animAbsLayer = document.createElementNS('http://www.w3.org/2000/svg', 'g');
      const animEffectLayer = document.createElementNS('http://www.w3.org/2000/svg', 'g');
      const animLabelLayer = document.createElementNS('http://www.w3.org/2000/svg', 'g');

      scene.appendChild(animLinkLayer);
      scene.appendChild(animVarLayer);
      scene.appendChild(animAbsLayer);
      scene.appendChild(animEffectLayer);
      scene.appendChild(animLabelLayer);

      const redexId = meta ? meta.redexId : null;
      const lamId = meta ? meta.lamId : null;
      const targetVarIds = meta ? (meta.targetVarIds || []) : [];

      // Multi-index mapping for before elements
      const beforeByNodeId = new Map();
      const beforeByPrevId = new Map();
      const beforeByOriginId = new Map();
      const usedBeforeEls = new Set();

      layoutBefore.elements.forEach(el => {
        if (el.nodeId != null) beforeByNodeId.set(el.nodeId, el);
        if (el.prevNodeId != null && !beforeByPrevId.has(el.prevNodeId)) beforeByPrevId.set(el.prevNodeId, el);
        if (el.originId != null && !beforeByOriginId.has(el.originId)) beforeByOriginId.set(el.originId, el);
      });

      function findBestBeforeMatch(afterEl) {
        // Priority 1: Exact unchanged node ID in layoutBefore
        if (afterEl.nodeId != null && beforeByNodeId.has(afterEl.nodeId)) {
          const cand = beforeByNodeId.get(afterEl.nodeId);
          if (cand.type === afterEl.type && !usedBeforeEls.has(cand)) {
            return cand;
          }
        }
        // Priority 2: Direct predecessor node ID from previous step
        if (afterEl.prevNodeId != null && beforeByNodeId.has(afterEl.prevNodeId)) {
          const cand = beforeByNodeId.get(afterEl.prevNodeId);
          if (cand.type === afterEl.type && !usedBeforeEls.has(cand)) {
            return cand;
          }
        }
        if (afterEl.prevNodeId != null && beforeByPrevId.has(afterEl.prevNodeId)) {
          const cand = beforeByPrevId.get(afterEl.prevNodeId);
          if (cand.type === afterEl.type && !usedBeforeEls.has(cand)) {
            return cand;
          }
        }
        // Priority 3: Origin ID match
        if (afterEl.originId != null && beforeByOriginId.has(afterEl.originId)) {
          const cand = beforeByOriginId.get(afterEl.originId);
          if (cand.type === afterEl.type && !usedBeforeEls.has(cand)) {
            return cand;
          }
        }
        if (afterEl.originId != null && beforeByNodeId.has(afterEl.originId)) {
          const cand = beforeByNodeId.get(afterEl.originId);
          if (cand.type === afterEl.type && !usedBeforeEls.has(cand)) {
            return cand;
          }
        }
        // Priority 4: Structural / Semantic match among unused elements of SAME type
        // Ignore redex lam and redex bridge when finding matches for persistent elements
        for (const cand of layoutBefore.elements) {
          if (usedBeforeEls.has(cand)) continue;
          if (cand.nodeId === lamId || cand.nodeId === redexId || targetVarIds.includes(cand.nodeId)) continue;
          if (cand.type !== afterEl.type) continue;

          if (afterEl.type === 'variable') {
            if (cand.name === afterEl.name) return cand;
          } else if (afterEl.type === 'abstraction') {
            if (cand.param === afterEl.param) return cand;
          } else if (afterEl.type === 'application') {
            return cand;
          }
        }
        // Priority 5: Any unused element of same type (for undo/reset)
        for (const cand of layoutBefore.elements) {
          if (usedBeforeEls.has(cand)) continue;
          if (cand.nodeId === lamId || cand.nodeId === redexId) continue;
          if (cand.type === afterEl.type) return cand;
        }
        return null;
      }

      const animItems = [];

      // 1. Elements in layoutAfter (Persistent or Dynamically Growing)
      layoutAfter.elements.forEach(afterEl => {
        const targetCoords = getElementCoords(afterEl);
        let startCoords = null;
        let startOpacity = 1;
        let isNew = false;
        const isClone = (afterEl.subInstance !== undefined && afterEl.subInstance !== null);

        let matchBefore = null;
        if (isClone) {
          const originEl = beforeByNodeId.get(afterEl.originId) || beforeByOriginId.get(afterEl.originId);
          if (originEl) {
            startCoords = getElementCoords(originEl);
            startOpacity = 1;
          } else {
            isNew = true;
          }
        } else {
          matchBefore = findBestBeforeMatch(afterEl);
          if (matchBefore) {
            usedBeforeEls.add(matchBefore);
            startCoords = getElementCoords(matchBefore);
            startOpacity = 1; // Persistent elements ALWAYS maintain full solid opacity
          } else {
            isNew = true;
          }
        }

        // Dynamic Axis-Aligned Growth for New Elements:
        if (isNew) {
          startOpacity = 0.0;
          if (afterEl.type === 'abstraction') {
            const midX = (targetCoords.x1 + targetCoords.x2) / 2;
            startCoords = { x1: midX, y1: targetCoords.y1, x2: midX, y2: targetCoords.y2 };
          } else if (afterEl.type === 'variable') {
            startCoords = { x1: targetCoords.x1, y1: targetCoords.y1, x2: targetCoords.x2, y2: targetCoords.y1 };
          } else if (afterEl.type === 'application') {
            startCoords = { x1: targetCoords.x1, y1: targetCoords.y1, x2: targetCoords.x1, y2: targetCoords.y2 };
          }
        }

        const line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
        const cls = afterEl.type === 'abstraction' ? 'diag-abs' : (afterEl.type === 'variable' ? 'diag-var' : 'diag-app');
        line.setAttribute('class', cls);
        const parentLayer = afterEl.type === 'abstraction' ? animAbsLayer : (afterEl.type === 'variable' ? animVarLayer : animLinkLayer);
        parentLayer.appendChild(line);

        animItems.push({
          dom: line,
          type: afterEl.type,
          start: { ...startCoords, opacity: startOpacity },
          target: { ...targetCoords, opacity: 1.0 }
        });

        // Synchronized Variable Junction Dots
        if (afterEl.type === 'variable') {
          const dot = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
          dot.setAttribute('r', 4.5);
          dot.setAttribute('class', 'diag-dot');
          animVarLayer.appendChild(dot);
          const dotStart = isNew
            ? { cx: targetCoords.x1, cy: targetCoords.y1, opacity: 0.0 }
            : { cx: startCoords.x1, cy: startCoords.y1, opacity: 1.0 };
          animItems.push({
            dom: dot,
            isDot: true,
            start: dotStart,
            target: { cx: targetCoords.x1, cy: targetCoords.y1, opacity: 1.0 }
          });
        }

        // Synchronized Text Labels
        if (appState.showLabels) {
          if (afterEl.type === 'abstraction') {
            const txt = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            txt.setAttribute('text-anchor', 'end');
            txt.setAttribute('class', 'diag-label');
            txt.textContent = `λ${afterEl.param}`;
            animLabelLayer.appendChild(txt);
            const lStart = isNew
              ? { x: startCoords.x1 - 6, y: targetCoords.y1 + 4, opacity: 0.0 }
              : { x: startCoords.x1 - 6, y: startCoords.y1 + 4, opacity: 1.0 };
            animItems.push({
              dom: txt,
              isLabel: true,
              start: lStart,
              target: { x: targetCoords.x1 - 6, y: targetCoords.y1 + 4, opacity: 1.0 }
            });
          } else if (afterEl.type === 'variable') {
            const lbl = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            lbl.setAttribute('text-anchor', 'middle');
            lbl.setAttribute('class', 'diag-label');
            lbl.setAttribute('font-size', '10px');
            lbl.textContent = afterEl.name;
            animLabelLayer.appendChild(lbl);
            const lStart = isNew
              ? { x: targetCoords.x1, y: targetCoords.y1 + 15, opacity: 0.0 }
              : { x: startCoords.x1, y: startCoords.y2 + 15, opacity: 1.0 };
            animItems.push({
              dom: lbl,
              isLabel: true,
              start: lStart,
              target: { x: targetCoords.x1, y: targetCoords.y2 + 15, opacity: 1.0 }
            });
          }
        }
      });

      // 2. Dissolving & Discarded Elements in layoutBefore (Dynamic Axis-Aligned Retraction)
      layoutBefore.elements.forEach(beforeEl => {
        const startCoords = getElementCoords(beforeEl);

        if (beforeEl.nodeId === lamId) {
          // Dissolving lambda bar retracts inward toward midpoint
          const midX = (startCoords.x1 + startCoords.x2) / 2;
          const line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
          line.setAttribute('class', 'diag-abs diag-dissolving');
          animAbsLayer.appendChild(line);
          animItems.push({
            dom: line,
            type: 'abstraction',
            start: { ...startCoords, opacity: 1.0 },
            target: { x1: midX, y1: startCoords.y1, x2: midX, y2: startCoords.y1, opacity: 0.0 }
          });

          if (appState.showLabels) {
            const txt = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            txt.setAttribute('text-anchor', 'end');
            txt.setAttribute('class', 'diag-label');
            txt.textContent = `λ${beforeEl.param}`;
            animLabelLayer.appendChild(txt);
            animItems.push({
              dom: txt,
              isLabel: true,
              start: { x: startCoords.x1 - 6, y: startCoords.y1 + 4, opacity: 1.0 },
              target: { x: midX - 6, y: startCoords.y1 + 4, opacity: 0.0 }
            });
          }
        } else if (beforeEl.nodeId === redexId) {
          // Dissolving redex bridge retracts horizontally into function anchor
          const line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
          line.setAttribute('class', 'diag-app diag-redex-bridge');
          animLinkLayer.appendChild(line);
          animItems.push({
            dom: line,
            type: 'application',
            start: { ...startCoords, opacity: 1.0 },
            target: { x1: startCoords.x1, y1: startCoords.y1, x2: startCoords.x1, y2: startCoords.y1, opacity: 0.0 }
          });
        } else if (targetVarIds.includes(beforeEl.nodeId)) {
          // Target bound variable retracts into socket
          const line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
          line.setAttribute('class', 'diag-var diag-target-var');
          animVarLayer.appendChild(line);
          animItems.push({
            dom: line,
            type: 'variable',
            start: { ...startCoords, opacity: 1.0 },
            target: { x1: startCoords.x1, y1: startCoords.y2, x2: startCoords.x2, y2: startCoords.y2, opacity: 0.0 }
          });

          // Expanding receptor socket pulse
          const socket = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
          socket.setAttribute('cx', startCoords.x2);
          socket.setAttribute('cy', startCoords.y2);
          socket.setAttribute('r', 6);
          socket.setAttribute('class', 'receptor-socket');
          animEffectLayer.appendChild(socket);

          animItems.push({
            dom: socket,
            isSocket: true,
            start: { r: 4.5, opacity: 1.0 },
            target: { r: 12, opacity: 0.0 }
          });

          if (appState.showLabels) {
            const lbl = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            lbl.setAttribute('text-anchor', 'middle');
            lbl.setAttribute('class', 'diag-label');
            lbl.setAttribute('font-size', '10px');
            lbl.textContent = beforeEl.name;
            animLabelLayer.appendChild(lbl);
            animItems.push({
              dom: lbl,
              isLabel: true,
              start: { x: startCoords.x1, y: startCoords.y2 + 15, opacity: 1.0 },
              target: { x: startCoords.x1, y: startCoords.y2 + 15, opacity: 0.0 }
            });
          }
        } else if (!usedBeforeEls.has(beforeEl)) {
          // Discarded invariant branch (e.g. dropped argument in K a b -> a)
          const line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
          const cls = beforeEl.type === 'abstraction' ? 'diag-abs' : (beforeEl.type === 'variable' ? 'diag-var' : 'diag-app');
          line.setAttribute('class', cls + ' diag-dissolving');
          const parentLayer = beforeEl.type === 'abstraction' ? animAbsLayer : (beforeEl.type === 'variable' ? animVarLayer : animLinkLayer);
          parentLayer.appendChild(line);

          let targetCollapse = null;
          if (beforeEl.type === 'abstraction' || beforeEl.type === 'application') {
            const midX = (startCoords.x1 + startCoords.x2) / 2;
            targetCollapse = { x1: midX, y1: startCoords.y1, x2: midX, y2: startCoords.y1, opacity: 0.0 };
          } else if (beforeEl.type === 'variable') {
            const midY = (startCoords.y1 + startCoords.y2) / 2;
            targetCollapse = { x1: startCoords.x1, y1: midY, x2: startCoords.x2, y2: midY, opacity: 0.0 };
          }

          animItems.push({
            dom: line,
            type: beforeEl.type,
            start: { ...startCoords, opacity: 1.0 },
            target: targetCollapse
          });

          if (appState.showLabels) {
            if (beforeEl.type === 'abstraction') {
              const txt = document.createElementNS('http://www.w3.org/2000/svg', 'text');
              txt.setAttribute('text-anchor', 'end');
              txt.setAttribute('class', 'diag-label');
              txt.textContent = `λ${beforeEl.param}`;
              animLabelLayer.appendChild(txt);
              animItems.push({
                dom: txt,
                isLabel: true,
                start: { x: startCoords.x1 - 6, y: startCoords.y1 + 4, opacity: 1.0 },
                target: { x: targetCollapse.x1 - 6, y: targetCollapse.y1 + 4, opacity: 0.0 }
              });
            } else if (beforeEl.type === 'variable') {
              const lbl = document.createElementNS('http://www.w3.org/2000/svg', 'text');
              lbl.setAttribute('text-anchor', 'middle');
              lbl.setAttribute('class', 'diag-label');
              lbl.setAttribute('font-size', '10px');
              lbl.textContent = beforeEl.name;
              animLabelLayer.appendChild(lbl);
              animItems.push({
                dom: lbl,
                isLabel: true,
                start: { x: startCoords.x1, y: startCoords.y2 + 15, opacity: 1.0 },
                target: { x: targetCollapse.x1, y: targetCollapse.y2 + 15, opacity: 0.0 }
              });
            }
          }
        }
      });

      const duration = Math.max(250, Math.min(1800, appState.playSpeed || 600));
      const startTime = performance.now();

      function ease(t) {
        return t < 0.5 ? 4 * t * t * t : 1 - Math.pow(-2 * t + 2, 3) / 2;
      }

      function lerp(a, b, t) {
        return a + (b - a) * t;
      }

      function tick(now) {
        const elapsed = now - startTime;
        const progress = Math.min(1.0, elapsed / duration);
        const e = ease(progress);

        animItems.forEach(item => {
          if (item.isSocket) {
            const r = lerp(item.start.r, item.target.r, e);
            const op = lerp(item.start.opacity, item.target.opacity, e);
            item.dom.setAttribute('r', r);
            item.dom.setAttribute('stroke-opacity', op);
            item.dom.style.opacity = op;
          } else if (item.isDot) {
            const cx = lerp(item.start.cx, item.target.cx, e);
            const cy = lerp(item.start.cy, item.target.cy, e);
            const op = lerp(item.start.opacity, item.target.opacity, e);
            item.dom.setAttribute('cx', cx);
            item.dom.setAttribute('cy', cy);
            item.dom.setAttribute('fill-opacity', op);
            item.dom.style.opacity = op;
          } else if (item.isLabel) {
            const x = lerp(item.start.x, item.target.x, e);
            const y = lerp(item.start.y, item.target.y, e);
            const op = lerp(item.start.opacity, item.target.opacity, e);
            item.dom.setAttribute('x', x);
            item.dom.setAttribute('y', y);
            item.dom.setAttribute('fill-opacity', op);
            item.dom.style.opacity = op;
          } else {
            const x1 = lerp(item.start.x1, item.target.x1, e);
            const y1 = lerp(item.start.y1, item.target.y1, e);
            const x2 = lerp(item.start.x2, item.target.x2, e);
            const y2 = lerp(item.start.y2, item.target.y2, e);
            const op = lerp(item.start.opacity, item.target.opacity, e);

            item.dom.setAttribute('x1', x1);
            item.dom.setAttribute('y1', y1);
            item.dom.setAttribute('x2', x2);
            item.dom.setAttribute('y2', y2);
            item.dom.setAttribute('stroke-opacity', op);
            item.dom.style.opacity = op;
          }
        });

        if (progress < 1.0) {
          activeAnimationRaf = requestAnimationFrame(tick);
        } else {
          activeAnimationRaf = null;
          if (onComplete) onComplete();
        }
      }

      activeAnimationRaf = requestAnimationFrame(tick);
    }

    function attachSVGInteractiveEvents() {
      const svg = document.getElementById('primarySvgDiagram');
      const tooltip = document.getElementById('nodeInspectorTooltip');
      const tHead = document.getElementById('tooltipHeader');
      const tBody = document.getElementById('tooltipContent');

      svg.querySelectorAll('.diag-abs, .diag-var, .diag-app').forEach(el => {
        el.addEventListener('mouseenter', () => {
          const info = el.getAttribute('data-info');
          const type = el.getAttribute('data-type');
          tHead.textContent = type === 'app' ? I18N[currentLang].tooltipApp : (type === 'abs' ? I18N[currentLang].tooltipAbs : I18N[currentLang].tooltipVar);
          tBody.textContent = info || '';
          tooltip.style.display = 'block';

          const id = parseInt(el.getAttribute('data-id'), 10);
          highlightCodeToken(id);
        });

        el.addEventListener('mousemove', (e) => {
          const rect = document.getElementById('svgCanvasContainer').getBoundingClientRect();
          tooltip.style.left = `${e.clientX - rect.left + 15}px`;
          tooltip.style.top = `${e.clientY - rect.top + 15}px`;
        });

        el.addEventListener('mouseleave', () => {
          tooltip.style.display = 'none';
          clearCodeTokenHighlight();
        });

        el.addEventListener('click', () => {
          if (el.getAttribute('data-is-redex') === 'true') {
            const redexId = parseInt(el.getAttribute('data-id'), 10);
            triggerSpecificRedex(redexId);
          }
        });
      });
    }

    function highlightCodeToken(nodeId) {
      document.querySelectorAll(`[data-node-id="${nodeId}"]`).forEach(el => {
        el.classList.add('highlight-scope');
      });
    }

    function clearCodeTokenHighlight() {
      document.querySelectorAll('.highlight-scope').forEach(el => {
        el.classList.remove('highlight-scope');
      });
    }

    function updateSceneTransform() {
      const scene = document.getElementById('diagramSceneGroup');
      scene.setAttribute('transform', `translate(${panZoom.x}, ${panZoom.y}) scale(${panZoom.scale})`);
    }