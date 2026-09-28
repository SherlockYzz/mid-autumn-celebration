// server/db.js - SQLite 数据库模块 (基于 Node.js 24 原生 node:sqlite)
const { DatabaseSync } = require('node:sqlite');
const path = require('node:path');
const fs = require('node:fs');
const crypto = require('node:crypto');

const DB_DIR = path.join(__dirname, 'data');
if (!fs.existsSync(DB_DIR)) {
  fs.mkdirSync(DB_DIR, { recursive: true });
}
const DB_PATH = path.join(DB_DIR, 'festival.db');

const db = new DatabaseSync(DB_PATH);

// 初始化数据库表结构
function initDatabase() {
  db.exec(`
    PRAGMA journal_mode = WAL;
    
    CREATE TABLE IF NOT EXISTS users (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      username TEXT UNIQUE NOT NULL,
      password_hash TEXT NOT NULL,
      salt TEXT NOT NULL,
      nickname TEXT NOT NULL,
      avatar TEXT DEFAULT '玉兔',
      title TEXT DEFAULT '岁时童生',
      exp INTEGER DEFAULT 0,
      level INTEGER DEFAULT 1,
      created_at TEXT NOT NULL,
      last_login_at TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS sessions (
      token TEXT PRIMARY KEY,
      user_id INTEGER NOT NULL,
      expires_at TEXT NOT NULL,
      created_at TEXT NOT NULL,
      FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
    );

    CREATE TABLE IF NOT EXISTS repos (
      id TEXT PRIMARY KEY,
      user_id INTEGER NOT NULL,
      title TEXT NOT NULL,
      code TEXT NOT NULL,
      mode TEXT DEFAULT 'classic',
      description TEXT,
      tags TEXT,
      is_public INTEGER DEFAULT 1,
      created_at TEXT NOT NULL,
      updated_at TEXT NOT NULL,
      FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
    );

    CREATE TABLE IF NOT EXISTS projects (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      user_id INTEGER NOT NULL,
      title TEXT NOT NULL,
      festival_id TEXT NOT NULL,
      description TEXT,
      config_json TEXT NOT NULL,
      is_public INTEGER DEFAULT 1,
      views INTEGER DEFAULT 0,
      likes INTEGER DEFAULT 0,
      created_at TEXT NOT NULL,
      updated_at TEXT NOT NULL,
      FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
    );

    CREATE TABLE IF NOT EXISTS wishes (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      user_id INTEGER,
      username TEXT,
      nickname TEXT NOT NULL,
      avatar TEXT DEFAULT '玉兔',
      festival_id TEXT NOT NULL,
      content TEXT NOT NULL,
      target_name TEXT DEFAULT '天下万家',
      likes INTEGER DEFAULT 0,
      is_public INTEGER DEFAULT 1,
      created_at TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS divinations (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      user_id INTEGER NOT NULL,
      festival_id TEXT NOT NULL,
      slip_title TEXT NOT NULL,
      poem TEXT NOT NULL,
      interpretation TEXT,
      buff_name TEXT,
      created_at TEXT NOT NULL,
      FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
    );

    CREATE TABLE IF NOT EXISTS riddles (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      user_id INTEGER NOT NULL,
      festival_id TEXT NOT NULL,
      riddle_id INTEGER NOT NULL,
      is_correct INTEGER NOT NULL,
      answer_time_ms INTEGER DEFAULT 0,
      created_at TEXT NOT NULL,
      FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
    );

    CREATE TABLE IF NOT EXISTS achievements (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      user_id INTEGER NOT NULL,
      achievement_id TEXT NOT NULL,
      unlocked_at TEXT NOT NULL,
      UNIQUE(user_id, achievement_id),
      FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
    );

    CREATE TABLE IF NOT EXISTS global_stats (
      key TEXT PRIMARY KEY,
      value INTEGER NOT NULL,
      updated_at TEXT NOT NULL
    );
  `);

  seedInitialData();
}

function hashPassword(password, salt) {
  return crypto.scryptSync(password, salt, 64).toString('hex');
}

