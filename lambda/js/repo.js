/**
 * John Tromp Lambda Diagrams Workstation - Expression Repository Controller
 * Cloud storage and offline repository manager (CRUD, search, tags, export/import).
 */

window.repoCtrl = {
  currentTab: 'my', // 'my' | 'public' | 'offline'
  currentFilter: { search: '', tag: 'all', mode: 'all' },
  cachedItems: [],

  init() {
    this.bindEvents();
  },

  openModal() {
    const modal = document.getElementById('repoModal');
    if (!modal) return;
    modal.style.display = 'flex';
    // If not logged in, default tab to 'public' or 'offline'
    if (!window.currentUser && this.currentTab === 'my') {
      this.currentTab = 'public';
    }
    this.switchTab(this.currentTab);
  },

  closeModal() {
    const modal = document.getElementById('repoModal');
    if (modal) modal.style.display = 'none';
  },

  openSaveModal() {
    const modal = document.getElementById('saveRepoModal');
    if (!modal) return;

    const exprInput = document.getElementById('expressionInput');
    const tabClassic = document.getElementById('tabModeClassic');
    const isClassic = tabClassic && tabClassic.classList.contains('active');

    const codeInput = document.getElementById('saveRepoCode');
    const titleInput = document.getElementById('saveRepoTitle');
    const descInput = document.getElementById('saveRepoDesc');
    const tagsInput = document.getElementById('saveRepoTags');

    if (codeInput && exprInput) codeInput.value = exprInput.value.trim();
    if (titleInput) titleInput.value = '';
    if (descInput) descInput.value = '';
    if (tagsInput) tagsInput.value = '';

    modal.style.display = 'flex';
  },

  closeSaveModal() {
    const modal = document.getElementById('saveRepoModal');
    if (modal) modal.style.display = 'none';
  },

  switchTab(tab) {
    this.currentTab = tab;

    const tabMy = document.getElementById('repoTabMy');
    const tabPub = document.getElementById('repoTabPublic');
    const tabOff = document.getElementById('repoTabOffline');

    if (tabMy) tabMy.classList.toggle('active', tab === 'my');
    if (tabPub) tabPub.classList.toggle('active', tab === 'public');
    if (tabOff) tabOff.classList.toggle('active', tab === 'offline');

    this.loadRepos();
  },

  async loadRepos() {
    const grid = document.getElementById('repoCardGrid');
    if (!grid) return;
    grid.innerHTML = `<div class="repo-empty-state">${currentLang === 'zh' ? '加载中...' : 'Loading...'}</div>`;

    try {
      let res;
      if (this.currentTab === 'my') {
        if (!window.currentUser) {
          grid.innerHTML = `
            <div class="repo-empty-state">
              <p style="margin-bottom: 10px;">${currentLang === 'zh' ? '请先登录以访问个人云端仓库' : 'Please sign in to access your cloud formulas'}</p>
              <button class="btn btn-solid" onclick="authCtrl.openModal()">${t('btnSubmitLogin')}</button>
            </div>
          `;
          return;
        }
        res = await api.getUserRepos(this.currentFilter);
      } else if (this.currentTab === 'public') {
        res = await api.getPublicRepos(this.currentFilter);
      } else {
        // Offline local store
        const items = api.getOfflineRepos();
        res = { success: true, items };
      }

      this.cachedItems = (res && res.items) || [];
      this.renderRepoGrid(this.cachedItems);
    } catch (err) {
      grid.innerHTML = `<div class="repo-empty-state" style="color: var(--text);">${err.message}</div>`;
    }
  },

  renderRepoGrid(items) {
    const grid = document.getElementById('repoCardGrid');
    if (!grid) return;

    if (!items || items.length === 0) {
      let emptyMsg = t('emptyMyRepos');
      if (this.currentTab === 'public') emptyMsg = t('emptyPublicRepos');
      if (this.currentTab === 'offline') emptyMsg = t('emptyOfflineRepos');
      grid.innerHTML = `<div class="repo-empty-state">${emptyMsg}</div>`;
      return;
    }

    grid.innerHTML = '';
    items.forEach(item => {
      const card = document.createElement('div');
      card.className = 'repo-card';

      const isOwner = (this.currentTab === 'my') || (this.currentTab === 'offline') ||
                      (window.currentUser && item.userId === window.currentUser.id);

      const tagHtml = Array.isArray(item.tags)
        ? item.tags.map(t => `<span class="tag-pill">${t}</span>`).join('')
        : '';

      const modeBadge = item.mode === 'debruijn' ? 'DB' : 'λ';
      const authorInfo = item.authorName ? `<span style="font-size: 10px; color: var(--text-dim);">${t('authorPrefix')} @${item.authorName}</span>` : '';

      card.innerHTML = `
        <div>
          <div class="repo-card-head">
            <div class="repo-card-title">${this.escapeHtml(item.title || 'Untitled')}</div>
            <span class="badge-tag">${modeBadge}</span>
          </div>
          <div class="repo-card-code"><code>${this.escapeHtml(item.code || '')}</code></div>
          ${item.desc ? `<div class="repo-card-desc">${this.escapeHtml(item.desc)}</div>` : ''}
          ${tagHtml ? `<div class="repo-card-tags">${tagHtml}</div>` : ''}
        </div>
        <div class="repo-card-actions">
          ${authorInfo}
          <div style="display: flex; gap: 4px; margin-left: auto;">
            <button class="btn btn-solid btn-repo-load" data-id="${item.id}" style="padding: 3px 8px; font-size: 11px;">${t('btnLoadPreset')}</button>
            ${isOwner ? `<button class="btn btn-danger btn-repo-del" data-id="${item.id}" style="padding: 3px 6px; font-size: 11px;">×</button>` : ''}
          </div>
        </div>
      `;

      card.querySelector('.btn-repo-load').addEventListener('click', () => {
        this.loadIntoWorkspace(item);
      });

      if (isOwner) {
        card.querySelector('.btn-repo-del').addEventListener('click', async (e) => {
          e.stopPropagation();
          if (confirm(t('msgDeleteConfirm'))) {
            await this.deleteItem(item.id);
          }
        });
      }

      grid.appendChild(card);
    });
  },

  loadIntoWorkspace(item) {
    const exprInput = document.getElementById('expressionInput');
    const tabClassic = document.getElementById('tabModeClassic');
    const tabDeBruijn = document.getElementById('tabModeDeBruijn');

    if (item.mode === 'debruijn') {
      if (tabDeBruijn) tabDeBruijn.click();
    } else {
      if (tabClassic) tabClassic.click();
    }

    if (exprInput) {
      exprInput.value = item.code;
    }

    // Trigger load and parse in appState
    if (typeof loadAndParse === 'function') {
      loadAndParse(item.code);
    }

    this.closeModal();
  },

  async deleteItem(id) {
    try {
      await api.deleteRepo(id);
      this.loadRepos();
      if (window.currentUser) {
        window.currentUser.repoCount = Math.max(0, (window.currentUser.repoCount || 1) - 1);
        authCtrl.updateHeaderBadge();
      }
    } catch (err) {
      alert(err.message);
    }
  },

  async handleSaveSubmit(e) {
    if (e) e.preventDefault();
    const titleInput = document.getElementById('saveRepoTitle');
    const codeInput = document.getElementById('saveRepoCode');
    const descInput = document.getElementById('saveRepoDesc');
    const tagsInput = document.getElementById('saveRepoTags');
    const pubCheck = document.getElementById('saveRepoPublic');

    const code = codeInput ? codeInput.value.trim() : '';
    if (!code) {
      alert(currentLang === 'zh' ? '表达式不能为空' : 'Expression code cannot be empty');
      return;
    }

    const tabClassic = document.getElementById('tabModeClassic');
    const isClassic = tabClassic && tabClassic.classList.contains('active');
    const mode = isClassic ? 'classic' : 'debruijn';

    const tags = tagsInput ? tagsInput.value.split(/[,，\s]+/).filter(Boolean) : [];
    const isPublic = pubCheck ? pubCheck.checked : false;

    const payload = {
      title: titleInput ? titleInput.value.trim() : 'Untitled',
      code,
      mode,
      desc: descInput ? descInput.value.trim() : '',
      tags,
      isPublic
    };

    try {
      await api.createRepo(payload);
      if (window.currentUser) {
        window.currentUser.repoCount = (window.currentUser.repoCount || 0) + 1;
        authCtrl.updateHeaderBadge();
      }
      this.closeSaveModal();
      alert(t('msgSavedSuccess'));
      if (this.currentTab) this.loadRepos();
    } catch (err) {
      alert(err.message);
    }
  },

  async exportJSON() {
    let items = [];
    if (this.currentTab === 'offline') {
      items = api.getOfflineRepos();
    } else {
      const res = await api.getUserRepos();
      items = (res && res.items) || [];
    }

    const blob = new Blob([JSON.stringify(items, null, 2)], { type: 'application/json' });
    const link = document.createElement('a');
    link.href = URL.createObjectURL(blob);
    link.download = `lambda_repository_${Date.now()}.json`;
    link.click();
    URL.revokeObjectURL(link.href);
  },

  async importJSON() {
    const input = document.createElement('input');
    input.type = 'file';
    input.accept = '.json,application/json';
    input.onchange = async (e) => {
      const file = e.target.files[0];
      if (!file) return;
      const text = await file.text();
      try {
        const parsed = JSON.parse(text);
        const list = Array.isArray(parsed) ? parsed : (parsed.items || []);
        if (list.length === 0) {
          alert('No formula items found in JSON');
          return;
        }
        await api.importRepos(list);
        this.loadRepos();
        alert(currentLang === 'zh' ? `成功导入 ${list.length} 个公式` : `Imported ${list.length} formulas`);
      } catch (err) {
        alert('Invalid JSON file: ' + err.message);
      }
    };
    input.click();
  },

  escapeHtml(str) {
    return (str || '')
      .replace(/&/g, '&amp;')
      .replace(/</g, '&lt;')
      .replace(/>/g, '&gt;')
      .replace(/"/g, '&quot;')
      .replace(/'/g, '&#39;');
  },

  bindEvents() {
    const repoOpenBtn = document.getElementById('repoOpenBtn');
    if (repoOpenBtn) {
      repoOpenBtn.addEventListener('click', () => this.openModal());
    }

    const saveToRepoBtn = document.getElementById('saveToRepoBtn');
    if (saveToRepoBtn) {
      saveToRepoBtn.addEventListener('click', () => this.openSaveModal());
    }

    const closeBtn = document.querySelector('#repoModal .close-modal-btn');
    if (closeBtn) {
      closeBtn.addEventListener('click', () => this.closeModal());
    }

    const closeSaveBtn = document.querySelector('#saveRepoModal .close-modal-btn');
    if (closeSaveBtn) {
      closeSaveBtn.addEventListener('click', () => this.closeSaveModal());
    }

    const modal = document.getElementById('repoModal');
    if (modal) {
      modal.addEventListener('click', (e) => {
        if (e.target === modal) this.closeModal();
      });
    }

    const saveModal = document.getElementById('saveRepoModal');
    if (saveModal) {
      saveModal.addEventListener('click', (e) => {
        if (e.target === saveModal) this.closeSaveModal();
      });
    }

    const tabMy = document.getElementById('repoTabMy');
    if (tabMy) tabMy.addEventListener('click', () => this.switchTab('my'));

    const tabPub = document.getElementById('repoTabPublic');
    if (tabPub) tabPub.addEventListener('click', () => this.switchTab('public'));

    const tabOff = document.getElementById('repoTabOffline');
    if (tabOff) tabOff.addEventListener('click', () => this.switchTab('offline'));

    const searchInput = document.getElementById('repoSearchInput');
    if (searchInput) {
      let timer = null;
      searchInput.addEventListener('input', (e) => {
        clearTimeout(timer);
        timer = setTimeout(() => {
          this.currentFilter.search = e.target.value.trim();
          this.loadRepos();
        }, 250);
      });
    }

    const saveForm = document.getElementById('saveRepoForm');
    if (saveForm) {
      saveForm.addEventListener('submit', (e) => this.handleSaveSubmit(e));
    }

    const exportBtn = document.getElementById('repoExportBtn');
    if (exportBtn) exportBtn.addEventListener('click', () => this.exportJSON());

    const importBtn = document.getElementById('repoImportBtn');
    if (importBtn) importBtn.addEventListener('click', () => this.importJSON());
  }
};
