// server/auth.js - 用户身份认证与会话管理
const crypto = require('node:crypto');
const { db, hashPassword } = require('./db.js');

const EXP_TITLES = [
  { minExp: 0, level: 1, title: '岁时童生' },
  { minExp: 100, level: 2, title: '踏月游侠' },
  { minExp: 300, level: 3, title: '月华雅士' },
  { minExp: 600, level: 4, title: '华夏鸿儒' },
  { minExp: 1000, level: 5, title: '锦绣学士' },
  { minExp: 1500, level: 6, title: '新春福星' },
  { minExp: 2200, level: 7, title: '山河令使' },
  { minExp: 3000, level: 8, title: '上元灯魁' },
  { minExp: 4000, level: 9, title: '万象宗师' },
  { minExp: 5000, level: 10, title: '岁时仙尊' }
];

function calculateLevelAndTitle(exp) {
  let current = EXP_TITLES[0];
  for (const tier of EXP_TITLES) {
    if (exp >= tier.minExp) {
      current = tier;
    } else {
      break;
    }
  }
  return { level: current.level, title: current.title };
}

function generateToken() {
  return crypto.randomBytes(32).toString('hex');
}

function registerUser({ username, password, nickname, avatar }) {
  if (!username || !password || !nickname) {
    throw new Error('用户名、密码和昵称不能为空');
  }
  if (username.length < 3) {
    throw new Error('用户名长度至少为 3 个字符');
  }
  if (password.length < 6) {
    throw new Error('密码长度至少为 6 个字符');
  }

  const existing = db.prepare('SELECT id FROM users WHERE username = ?').get(username);
  if (existing) {
    throw new Error('该用户名已被注册，请尝试其他用户名');
  }

  const salt = crypto.randomBytes(16).toString('hex');
  const passwordHash = hashPassword(password, salt);
  const now = new Date().toISOString();
  const avatarChoice = avatar || '玉兔';

  const result = db.prepare(`
    INSERT INTO users (username, password_hash, salt, nickname, avatar, title, exp, level, created_at, last_login_at)
    VALUES (?, ?, ?, ?, ?, '岁时童生', 0, 1, ?, ?)
  `).run(username, passwordHash, salt, nickname, avatarChoice, now, now);

  const userId = Number(result.lastInsertRowid);
  const token = generateToken();
  const expiresAt = new Date(Date.now() + 30 * 86400000).toISOString(); // 30天有效

  db.prepare(`
    INSERT INTO sessions (token, user_id, expires_at, created_at)
    VALUES (?, ?, ?, ?)
  `).run(token, userId, expiresAt, now);

  return {
    token,
    user: {
      id: userId,
      username,
      nickname,
      avatar: avatarChoice,
      title: '岁时童生',
      exp: 0,
      level: 1
    }
  };
}

function loginUser({ username, password }) {
  if (!username || !password) {
    throw new Error('用户名与密码不能为空');
  }

  const user = db.prepare('SELECT * FROM users WHERE username = ?').get(username);
  if (!user) {
    throw new Error('用户不存在或密码错误');
  }

  const calculatedHash = hashPassword(password, user.salt);
  if (calculatedHash !== user.password_hash) {
    throw new Error('用户不存在或密码错误');
  }

  const now = new Date().toISOString();
  db.prepare('UPDATE users SET last_login_at = ? WHERE id = ?').run(now, user.id);

  const token = generateToken();
  const expiresAt = new Date(Date.now() + 30 * 86400000).toISOString();

  db.prepare(`
    INSERT INTO sessions (token, user_id, expires_at, created_at)
    VALUES (?, ?, ?, ?)
  `).run(token, user.id, expiresAt, now);

  return {
    token,
    user: {
      id: user.id,
      username: user.username,
      nickname: user.nickname,
      avatar: user.avatar,
      title: user.title,
      exp: user.exp,
      level: user.level
    }
  };
}

function guestLogin() {
  const guestRand = crypto.randomBytes(3).toString('hex');
  const username = `guest_${guestRand}`;
  const nickname = `游园客_${guestRand.slice(0, 4).toUpperCase()}`;
  const avatars = ['玉兔', '瑞狮', '金鲤', '灵鹿'];
  const avatar = avatars[Math.floor(Math.random() * avatars.length)];
  const salt = crypto.randomBytes(8).toString('hex');
  const passwordHash = hashPassword(guestRand, salt);
  const now = new Date().toISOString();

  const result = db.prepare(`
    INSERT INTO users (username, password_hash, salt, nickname, avatar, title, exp, level, created_at, last_login_at)
    VALUES (?, ?, ?, ?, ?, '岁时童生', 60, 1, ?, ?)
  `).run(username, passwordHash, salt, nickname, avatar, now, now);

  const userId = Number(result.lastInsertRowid);
  const token = generateToken();
  const expiresAt = new Date(Date.now() + 7 * 86400000).toISOString();

  db.prepare(`
    INSERT INTO sessions (token, user_id, expires_at, created_at)
    VALUES (?, ?, ?, ?)
  `).run(token, userId, expiresAt, now);

  return {
    token,
    isGuest: true,
    user: {
      id: userId,
      username,
      nickname,
      avatar,
      title: '岁时童生',
      exp: 60,
      level: 1
    }
  };
}

function authenticateToken(token) {
  if (!token) return null;
  const session = db.prepare(`
    SELECT s.*, u.id as u_id, u.username, u.nickname, u.avatar, u.title, u.exp, u.level
    FROM sessions s
    JOIN users u ON s.user_id = u.id
    WHERE s.token = ?
  `).get(token);

  if (!session) return null;

  if (new Date(session.expires_at) < new Date()) {
    db.prepare('DELETE FROM sessions WHERE token = ?').run(token);
    return null;
  }

  return {
    id: session.u_id,
    username: session.username,
    nickname: session.nickname,
    avatar: session.avatar,
    title: session.title,
    exp: session.exp,
    level: session.level
  };
}

function addExperience(userId, expToAdd) {
  const user = db.prepare('SELECT exp, level, title FROM users WHERE id = ?').get(userId);
  if (!user) return null;

  const newExp = user.exp + expToAdd;
  const { level, title } = calculateLevelAndTitle(newExp);

  db.prepare('UPDATE users SET exp = ?, level = ?, title = ? WHERE id = ?').run(newExp, level, title, userId);

  return {
    userId,
    newExp,
    level,
    title,
    leveledUp: level > user.level
  };
}

function updateProfile(userId, { nickname, avatar }) {
  const updates = [];
  const params = [];
  if (nickname) {
    updates.push('nickname = ?');
    params.push(nickname.trim());
  }
  if (avatar) {
    updates.push('avatar = ?');
    params.push(avatar.trim());
  }
  if (updates.length === 0) return null;

  params.push(userId);
  db.prepare(`UPDATE users SET ${updates.join(', ')} WHERE id = ?`).run(...params);

  const updated = db.prepare('SELECT id, username, nickname, avatar, title, exp, level FROM users WHERE id = ?').get(userId);
  return updated;
}

module.exports = {
  registerUser,
  loginUser,
  guestLogin,
  authenticateToken,
  addExperience,
  updateProfile,
  calculateLevelAndTitle
};
