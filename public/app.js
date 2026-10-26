// ============================================================================
// Browser Backtracking Simulator - Frontend Application Logic
// Communicates with C++ HTTP Server via REST API
// ============================================================================

// Configuration: Use relative path by default (same origin in production & local),
// or use window.API_BASE_URL if configured for cross-origin deployments.
const API_BASE = window.API_BASE_URL || '';

// Known metadata for predefined websites (visual icons & brand accents)
const WEBSITE_META = {
  'https://www.youtube.com/': {
    name: 'YouTube',
    accent: '#ef4444',
    icon: `<svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor" color="#ef4444"><path d="M23.498 6.186a3.016 3.016 0 0 0-2.122-2.136C19.505 3.5 12 3.5 12 3.5s-7.505 0-9.377.55a3.016 3.016 0 0 0-2.122 2.136C0 8.07 0 12 0 12s0 3.93.502 5.814a3.016 3.016 0 0 0 2.122 2.136c1.871.55 9.376.55 9.376.55s7.505 0 9.377-.55a3.015 3.015 0 0 0 2.122-2.136C24 15.93 24 12 24 12s0-3.93-.502-5.814zM9.545 15.568V8.432L15.818 12l-6.273 3.568z"/></svg>`
  },
  'https://www.google.com/intl/en_in/chrome/': {
    name: 'Google Chrome',
    accent: '#3b82f6',
    icon: `<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" color="#3b82f6"><circle cx="12" cy="12" r="10"></circle><circle cx="12" cy="12" r="4"></circle><line x1="21.17" y1="8" x2="12" y2="8"></line><line x1="3.95" y1="6.06" x2="8.54" y2="14"></line><line x1="10.88" y1="21.94" x2="15.46" y2="14"></line></svg>`
  },
  'https://www.instagram.com/?hl=en': {
    name: 'Instagram',
    accent: '#ec4899',
    icon: `<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" color="#ec4899"><rect x="2" y="2" width="20" height="20" rx="5" ry="5"></rect><path d="M16 11.37A4 4 0 1 1 12.63 8 4 4 0 0 1 16 11.37z"></path><line x1="17.5" y1="6.5" x2="17.51" y2="6.5"></line></svg>`
  },
  'https://learnerrscoe.edupluscampus.com/': {
    name: 'Learner RSCOE Campus',
    accent: '#10b981',
    icon: `<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" color="#10b981"><path d="M22 10v6M2 10l10-5 10 5-10 5z"></path><path d="M6 12v5c3 3 9 3 12 0v-5"></path></svg>`
  }
};

// Fallback website list if API is starting up
const DEFAULT_WEBSITES = [
  { id: 'youtube', name: 'YouTube', url: 'https://www.youtube.com/' },
  { id: 'chrome', name: 'Google Chrome', url: 'https://www.google.com/intl/en_in/chrome/' },
  { id: 'instagram', name: 'Instagram', url: 'https://www.instagram.com/?hl=en' },
  { id: 'rscoe', name: 'Learner RSCOE Campus', url: 'https://learnerrscoe.edupluscampus.com/' }
];

// Application State Cache
let currentState = {
  currentPage: '',
  history: [],
  historySize: 0,
  canGoBack: false
};

// DOM Elements
const currentUrlInput = document.getElementById('current-url-input');
const btnBack = document.getElementById('btn-back');
const btnClear = document.getElementById('btn-clear');
const btnReopenCurrent = document.getElementById('btn-reopen-current');
const btnRefreshHistory = document.getElementById('btn-refresh-history');
const visualStackContainer = document.getElementById('visual-stack-container');
const stackCountBadge = document.getElementById('stack-count-badge');
const stackTopIndicator = document.getElementById('stack-top-indicator');
const websiteCardsContainer = document.getElementById('website-cards-container');
const historyItemsContainer = document.getElementById('history-items-container');
const toastContainer = document.getElementById('toast-container');
const backendStatusText = document.getElementById('backend-status-text');
const backendStatusBadge = document.getElementById('backend-status');
const statusHint = document.getElementById('status-hint');

