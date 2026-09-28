/**
 * John Tromp Lambda Diagrams Workstation - Beta Reduction & Lineage Engine
 * Normal Order (Leftmost-Outermost) & Applicative Order (Innermost-First).
 * Lineage tracking for AST nodes to support continuous tween interpolation.
 * 5-Phase visual metadata and narratives generator (Zero emojis).
 */

function cloneTreeWithLineage(term, subInstance = null, targetVarId = null) {
  if (!term) return null;
  const newId = nextId();
  const originId = term.originId || term.id;
  const prevId = term.id;
  if (term.type === 'db_var') {
    return {
      ...term,
      id: newId,
      prevNodeId: prevId,
      originId: originId,
      subInstance: subInstance,
      targetVarId: targetVarId
    };
  }
  if (term.type === 'db_app') {
    return {
      ...term,
      id: newId,
      prevNodeId: prevId,
      originId: originId,
      subInstance: subInstance,
      targetVarId: targetVarId,
      fn: cloneTreeWithLineage(term.fn, subInstance, targetVarId),
      arg: cloneTreeWithLineage(term.arg, subInstance, targetVarId)
    };
  }
  if (term.type === 'db_lam') {
    return {
      ...term,
      id: newId,
      prevNodeId: prevId,
      originId: originId,
      subInstance: subInstance,
      targetVarId: targetVarId,
      body: cloneTreeWithLineage(term.body, subInstance, targetVarId)
    };
  }
  return term;
}

function incvWithLineage(i, term) {
  if (!term) return null;
  const originId = term.originId || term.id;
  const prevId = term.id;
  if (term.type === 'db_var') {
    return {
      ...term,
      id: nextId(),
      prevNodeId: prevId,
      originId: originId,
      index: term.index >= i ? term.index + 1 : term.index
    };
  }
  if (term.type === 'db_app') {
    return {
      id: nextId(),
      prevNodeId: prevId,
      originId: originId,
      type: 'db_app',
      subInstance: term.subInstance,
      targetVarId: term.targetVarId,
      fn: incvWithLineage(i, term.fn),
      arg: incvWithLineage(i, term.arg)
    };
  }
  if (term.type === 'db_lam') {
    return {
      ...term,
      id: nextId(),
      prevNodeId: prevId,
      originId: originId,
      subInstance: term.subInstance,
      targetVarId: term.targetVarId,
      body: incvWithLineage(i + 1, term.body)
    };
  }
  return term;
}

function substWithLineage(i, body, arg, counterState) {
  if (!body) return null;
  const originId = body.originId || body.id;
  const prevId = body.id;
  if (body.type === 'db_var') {
    if (body.index === i) {
      const instIdx = counterState.count++;
      return cloneTreeWithLineage(arg, instIdx, body.id);
    }
    if (body.index > i) {
      return {
        ...body,
        id: nextId(),
        prevNodeId: prevId,
        originId: originId,
        index: body.index - 1
      };
    }
    return { ...body, prevNodeId: prevId, originId: originId };
  }
  if (body.type === 'db_app') {
    return {
      ...body,
      id: nextId(),
      prevNodeId: prevId,
      originId: originId,
      fn: substWithLineage(i, body.fn, arg, counterState),
      arg: substWithLineage(i, body.arg, arg, counterState)
    };
  }
  if (body.type === 'db_lam') {
    return {
      ...body,
      id: nextId(),
      prevNodeId: prevId,
      originId: originId,
      body: substWithLineage(i + 1, body.body, incvWithLineage(0, arg), counterState)
    };
  }
  return body;
}

function findTargetVariables(body, targetIndex, currentDepth = 0) {
  const vars = [];
  function walk(node, depth) {
    if (!node) return;
    if (node.type === 'db_var') {
      if (node.index === targetIndex + depth) {
        vars.push({ id: node.id, name: node.name || `v${node.index + 1}`, index: node.index });
      }
    } else if (node.type === 'db_lam') {
      walk(node.body, depth + 1);
    } else if (node.type === 'db_app') {
      walk(node.fn, depth);
      walk(node.arg, depth);
    }
  }
  walk(body, currentDepth);
  return vars;
}

function findAllRedexes(term, path = []) {
  const list = [];
  if (!term) return list;
  if (term.type === 'db_app') {
    if (term.fn && term.fn.type === 'db_lam') {
      list.push({ id: term.id, term, path });
    }
    list.push(...findAllRedexes(term.fn, [...path, 'fn']));
    list.push(...findAllRedexes(term.arg, [...path, 'arg']));
  } else if (term.type === 'db_lam') {
    list.push(...findAllRedexes(term.body, [...path, 'body']));
  }
  return list;
}

