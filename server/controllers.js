// server/controllers.js - 业务逻辑与记录管理
const { db } = require('./db.js');
const { addExperience } = require('./auth.js');

// 12款传统文化成就系统定义
const ACHIEVEMENTS_DEF = [
  { id: 'first_firework', title: '初试锋芒', desc: '点燃第一枚节日庆典烟花', icon: '🎆', exp: 30 },
  { id: 'grand_celebration', title: '万象共鸣', desc: '触发一次全景大盛典 [ENTER]', icon: '🌟', exp: 80 },
  { id: 'divine_fortune', title: '蟾宫折桂', desc: '完成一次岁时祈福摇签', icon: '🎋', exp: 50 },
  { id: 'riddle_scholar', title: '灯市魁首', desc: '答对任意 3 道灯谜或传统文化问答', icon: '🏮', exp: 100 },
  { id: 'wish_sent', title: '天灯寄情', desc: '放飞一盏祈愿天灯或水云灯', icon: '🎐', exp: 40 },
  { id: 'custom_project', title: '鲁班巧匠', desc: '在庆典工坊创建并激活一个自定义项目', icon: '🏛️', exp: 100 },
  { id: 'heat_master', title: '热度如潮', desc: '贡献超过 1000 点节庆热度', icon: '🔥', exp: 150 },
  { id: 'spring_lion', title: '金狮献瑞', desc: '唤醒新春瑞狮腾跃起舞', icon: '🦁', exp: 40 },
  { id: 'national_drum', title: '山河雷动', desc: '擂响国庆山河金鼓', icon: '🥁', exp: 40 },
  { id: 'lantern_carp', title: '鱼跃龙门', desc: '引动上元金鳞锦鲤腾跃', icon: '🎏', exp: 40 },
  { id: 'couplet_master', title: '妙笔生花', desc: '更换并挂起自选吉祥对联', icon: '📜', exp: 50 },
  { id: 'festive_master', title: '四海同春', desc: '游历体验全部 4 大节日主题', icon: '👑', exp: 200 }
];

// ========== 项目管理模块 ==========
function getProjects({ userId = null, publicOnly = true, limit = 50, search = '' }) {
  let query = `
    SELECT p.*, u.nickname as author_nickname, u.avatar as author_avatar
    FROM projects p
    LEFT JOIN users u ON p.user_id = u.id
    WHERE 1=1
  `;
  const params = [];

  if (userId) {
    query += ` AND p.user_id = ?`;
    params.push(userId);
  } else if (publicOnly) {
    query += ` AND p.is_public = 1`;
  }

  if (search) {
    query += ` AND (p.title LIKE ? OR p.description LIKE ?)`;
    params.push(`%${search}%`, `%${search}%`);
  }

  query += ` ORDER BY p.id DESC LIMIT ?`;
  params.push(limit);

  const rows = db.prepare(query).all(...params);
  return rows.map(r => {
    let parsedConfig = {};
    try {
      parsedConfig = JSON.parse(r.config_json);
    } catch (e) {
      parsedConfig = {};
    }
    return {
      id: r.id,
      userId: r.user_id,
      title: r.title,
      festivalId: r.festival_id,
      description: r.description,
      config: parsedConfig,
      isPublic: Boolean(r.is_public),
      views: r.views,
      likes: r.likes,
      authorNickname: r.author_nickname || '匿名雅士',
      authorAvatar: r.author_avatar || '玉兔',
      createdAt: r.created_at,
      updatedAt: r.updated_at
    };
  });
}

function getProjectById(projectId) {
  const row = db.prepare(`
    SELECT p.*, u.nickname as author_nickname, u.avatar as author_avatar
    FROM projects p
    LEFT JOIN users u ON p.user_id = u.id
    WHERE p.id = ?
  `).get(projectId);

  if (!row) return null;

  // 增加浏览量
  db.prepare('UPDATE projects SET views = views + 1 WHERE id = ?').run(projectId);

  let parsedConfig = {};
  try {
    parsedConfig = JSON.parse(row.config_json);
  } catch (e) {}

  return {
    id: row.id,
    userId: row.user_id,
    title: row.title,
    festivalId: row.festival_id,
    description: row.description,
    config: parsedConfig,
    isPublic: Boolean(row.is_public),
    views: row.views + 1,
    likes: row.likes,
    authorNickname: row.author_nickname || '匿名雅士',
    authorAvatar: row.author_avatar || '玉兔',
    createdAt: row.created_at,
    updatedAt: row.updated_at
  };
}

