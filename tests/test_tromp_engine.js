const fs = require('fs');

// In node vm, declare var window = this;
const code1 = fs.readFileSync('lambda/js/ast.js', 'utf8');
const code2 = fs.readFileSync('lambda/js/reduction.js', 'utf8');

const combined = `
var window = this;
${code1}
${code2}
exports.parseNamedLambda = parseNamedLambda;
exports.convertNamedToDB = convertNamedToDB;
exports.prepareReductionStep = prepareReductionStep;
exports.termToPlainString = termToPlainString;
`;

const mod = {};
const fn = new Function('exports', combined);
fn(mod);

console.log('Testing Tromp Engine in Node...');
const parsedAst = mod.parseNamedLambda('(\\\\x. x) y');
console.log('Parsed AST:', parsedAst.type);
const dbTerm = mod.convertNamedToDB(parsedAst);
console.log('dbTerm:', JSON.stringify(dbTerm, null, 2));
const stepMeta = mod.prepareReductionStep(dbTerm);
console.log('stepMeta keys:', Object.keys(stepMeta));
console.log('termAfter:', JSON.stringify(stepMeta.termAfter, null, 2));
console.log('phases count:', stepMeta.phases ? stepMeta.phases.length : 0);
if (stepMeta.phases) {
  console.log('phase 1:', stepMeta.phases[0].narrativeZh);
}