function seedInitialData() {
  // 检查是否有演示账号，若无则初始化演示账号
  const checkUser = db.prepare('SELECT count(*) as count FROM users').get();
  if (checkUser.count === 0) {
    const demoSalt = crypto.randomBytes(16).toString('hex');
    const demoHash = hashPassword('123456', demoSalt);
    const now = new Date().toISOString();

    const insertUser = db.prepare(`
      INSERT INTO users (username, password_hash, salt, nickname, avatar, title, exp, level, created_at, last_login_at)
      VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    `);

    // 默认管理员/作者账号
    insertUser.run(
      'Sherlock',
      demoHash,
      demoSalt,
      '司岁御史 · 华夏客',
      '青龙',
      '万象宗师',
      4200,
      9,
      now,
      now
    );

    // 预设公共庆典项目
    const insertProject = db.prepare(`
      INSERT INTO projects (user_id, title, festival_id, description, config_json, is_public, views, likes, created_at, updated_at)
      VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    `);

    const midAutumnConfig = JSON.stringify({
      festival: 'mid_autumn',
      themeTitle: '【官方推荐】月华如愿 · 宵月游园雅集',
      bannerText: '祝天下华人中秋佳节团圆美满',
      coupletLeft: '满地月华涵太液',
      coupletRight: '一轮秋水照昆仑',
      lanternTheme: 'gold_purple',
      musicStyle: 'pentatonic_guzheng',
      customRiddles: ['云开月露（打一字）: 胆']
    });

    const nationalConfig = JSON.stringify({
      festival: 'national_day',
      themeTitle: '【盛世献礼】万里山河 · 国泰民安华彩展',
      bannerText: '祝愿祖国繁荣昌盛，山河锦绣！',
      coupletLeft: '九州日月开新纪',
      coupletRight: '万仞江山展画屏',
      lanternTheme: 'crimson_gold',
      musicStyle: 'brass_fanfare',
      customRiddles: ['千里共婵娟（打一字）: 仙']
    });

    const springConfig = JSON.stringify({
      festival: 'spring_festival',
      themeTitle: '【除夕贺岁】瑞狮腾骧 · 金玉满堂福星照',
      bannerText: '祝新春大吉，诸事顺遂，财源广进！',
      coupletLeft: '九州日丽迎新岁',
      coupletRight: '四海春温发泰亨',
      lanternTheme: 'vermilion',
      musicStyle: 'firecracker_drum',
      customRiddles: ['守岁到天明（打一成语）: 除旧迎新']
    });

    const lanternConfig = JSON.stringify({
      festival: 'lantern_festival',
      themeTitle: '【上元千灯】花市灯如昼 · 锦鲤跃波祈愿会',
      bannerText: '祝上元良宵，万家灯火通明，所求皆所愿！',
      coupletLeft: '玉宇无尘千顷碧',
      coupletRight: '银花有约万家春',
      lanternTheme: 'cyan_glow',
      musicStyle: 'chime_bells',
      customRiddles: ['月下故人来（打一字）: 倩']
    });

    insertProject.run(1, '月华如愿 · 宵月游园雅集', 'mid_autumn', '中秋追月经典游园雅集，融合灵兔捣药、古筝清音与万家灯火。', midAutumnConfig, 1, 128, 66, now, now);
    insertProject.run(1, '万里山河 · 国泰民安华彩展', 'national_day', '国庆盛世主题，三军礼炮齐鸣，气球长卷与天安门金光闪耀。', nationalConfig, 1, 356, 128, now, now);
    insertProject.run(1, '瑞狮腾骧 · 金玉满堂福星照', 'spring_festival', '除夕新春定制项目，活泼南狮跃起，自定千祥春联，财神元宝天降。', springConfig, 1, 240, 99, now, now);
    insertProject.run(1, '花市灯如昼 · 锦鲤跃波祈愿会', 'lantern_festival', '上元灯节水云灯随波漫游，双尾锦鲤腾江破浪，千灯祈福。', lanternConfig, 1, 180, 77, now, now);

    // 预设祈愿心愿墙
    const insertWish = db.prepare(`
      INSERT INTO wishes (user_id, username, nickname, avatar, festival_id, content, target_name, likes, is_public, created_at)
      VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    `);

    insertWish.run(1, 'Sherlock', '司岁御史 · 华夏客', '青龙', 'national_day', '愿祖国山河无恙，繁荣昌盛，四海同春！', '华夏神州', 88, 1, now);
    insertWish.run(1, 'Sherlock', '月下漫步者', '玉兔', 'mid_autumn', '愿远方家人平安健康，岁岁有今日，年年有明月。', '全家亲朋', 56, 1, now);
    insertWish.run(1, 'Sherlock', '春临九州', '瑞狮', 'spring_festival', '愿新的一年事业顺利，所得皆所愿，万事顺意！', '全体开发者', 72, 1, now);
    insertWish.run(1, 'Sherlock', '临水照花人', '金鲤', 'lantern_festival', '愿灯火长明，喜乐常伴，千里共团圆。', '挚友知音', 43, 1, now);
  }

  // 检查是否初始化 Lambda 形式化拓扑算筹预设
  const checkRepo = db.prepare('SELECT count(*) as count FROM repos').get();
  if (checkRepo.count === 0) {
    const now = new Date().toISOString();
    const insertRepo = db.prepare(`
      INSERT INTO repos (id, user_id, title, code, mode, description, tags, is_public, created_at, updated_at)
      VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    `);

    insertRepo.run('repo_church_two', 1, '丘奇自然数 2 (Church Two)', '\\f. \\x. f (f x)', 'classic', '高阶函数两次复合，形式化算术的基石', JSON.stringify(['church', 'arithmetic', 'classic']), 1, now, now);
    insertRepo.run('repo_ski_s', 1, 'S 组合子 (Substitution)', '\\x. \\y. \\z. x z (y z)', 'classic', 'S-K-I 完备组合子系统之代换分配核', JSON.stringify(['combinator', 'ski', 'foundational']), 1, now, now);
    insertRepo.run('repo_ski_k', 1, 'K 组合子 (Constant)', '\\x. \\y. x', 'classic', '常数发生器，丢弃次要参数', JSON.stringify(['combinator', 'ski']), 1, now, now);
    insertRepo.run('repo_ski_i', 1, 'I 组合子 (Identity)', '\\x. x', 'classic', '恒等变换，太极归一', JSON.stringify(['combinator', 'ski', 'identity']), 1, now, now);
    insertRepo.run('repo_add_two_one', 1, '丘奇加法 2 + 1', '(\\m. \\n. \\f. \\x. m f (n f x)) (\\f. \\x. f (f x)) (\\f. \\x. f x)', 'classic', '两仪生三象，二项式加法逐步规约演化', JSON.stringify(['church', 'addition', 'reduction']), 1, now, now);
    insertRepo.run('repo_y_combinator', 1, 'Y 组合子 (不动点算子 Fixpoint)', '\\f. (\\x. f (x x)) (\\x. f (x x))', 'classic', '无递归语法实现通用递归计算，计算机科学最深邃的奇迹', JSON.stringify(['fixpoint', 'recursion', 'advanced']), 1, now, now);
  }

  // 初始化全局热度
  const heatRow = db.prepare('SELECT value FROM global_stats WHERE key = ?').get('total_heat');
  if (!heatRow) {
    db.prepare('INSERT INTO global_stats (key, value, updated_at) VALUES (?, ?, ?)').run('total_heat', 666, new Date().toISOString());
    db.prepare('INSERT INTO global_stats (key, value, updated_at) VALUES (?, ?, ?)').run('total_wishes', 4, new Date().toISOString());
    db.prepare('INSERT INTO global_stats (key, value, updated_at) VALUES (?, ?, ?)').run('total_fireworks', 88, new Date().toISOString());
  }
}

// 初始化
initDatabase();

module.exports = {
  db,
  hashPassword
};