// Toast Notification Helper
function showToast(message, type = 'info', duration = 4000) {
  const toast = document.createElement('div');
  toast.className = `toast toast-${type}`;
  toast.setAttribute('role', 'alert');

  let iconSvg = '';
  if (type === 'success') {
    iconSvg = `<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="#10b981" stroke-width="2"><polyline points="20 6 9 17 4 12"></polyline></svg>`;
  } else if (type === 'error') {
    iconSvg = `<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="#ef4444" stroke-width="2"><circle cx="12" cy="12" r="10"></circle><line x1="15" y1="9" x2="9" y2="15"></line><line x1="9" y1="9" x2="15" y2="15"></line></svg>`;
  } else if (type === 'warning') {
    iconSvg = `<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="#f59e0b" stroke-width="2"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"></path><line x1="12" y1="9" x2="12" y2="13"></line><line x1="12" y1="17" x2="12.01" y2="17"></line></svg>`;
  } else {
    iconSvg = `<svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="#3b82f6" stroke-width="2"><circle cx="12" cy="12" r="10"></circle><line x1="12" y1="16" x2="12" y2="12"></line><line x1="12" y1="8" x2="12.01" y2="8"></line></svg>`;
  }

  toast.innerHTML = `
    <div class="toast-icon">${iconSvg}</div>
    <div class="toast-message">${escapeHtml(message)}</div>
  `;

  toastContainer.appendChild(toast);

  setTimeout(() => {
    toast.style.opacity = '0';
    toast.style.transform = 'translateX(30px)';
    toast.style.transition = 'all 0.3s ease';
    setTimeout(() => toast.remove(), 300);
  }, duration);
}

// HTML escape utility
function escapeHtml(str) {
  if (!str) return '';
  return str.replace(/[&<>"']/g, m => ({
    '&': '&amp;',
    '<': '&lt;',
    '>': '&gt;',
    '"': '&quot;',
    "'": '&#39;'
  }[m]));
}

// Get clean display name for a URL
function getSiteName(url) {
  if (WEBSITE_META[url]) return WEBSITE_META[url].name;
  try {
    const parsed = new URL(url);
    return parsed.hostname.replace('www.', '');
  } catch (e) {
    return url;
  }
}

// Set Backend Connection Status
function setBackendStatus(online, message = '') {
  if (online) {
    backendStatusBadge.className = 'status-badge';
    backendStatusText.textContent = 'C++ Backend Online (Connected)';
  } else {
    backendStatusBadge.className = 'status-badge status-offline';
    backendStatusText.textContent = message || 'Backend Disconnected';
  }
}

// API: Fetch Predefined Websites
async function fetchWebsites() {
  try {
    const res = await fetch(`${API_BASE}/api/websites`);
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    const data = await res.json();
    renderWebsites(data.websites || DEFAULT_WEBSITES);
    setBackendStatus(true);
  } catch (err) {
    console.warn('Could not fetch websites from backend, using default list:', err);
    renderWebsites(DEFAULT_WEBSITES);
    setBackendStatus(false, 'Connecting to C++ backend...');
  }
}

// API: Fetch Current Simulator State
async function fetchState() {
  try {
    const res = await fetch(`${API_BASE}/api/state`);
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    const data = await res.json();
    updateUI(data);
    setBackendStatus(true);
  } catch (err) {
    console.error('Error fetching simulator state:', err);
    setBackendStatus(false, 'Backend Offline');
  }
}

// API: Visit Page
async function visitPage(url, siteName = '') {
  try {
    // 1. Open external website in new browser tab
    window.open(url, '_blank', 'noopener,noreferrer');

    // 2. Synchronize with C++ backend stack
    const res = await fetch(`${API_BASE}/api/visit?url=${encodeURIComponent(url)}`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ url: url })
    });

    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    const data = await res.json();
    updateUI(data);

    const displayName = siteName || getSiteName(url);
    showToast(`Opened ${displayName} in new tab. Previous page pushed onto C++ stack.`, 'success');
  } catch (err) {
    console.error('Error visiting page:', err);
    showToast(`Failed to update backend: ${err.message}`, 'error');
  }
}

