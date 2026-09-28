/**
 * John Tromp Lambda Diagrams Workstation - Dual-Style Layout Engine
 * Computes standard style (leftmost variable connection) and alternative style (deepest nearest connection).
 * Generates Unicode boxChar art and ASCII dot-matrix representations.
 */

function getLeftmostVarNode(node) {
  if (!node) return null;
  while (node.type === 'db_lam') node = node.body;
  if (node.type === 'db_var') return node;
  if (node.type === 'db_app') return getLeftmostVarNode(node.fn);
  return null;
}

function getRightmostVarNode(node) {
  if (!node) return null;
  while (node.type === 'db_lam') node = node.body;
  if (node.type === 'db_var') return node;
  if (node.type === 'db_app') return getRightmostVarNode(node.arg);
  return null;
}

function computeTrompLayout(rootTerm, alt = false) {
  const elements = [];
  const varMap = new Map();
  const lamMap = new Map();
  const appMap = new Map();
  let breadth = 0;
  let depth = 0;

  function traverse(node, env) {
    if (!node) return;
    if (node.type === 'db_var') {
      const binder = (node.index < env.length) ? env[node.index] : null;
      const bLine = binder ? lamMap.get(binder.id) : null;
      const startY = bLine ? bLine.y : 0;
      const varLine = {
        id: node.id,
        nodeId: node.id,
        prevNodeId: node.prevNodeId || node.id,
        originId: node.originId || node.id,
        subInstance: node.subInstance,
        targetVarId: node.targetVarId,
        type: 'variable',
        name: node.name || (binder ? binder.param : (node.freeName || `v${node.index + 1}`)),
        index: node.index,
        isFree: !binder,
        binderId: binder ? binder.id : null,
        col: breadth,
        x1: breadth,
        x2: breadth,
        y1: startY,
        y2: 9999
      };
      varMap.set(node.id, varLine);
      elements.push(varLine);
      breadth += 1;
    } else if (node.type === 'db_lam') {
      const startX = breadth;
      const lamLine = {
        id: node.id,
        nodeId: node.id,
        prevNodeId: node.prevNodeId || node.id,
        originId: node.originId || node.id,
        subInstance: node.subInstance,
        targetVarId: node.targetVarId,
        type: 'abstraction',
        param: node.param || 'x',
        y: depth,
        y1: depth,
        y2: depth,
        x1: startX,
        x2: 9999
      };
      lamMap.set(node.id, lamLine);
      elements.push(lamLine);

      depth += 1;
      traverse(node.body, [node, ...env]);
      depth -= 1;

      lamLine.x2 = Math.max(startX, breadth - 1);
    } else if (node.type === 'db_app') {
      traverse(node.fn, env);
      traverse(node.arg, env);

      const leftVarNode = alt ? getRightmostVarNode(node.fn) : getLeftmostVarNode(node.fn);
      const rightVarNode = getLeftmostVarNode(node.arg);
      const leftVarLine = leftVarNode ? varMap.get(leftVarNode.id) : null;
      const rightVarLine = rightVarNode ? varMap.get(rightVarNode.id) : null;

      let lowestY = 0;
      const leftCol = leftVarLine ? leftVarLine.col : 0;
      const rightCol = rightVarLine ? rightVarLine.col : breadth - 1;

      for (const l of elements) {
        if (l.type === 'abstraction' || l.type === 'application') {
          if (l.y > lowestY && l.x2 >= leftCol) {
            lowestY = l.y;
          }
        }
      }

      const appY = lowestY + 1;
      if (rightVarLine) {
        rightVarLine.y2 = appY;
      }

      const isRedex = (node.fn && node.fn.type === 'db_lam');
      const appLine = {
        id: node.id,
        nodeId: node.id,
        prevNodeId: node.prevNodeId || node.id,
        originId: node.originId || node.id,
        subInstance: node.subInstance,
        targetVarId: node.targetVarId,
        type: 'application',
        isRedex: isRedex,
        y: appY,
        y1: appY,
        y2: appY,
        x1: leftCol,
        x2: rightCol,
        fnVarId: leftVarNode ? leftVarNode.id : null,
        argVarId: rightVarNode ? rightVarNode.id : null
      };
      appMap.set(node.id, appLine);
      elements.push(appLine);
    }
  }

  traverse(rootTerm, []);

  let maxHorizY = 0;
  for (const l of elements) {
    if ((l.type === 'abstraction' || l.type === 'application') && l.y > maxHorizY) {
      maxHorizY = l.y;
    }
  }

  const mainVar = getLeftmostVarNode(rootTerm);
  const mainLine = mainVar ? varMap.get(mainVar.id) : null;
  if (mainLine) {
    if (!alt) {
      mainLine.y2 = maxHorizY + 1;
    } else {
      if (mainLine.y2 >= 9999) {
        mainLine.y2 = maxHorizY;
      }
    }
  }

  for (const l of elements) {
    if (l.type === 'variable' && l.y2 >= 9999) {
      l.y2 = maxHorizY + (alt ? 0 : 1);
    }
  }

  let maxX = 0, maxY = 0;
  for (const l of elements) {
    maxX = Math.max(maxX, l.x1, l.x2);
    maxY = Math.max(maxY, l.y1, l.y2);
  }

  return {
    elements,
    maxX,
    maxY,
    dim: [[maxY, maxX], [0, maxX]]
  };
}

