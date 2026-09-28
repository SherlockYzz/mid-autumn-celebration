// server/server.js - 全栈 HTTP 服务端与 RESTful API
const http = require('node:http');
const fs = require('node:fs');
const path = require('node:path');
const url = require('node:url');

const auth = require('./auth.js');
const controllers = require('./controllers.js');
const agent = require('./agent.js');

const PORT = process.env.PORT || 3000;
const ROOT_DIR = path.resolve(__dirname, '..');

// 辅助函数：发送 JSON 响应
function sendJson(res, statusCode, data) {
  res.writeHead(statusCode, {
    'Content-Type': 'application/json; charset=utf-8',
    'Access-Control-Allow-Origin': '*',
    'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, OPTIONS',
    'Access-Control-Allow-Headers': 'Content-Type, Authorization'
  });
  res.end(JSON.stringify(data));
}

// 辅助函数：解析请求体 JSON
function parseJsonBody(req) {
  return new Promise((resolve, reject) => {
    let body = '';
    req.on('data', chunk => {
      body += chunk;
      if (body.length > 2 * 1024 * 1024) { // 限制 2MB
        reject(new Error('Payload too large'));
      }
    });
    req.on('end', () => {
      if (!body) return resolve({});
      try {
        resolve(JSON.parse(body));
      } catch (err) {
        reject(new Error('Invalid JSON'));
      }
    });
    req.on('error', reject);
  });
}

// 辅助函数：提取鉴权用户
function getAuthUser(req) {
  const authHeader = req.headers['authorization'] || '';
  if (!authHeader.startsWith('Bearer ')) return null;
  const token = authHeader.slice(7).trim();
  return auth.authenticateToken(token);
}

// 辅助函数：静态文件服务
function serveStaticFile(req, res, filePath) {
  const mimeTypes = {
    '.html': 'text/html; charset=utf-8',
    '.js': 'text/javascript; charset=utf-8',
    '.css': 'text/css; charset=utf-8',
    '.json': 'application/json; charset=utf-8',
    '.png': 'image/png',
    '.jpg': 'image/jpeg',
    '.gif': 'image/gif',
    '.svg': 'image/svg+xml',
    '.ico': 'image/x-icon',
    '.wav': 'audio/wav',
    '.mp3': 'audio/mpeg'
  };

  const ext = path.extname(filePath).toLowerCase();
  const contentType = mimeTypes[ext] || 'application/octet-stream';

  fs.readFile(filePath, (err, content) => {
    if (err) {
      if (err.code === 'ENOENT') {
        res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('404 Not Found');
      } else {
        res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('500 Server Internal Error');
      }
      return;
    }
    res.writeHead(200, {
      'Content-Type': contentType,
      'Cache-Control': 'no-cache',
      'Access-Control-Allow-Origin': '*'
    });
    res.end(content);
  });
}

