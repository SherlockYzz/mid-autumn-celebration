/**
 * John Tromp Lambda Diagrams Workstation - Authentication Controller
 * Manages user session, login/registration modal, and profile card.
 */

window.currentUser = null;

const authCtrl = {
  currentTab: 'login', // 'login' | 'register'

  async init() {
    this.bindEvents();
    try {
      const res = await api.getMe();
      if (res && res.user) {
        this.setUser(res.user);
      } else {
        this.setUser(null);
      }
    } catch {
      this.setUser(null);
    }
  },

  setUser(user) {
    window.currentUser = user;
    this.updateHeaderBadge();
  },

  updateHeaderBadge() {
    const btn = document.getElementById('authOpenBtn');
    if (!btn) return;

    if (window.currentUser) {
      btn.innerHTML = `<span style="font-family: var(--font-code); font-weight: 700;">@${window.currentUser.username}</span>`;
      btn.classList.add('btn-active');
      btn.title = `${t('userProfileTitle')} (@${window.currentUser.username})`;
    } else {
      btn.textContent = t('authBtn');
      btn.classList.remove('btn-active');
      btn.title = t('authBtn');
    }
  },

  openModal() {
    const modal = document.getElementById('authModal');
    if (!modal) return;

    this.clearMessages();
    const loginForm = document.getElementById('authLoginForm');
    const regForm = document.getElementById('authRegisterForm');
    const profileView = document.getElementById('authProfileView');
    const tabs = document.getElementById('authModalTabs');

    if (window.currentUser) {
      if (tabs) tabs.style.display = 'none';
      if (loginForm) loginForm.style.display = 'none';
      if (regForm) regForm.style.display = 'none';
      if (profileView) {
        profileView.style.display = 'block';
        this.renderProfile();
      }
    } else {
      if (tabs) tabs.style.display = 'flex';
      if (profileView) profileView.style.display = 'none';
      this.switchTab(this.currentTab);
    }

    modal.style.display = 'flex';
  },

  closeModal() {
    const modal = document.getElementById('authModal');
    if (modal) modal.style.display = 'none';
    this.clearMessages();
  },

  switchTab(tab) {
    this.currentTab = tab;
    this.clearMessages();

    const tabLogin = document.getElementById('authTabLogin');
    const tabReg = document.getElementById('authTabRegister');
    const formLogin = document.getElementById('authLoginForm');
    const formReg = document.getElementById('authRegisterForm');

    if (tab === 'login') {
      if (tabLogin) tabLogin.classList.add('active');
      if (tabReg) tabReg.classList.remove('active');
      if (formLogin) formLogin.style.display = 'block';
      if (formReg) formReg.style.display = 'none';
    } else {
      if (tabLogin) tabLogin.classList.remove('active');
      if (tabReg) tabReg.classList.add('active');
      if (formLogin) formLogin.style.display = 'none';
      if (formReg) formReg.style.display = 'block';
    }
  },

  showMessage(msg, isError = true) {
    const msgEl = document.getElementById('authStatusMessage');
    if (!msgEl) return;
    msgEl.textContent = msg;
    msgEl.className = `auth-message ${isError ? 'error' : 'success'}`;
    msgEl.style.display = 'block';
  },

  clearMessages() {
    const msgEl = document.getElementById('authStatusMessage');
    if (msgEl) {
      msgEl.textContent = '';
      msgEl.style.display = 'none';
    }
  },

  renderProfile() {
    if (!window.currentUser) return;
    const nameEl = document.getElementById('profileUsername');
    const dateEl = document.getElementById('profileCreated');
    const countEl = document.getElementById('profileRepoCount');

    if (nameEl) nameEl.textContent = window.currentUser.username;
    if (dateEl) {
      const dt = new Date(window.currentUser.createdAt);
      dateEl.textContent = isNaN(dt.getTime()) ? 'Offline' : dt.toLocaleDateString();
    }
    if (countEl) countEl.textContent = `${window.currentUser.repoCount || 0} ${t('presetCountSuffix')}`;
  },

  async handleLogin(e) {
    if (e) e.preventDefault();
    this.clearMessages();

    const userInput = document.getElementById('loginUsername');
    const passInput = document.getElementById('loginPassword');
    const username = userInput ? userInput.value.trim() : '';
    const password = passInput ? passInput.value.trim() : '';

    if (!username || !password) {
      return this.showMessage(currentLang === 'zh' ? '请输入用户名和密码' : 'Please enter username and password');
    }

    try {
      const res = await api.login(username, password);
      if (res && res.user) {
        this.setUser(res.user);
        this.showMessage(t('msgLoginSuccess'), false);
        if (userInput) userInput.value = '';
        if (passInput) passInput.value = '';
        setTimeout(() => {
          this.closeModal();
          // Reload repos if repo modal or list is open
          if (window.repoCtrl) window.repoCtrl.loadRepos();
        }, 600);
      }
    } catch (err) {
      this.showMessage(err.message || 'Login failed');
    }
  },

  async handleRegister(e) {
    if (e) e.preventDefault();
    this.clearMessages();

    const userInput = document.getElementById('regUsername');
    const passInput = document.getElementById('regPassword');
    const username = userInput ? userInput.value.trim() : '';
    const password = passInput ? passInput.value.trim() : '';

    if (!username || username.length < 3) {
      return this.showMessage(currentLang === 'zh' ? '用户名至少需要3个字符' : 'Username must be at least 3 characters');
    }
    if (!password || password.length < 6) {
      return this.showMessage(currentLang === 'zh' ? '密码长度至少需要6位' : 'Password must be at least 6 characters');
    }

    try {
      const res = await api.register(username, password);
      if (res && res.user) {
        this.setUser(res.user);
        this.showMessage(t('msgRegSuccess'), false);
        if (userInput) userInput.value = '';
        if (passInput) passInput.value = '';
        setTimeout(() => {
          this.closeModal();
          if (window.repoCtrl) window.repoCtrl.loadRepos();
        }, 600);
      }
    } catch (err) {
      this.showMessage(err.message || 'Registration failed');
    }
  },

  async handleLogout() {
    await api.logout();
    this.setUser(null);
    this.closeModal();
    if (window.repoCtrl) window.repoCtrl.loadRepos();
  },

  bindEvents() {
    const authOpenBtn = document.getElementById('authOpenBtn');
    if (authOpenBtn) {
      authOpenBtn.addEventListener('click', () => this.openModal());
    }

    const closeBtn = document.querySelector('#authModal .close-modal-btn');
    if (closeBtn) {
      closeBtn.addEventListener('click', () => this.closeModal());
    }

    const modal = document.getElementById('authModal');
    if (modal) {
      modal.addEventListener('click', (e) => {
        if (e.target === modal) this.closeModal();
      });
    }

    const tabLogin = document.getElementById('authTabLogin');
    if (tabLogin) tabLogin.addEventListener('click', () => this.switchTab('login'));

    const tabReg = document.getElementById('authTabRegister');
    if (tabReg) tabReg.addEventListener('click', () => this.switchTab('register'));

    const loginForm = document.getElementById('authLoginForm');
    if (loginForm) loginForm.addEventListener('submit', (e) => this.handleLogin(e));

    const regForm = document.getElementById('authRegisterForm');
    if (regForm) regForm.addEventListener('submit', (e) => this.handleRegister(e));

    const logoutBtn = document.getElementById('authLogoutBtn');
    if (logoutBtn) logoutBtn.addEventListener('click', () => this.handleLogout());

    window.addEventListener('languagechange', () => this.updateHeaderBadge());
  }
};
