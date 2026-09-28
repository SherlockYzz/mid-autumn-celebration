// tests/test_unified_server.js - 全栈统一系统双引擎自动化集成回归测试
const http = require('http');
const server = require('../server/server.js');

const TEST_PORT = 3005;

function request(options, postData = null) {
  return new Promise((resolve, reject) => {
    const req = http.request(options, (res) => {
      let data = '';
      res.on('data', chunk => data += chunk);
      res.on('end', () => {
        try {
          resolve({ status: res.statusCode, headers: res.headers, body: JSON.parse(data) });
        } catch {
          resolve({ status: res.statusCode, headers: res.headers, body: data });
        }
      });
    });
    req.on('error', reject);
    if (postData) {
      req.write(typeof postData === 'string' ? postData : JSON.stringify(postData));
    }
    req.end();
  });
}

async function runTests() {
  console.log('🚀 开始执行《两仪天工 · 智算文心》全栈集成自动化回归测试...\n');
  let passed = 0;
  let failed = 0;

  function assert(condition, name) {
    if (condition) {
      console.log(`  ✅ [PASS] ${name}`);
      passed++;
    } else {
      console.error(`  ❌ [FAIL] ${name}`);
      failed++;
    }
  }

  // 1. 启动测试服务器
  await new Promise(resolve => server.listen(TEST_PORT, resolve));
  console.log(`📡 测试服务器已就绪: http://localhost:${TEST_PORT}\n`);

  try {
    // Test 1: 静态主页加载
    const homeRes = await request({ hostname: 'localhost', port: TEST_PORT, path: '/', method: 'GET' });
    assert(homeRes.status === 200 && typeof homeRes.body === 'string' && homeRes.body.includes('两仪天工 · 智算文心'), '根路由服务返回统一入口主页且包含两仪天工');

    // Test 2: Lambda 工坊静态页面加载
    const lambdaRes = await request({ hostname: 'localhost', port: TEST_PORT, path: '/lambda/index.html', method: 'GET' });
    assert(lambdaRes.status === 200 && typeof lambdaRes.body === 'string' && lambdaRes.body.includes('John Tromp'), 'Lambda 拓扑工坊独立页面静态资源正常挂载');

    // Test 3: 公开 Lambda 算筹仓库预设检索
    const publicReposRes = await request({ hostname: 'localhost', port: TEST_PORT, path: '/api/repo/public', method: 'GET' });
    assert(publicReposRes.status === 200 && Array.isArray(publicReposRes.body.items) && publicReposRes.body.items.length >= 5, '公共算筹公式库预设初始种子已正确入库');

    // Test 4: 用户注册
    const testUsername = `user_${Date.now()}`;
    const regRes = await request({
      hostname: 'localhost', port: TEST_PORT, path: '/api/auth/register', method: 'POST',
      headers: { 'Content-Type': 'application/json' }
    }, { username: testUsername, password: 'password123', nickname: '两仪雅客' });
    assert(regRes.status === 201 && regRes.body.token, '统一用户认证体系：注册成功并签发令牌');
    const token = regRes.body.token;

    // Test 5: 用户信息鉴权与双引擎计数回显
    const meRes = await request({
      hostname: 'localhost', port: TEST_PORT, path: '/api/auth/me', method: 'GET',
      headers: { 'Authorization': `Bearer ${token}` }
    });
    assert(meRes.status === 200 && meRes.body.user.username === testUsername && typeof meRes.body.user.repoCount === 'number', '统一用户会话：正确回显个人账户与算筹仓库计数');

    // Test 6: 在同一账户下创建 Lambda 算筹
    const createRepoRes = await request({
      hostname: 'localhost', port: TEST_PORT, path: '/api/repo', method: 'POST',
      headers: { 'Authorization': `Bearer ${token}`, 'Content-Type': 'application/json' }
    }, {
      title: '测试丘奇加法算筹',
      code: '(\\m. \\n. \\f. \\x. m f (n f x)) (\\f. \\x. f (f x)) (\\f. \\x. f x)',
      mode: 'classic',
      desc: '回归测试算筹项',
      tags: ['test', 'church'],
      isPublic: true
    });
    assert(createRepoRes.status === 201 && createRepoRes.body.item && createRepoRes.body.item.id, '算筹仓库模块：保存自定义 Lambda 拓扑公式成功');
    const createdRepoId = createRepoRes.body.item ? createRepoRes.body.item.id : null;

    // Test 7: 在同一账户下创建华彩岁时项目
    const createProjRes = await request({
      hostname: 'localhost', port: TEST_PORT, path: '/api/projects', method: 'POST',
      headers: { 'Authorization': `Bearer ${token}`, 'Content-Type': 'application/json' }
    }, {
      title: '两仪岁时雅集',
      festivalId: 'mid_autumn',
      description: '双引擎合并测试项目',
      config: { banner: '祝天下华人阖家团圆' }
    });
    assert(createProjRes.status === 201 && createProjRes.body.project && createProjRes.body.project.id, '岁时庆典模块：保存自定节日工程成功');

    // Test 8: AI 智能体传统文化决策
    const agentCultureRes = await request({
      hostname: 'localhost', port: TEST_PORT, path: '/api/agent/chat', method: 'POST',
      headers: { 'Authorization': `Bearer ${token}`, 'Content-Type': 'application/json' }
    }, { prompt: '请为中秋节作一副对联并引燃烟花', festival: 'mid_autumn' });
    assert(agentCultureRes.status === 200 && agentCultureRes.body.success && agentCultureRes.body.actions.some(a => a.type === 'update_couplets'), '智能体决策：传统文化楹联与烟花调度成功');

    // Test 9: AI 智能体数理形式化拓扑与天象演算决策 (跨模块联动)
    const agentMathRes = await request({
      hostname: 'localhost', port: TEST_PORT, path: '/api/agent/chat', method: 'POST',
      headers: { 'Authorization': `Bearer ${token}`, 'Content-Type': 'application/json' }
    }, { prompt: '帮我计算 2 + 1 的 Lambda 演算，并在天象中展示' });
    const hasVpSwitch = agentMathRes.body.actions && agentMathRes.body.actions.some(a => a.type === 'switch_viewport' && a.payload === 'celestial');
    const hasReduction = agentMathRes.body.actions && agentMathRes.body.actions.some(a => a.type === 'run_celestial_reduction');
    assert(agentMathRes.status === 200 && hasVpSwitch && hasReduction, '智能体决策：数理计算意图识别并自动调度【星汉算筹天象视口】');

    // Test 10: 算筹清理
    if (createdRepoId) {
      const delRepoRes = await request({
        hostname: 'localhost', port: TEST_PORT, path: `/api/repo/${createdRepoId}`, method: 'DELETE',
        headers: { 'Authorization': `Bearer ${token}` }
      });
      assert(delRepoRes.status === 200 && delRepoRes.body.success, '算筹仓库模块：生命周期清理删除完成');
    }

  } catch (err) {
    console.error('测试运行异常:', err);
    failed++;
  } finally {
    await new Promise(resolve => server.close(resolve));
    console.log(`\n=================================================`);
    console.log(`测试完成: 通过 ${passed} 项，失败 ${failed} 项`);
    console.log(`=================================================`);
    process.exit(failed > 0 ? 1 : 0);
  }
}

runTests();
