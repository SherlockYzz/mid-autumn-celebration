/**
 * John Tromp Lambda Diagrams Workstation - Client API Service
 * Dual-Mode Architecture:
 * - Online: Communicates with Node.js REST API endpoints (/api/auth/*, /api/repo/*)
 * - Offline Fallback: Seamlessly falls back to localStorage if server is offline or file:// protocol is used.
 */

const API_CONFIG = {
  // Relative path so it works whether deployed at root, subfolder, or localhost
  baseUrl: window.location.origin && window.location.origin !== 'null' ? window.location.origin : 'http://localhost:3000',
  tokenKey: 'lambda_diagram_token',
  offlineReposKey: 'lambda_offline_repos',
  offlineUserKey: 'lambda_offline_user'
};

const api = {
  isServerAvailable: false,

  getToken() {
    return localStorage.getItem(API_CONFIG.tokenKey) || '';
  },

  setToken(token) {
    if (token) {
      localStorage.setItem(API_CONFIG.tokenKey, token);
    } else {
      localStorage.removeItem(API_CONFIG.tokenKey);
    }
  },

  getAuthHeaders() {
    const token = this.getToken();
    const headers = { 'Content-Type': 'application/json' };
    if (token) {
      headers['Authorization'] = `Bearer ${token}`;
    }
    return headers;
  },

  async request(endpoint, options = {}) {
    const url = `${API_CONFIG.baseUrl}${endpoint}`;
    const headers = {
      ...this.getAuthHeaders(),
      ...(options.headers || {})
    };

    try {
      const res = await fetch(url, {
        ...options,
        headers
      });

      this.isServerAvailable = true;
      const data = await res.json().catch(() => ({}));
      if (!res.ok) {
        throw new Error(data.error || `HTTP ${res.status}: ${res.statusText}`);
      }
      return data;
    } catch (err) {
      // Network failure / server offline
      if (err.name === 'TypeError' || err.message.includes('Failed to fetch') || err.message.includes('NetworkError')) {
        this.isServerAvailable = false;
      }
      throw err;
    }
  },

  // Health check to test server connectivity
  async ping() {
    try {
      const res = await fetch(`${API_CONFIG.baseUrl}/api/repo/public`, { method: 'GET' });
      this.isServerAvailable = res.ok;
      return res.ok;
    } catch {
      this.isServerAvailable = false;
      return false;
    }
  },

  // =========================================================================
  // AUTH METHODS
  // =========================================================================
  async register(username, password) {
    try {
      const data = await this.request('/api/auth/register', {
        method: 'POST',
        body: JSON.stringify({ username, password })
      });
      if (data.token) {
        this.setToken(data.token);
      }
      return data;
    } catch (err) {
      if (!this.isServerAvailable) {
        // Offline registration fallback
        const offlineUser = {
          id: `local_${Date.now()}`,
          username: username.trim(),
          createdAt: new Date().toISOString(),
          repoCount: this.getOfflineRepos().length
        };
        localStorage.setItem(API_CONFIG.offlineUserKey, JSON.stringify(offlineUser));
        return { success: true, user: offlineUser, offline: true };
      }
      throw err;
    }
  },

  async login(username, password) {
    try {
      const data = await this.request('/api/auth/login', {
        method: 'POST',
        body: JSON.stringify({ username, password })
      });
      if (data.token) {
        this.setToken(data.token);
      }
      return data;
    } catch (err) {
      if (!this.isServerAvailable) {
        // Offline login fallback
        const saved = localStorage.getItem(API_CONFIG.offlineUserKey);
        if (saved) {
          const u = JSON.parse(saved);
          if (u.username.toLowerCase() === username.trim().toLowerCase()) {
            return { success: true, user: u, offline: true };
          }
        }
        const offlineUser = {
          id: `local_${Date.now()}`,
          username: username.trim(),
          createdAt: new Date().toISOString(),
          repoCount: this.getOfflineRepos().length
        };
        localStorage.setItem(API_CONFIG.offlineUserKey, JSON.stringify(offlineUser));
        return { success: true, user: offlineUser, offline: true };
      }
      throw err;
    }
  },

  async getMe() {
    if (!this.getToken()) {
      if (!this.isServerAvailable) {
        const saved = localStorage.getItem(API_CONFIG.offlineUserKey);
        return saved ? { success: true, user: JSON.parse(saved), offline: true } : null;
      }
      return null;
    }

    try {
      return await this.request('/api/auth/me', { method: 'GET' });
    } catch (err) {
      if (!this.isServerAvailable) {
        const saved = localStorage.getItem(API_CONFIG.offlineUserKey);
        return saved ? { success: true, user: JSON.parse(saved), offline: true } : null;
      }
      this.setToken(null);
      return null;
    }
  },

  async logout() {
    try {
      if (this.isServerAvailable && this.getToken()) {
        await this.request('/api/auth/logout', { method: 'POST' }).catch(() => {});
      }
    } finally {
      this.setToken(null);
      localStorage.removeItem(API_CONFIG.offlineUserKey);
    }
    return { success: true };
  },

  // =========================================================================
  // REPOSITORY METHODS
  // =========================================================================
  getOfflineRepos() {
    try {
      const raw = localStorage.getItem(API_CONFIG.offlineReposKey);
      return raw ? JSON.parse(raw) : [];
    } catch {
      return [];
    }
  },

  saveOfflineRepos(items) {
    try {
      localStorage.setItem(API_CONFIG.offlineReposKey, JSON.stringify(items));
    } catch (e) {
      console.error('[Offline Store] Error saving items:', e);
    }
  },

  async getUserRepos(options = {}) {
    try {
      const params = new URLSearchParams();
      if (options.search) params.append('search', options.search);
      if (options.tag && options.tag !== 'all') params.append('tag', options.tag);
      if (options.mode && options.mode !== 'all') params.append('mode', options.mode);

      const qs = params.toString() ? `?${params.toString()}` : '';
      return await this.request(`/api/repo${qs}`, { method: 'GET' });
    } catch (err) {
      if (!this.isServerAvailable) {
        let items = this.getOfflineRepos();
        const { search = '', tag = '', mode = 'all' } = options;
        const q = search.toLowerCase().trim();

        items = items.filter(item => {
          if (mode !== 'all' && item.mode !== mode) return false;
          if (tag && tag !== 'all') {
            if (!Array.isArray(item.tags) || !item.tags.includes(tag)) return false;
          }
          if (q) {
            const matchTitle = (item.title || '').toLowerCase().includes(q);
            const matchCode = (item.code || '').toLowerCase().includes(q);
            if (!matchTitle && !matchCode) return false;
          }
          return true;
        });
        return { success: true, items, offline: true };
      }
      throw err;
    }
  },

  async getPublicRepos(options = {}) {
    try {
      const params = new URLSearchParams();
      if (options.search) params.append('search', options.search);
      if (options.tag && options.tag !== 'all') params.append('tag', options.tag);
      const qs = params.toString() ? `?${params.toString()}` : '';

      return await this.request(`/api/repo/public${qs}`, { method: 'GET' });
    } catch (err) {
      if (!this.isServerAvailable) {
        return { success: true, items: [], offline: true };
      }
      throw err;
    }
  },

  async createRepo(payload) {
    try {
      return await this.request('/api/repo', {
        method: 'POST',
        body: JSON.stringify(payload)
      });
    } catch (err) {
      if (!this.isServerAvailable) {
        const items = this.getOfflineRepos();
        const newItem = {
          id: `local_repo_${Date.now()}`,
          title: payload.title || 'Untitled Lambda Term',
          code: payload.code.trim(),
          mode: payload.mode || 'classic',
          desc: (payload.desc || '').trim(),
          tags: Array.isArray(payload.tags) ? payload.tags : [],
          isPublic: Boolean(payload.isPublic),
          createdAt: new Date().toISOString(),
          updatedAt: new Date().toISOString(),
          offline: true
        };
        items.unshift(newItem);
        this.saveOfflineRepos(items);
        return { success: true, item: newItem, offline: true };
      }
      throw err;
    }
  },

  async deleteRepo(id) {
    try {
      return await this.request(`/api/repo/${id}`, { method: 'DELETE' });
    } catch (err) {
      if (!this.isServerAvailable) {
        let items = this.getOfflineRepos();
        items = items.filter(r => r.id !== id);
        this.saveOfflineRepos(items);
        return { success: true, message: 'Deleted offline' };
      }
      throw err;
    }
  },

  async importRepos(items) {
    try {
      return await this.request('/api/repo/import', {
        method: 'POST',
        body: JSON.stringify({ items })
      });
    } catch (err) {
      if (!this.isServerAvailable) {
        const existing = this.getOfflineRepos();
        const imported = items.map(item => ({
          id: `local_repo_${Date.now()}_${Math.random().toString(36).substring(2, 6)}`,
          title: item.title || 'Imported Term',
          code: item.code.trim(),
          mode: item.mode || 'classic',
          desc: item.desc || '',
          tags: item.tags || ['imported'],
          isPublic: false,
          createdAt: new Date().toISOString(),
          updatedAt: new Date().toISOString(),
          offline: true
        }));
        this.saveOfflineRepos([...imported, ...existing]);
        return { success: true, count: imported.length, items: imported, offline: true };
      }
      throw err;
    }
  },

  async exportRepos() {
    try {
      return await this.request('/api/repo/export', { method: 'GET' });
    } catch (err) {
      if (!this.isServerAvailable) {
        return this.getOfflineRepos();
      }
      throw err;
    }
  }
};
