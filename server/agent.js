// server/agent.js - 岁时智能体 (AI Festival Cultural Agent) 核心分析与决策引擎
const https = require('node:https');

// 传统文化经典知识库与对联意象生成模板
const CULTURAL_TEMPLATES = {
  mid_autumn: {
    couplets: [
      { left: '满地月华涵太液', right: '一轮秋水照昆仑', banner: '月华如愿 · 四海团圆' },
      { left: '三五良宵开碧宇', right: '万家清景映朱楼', banner: '宵月良宵 · 阖家安康' },
      { left: '皓月清辉延鹤算', right: '金风玉露驻仙龄', banner: '福寿康宁 · 岁岁常安' },
      { left: '桂子月中落灵药', right: '天香云外飘玉尘', banner: '灵芝纳福 · 诸事顺遂' }
    ],
    buffs: ['月华脉冲暴涨 300%', '金桂落英缤纷', '万家灯火长明'],
    music: 'pentatonic_guzheng'
  },
  national_day: {
    couplets: [
      { left: '九州日月开新纪', right: '万仞江山展画屏', banner: '盛世华诞 · 锦绣山河' },
      { left: '万里晴空舒锦绣', right: '四时佳景入诗篇', banner: '国泰民安 · 山河无恙' },
      { left: '长城巍峨迎朝日', right: '金鼓齐鸣壮九霄', banner: '华夏腾飞 · 四海同春' }
    ],
    buffs: ['三军礼炮连珠轰鸣', '万千庆典气球腾飞', '金光华表照彻苍穹'],
    music: 'brass_fanfare'
  },
  spring_festival: {
    couplets: [
      { left: '九州日丽迎新岁', right: '四海春温发泰亨', banner: '金龙呈祥 · 辞旧迎新' },
      { left: '财源似水流长盛', right: '事业如春万象新', banner: '金玉满堂 · 财运亨通' },
      { left: '天增岁月人增寿', right: '春满乾坤福满门', banner: '福满人间 · 岁岁平安' },
      { left: '瑞狮起舞千家乐', right: '爆竹齐鸣万象春', banner: '吉庆有余 · 大吉大利' }
    ],
    buffs: ['金狮祥瑞凌空献瑞', '元宝金币天降如雨', '大地红百子爆竹长鸣'],
    music: 'firecracker_drum'
  },
  lantern_festival: {
    couplets: [
      { left: '玉宇无尘千顷碧', right: '银花有约万家春', banner: '花市灯如昼 · 上元良宵' },
      { left: '水云灯照双溪碧', right: '锦鲤浪翻千叠金', banner: '鱼跃龙门 · 吉星高照' },
      { left: '凤箫声动临安夜', right: '玉壶光转不夜天', banner: '千灯祈愿 · 所求皆愿' }
    ],
    buffs: ['水云灯舰队随波漫游', '双尾金鳞锦鲤破浪', '空灵编钟水云调'],
    music: 'chime_bells'
  }
};

/**
 * 智能体核心决策逻辑
 * @param {string} prompt 用户自然语言输入
 * @param {object} context 当前会话环境（节日、用户信息、热度）
 */
async function processAgentQuery(prompt, context = {}) {
  const text = (prompt || '').trim();
  if (!text) {
    return {
      reply: '司律灵仙在此。请向我吩咐您的节日心愿、定制对联、祈福意象或场景盛典安排。',
      thought: '输入为空，向用户展示智能体服务说明与引导。',
      actions: []
    };
  }

  // 1. 若配置了外部 Gemini API，尝试调用大模型；否则启用自研文化专家推理状态机
  if (process.env.GEMINI_API_KEY) {
    try {
      const llmResult = await callGeminiLLM(text, context);
      if (llmResult) return llmResult;
    } catch (e) {
      console.warn('Gemini API 调用异常，自动切换为本地高精度文化推理智能体:', e.message);
    }
  }

  // 2. 本地高精度岁时文化专家推理引擎 (Rule-based & Semantic Analysis)
  return analyzeAndPlanLocally(text, context);
}

