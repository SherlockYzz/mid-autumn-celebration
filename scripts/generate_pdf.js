const { execSync } = require('child_process');
const path = require('path');
const fs = require('fs');

const edgePath = 'C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe';
const htmlPath = path.resolve('docs/两仪天工_智算文心_参赛项目企划书.html');
const pdfPath = path.resolve('docs/两仪天工_智算文心_参赛项目企划书.pdf');

console.log('Generating PDF from:', htmlPath);
console.log('Target PDF:', pdfPath);

const cmd = `"${edgePath}" --headless=new --disable-gpu --no-pdf-header-footer --print-to-pdf="${pdfPath}" "file:///${htmlPath.replace(/\\/g, '/')}"`;
execSync(cmd, { stdio: 'inherit' });

if (fs.existsSync(pdfPath)) {
  const stats = fs.statSync(pdfPath);
  console.log(`PDF created successfully: ${stats.size} bytes`);
  const buf = fs.readFileSync(pdfPath);
  const content = buf.toString('latin1');
  const pageMatches = content.match(/\/Type\s*\/Page\b/g);
  console.log('Total pages:', pageMatches ? pageMatches.length : 'Unknown');
} else {
  console.error('PDF creation failed!');
  process.exit(1);
}