function createProject(user, { title, festivalId, description, config }) {
  if (!title || !festivalId) {
    throw new Error('项目名称与所属节日不能为空');
  }

  const now = new Date().toISOString();
  const configStr = typeof config === 'string' ? config : JSON.stringify(config || {});

  const result = db.prepare(`
    INSERT INTO projects (user_id, title, festival_id, description, config_json, is_public, views, likes, created_at, updated_at)
    VALUES (?, ?, ?, ?, ?, 1, 1, 0, ?, ?)
  `).run(user.id, title.trim(), festivalId, description || '', configStr, now, now);

  const projectId = Number(result.lastInsertRowid);

  // 经验与成就触发
  addExperience(user.id, 80);
  unlockAchievement(user.id, 'custom_project');

  return getProjectById(projectId);
}

function updateProject(user, projectId, { title, festivalId, description, config, isPublic }) {
  const existing = db.prepare('SELECT user_id FROM projects WHERE id = ?').get(projectId);
  if (!existing) {
    throw new Error('未找到指定项目');
  }
  if (existing.user_id !== user.id) {
    throw new Error('无权限修改此项目');
  }

  const updates = [];
  const params = [];

  if (title) {
    updates.push('title = ?');
    params.push(title.trim());
  }
  if (festivalId) {
    updates.push('festival_id = ?');
    params.push(festivalId);
  }
  if (description !== undefined) {
    updates.push('description = ?');
    params.push(description);
  }
  if (config !== undefined) {
    updates.push('config_json = ?');
    params.push(typeof config === 'string' ? config : JSON.stringify(config));
  }
  if (isPublic !== undefined) {
    updates.push('is_public = ?');
    params.push(isPublic ? 1 : 0);
  }

  updates.push('updated_at = ?');
  params.push(new Date().toISOString());

  params.push(projectId);
  db.prepare(`UPDATE projects SET ${updates.join(', ')} WHERE id = ?`).run(...params);

  return getProjectById(projectId);
}

function deleteProject(user, projectId) {
  const existing = db.prepare('SELECT user_id FROM projects WHERE id = ?').get(projectId);
  if (!existing) {
    throw new Error('未找到指定项目');
  }
  if (existing.user_id !== user.id) {
    throw new Error('无权限删除此项目');
  }
  db.prepare('DELETE FROM projects WHERE id = ?').run(projectId);
  return { success: true, message: '项目已成功删除' };
}

function likeProject(projectId) {
  db.prepare('UPDATE projects SET likes = likes + 1 WHERE id = ?').run(projectId);
  const row = db.prepare('SELECT likes FROM projects WHERE id = ?').get(projectId);
  return { likes: row ? row.likes : 0 };
}

// ========== 祈愿墙与心愿模块 ==========
function getWishes({ festivalId = null, limit = 50 }) {
  let query = 'SELECT * FROM wishes WHERE is_public = 1';
  const params = [];
  if (festivalId) {
    query += ' AND festival_id = ?';
    params.push(festivalId);
  }
  query += ' ORDER BY id DESC LIMIT ?';
  params.push(limit);

  return db.prepare(query).all(...params);
}

function getUserWishes(userId) {
  return db.prepare('SELECT * FROM wishes WHERE user_id = ? ORDER BY id DESC').all(userId);
}

function createWish(user, { festivalId, content, targetName }) {
  if (!content) throw new Error('心愿内容不能为空');
  const now = new Date().toISOString();
  const res = db.prepare(`
    INSERT INTO wishes (user_id, username, nickname, avatar, festival_id, content, target_name, likes, is_public, created_at)
    VALUES (?, ?, ?, ?, ?, ?, ?, 0, 1, ?)
  `).run(
    user ? user.id : null,
    user ? user.username : 'guest',
    user ? user.nickname : '游园客',
    user ? user.avatar : '玉兔',
    festivalId || 'mid_autumn',
    content.trim(),
    targetName ? targetName.trim() : '天下万家',
    now
  );

  if (user) {
    addExperience(user.id, 30);
    unlockAchievement(user.id, 'wish_sent');
  }

  // 增加全局心愿数
  db.prepare('UPDATE global_stats SET value = value + 1, updated_at = ? WHERE key = ?').run(now, 'total_wishes');

  return { id: Number(res.lastInsertRowid), success: true };
}

function likeWish(wishId) {
  db.prepare('UPDATE wishes SET likes = likes + 1 WHERE id = ?').run(wishId);
  const row = db.prepare('SELECT likes FROM wishes WHERE id = ?').get(wishId);
  return { likes: row ? row.likes : 0 };
}