function generateBoxChar(term, alt = false) {
  const layout = computeTrompLayout(term, alt);
  const { elements, maxX, maxY } = layout;

  const UP = 1, RIGHT = 2, DOWN = 4, LEFT = 8;
  const grid = Array.from({ length: maxY + 1 }, () => new Uint8Array(maxX + 1));

  for (const l of elements) {
    if (l.type === 'variable') {
      const c = l.col;
      for (let r = l.y1; r < l.y2; r++) {
        grid[r][c] |= DOWN;
        grid[r + 1][c] |= UP;
      }
    } else if (l.type === 'abstraction' || l.type === 'application') {
      const r = l.y;
      for (let c = l.x1; c < l.x2; c++) {
        grid[r][c] |= RIGHT;
        grid[r][c + 1] |= LEFT;
      }
    }
  }

  const boxChars = {
    0: ' ',
    [UP | DOWN]: '│',
    [LEFT | RIGHT]: '─',
    [UP | RIGHT]: '└',
    [UP | LEFT]: '┘',
    [DOWN | RIGHT]: '┌',
    [DOWN | LEFT]: '┐',
    [UP | DOWN | RIGHT]: '├',
    [UP | DOWN | LEFT]: '┤',
    [LEFT | RIGHT | DOWN]: '┬',
    [LEFT | RIGHT | UP]: '┴',
    [UP | DOWN | LEFT | RIGHT]: '┼',
    [DOWN]: '│',
    [UP]: '│',
    [RIGHT]: '─',
    [LEFT]: '─'
  };

  const lines = [];
  for (let r = 0; r <= maxY; r++) {
    let rowStr = '';
    for (let c = 0; c <= maxX; c++) {
      rowStr += (boxChars[grid[r][c]] || ' ') + ' ';
    }
    lines.push(rowStr.replace(/\s+$/, ''));
  }
  return lines.join('\n');
}

function generateAsciiGrid(term, alt = false) {
  const layout = computeTrompLayout(term, alt);
  const { elements, maxX, maxY } = layout;
  const grid = Array.from({ length: maxY + 1 }, () => Array(maxX + 1).fill(' '));

  for (const l of elements) {
    if (l.type === 'abstraction' || l.type === 'application') {
      for (let c = l.x1; c <= l.x2; c++) {
        grid[l.y][c] = (grid[l.y][c] === '|' ? '+' : '_');
      }
    } else if (l.type === 'variable') {
      for (let r = l.y1; r <= l.y2; r++) {
        grid[r][l.col] = (grid[r][l.col] === '_' ? '+' : '|');
      }
    }
  }

  return grid.map(row => row.join(' ').replace(/\s+$/, '')).join('\n');
}
