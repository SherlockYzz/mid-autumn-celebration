// server/repo_controller.js - John Tromp Lambda 形式化拓扑算筹仓库控制器
const { db } = require('./db.js');

function getPublicRepos({ search = '', tag = '' } = {}) {
  let query = `
    SELECT r.*, u.username as author_username, u.nickname as author_nickname, u.avatar as author_avatar
    FROM repos r
    LEFT JOIN users u ON r.user_id = u.id
    WHERE r.is_public = 1
  `;
  const params = [];

  if (search) {
    query += ` AND (r.title LIKE ? OR r.code LIKE ? OR r.description LIKE ?)`;
    params.push(`%${search}%`, `%${search}%`, `%${search}%`);
  }

  query += ` ORDER BY r.updated_at DESC LIMIT 100`;

  const rows = db.prepare(query).all(...params);
  return rows.map(r => formatRepoRow(r, tag)).filter(Boolean);
}

function getUserRepos(userId, { search = '', tag = '', mode = 'all' } = {}) {
  let query = `
    SELECT r.*, u.username as author_username, u.nickname as author_nickname, u.avatar as author_avatar
    FROM repos r
    LEFT JOIN users u ON r.user_id = u.id
    WHERE r.user_id = ?
  `;
  const params = [userId];

  if (mode !== 'all') {
    query += ` AND r.mode = ?`;
    params.push(mode);
  }

  if (search) {
    query += ` AND (r.title LIKE ? OR r.code LIKE ? OR r.description LIKE ?)`;
    params.push(`%${search}%`, `%${search}%`, `%${search}%`);
  }

  query += ` ORDER BY r.updated_at DESC`;

  const rows = db.prepare(query).all(...params);
  return rows.map(r => formatRepoRow(r, tag)).filter(Boolean);
}

function getRepoById(id) {
  const row = db.prepare(`
    SELECT r.*, u.username as author_username, u.nickname as author_nickname, u.avatar as author_avatar
    FROM repos r
    LEFT JOIN users u ON r.user_id = u.id
    WHERE r.id = ?
  `).get(id);

  if (!row) return null;
  return formatRepoRow(row);
}

function createRepo({ userId, title, code, mode = 'classic', desc = '', tags = [], isPublic = true }) {
  const id = `repo_${Date.now()}_${Math.random().toString(36).substring(2, 7)}`;
  const now = new Date().toISOString();
  const tagsJson = JSON.stringify(Array.isArray(tags) ? tags : []);

  db.prepare(`
    INSERT INTO repos (id, user_id, title, code, mode, description, tags, is_public, created_at, updated_at)
    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
  `).run(
    id,
    userId,
    title.trim() || '未命名 Lambda 拓扑算筹',
    code.trim(),
    mode || 'classic',
    desc || '',
    tagsJson,
    isPublic ? 1 : 0,
    now,
    now
  );

  return getRepoById(id);
}

function updateRepo(id, userId, updates = {}) {
  const existing = getRepoById(id);
  if (!existing || existing.userId !== userId) {
    return null;
  }

  const title = updates.title !== undefined ? updates.title.trim() : existing.title;
  const code = updates.code !== undefined ? updates.code.trim() : existing.code;
  const mode = updates.mode !== undefined ? updates.mode : existing.mode;
  const desc = updates.desc !== undefined ? updates.desc : existing.desc;
  const tags = updates.tags !== undefined ? JSON.stringify(updates.tags) : JSON.stringify(existing.tags);
  const isPublic = updates.isPublic !== undefined ? (updates.isPublic ? 1 : 0) : (existing.isPublic ? 1 : 0);
  const now = new Date().toISOString();

  db.prepare(`
    UPDATE repos
    SET title = ?, code = ?, mode = ?, description = ?, tags = ?, is_public = ?, updated_at = ?
    WHERE id = ? AND user_id = ?
  `).run(title, code, mode, desc, tags, isPublic, now, id, userId);

  return getRepoById(id);
}

function deleteRepo(id, userId) {
  const existing = getRepoById(id);
  if (!existing || existing.userId !== userId) {
    return false;
  }

  const res = db.prepare('DELETE FROM repos WHERE id = ? AND user_id = ?').run(id, userId);
  return res.changes > 0;
}

function countUserRepos(userId) {
  const row = db.prepare('SELECT count(*) as count FROM repos WHERE user_id = ?').get(userId);
  return row ? row.count : 0;
}

function importRepos(userId, items = []) {
  const imported = [];
  for (const item of items) {
    if (!item.code) continue;
    const created = createRepo({
      userId,
      title: item.title || '导入项',
      code: item.code,
      mode: item.mode || 'classic',
      desc: item.desc || '',
      tags: item.tags || [],
      isPublic: item.isPublic !== false
    });
    imported.push(created);
  }
  return imported;
}

function formatRepoRow(row, filterTag = '') {
  let tagsArr = [];
  try {
    tagsArr = JSON.parse(row.tags || '[]');
  } catch (e) {
    tagsArr = [];
  }

  if (filterTag && filterTag !== 'all' && !tagsArr.includes(filterTag)) {
    return null;
  }

  return {
    id: row.id,
    userId: row.user_id,
    title: row.title,
    code: row.code,
    mode: row.mode,
    desc: row.description || '',
    tags: tagsArr,
    isPublic: Boolean(row.is_public),
    authorName: row.author_nickname || row.author_username || '匿名行者',
    authorAvatar: row.author_avatar || '玉兔',
    createdAt: row.created_at,
    updatedAt: row.updated_at
  };
}

module.exports = {
  getPublicRepos,
  getUserRepos,
  getRepoById,
  createRepo,
  updateRepo,
  deleteRepo,
  countUserRepos,
  importRepos
};