// ========== 签文与卜卦记录 ==========
function recordDivination(user, { festivalId, slipTitle, poem, interpretation, buffName }) {
  const now = new Date().toISOString();
  const res = db.prepare(`
    INSERT INTO divinations (user_id, festival_id, slip_title, poem, interpretation, buff_name, created_at)
    VALUES (?, ?, ?, ?, ?, ?, ?)
  `).run(user.id, festivalId || 'mid_autumn', slipTitle, poem, interpretation || '', buffName || '', now);

  addExperience(user.id, 30);
  unlockAchievement(user.id, 'divine_fortune');

  return { id: Number(res.lastInsertRowid), success: true };
}

function getUserDivinations(userId) {
  return db.prepare('SELECT * FROM divinations WHERE user_id = ? ORDER BY id DESC LIMIT 50').all(userId);
}

// ========== 灯谜与问答记录 ==========
function recordRiddle(user, { festivalId, riddleId, isCorrect, answerTimeMs }) {
  const now = new Date().toISOString();
  db.prepare(`
    INSERT INTO riddles (user_id, festival_id, riddle_id, is_correct, answer_time_ms, created_at)
    VALUES (?, ?, ?, ?, ?, ?)
  `).run(user.id, festivalId, riddleId, isCorrect ? 1 : 0, answerTimeMs || 0, now);

  if (isCorrect) {
    addExperience(user.id, 40);
    // 检查是否答对3道题解锁成就
    const correctCount = db.prepare('SELECT count(*) as count FROM riddles WHERE user_id = ? AND is_correct = 1').get(user.id).count;
    if (correctCount >= 3) {
      unlockAchievement(user.id, 'riddle_scholar');
    }
  }

  return { success: true };
}

function getUserRiddleStats(userId) {
  const total = db.prepare('SELECT count(*) as count FROM riddles WHERE user_id = ?').get(userId).count;
  const correct = db.prepare('SELECT count(*) as count FROM riddles WHERE user_id = ? AND is_correct = 1').get(userId).count;
  return {
    total,
    correct,
    accuracy: total > 0 ? Math.round((correct / total) * 100) : 0
  };
}

// ========== 成就系统 ==========
function unlockAchievement(userId, achievementId) {
  const def = ACHIEVEMENTS_DEF.find(a => a.id === achievementId);
  if (!def) return null;

  try {
    const now = new Date().toISOString();
    db.prepare(`
      INSERT INTO achievements (user_id, achievement_id, unlocked_at)
      VALUES (?, ?, ?)
    `).run(userId, achievementId, now);

    addExperience(userId, def.exp);
    return { unlocked: true, achievement: def };
  } catch (err) {
    // 已经解锁过
    return { unlocked: false, reason: 'already_unlocked' };
  }
}

function getUserAchievements(userId) {
  const unlockedRows = db.prepare('SELECT achievement_id, unlocked_at FROM achievements WHERE user_id = ?').all(userId);
  const unlockedMap = new Map(unlockedRows.map(r => [r.achievement_id, r.unlocked_at]));

  return ACHIEVEMENTS_DEF.map(def => ({
    ...def,
    unlocked: unlockedMap.has(def.id),
    unlockedAt: unlockedMap.get(def.id) || null
  }));
}

// ========== 全局热度与统计 ==========
function getGlobalStats() {
  const rows = db.prepare('SELECT key, value FROM global_stats').all();
  const stats = {};
  for (const r of rows) {
    stats[r.key] = r.value;
  }
  return {
    totalHeat: stats.total_heat || 666,
    totalWishes: stats.total_wishes || 0,
    totalFireworks: stats.total_fireworks || 0
  };
}

function addHeat(amount = 20) {
  const now = new Date().toISOString();
  db.prepare('UPDATE global_stats SET value = value + ?, updated_at = ? WHERE key = ?').run(amount, now, 'total_heat');
  const row = db.prepare('SELECT value FROM global_stats WHERE key = ?').get('total_heat');
  return { totalHeat: row ? row.value : 666 };
}

module.exports = {
  ACHIEVEMENTS_DEF,
  getProjects,
  getProjectById,
  createProject,
  updateProject,
  deleteProject,
  likeProject,
  getWishes,
  getUserWishes,
  createWish,
  likeWish,
  recordDivination,
  getUserDivinations,
  recordRiddle,
  getUserRiddleStats,
  unlockAchievement,
  getUserAchievements,
  getGlobalStats,
  addHeat
};