/**
 * 本地语义分析与多模块动作规划
 */
function analyzeAndPlanLocally(text, context) {
  const lower = text.toLowerCase();
  const actions = [];
  let reply = '';
  let thought = '';
  let targetFestival = context.currentFestival || 'mid_autumn';

  // 意图分析：识别节日归属
  if (text.includes('国庆') || text.includes('祖国') || text.includes('山河') || text.includes('华诞') || text.includes('礼炮')) {
    targetFestival = 'national_day';
  } else if (text.includes('新春') || text.includes('除夕') || text.includes('过年') || text.includes('发财') || text.includes('舞狮') || text.includes('拜年')) {
    targetFestival = 'spring_festival';
  } else if (text.includes('元宵') || text.includes('上元') || text.includes('灯会') || text.includes('锦鲤') || text.includes('河灯')) {
    targetFestival = 'lantern_festival';
  } else if (text.includes('中秋') || text.includes('月亮') || text.includes('玉兔') || text.includes('团圆') || text.includes('桂花')) {
    targetFestival = 'mid_autumn';
  }

  actions.push({ type: 'switch_festival', payload: targetFestival });

  const templates = CULTURAL_TEMPLATES[targetFestival];

  // 意图分析：形式化拓扑演算 (Lambda Calculus) 与星汉算筹天象演算法
  if (
    lower.includes('lambda') || lower.includes('演算') || lower.includes('规约') ||
    lower.includes('beta') || lower.includes('redex') || lower.includes('不动点') ||
    lower.includes('y组合子') || lower.includes('y combinator') || lower.includes('ski') ||
    lower.includes('church') || lower.includes('丘奇') || lower.includes('算筹') ||
    lower.includes('天象') || lower.includes('拓扑') || lower.includes('debruijn') ||
    lower.includes('德布鲁因') || lower.includes('星汉') || lower.includes('组合子') ||
    text.includes('\\') || text.includes('λ')
  ) {
    const isCelestial = text.includes('天象') || text.includes('星汉') || text.includes('星空') || text.includes('演示') || text.includes('演化') || text.includes('推演');
    const targetVp = isCelestial ? 'celestial' : 'lambda';

    actions.push({ type: 'switch_viewport', payload: targetVp });

    let termCode = '\\f. \\x. f (f x)';
    let termName = '丘奇数 2 (Church Two)';
    let mathCouplet = { left: '函子双重涵造化', right: '参变量度见乾坤', banner: '两仪演数' };

    if (text.includes('加') || text.includes('add') || text.includes('+') || text.includes('2+1') || text.includes('1+1')) {
      termCode = '(\\m. \\n. \\f. \\x. m f (n f x)) (\\f. \\x. f (f x)) (\\f. \\x. f x)';
      termName = '丘奇加法 (2 + 1)';
      mathCouplet = { left: '二仪生象加法立', right: '万物归函法理通', banner: '合数天成' };
    } else if (text.includes('乘') || text.includes('mul') || text.includes('*')) {
      termCode = '(\\m. \\n. \\f. m (n f)) (\\f. \\x. f (f x)) (\\f. \\x. f (f x))';
      termName = '丘奇乘法 (2 * 2)';
      mathCouplet = { left: '双重复合穷玄理', right: '四象流转入微茫', banner: '算筹入妙' };
    } else if (text.includes('y') || text.includes('不动点') || text.includes('递归') || text.includes('fixpoint')) {
      termCode = '\\f. (\\x. f (x x)) (\\x. f (x x))';
      termName = 'Y 组合子 (不动点算子)';
      mathCouplet = { left: '无名函数成千劫', right: '不动灵枢立太虚', banner: '玄妙自指' };
    } else if (text.includes('ski') || text.includes('替换') || text.includes('s组合')) {
      termCode = '\\x. \\y. \\z. x z (y z)';
      termName = 'S 组合子 (代换分配核)';
      mathCouplet = { left: '虚静生机分万类', right: '代换分配理千端', banner: '三元化育' };
    } else if (text.includes('恒等') || text.includes('identity') || text.includes('i组合')) {
      termCode = '\\x. x';
      termName = 'I 组合子 (太极归一)';
      mathCouplet = { left: '太极元初同一质', right: '虚舟不系自天成', banner: '太极归一' };
    }

    actions.push({
      type: 'load_lambda_formula',
      payload: { code: termCode, name: termName }
    });

    if (isCelestial) {
      actions.push({
        type: 'run_celestial_reduction',
        payload: { code: termCode, name: termName }
      });
      actions.push({ type: 'trigger_effect', payload: 'celestial_chime' });
    }

    actions.push({
      type: 'update_couplets',
      payload: { left: mathCouplet.left, right: mathCouplet.right }
    });
    actions.push({
      type: 'update_banner',
      payload: mathCouplet.banner
    });

    thought = `【意图识别】形式化拓扑计算与计算哲学意图；【技术决策】激活${isCelestial ? '【星汉算筹 · 天象演算法】' : '【数理拓扑工坊】'}视口，加载经典项【${termName}】，将 Tromp 抽象梁与变量导线映射为天象星官算筹；【跨域协同】同步奏响五音编钟，并在天穹触发拓扑规约烟火！`;
    reply = `灵仙已通阴阳数理！已为您调取【${termName}】（\`${termCode}\`），并切入${isCelestial ? '【星汉算筹 · 天象演算法】全景星宿天幕' : '【数理拓扑工坊】'}！在苍穹星光之中，抽象横梁如玉宇飞檐，变量回环似周天星斗，每一次 β-规约均将伴随五音金石编钟奏响天籁！特题对联：上联“${mathCouplet.left}”，下联“${mathCouplet.right}”，横批【${mathCouplet.banner}】！`;
    return {
      reply,
      thought,
      actions,
      festival: targetFestival
    };
  }

  // 意图分析：定制对联与春联
  if (text.includes('联') || text.includes('对子') || text.includes('写') || text.includes('题')) {
    // 匹配最适合的对联风格
    let picked = templates.couplets[0];
    if (text.includes('寿') || text.includes('长辈') || text.includes('健康') || text.includes('父母')) {
      picked = templates.couplets.find(c => c.banner.includes('安') || c.banner.includes('康')) || templates.couplets[2];
    } else if (text.includes('财') || text.includes('事业') || text.includes('公司') || text.includes('开业')) {
      picked = templates.couplets.find(c => c.banner.includes('财') || c.banner.includes('亨')) || templates.couplets[1];
    } else {
      picked = templates.couplets[Math.floor(Math.random() * templates.couplets.length)];
    }

    actions.push({
      type: 'update_couplets',
      payload: { left: picked.left, right: picked.right }
    });
    actions.push({
      type: 'update_banner',
      payload: picked.banner
    });

    thought = `【意图识别】检测到用户希望定制楹联诗赋；【场景决策】智能匹配${getFestivalName(targetFestival)}古典平仄名联；【动作派发】同步更新两侧立柱门联与正中主匾额。`;
    reply = `已为您撰得吉祥雅联！上联：“${picked.left}”，下联：“${picked.right}”。匾额已更新为【${picked.banner}】，并已为您实时挂载至全景门楼！`;
  }
  // 意图分析：盛典与烟花爆发
  else if (text.includes('盛典') || text.includes('烟花') || text.includes('爆炸') || text.includes('庆祝') || text.includes('开火') || text.includes('嗨')) {
    actions.push({ type: 'trigger_effect', payload: 'grand_celebration' });
    actions.push({ type: 'update_banner', payload: `盛世欢歌 · 祝愿${text.replace(/[，。！]/g, '')}` });

    thought = `【意图识别】全景庆祝爆发意图；【场景决策】调度三军礼炮、连珠烟花与全景祥瑞特效；【协同模块】热度大幅提升 + 触发全景万象共鸣。`;
    reply = `遵命！万象共鸣盛典已全面引爆！连珠烟花怒放、祥瑞战鼓齐鸣，天地同庆！`;
  }
  // 意图分析：求签与占卜
  else if (text.includes('签') || text.includes('卜') || text.includes('运势') || text.includes('前程') || text.includes('考试') || text.includes('姻缘')) {
    actions.push({ type: 'trigger_effect', payload: 'divination' });
    thought = `【意图识别】岁时祈福占卜意图；【场景决策】引动岁时灵签竹筒，结合用户心愿测算祥瑞吉兆。`;
    reply = `岁时灵签已为您摇落！此乃上吉之兆，满天星辉正护佑您的运途，所求皆所愿！`;
  }
  // 意图分析：祈愿与寄语
  else if (text.includes('愿') || text.includes('祝') || text.includes('希望') || text.includes('保佑')) {
    actions.push({
      type: 'post_wish',
      payload: { content: text, target: '天下万家' }
    });
    actions.push({ type: 'trigger_effect', payload: 'firework' });

    thought = `【意图识别】虔诚祈愿；【场景决策】生成天灯粒子升空，心愿长驻四海祈愿墙。`;
    reply = `您的心愿：“${text}”已化作一盏长明孔明灯升入星河，并永久收录于《四海祈愿长生册》！`;
  }
  // 通用对话与引导
  else {
    actions.push({ type: 'trigger_effect', payload: 'firework' });
    thought = `【意图识别】日常节庆互动；【场景决策】调集烟花为用户助兴，并给出智能体操作推荐。`;
    reply = `收到！司律灵仙已为您引燃庆典烟花。您可以对我说：“帮我写一副新春发财对联”、“来一场盛大国庆烟火”、或“帮我向家人许愿中秋安康”！`;
  }

  return {
    reply,
    thought,
    actions,
    festival: targetFestival
  };
}