// API: Navigate Back
async function goBack() {
  if (!currentState.canGoBack) {
    showToast('No previous page available in history stack.', 'warning');
    return;
  }

  try {
    const res = await fetch(`${API_BASE}/api/back`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' }
    });

    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    const data = await res.json();

    if (data.success) {
      updateUI(data);
      const prevSite = getSiteName(data.currentPage);
      showToast(`Navigated back to: ${prevSite}`, 'success');
    } else {
      showToast(data.message || 'Cannot go back.', 'warning');
    }
  } catch (err) {
    console.error('Error navigating back:', err);
    showToast(`Backend communication error: ${err.message}`, 'error');
  }
}

// API: Clear History
async function clearHistory() {
  try {
    const res = await fetch(`${API_BASE}/api/clear`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' }
    });

    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    const data = await res.json();
    updateUI(data);
    showToast('History stack cleared and current page reset.', 'info');
  } catch (err) {
    console.error('Error clearing history:', err);
    showToast(`Error clearing history: ${err.message}`, 'error');
  }
}

// Render Website Cards
function renderWebsites(websites) {
  websiteCardsContainer.innerHTML = '';
  websites.forEach(site => {
    const meta = WEBSITE_META[site.url] || {
      name: site.name,
      accent: '#3b82f6',
      icon: `<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"></circle></svg>`
    };

    const card = document.createElement('article');
    card.className = 'website-card';
    card.style.setProperty('--card-accent', meta.accent);

    card.innerHTML = `
      <div class="card-top">
        <div class="card-icon">${meta.icon}</div>
        <div class="card-meta">
          <h3>${escapeHtml(site.name)}</h3>
          <p class="card-url" title="${escapeHtml(site.url)}">${escapeHtml(site.url)}</p>
        </div>
      </div>
      <div class="card-actions">
        <button class="btn btn-visit" data-url="${escapeHtml(site.url)}" data-name="${escapeHtml(site.name)}">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <path d="M18 13v6a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h6"></path>
            <polyline points="15 3 21 3 21 9"></polyline>
            <line x1="10" y1="14" x2="21" y2="3"></line>
          </svg>
          <span>Open Website</span>
        </button>
      </div>
    `;

    const visitBtn = card.querySelector('.btn-visit');
    visitBtn.addEventListener('click', () => {
      visitPage(site.url, site.name);
    });

    websiteCardsContainer.appendChild(card);
  });
}

