/**
 * John Tromp Lambda Diagrams Workstation - Presets Library
 * 23 Classic Lambda Calculus Combinators, Church Numerals, and Tromp Classics.
 */

const PRESET_DEFINITIONS = [
  {
    id: 'TWO',
    group: 'church',
    name: 'Church 2',
    code: '\\f. \\x. f (f x)',
    zh: '\\f. \\x. f (f x) (Church 数 2)',
    en: '\\f. \\x. f (f x) (Church 2)',
    zh_desc: 'Church 数 2，对输入参数连续应用两次函数 f',
    en_desc: 'Church numeral 2, applies function f twice'
  },
  {
    id: 'ZERO',
    group: 'church',
    name: 'Church 0',
    code: '\\f. \\x. x',
    zh: '\\f. \\x. x (Church 数 0)',
    en: '\\f. \\x. x (Church 0)',
    zh_desc: 'Church 数 0，忽略函数 f 并直接返回参数 x',
    en_desc: 'Church numeral 0, returns x unmodified'
  },
  {
    id: 'ONE',
    group: 'church',
    name: 'Church 1',
    code: '\\f. \\x. f x',
    zh: '\\f. \\x. f x (Church 数 1)',
    en: '\\f. \\x. f x (Church 1)',
    zh_desc: 'Church 数 1，对参数 x 应用一次函数 f',
    en_desc: 'Church numeral 1, applies function f once'
  },
  {
    id: 'THREE',
    group: 'church',
    name: 'Church 3',
    code: '\\f. \\x. f (f (f x))',
    zh: '\\f. \\x. f (f (f x)) (Church 数 3)',
    en: '\\f. \\x. f (f (f x)) (Church 3)',
    zh_desc: 'Church 数 3，对参数 x 应用三次函数 f',
    en_desc: 'Church numeral 3, applies function f thrice'
  },
  {
    id: 'FOUR',
    group: 'church',
    name: 'Church 4',
    code: '\\f. \\x. f (f (f (f x)))',
    zh: '\\f. \\x. f (f (f (f x))) (Church 数 4)',
    en: '\\f. \\x. f (f (f (f x))) (Church 4)',
    zh_desc: 'Church 数 4，对参数 x 连续应用四次函数 f',
    en_desc: 'Church numeral 4, applies function f 4 times'
  },
  {
    id: 'I',
    group: 'core',
    name: 'I',
    code: '\\x. x',
    zh: '\\x. x (恒等组合子 I)',
    en: '\\x. x (Identity - I)',
    zh_desc: '恒等组合子 I (Identity)，直接返回接收到的参数',
    en_desc: 'Identity combinator I, returns parameter unchanged'
  },
  {
    id: 'K',
    group: 'core',
    name: 'K / True',
    code: '\\x. \\y. x',
    zh: '\\x. \\y. x (常数组合子 K / True)',
    en: '\\x. \\y. x (Constant / True - K)',
    zh_desc: '常数组合子 K / True，丢弃第二个参数并返回第一个',
    en_desc: 'Constant / True combinator K, drops 2nd argument'
  },
  {
    id: 'FALSE',
    group: 'core',
    name: 'False / 0',
    code: '\\x. \\y. y',
    zh: '\\x. \\y. y (布尔 False / 0)',
    en: '\\x. \\y. y (False / 0)',
    zh_desc: '布尔假 / 0，丢弃第一个参数并返回第二个',
    en_desc: 'Boolean False / 0, selects 2nd argument'
  },
  {
    id: 'S',
    group: 'core',
    name: 'S',
    code: '\\x. \\y. \\z. (x z) (y z)',
    zh: '\\x. \\y. \\z. (x z) (y z) (S 组合子)',
    en: '\\x. \\y. \\z. (x z) (y z) (Combinator S)',
    zh_desc: '强代换组合子 S，将环境参数 z 同时分发给 x 与 y',
    en_desc: 'Substitution combinator S, distributes z to x and y'
  },
  {
    id: 'B',
    group: 'core',
    name: 'B (Compose)',
    code: '\\x. \\y. \\z. x (y z)',
    zh: '\\x. \\y. \\z. x (y z) (复合组合子 B)',
    en: '\\x. \\y. \\z. x (y z) (Combinator B / Compose)',
    zh_desc: '复合组合子 B (Compose)，相当于函数复合 (f ∘ g)',
    en_desc: 'Composition combinator B (f ∘ g)'
  },
  {
    id: 'C',
    group: 'core',
    name: 'C (Flip)',
    code: '\\x. \\y. \\z. x z y',
    zh: '\\x. \\y. \\z. x z y (交换组合子 C)',
    en: '\\x. \\y. \\z. x z y (Combinator C / Flip)',
    zh_desc: '交换组合子 C (Flip)，交换后续两个参数的接受次序',
    en_desc: 'Flip combinator C, swaps arguments'
  },
  {
    id: 'W',
    group: 'core',
    name: 'W (Dup)',
    code: '\\x. \\y. x y y',
    zh: '\\x. \\y. x y y (复制组合子 W)',
    en: '\\x. \\y. x y y (Combinator W / Duplicate)',
    zh_desc: '复制组合子 W (Duplicate)，将参数 y 重复传递给 x',
    en_desc: 'Duplication combinator W, feeds y twice to x'
  },
  {
    id: 'SKK',
    group: 'core',
    name: 'S K K -> I',
    code: '(\\x. \\y. \\z. (x z) (y z)) (\\x. \\y. x) (\\x. \\y. x)',
    zh: 'S K K -> I (S K K 规约到恒等)',
    en: 'S K K -> I (S K K reduces to I)',
    zh_desc: '经典组合子定理：S K K 经过两步规约可恒等化为 I',
    en_desc: 'SK basis theorem: S K K reduces directly to I'
  },
  {
    id: 'Y',
    group: 'core',
    name: 'Y Combinator',
    code: '\\f. (\\x. f (x x)) (\\x. f (x x))',
    zh: 'Y 不动点组合子',
    en: 'Y Combinator (Fixed Point)',
    zh_desc: '著名的不动点组合子 Y，用于在无类型演算中构造递归',
    en_desc: 'Fixed-point combinator Y, enables recursion'
  },
  {
    id: 'OMEGA',
    group: 'core',
    name: 'Omega (Ω)',
    code: '(\\x. x x) (\\x. x x)',
    zh: 'Omega (发散项 / 死循环)',
    en: 'Omega (Divergent Loop)',
    zh_desc: '发散项 / 死循环，每步 β-规约均恢复自身',
    en_desc: 'Divergent loop, reproduces itself upon reduction'
  },
  {
    id: 'SUCC_1',
    group: 'arithmetic',
    name: 'SUCC 1 -> 2',
    code: '(\\n. \\f. \\x. f (n f x)) (\\f. \\x. f x)',
    zh: 'SUCC 1 -> 2 (后继函数)',
    en: 'SUCC 1 -> 2 (Successor)',
    zh_desc: 'Church 后继运算：1 的后继可规约为 2',
    en_desc: 'Successor applied to 1, reduces to 2'
  },
  {
    id: 'PLUS_1_2',
    group: 'arithmetic',
    name: '1 + 2 -> 3',
    code: '(\\m. \\n. \\f. \\x. m f (n f x)) (\\f. \\x. f x) (\\f. \\x. f (f x))',
    zh: '1 + 2 -> 3 (加法运算)',
    en: '1 + 2 -> 3 (Addition)',
    zh_desc: 'Church 加法：计算 1 + 2 逐步规约为 3',
    en_desc: 'Church addition: 1 + 2 reduces to 3'
  },
  {
    id: 'MULT_2_2',
    group: 'arithmetic',
    name: '2 * 2 -> 4',
    code: '(\\m. \\n. \\f. m (n f)) (\\f. \\x. f (f x)) (\\f. \\x. f (f x))',
    zh: '2 * 2 -> 4 (乘法运算)',
    en: '2 * 2 -> 4 (Multiplication)',
    zh_desc: 'Church 乘法：计算 2 * 2 逐步规约为 4',
    en_desc: 'Church multiplication: 2 * 2 reduces to 4'
  },
  {
    id: 'PRED_2',
    group: 'arithmetic',
    name: 'PRED 2 -> 1',
    code: '(\\n. \\f. \\x. n (\\g. \\h. h (g f)) (\\u. x) (\\u. u)) (\\f. \\x. f (f x))',
    zh: 'PRED 2 -> 1 (前驱函数)',
    en: 'PRED 2 -> 1 (Predecessor)',
    zh_desc: 'Church 前驱运算：计算 2 的前驱规约为 1',
    en_desc: 'Church predecessor: PRED 2 reduces to 1'
  },
  {
    id: 'AND_TF',
    group: 'arithmetic',
    name: 'AND True False',
    code: '(\\p. \\q. p q p) (\\x. \\y. x) (\\x. \\y. y)',
    zh: 'AND True False -> False',
    en: 'AND True False -> False',
    zh_desc: '布尔逻辑与：True AND False 规约为 False',
    en_desc: 'Boolean AND: True AND False reduces to False'
  },
  {
    id: 'OR_FT',
    group: 'arithmetic',
    name: 'OR False True',
    code: '(\\p. \\q. p p q) (\\x. \\y. y) (\\x. \\y. x)',
    zh: 'OR False True -> True',
    en: 'OR False True -> True',
    zh_desc: '布尔逻辑或：False OR True 规约为 True',
    en_desc: 'Boolean OR: False OR True reduces to True'
  },
  {
    id: 'NOT_T',
    group: 'arithmetic',
    name: 'NOT True',
    code: '(\\p. p (\\x. \\y. y) (\\x. \\y. x)) (\\x. \\y. x)',
    zh: 'NOT True -> False',
    en: 'NOT True -> False',
    zh_desc: '布尔逻辑非：NOT True 规约为 False',
    en_desc: 'Boolean NOT: NOT True reduces to False'
  },
  {
    id: 'TROMP_PRIMES_FRAG',
    group: 'classics',
    name: 'Prime Sieve Frag',
    code: '(\\n. \\f. n (\\f. \\n. n (f (\\f. \\x. n f (f x)))) (\\x. f) (\\x. x)) (\\f. \\x. f x)',
    zh: 'Tromp 167 位素数筛片段',
    en: 'Tromp 167-bit Prime Sieve Fragment',
    zh_desc: 'John Tromp 167 位素数筛核心状态推进片段',
    en_desc: 'Fragment of John Tromp 167-bit Prime Sieve'
  }
];

function getPresetById(id) {
  return PRESET_DEFINITIONS.find(p => p.id === id) || null;
}
