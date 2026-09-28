/**
 * John Tromp Lambda Diagrams Workstation - AST & Parser Engine
 * Multi-format parser (Classic Named Lambda, De Bruijn notation, Let-bindings)
 * Full AST converters, HTML syntax highlighters with scope & redex tagging.
 */

let globalIdGen = 1;
function nextId() {
  return globalIdGen++;
}

function tokenizeNamed(input) {
  const tokens = [];
  let i = 0;
  while (i < input.length) {
    const c = input[i];
    if (/\s/.test(c)) {
      i++;
      continue;
    }
    if (c === '(') {
      tokens.push({ type: 'LPAREN', pos: i });
      i++;
      continue;
    }
    if (c === ')') {
      tokens.push({ type: 'RPAREN', pos: i });
      i++;
      continue;
    }
    if (c === '\\' || c === 'λ' || c === '/') {
      tokens.push({ type: 'LAMBDA', pos: i });
      i++;
      continue;
    }
    if (c === '.') {
      tokens.push({ type: 'DOT', pos: i });
      i++;
      continue;
    }
    if (c === '=') {
      tokens.push({ type: 'EQUALS', pos: i });
      i++;
      continue;
    }
    if (/[a-zA-Z0-9_']/.test(c)) {
      const start = i;
      let val = '';
      while (i < input.length && /[a-zA-Z0-9_']/.test(input[i])) {
        val += input[i++];
      }
      if (val === 'let') tokens.push({ type: 'LET', pos: start });
      else if (val === 'in') tokens.push({ type: 'IN', pos: start });
      else tokens.push({ type: 'ID', value: val, pos: start });
      continue;
    }
    i++;
  }
  tokens.push({ type: 'EOF', pos: i });
  return tokens;
}

function parseNamedLambda(input) {
  const tokens = tokenizeNamed(input);
  let idx = 0;

  function cur() {
    return tokens[idx];
  }
  function consume(type) {
    if (cur().type === type) return tokens[idx++];
    throw new Error(`Expected '${type}' but found '${cur().type}' at char ${cur().pos}`);
  }

  function parseExpr() {
    if (cur().type === 'LET') {
      consume('LET');
      const varName = consume('ID').value;
      consume('EQUALS');
      const val = parseExpr();
      consume('IN');
      const body = parseExpr();
      return { type: 'app', fn: { type: 'lam', param: varName, body }, arg: val };
    }

    if (cur().type === 'LAMBDA') {
      consume('LAMBDA');
      const params = [];
      while (cur().type === 'ID') {
        params.push(consume('ID').value);
      }
      if (cur().type === 'DOT') consume('DOT');
      let body = parseExpr();
      for (let k = params.length - 1; k >= 0; k--) {
        body = { type: 'lam', param: params[k], body };
      }
      return body;
    }

    const atoms = [];
    while (cur().type === 'LPAREN' || cur().type === 'ID' || cur().type === 'LAMBDA') {
      if (cur().type === 'LAMBDA') {
        atoms.push(parseExpr());
        break;
      }
      atoms.push(parseAtom());
    }

    if (atoms.length === 0) {
      throw new Error(`Expected expression near '${cur().type}' at char ${cur().pos}`);
    }

    let res = atoms[0];
    for (let k = 1; k < atoms.length; k++) {
      res = { type: 'app', fn: res, arg: atoms[k] };
    }
    return res;
  }

  function parseAtom() {
    if (cur().type === 'LPAREN') {
      consume('LPAREN');
      const e = parseExpr();
      consume('RPAREN');
      return e;
    }
    if (cur().type === 'ID') {
      return { type: 'var', name: consume('ID').value };
    }
    throw new Error(`Unexpected token '${cur().type}' in atom`);
  }

  const ast = parseExpr();
  if (cur().type !== 'EOF') {
    throw new Error(`Extra unexpected token '${cur().type}' at char ${cur().pos}`);
  }
  return ast;
}

function parseDeBruijnString(input) {
  let s = input.replace(/[λ\/]/g, '\\');
  let i = 0;

  function skipWs() {
    while (i < s.length && /\s/.test(s[i])) i++;
  }

  function parseAtom() {
    skipWs();
    if (i >= s.length) return null;
    if (s[i] === '(') {
      i++;
      const expr = parseExpr();
      skipWs();
      if (s[i] === ')') i++;
      return expr;
    }
    if (s[i] === '\\') {
      i++;
      skipWs();
      const body = parseExpr();
      return { id: nextId(), type: 'db_lam', param: 'x', body };
    }
    if (/[0-9]/.test(s[i])) {
      let numStr = '';
      while (i < s.length && /[0-9]/.test(s[i])) {
        numStr += s[i++];
      }
      const n = parseInt(numStr, 10);
      return { id: nextId(), type: 'db_var', index: Math.max(0, n - 1), name: `v${n}` };
    }
    if (/[a-zA-Z_']/.test(s[i])) {
      let name = '';
      while (i < s.length && /[a-zA-Z0-9_']/.test(s[i])) {
        name += s[i++];
      }
      return { id: nextId(), type: 'db_var', index: 99, freeName: name };
    }
    return null;
  }

  function parseExpr() {
    const atoms = [];
    while (true) {
      skipWs();
      if (i >= s.length || s[i] === ')') break;
      const atom = parseAtom();
      if (!atom) break;
      atoms.push(atom);
    }
    if (atoms.length === 0) throw new Error('Expected De Bruijn term near position ' + i);
    let res = atoms[0];
    for (let k = 1; k < atoms.length; k++) {
      res = { id: nextId(), type: 'db_app', fn: res, arg: atoms[k] };
    }
    return res;
  }

  return parseExpr();
}

function convertNamedToDB(ast, env = []) {
  if (ast.type === 'var') {
    const idx = env.indexOf(ast.name);
    if (idx === -1) {
      return { id: nextId(), type: 'db_var', index: env.length, freeName: ast.name };
    }
    return { id: nextId(), type: 'db_var', index: idx, name: ast.name };
  }
  if (ast.type === 'lam') {
    return {
      id: nextId(),
      type: 'db_lam',
      param: ast.param,
      body: convertNamedToDB(ast.body, [ast.param, ...env])
    };
  }
  if (ast.type === 'app') {
    return {
      id: nextId(),
      type: 'db_app',
      fn: convertNamedToDB(ast.fn, env),
      arg: convertNamedToDB(ast.arg, env)
    };
  }
}

function dbToNamedAST(term, env = [], used = new Set()) {
  if (term.type === 'db_var') {
    if (term.index < env.length) {
      return { type: 'var', name: env[term.index], id: term.id, index: term.index };
    }
    return {
      type: 'var',
      name: term.freeName || `v${term.index - env.length + 1}`,
      id: term.id,
      index: term.index
    };
  }
  if (term.type === 'db_lam') {
    let base = term.param || 'x';
    let cand = base;
    let c = 1;
    while (used.has(cand)) {
      cand = `${base}${c++}`;
    }
    const newUsed = new Set(used);
    newUsed.add(cand);
    return {
      type: 'lam',
      param: cand,
      body: dbToNamedAST(term.body, [cand, ...env], newUsed),
      id: term.id
    };
  }
  if (term.type === 'db_app') {
    return {
      type: 'app',
      fn: dbToNamedAST(term.fn, env, used),
      arg: dbToNamedAST(term.arg, env, used),
      id: term.id
    };
  }
}

function renderNamedHtml(ast, parentPrec = 0, highlights = {}) {
  const isRedex = highlights.redexId === ast.id;
  const isTargetVar = highlights.targetVarIds && highlights.targetVarIds.includes(ast.id);
  const isArg = highlights.argId === ast.id;

  let cls = 'syntax-token';
  if (isRedex) cls += ' highlight-redex';
  if (isTargetVar) cls += ' highlight-target-var';
  if (isArg) cls += ' highlight-arg';

  if (ast.type === 'var') {
    return `<span class="${cls}" data-node-id="${ast.id}">${ast.name}</span>`;
  }
  if (ast.type === 'lam') {
    const bodyStr = renderNamedHtml(ast.body, 0, highlights);
    const s = `<span class="${cls}" data-node-id="${ast.id}">λ${ast.param}.</span> ${bodyStr}`;
    return parentPrec > 0 ? `(${s})` : s;
  }
  if (ast.type === 'app') {
    const fnStr = renderNamedHtml(ast.fn, 1, highlights);
    const argStr = renderNamedHtml(ast.arg, 2, highlights);
    const s = `${fnStr} ${argStr}`;
    const wrapped = parentPrec > 1 ? `(${s})` : s;
    return `<span class="${cls}" data-node-id="${ast.id}">${wrapped}</span>`;
  }
  return '';
}

function termToPlainString(term) {
  if (!term) return '';
  const named = dbToNamedAST(term);
  function toStr(ast, prec = 0) {
    if (!ast) return '';
    if (ast.type === 'var') return ast.name;
    if (ast.type === 'lam') {
      const s = `λ${ast.param}. ${toStr(ast.body, 0)}`;
      return prec > 0 ? `(${s})` : s;
    }
    if (ast.type === 'app') {
      const fnStr = toStr(ast.fn, 1);
      const argStr = toStr(ast.arg, 2);
      const s = `${fnStr} ${argStr}`;
      return prec > 1 ? `(${s})` : s;
    }
    return '';
  }
  return toStr(named, 0);
}

function renderDeBruijnHtml(term, parentPrec = 0, highlights = {}) {
  const isRedex = highlights.redexId === term.id;
  const isTargetVar = highlights.targetVarIds && highlights.targetVarIds.includes(term.id);
  const isArg = highlights.argId === term.id;

  let cls = 'syntax-token';
  if (isRedex) cls += ' highlight-redex';
  if (isTargetVar) cls += ' highlight-target-var';
  if (isArg) cls += ' highlight-arg';

  if (term.type === 'db_var') {
    return `<span class="${cls}" data-node-id="${term.id}">${term.index + 1}</span>`;
  }
  if (term.type === 'db_lam') {
    const bodyStr = renderDeBruijnHtml(term.body, 0, highlights);
    const s = `<span class="${cls}" data-node-id="${term.id}">\\</span> ${bodyStr}`;
    return parentPrec > 0 ? `(${s})` : s;
  }
  if (term.type === 'db_app') {
    const fnStr = renderDeBruijnHtml(term.fn, 1, highlights);
    const argStr = renderDeBruijnHtml(term.arg, 2, highlights);
    const s = `${fnStr} ${argStr}`;
    const wrapped = parentPrec > 1 ? `(${s})` : s;
    return `<span class="${cls}" data-node-id="${term.id}">${wrapped}</span>`;
  }
  return '';
}