const server = http.createServer(async (req, res) => {
  // 处理跨域预检请求
  if (req.method === 'OPTIONS') {
    res.writeHead(204, {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, OPTIONS',
      'Access-Control-Allow-Headers': 'Content-Type, Authorization',
      'Access-Control-Max-Age': '86400'
    });
    res.end();
    return;
  }

  const requestUrl = new URL(req.url, `http://${req.headers.host || 'localhost'}`);
  const pathname = requestUrl.pathname;
  const searchParams = requestUrl.searchParams;

  // ========== API 路由处理 ==========
  if (pathname.startsWith('/api/')) {
    try {
      // 1. 认证模块
      if (pathname === '/api/auth/register' && req.method === 'POST') {
        const body = await parseJsonBody(req);
        const result = auth.registerUser(body);
        return sendJson(res, 201, { success: true, ...result });
      }

      if (pathname === '/api/auth/login' && req.method === 'POST') {
        const body = await parseJsonBody(req);
        const result = auth.loginUser(body);
        return sendJson(res, 200, { success: true, ...result });
      }

      if (pathname === '/api/auth/guest' && req.method === 'POST') {
        const result = auth.guestLogin();
        return sendJson(res, 200, { success: true, ...result });
      }

      if (pathname === '/api/auth/me' && req.method === 'GET') {
        const user = getAuthUser(req);
        if (!user) return sendJson(res, 401, { error: '未授权或登录已过期' });
        return sendJson(res, 200, { success: true, user });
      }

      if (pathname === '/api/auth/profile' && req.method === 'PUT') {
        const user = getAuthUser(req);
        if (!user) return sendJson(res, 401, { error: '请先登录' });
        const body = await parseJsonBody(req);
        const updated = auth.updateProfile(user.id, body);
        return sendJson(res, 200, { success: true, user: updated });
      }

      // 2. 庆典项目管理
      if (pathname === '/api/projects' && req.method === 'GET') {
        const user = getAuthUser(req);
        const mine = searchParams.get('mine') === 'true';
        const search = searchParams.get('search') || '';
        const limit = Number(searchParams.get('limit')) || 50;

        const projects = controllers.getProjects({
          userId: mine && user ? user.id : null,
          publicOnly: !mine,
          limit,
          search
        });
        return sendJson(res, 200, { success: true, projects });
      }

      if (pathname === '/api/projects' && req.method === 'POST') {
        const user = getAuthUser(req);
        if (!user) return sendJson(res, 401, { error: '请先登录或以游客体验创建项目' });
        const body = await parseJsonBody(req);
        const project = controllers.createProject(user, body);
        return sendJson(res, 201, { success: true, project });
      }

      const projectMatch = pathname.match(/^\/api\/projects\/(\d+)(\/(like))?$/);
      if (projectMatch) {
        const projectId = Number(projectMatch[1]);
        const subAction = projectMatch[3];

        if (subAction === 'like' && req.method === 'POST') {
          const result = controllers.likeProject(projectId);
          return sendJson(res, 200, { success: true, ...result });
        }

        if (req.method === 'GET') {
          const project = controllers.getProjectById(projectId);
          if (!project) return sendJson(res, 404, { error: '项目未找到' });
          return sendJson(res, 200, { success: true, project });
        }

        if (req.method === 'PUT') {
          const user = getAuthUser(req);
          if (!user) return sendJson(res, 401, { error: '未授权' });
          const body = await parseJsonBody(req);
          const updated = controllers.updateProject(user, projectId, body);
          return sendJson(res, 200, { success: true, project: updated });
        }

        if (req.method === 'DELETE') {
          const user = getAuthUser(req);
          if (!user) return sendJson(res, 401, { error: '未授权' });
          const result = controllers.deleteProject(user, projectId);
          return sendJson(res, 200, result);
        }
      }

      // 3. 祈愿心愿墙
      if (pathname === '/api/wishes' && req.method === 'GET') {
        const festivalId = searchParams.get('festivalId') || null;
        const wishes = controllers.getWishes({ festivalId });
        return sendJson(res, 200, { success: true, wishes });
      }

      if (pathname === '/api/wishes' && req.method === 'POST') {
        const user = getAuthUser(req);
        const body = await parseJsonBody(req);
        const result = controllers.createWish(user, body);
        return sendJson(res, 201, result);
      }

      const wishMatch = pathname.match(/^\/api\/wishes\/(\d+)\/like$/);
      if (wishMatch && req.method === 'POST') {
        const wishId = Number(wishMatch[1]);
        const result = controllers.likeWish(wishId);
        return sendJson(res, 200, { success: true, ...result });
      }

      // 4. 个人记录汇总 (签文、灯谜、成就)
      if (pathname === '/api/records/summary' && req.method === 'GET') {
        const user = getAuthUser(req);
        if (!user) return sendJson(res, 401, { error: '请先登录' });

        const divinations = controllers.getUserDivinations(user.id);
        const riddleStats = controllers.getUserRiddleStats(user.id);
        const achievements = controllers.getUserAchievements(user.id);
        const wishes = controllers.getUserWishes(user.id);
        const myProjects = controllers.getProjects({ userId: user.id, publicOnly: false });

        return sendJson(res, 200, {
          success: true,
          records: {
            user,
            divinations,
            riddleStats,
            achievements,
            wishes,
            myProjects
          }
        });
      }

      // 5. 单项记录打点
      if (pathname === '/api/records/divination' && req.method === 'POST') {
        const user = getAuthUser(req);
        const body = await parseJsonBody(req);
        if (user) {
          const resObj = controllers.recordDivination(user, body);
          return sendJson(res, 200, resObj);
        }
        return sendJson(res, 200, { success: true, guest: true });
      }

      if (pathname === '/api/records/riddle' && req.method === 'POST') {
        const user = getAuthUser(req);
        const body = await parseJsonBody(req);
        if (user) {
          const resObj = controllers.recordRiddle(user, body);
          return sendJson(res, 200, resObj);
        }
        return sendJson(res, 200, { success: true, guest: true });
      }

      if (pathname === '/api/records/achievement' && req.method === 'POST') {
        const user = getAuthUser(req);
        const body = await parseJsonBody(req);
        if (user && body.achievementId) {
          const resObj = controllers.unlockAchievement(user.id, body.achievementId);
          return sendJson(res, 200, resObj || { success: false });
        }
        return sendJson(res, 200, { success: false });
      }

      // 6. 热度与统计
      if (pathname === '/api/stats/heat' && req.method === 'GET') {
        const stats = controllers.getGlobalStats();
        return sendJson(res, 200, { success: true, stats });
      }

      if (pathname === '/api/stats/heat' && req.method === 'POST') {
        const body = await parseJsonBody(req);
        const delta = Number(body.delta) || 20;
        const result = controllers.addHeat(delta);
        return sendJson(res, 200, { success: true, ...result });
      }

      // 7. AI 岁时智能体交互接口
      if (pathname === '/api/agent/chat' && req.method === 'POST') {
        const user = getAuthUser(req);
        const body = await parseJsonBody(req);
        const result = await agent.processAgentQuery(body.prompt, {
          user,
          currentFestival: body.festival || 'mid_autumn'
        });
        return sendJson(res, 200, { success: true, ...result });
      }

      // 未匹配的 API 路径
      return sendJson(res, 404, { error: 'API Endpoint Not Found' });
    } catch (err) {
      console.error('API Error:', err);
      return sendJson(res, 400, { error: err.message || '服务器处理异常' });
    }
  }

  // ========== 静态页面与资源服务 ==========
  let decodedPathname = pathname;
  try {
    decodedPathname = decodeURIComponent(pathname);
  } catch (e) {}
  let filePath = path.join(ROOT_DIR, decodedPathname === '/' ? 'index.html' : decodedPathname);

  // 安全检查，防止路径遍历
  if (!filePath.startsWith(ROOT_DIR)) {
    res.writeHead(403, { 'Content-Type': 'text/plain; charset=utf-8' });
    res.end('403 Forbidden');
    return;
  }

  // 如果访问的是目录，则默认读取其 index.html
  if (fs.existsSync(filePath) && fs.statSync(filePath).isDirectory()) {
    filePath = path.join(filePath, 'index.html');
  }

  serveStaticFile(req, res, filePath);
});

server.listen(PORT, () => {
  console.log(`🏮 中华华节盛典 · 全栈服务已启动`);
  console.log(`🌐 访问地址: http://localhost:${PORT}`);
  console.log(`📡 数据库: SQLite 文件已挂载`);
});

// 优雅停机
process.on('SIGINT', () => {
  console.log('正在关闭服务...');
  server.close(() => {
    process.exit(0);
  });
});