function selectRedex(term, strategy = 'normal', targetId = null) {
  const all = findAllRedexes(term);
  if (all.length === 0) return null;
  if (targetId != null) {
    const found = all.find(r => r.id === targetId);
    if (found) return found;
  }
  if (strategy === 'applicative') {
    all.sort((a, b) => b.path.length - a.path.length);
    return all[0];
  }
  return all[0];
}

function prepareReductionStep(term, strategy = 'normal', targetRedexId = null) {
  const redex = selectRedex(term, strategy, targetRedexId);
  if (!redex) return null;

  const targetApp = redex.term;
  const lamNode = targetApp.fn;
  const argNode = targetApp.arg;
  const boundVars = findTargetVariables(lamNode.body, 0);

  const counterState = { count: 0 };
  function replaceInTree(t, pathIdx = 0) {
    if (pathIdx === redex.path.length) {
      return substWithLineage(0, lamNode.body, argNode, counterState);
    }
    const dir = redex.path[pathIdx];
    if (dir === 'fn') return { ...t, prevNodeId: t.id, fn: replaceInTree(t.fn, pathIdx + 1) };
    if (dir === 'arg') return { ...t, prevNodeId: t.id, arg: replaceInTree(t.arg, pathIdx + 1) };
    if (dir === 'body') return { ...t, prevNodeId: t.id, body: replaceInTree(t.body, pathIdx + 1) };
    return t;
  }

  const nextTerm = replaceInTree(term);
  const pName = lamNode.param || 'x';

  function collectSubtreeNodeIds(n) {
    if (!n) return [];
    const res = [n.id];
    if (n.type === 'db_lam') res.push(...collectSubtreeNodeIds(n.body));
    else if (n.type === 'db_app') res.push(...collectSubtreeNodeIds(n.fn), ...collectSubtreeNodeIds(n.arg));
    return res;
  }

  const argNodeIds = collectSubtreeNodeIds(argNode);
  const redexNodeIds = collectSubtreeNodeIds(targetApp);
  const argSummary = termToPlainString(argNode) || 'N';

  return {
    termBefore: term,
    termAfter: nextTerm,
    redexId: targetApp.id,
    lamId: lamNode.id,
    paramName: pName,
    argId: argNode.id,
    argNode: argNode,
    argNodeIds: argNodeIds,
    redexNodeIds: redexNodeIds,
    argSummary: argSummary,
    targetVarIds: boundVars.map(v => v.id),
    varCount: boundVars.length,
    narratives: {
      zh: {
        1: `阶段 1 (锁定红基)：识别待规约应用 ((λ${pName}. M) ${argSummary})，焦点框高亮调用横梁与实参子图。`,
        2: `阶段 2 (关联形参与实参)：在函数体锁定 ${boundVars.length} 处形参 '${pName}'，实参 [${argSummary}] 框选就绪并绘制代换流向线。`,
        3: `阶段 3 (溶解横梁与开启插槽)：λ${pName} 抽象横梁与红基应用桥溶解解除，形参引线末端开启双环参数接收插槽。`,
        4: `阶段 4 (执行代换接入)：实参 [${argSummary}] ${boundVars.length > 0 ? '复制并直接代入接入插槽' : '无引用，实参被直接丢弃 (K 组合子)'}，完成自由变量移位。`,
        5: `阶段 5 (最终项整流归位)：β-规约完成！项几何坐标与拓扑网格整流归位，结算至规范 Tromp 图。`
      },
      en: {
        1: `Phase 1 (Redex Discovery): Spotlighted redex ((λ${pName}. M) ${argSummary}), highlighting application bridge and argument.`,
        2: `Phase 2 (Match Vars & Arg): Located ${boundVars.length} occurrence(s) of parameter '${pName}'. Argument [${argSummary}] framed with dynamic flow traces.`,
        3: `Phase 3 (Dissolve λ & Open Sockets): Abstraction bar λ${pName} and application bridge dissolve, opening receptor sockets.`,
        4: `Phase 4 (Execute Substitution): Argument [${argSummary}] ${boundVars.length > 0 ? 'duplicated and docked into sockets' : 'is discarded (0 references in K-combinator)'}, free variables shifted.`,
        5: `Phase 5 (Canonical Settlement): β-reduction complete! Coordinates normalized and settled into canonical Tromp diagram.`
      }
    }
  };
}