// Update UI based on C++ Backend State
function updateUI(state) {
  currentState = state;

  // 1. Active URL & Controls
  if (state.currentPage) {
    currentUrlInput.value = state.currentPage;
    btnReopenCurrent.disabled = false;
    statusHint.textContent = `Active page: ${getSiteName(state.currentPage)}`;
  } else {
    currentUrlInput.value = 'No webpage open yet (Empty State)';
    btnReopenCurrent.disabled = true;
    statusHint.textContent = 'Click "Open Website" on any card below to start browsing.';
  }

  // Back Button State
  btnBack.disabled = !state.canGoBack;
  btnClear.disabled = (!state.currentPage && (!state.history || state.history.length === 0));

  // 2. Stack Representation
  const historyList = state.history || [];
  stackCountBadge.textContent = `Size: ${historyList.length}`;

  if (historyList.length === 0) {
    stackTopIndicator.style.display = 'none';
    visualStackContainer.innerHTML = `
      <div class="stack-empty-state">
        <svg width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5">
          <path d="M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z"></path>
        </svg>
        <p>History stack is currently empty</p>
        <small>No previous pages stored in memory</small>
      </div>
    `;
  } else {
    stackTopIndicator.style.display = 'flex';
    visualStackContainer.innerHTML = '';

    // historyList is ordered most-recent-first (Top of stack is index 0)
    historyList.forEach((url, index) => {
      const frame = document.createElement('div');
      const isTop = (index === 0);
      frame.className = `stack-frame ${isTop ? 'is-top' : ''}`;

      const siteName = getSiteName(url);
      frame.innerHTML = `
        <div class="frame-info">
          <span class="frame-name">${escapeHtml(siteName)}</span>
          <span class="frame-url" title="${escapeHtml(url)}">${escapeHtml(url)}</span>
        </div>
        <span class="frame-badge ${isTop ? 'top-badge' : ''}">
          ${isTop ? 'TOP [idx 0]' : `idx ${index}`}
        </span>
      `;
      visualStackContainer.appendChild(frame);
    });
  }

  // 3. Backtracking History Table
  if (historyList.length === 0) {
    historyItemsContainer.innerHTML = `<div class="history-empty-message">No history entries. Start browsing to populate the stack.</div>`;
  } else {
    historyItemsContainer.innerHTML = '';
    historyList.forEach((url, index) => {
      const row = document.createElement('div');
      row.className = 'history-row';
      const isTop = (index === 0);
      const siteName = getSiteName(url);

      row.innerHTML = `
        <div class="history-pos">
          <span class="pos-badge ${isTop ? 'top-badge' : ''}">${isTop ? 'TOP OF STACK' : `Level ${historyList.length - index}`}</span>
        </div>
        <div class="history-name">${escapeHtml(siteName)}</div>
        <div class="history-url" title="${escapeHtml(url)}">${escapeHtml(url)}</div>
        <div>
          <button class="btn btn-ghost btn-sm" onclick="window.open('${escapeHtml(url)}', '_blank', 'noopener,noreferrer')">
            Visit
          </button>
        </div>
      `;
      historyItemsContainer.appendChild(row);
    });
  }
}

// Event Listeners
btnBack.addEventListener('click', goBack);
btnClear.addEventListener('click', clearHistory);
btnRefreshHistory.addEventListener('click', () => {
  fetchState();
  showToast('Synchronized with C++ backend.', 'info');
});

btnReopenCurrent.addEventListener('click', () => {
  if (currentState.currentPage) {
    window.open(currentState.currentPage, '_blank', 'noopener,noreferrer');
  }
});

// Team & Viva Modals
const teamModal = document.getElementById('team-modal');
const vivaModal = document.getElementById('viva-modal');
const btnTeamModal = document.getElementById('btn-team-modal');
const btnVivaModal = document.getElementById('btn-viva-modal');
const btnCloseTeam = document.getElementById('btn-close-team');
const btnCloseViva = document.getElementById('btn-close-viva');

if (btnTeamModal && teamModal) {
  btnTeamModal.addEventListener('click', () => {
    if (typeof teamModal.showModal === 'function') {
      teamModal.showModal();
    } else {
      teamModal.setAttribute('open', '');
    }
  });
}

if (btnCloseTeam && teamModal) {
  btnCloseTeam.addEventListener('click', () => {
    if (typeof teamModal.close === 'function') {
      teamModal.close();
    } else {
      teamModal.removeAttribute('open');
    }
  });
  teamModal.addEventListener('click', (e) => {
    if (e.target === teamModal) teamModal.close();
  });
}

if (btnVivaModal && vivaModal) {
  btnVivaModal.addEventListener('click', () => {
    if (typeof vivaModal.showModal === 'function') {
      vivaModal.showModal();
    } else {
      vivaModal.setAttribute('open', '');
    }
  });
}

if (btnCloseViva && vivaModal) {
  btnCloseViva.addEventListener('click', () => {
    if (typeof vivaModal.close === 'function') {
      vivaModal.close();
    } else {
      vivaModal.removeAttribute('open');
    }
  });
  vivaModal.addEventListener('click', (e) => {
    if (e.target === vivaModal) vivaModal.close();
  });
}

// Periodic heartbeat & initial load
window.addEventListener('DOMContentLoaded', () => {
  fetchWebsites();
  fetchState();

  // Periodically verify connection every 10 seconds
  setInterval(fetchState, 10000);
});