function getFestivalName(id) {
  const map = {
    mid_autumn: '中秋追月',
    national_day: '盛世国庆',
    spring_festival: '新春除夕',
    lantern_festival: '上元灯会'
  };
  return map[id] || '岁时盛典';
}

/**
 * 外部 Gemini API 调用封装 (若配置了密钥)
 */
async function callGeminiLLM(prompt, context) {
  // 当配置了 GEMINI_API_KEY 时执行
  const apiKey = process.env.GEMINI_API_KEY;
  if (!apiKey) return null;

  // 使用 HTTPS 构造标准 Gemini API generateContent 请求
  return new Promise((resolve, reject) => {
    const postData = JSON.stringify({
      contents: [{
        parts: [{
          text: `你是一个中国传统文化与现代多节日全景交互庆典系统中的核心智能体【岁时司律灵仙】。
用户输入: "${prompt}"
当前节日: "${context.currentFestival || 'mid_autumn'}"
请输出JSON格式：
{
  "reply": "对用户的温雅有韵味的文言白话回答",
  "thought": "你的意图分析与技术决策过程",
  "actions": [
    { "type": "switch_festival|update_couplets|update_banner|trigger_effect", "payload": ... }
  ]
}`
        }]
      }],
      generationConfig: { responseMimeType: 'application/json' }
    });

    const options = {
      hostname: 'generativelanguage.googleapis.com',
      path: `/v1beta/models/gemini-1.5-flash:generateContent?key=${apiKey}`,
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Content-Length': Buffer.byteLength(postData)
      },
      timeout: 5000
    };

    const req = https.request(options, (res) => {
      let data = '';
      res.on('data', chunk => data += chunk);
      res.on('end', () => {
        try {
          const parsed = JSON.parse(data);
          const rawText = parsed.candidates[0].content.parts[0].text;
          const result = JSON.parse(rawText);
          resolve(result);
        } catch (e) {
          resolve(null);
        }
      });
    });

    req.on('error', () => resolve(null));
    req.on('timeout', () => { req.destroy(); resolve(null); });
    req.write(postData);
    req.end();
  });
}

module.exports = {
  processAgentQuery
};
