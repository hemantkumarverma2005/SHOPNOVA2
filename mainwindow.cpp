#include "mainwindow.h"
#include <QMessageBox>
#include <QApplication>

// ═══════════════════════════════════════════════════════════════
//  SHOPNOVA — Premium Dark Theme  (high-contrast edition)
// ═══════════════════════════════════════════════════════════════
static const char* APP_STYLE = R"(

/* ── GLOBAL ─────────────────────────────────── */
QMainWindow, QWidget, QDialog {
    background-color: #0c0c14;
    color: #e0e0f0;
    font-family: "Segoe UI", "Ubuntu", "Helvetica Neue", sans-serif;
    font-size: 13px;
}

/* ── GROUP BOX (CARD style) ─────────────────── */
QGroupBox {
    border: 1px solid #252540;
    border-radius: 16px;
    margin-top: 14px;
    padding: 14px 10px 10px 10px;
    font-weight: bold;
    color: #ff9a5c;
    background-color: #1a1a2e;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 16px;
    padding: 0 8px;
    color: #ff9a5c;
}

/* ── BUTTONS ────────────────────────────────── */
QPushButton {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #ff6b2b, stop:1 #e85d20);
    color: #ffffff;
    border: none;
    border-radius: 20px;
    padding: 9px 22px;
    font-weight: 600;
    font-size: 13px;
}
QPushButton:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #ff9a5c, stop:1 #ff6b2b);
}
QPushButton:pressed {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #d4551c, stop:1 #c04a17);
}

QPushButton#primaryBtn {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #ff6b2b, stop:1 #e85d20);
    color: #fff;
}
QPushButton#primaryBtn:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #ff9a5c, stop:1 #ff6b2b);
}

QPushButton#successBtn {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #06d6a0, stop:1 #05b588);
    color: #ffffff;
    font-weight: 700;
}
QPushButton#successBtn:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #2aedb8, stop:1 #06d6a0);
}

QPushButton#dangerBtn {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #ff4444, stop:1 #cc3636);
    color: #fff;
}
QPushButton#dangerBtn:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #ff6666, stop:1 #ff4444);
}

QPushButton#warnBtn {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #ffd166, stop:1 #e6b84d);
    color: #ffffff;
    font-weight: 700;
}
QPushButton#warnBtn:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #ffdd88, stop:1 #ffd166);
}

QPushButton#logoutBtn {
    background: #1a1a2e;
    border: 1px solid #252540;
    color: #b0b0c8;
    border-radius: 20px;
}
QPushButton#logoutBtn:hover {
    background: #222240;
    color: #f0f0f5;
    border-color: #3a3a55;
}

QPushButton#ghostBtn {
    background: transparent;
    border: 1px solid #252540;
    color: #b0b0c8;
    border-radius: 20px;
}
QPushButton#ghostBtn:hover {
    background: #1a1a2e;
    color: #f0f0f5;
    border-color: #3a3a55;
}

/* ── SIDEBAR NAV BUTTON ────────────────────── */
QPushButton#navBtn {
    background: transparent;
    color: #a0a0b8;
    border-radius: 10px;
    text-align: left;
    padding: 0 16px;
    font-size: 13px;
    font-weight: 500;
    border: none;
}
QPushButton#navBtn:hover {
    background: rgba(255, 107, 43, 0.10);
    color: #e8e8f0;
}
QPushButton#navBtn:checked {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #ff6b2b, stop:1 #ff8844);
    color: #ffffff;
    font-weight: 700;
}

/* ── APP SHELL LAYOUT COMPONENTS ───────────── */
QWidget#globalHeader {
    background: #0e0e18;
    border-bottom: 1px solid #252540;
}
QFrame#floatingSidebar {
    background: #161630;
    border: 1px solid #252540;
    border-radius: 12px;
}

/* ── INPUT FIELDS ───────────────────────────── */
QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
    background-color: #1a1a2e;
    border: 1px solid #2a2a42;
    border-radius: 8px;
    padding: 9px 14px;
    color: #e8e8f0;
    font-size: 14px;
    selection-background-color: rgba(255, 107, 43, 0.3);
}
QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
    border: 1px solid #ff6b2b;
}
QLineEdit::placeholder {
    color: #6a6a85;
}
QComboBox::drop-down {
    border: none;
    width: 24px;
}
QComboBox QAbstractItemView, QListView {
    background-color: #1a1a2e;
    color: #e8e8f0;
    selection-background-color: rgba(255, 107, 43, 0.2);
    selection-color: #ff9a5c;
    border: 1px solid #2a2a42;
    border-radius: 8px;
    padding: 4px;
}

/* ── TABLE ──────────────────────────────────── */
QTableWidget {
    background-color: #161630;
    color: #e0e0f0;
    border: 1px solid #252540;
    gridline-color: #1e1e38;
    border-radius: 12px;
    alternate-background-color: #1c1c36;
    selection-background-color: rgba(255, 107, 43, 0.15);
    selection-color: #ff9a5c;
}
QTableWidget::item {
    color: #d8d8e8;
    padding: 6px 12px;
    border-bottom: 1px solid #1e1e38;
}
QTableWidget::item:selected {
    background-color: rgba(255, 107, 43, 0.15);
    color: #ff9a5c;
}
QHeaderView::section {
    background-color: #1a1a34;
    color: #9898b8;
    padding: 10px 12px;
    border: none;
    border-bottom: 2px solid #ff6b2b;
    font-weight: 700;
    font-size: 11px;
    text-transform: uppercase;
    letter-spacing: 0.5px;
}

/* ── TAB WIDGET ─────────────────────────────── */
QTabWidget::pane {
    border: 1px solid #252540;
    border-radius: 12px;
    background: #0c0c14;
    top: -1px;
}
QTabBar::tab {
    background: transparent;
    color: #7878a0;
    padding: 10px 24px;
    border: none;
    border-bottom: 2px solid transparent;
    margin-right: 0px;
    font-size: 13px;
    font-weight: 500;
}
QTabBar::tab:hover {
    color: #b0b0c8;
}
QTabBar::tab:selected {
    color: #ff9a5c;
    border-bottom: 2px solid #ff6b2b;
    font-weight: 600;
}

/* ── LABELS ─────────────────────────────────── */
QLabel#titleLabel {
    font-size: 36px;
    font-weight: 800;
    color: #ff6b2b;
    letter-spacing: -1px;
}
QLabel#subtitleLabel {
    font-size: 14px;
    color: #8888a8;
}
QLabel#pageTitle {
    font-size: 22px;
    font-weight: 700;
    color: #f0f0f5;
    letter-spacing: -0.5px;
}
QLabel#statsLabel {
    background-color: #1a1a2e;
    border: 1px solid #252540;
    border-radius: 12px;
    padding: 12px 20px;
    color: #b0b0c8;
    font-size: 13px;
}
QLabel#balanceLabel {
    font-size: 16px;
    font-weight: 700;
    color: #2ee8b0;
}
QLabel#notifLabel {
    color: #ffdd77;
    font-weight: 600;
    font-size: 13px;
}
QLabel#hintLabel {
    color: #7878a0;
    font-size: 11px;
    line-height: 1.5;
}
QLabel#profileLabel {
    font-size: 14px;
    color: #e0e0f0;
    padding: 24px;
    background-color: #1a1a2e;
    border: 1px solid #252540;
    border-radius: 16px;
    line-height: 1.8;
}
QLabel#statCard {
    background-color: #1a1a2e;
    border: 1px solid #252540;
    border-radius: 16px;
    padding: 20px;
    font-size: 13px;
    color: #b0b0c8;
}
QLabel#accentLabel {
    color: #ff9a5c;
    font-weight: 600;
}
QLabel#roleBadge {
    padding: 5px 14px;
    border-radius: 20px;
    font-size: 11px;
    font-weight: 700;
    letter-spacing: 0.5px;
}

/* ── SCROLLBAR ──────────────────────────────── */
QScrollBar:vertical {
    background: transparent;
    width: 6px;
    border-radius: 3px;
}
QScrollBar::handle:vertical {
    background: #2a2a42;
    border-radius: 3px;
    min-height: 30px;
}
QScrollBar::handle:vertical:hover {
    background: #3a3a55;
}
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0px;
}
QScrollBar:horizontal {
    background: transparent;
    height: 6px;
    border-radius: 3px;
}
QScrollBar::handle:horizontal {
    background: #2a2a42;
    border-radius: 3px;
}

/* ── DIALOG / MESSAGE BOX ───────────────────── */
QMessageBox {
    background-color: #1a1a2e;
    color: #e0e0f0;
}
QMessageBox QLabel {
    color: #e0e0f0;
}
QInputDialog {
    background-color: #1a1a2e;
    color: #e0e0f0;
}
QInputDialog QLabel {
    color: #e0e0f0;
}
QDialog {
    background-color: #1a1a2e;
    color: #e0e0f0;
}

/* ── FORM LAYOUT LABELS ─────────────────────── */
QFormLayout QLabel {
    color: #c0c0d8;
    font-weight: 500;
    font-size: 13px;
}

/* ── STATUS PILL BADGES ────────────────────── */
QLabel#statusPending {
    background: rgba(255, 209, 102, 0.18);
    color: #ffdd77;
    border: 1px solid rgba(255, 209, 102, 0.35);
    border-radius: 12px;
    padding: 4px 12px;
    font-size: 11px;
    font-weight: 700;
}
QLabel#statusConfirmed {
    background: rgba(76, 201, 240, 0.18);
    color: #66d4f4;
    border: 1px solid rgba(76, 201, 240, 0.35);
    border-radius: 12px;
    padding: 4px 12px;
    font-size: 11px;
    font-weight: 700;
}
QLabel#statusShipped {
    background: rgba(160, 130, 220, 0.18);
    color: #c4b0ee;
    border: 1px solid rgba(160, 130, 220, 0.35);
    border-radius: 12px;
    padding: 4px 12px;
    font-size: 11px;
    font-weight: 700;
}
QLabel#statusDelivered {
    background: rgba(6, 214, 160, 0.18);
    color: #2ee8b0;
    border: 1px solid rgba(6, 214, 160, 0.35);
    border-radius: 12px;
    padding: 4px 12px;
    font-size: 11px;
    font-weight: 700;
}
QLabel#statusCancelled {
    background: rgba(255, 80, 80, 0.18);
    color: #ff6666;
    border: 1px solid rgba(255, 80, 80, 0.35);
    border-radius: 12px;
    padding: 4px 12px;
    font-size: 11px;
    font-weight: 700;
}
QLabel#statusReturned {
    background: rgba(255, 154, 92, 0.18);
    color: #ffb080;
    border: 1px solid rgba(255, 154, 92, 0.35);
    border-radius: 12px;
    padding: 4px 12px;
    font-size: 11px;
    font-weight: 700;
}
QLabel#statusActive {
    background: rgba(6, 214, 160, 0.18);
    color: #2ee8b0;
    border: 1px solid rgba(6, 214, 160, 0.35);
    border-radius: 12px;
    padding: 4px 12px;
    font-size: 11px;
    font-weight: 700;
}
QLabel#statusInactive {
    background: rgba(255, 80, 80, 0.18);
    color: #ff6666;
    border: 1px solid rgba(255, 80, 80, 0.35);
    border-radius: 12px;
    padding: 4px 12px;
    font-size: 11px;
    font-weight: 700;
}
QLabel#statusVerified {
    background: rgba(6, 214, 160, 0.18);
    color: #2ee8b0;
    border: 1px solid rgba(6, 214, 160, 0.35);
    border-radius: 12px;
    padding: 4px 12px;
    font-size: 11px;
    font-weight: 700;
}

/* ── WELCOME BANNER ────────────────────────── */
QFrame#welcomeBanner {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #1a1020, stop:0.5 #101a18, stop:1 #0c1020);
    border: 1px solid #252540;
    border-radius: 20px;
    padding: 28px 32px;
}
QFrame#welcomeBanner QLabel {
    background: transparent;
    border: none;
}

/* ── VERSION BADGE ─────────────────────────── */
QLabel#versionBadge {
    background: rgba(255, 107, 43, 0.14);
    border: 1px solid rgba(255, 107, 43, 0.30);
    border-radius: 12px;
    padding: 4px 14px;
    font-size: 11px;
    font-weight: 600;
    color: #ff9a5c;
    letter-spacing: 0.5px;
}

/* ── SELLER VERIFIED BADGE ─────────────────── */
QLabel#verifiedBadge {
    background: rgba(6, 214, 160, 0.18);
    color: #2ee8b0;
    border: 1px solid rgba(6, 214, 160, 0.35);
    border-radius: 20px;
    padding: 5px 14px;
    font-size: 11px;
    font-weight: 700;
}
QLabel#pendingBadge {
    background: rgba(255, 209, 102, 0.18);
    color: #ffdd77;
    border: 1px solid rgba(255, 209, 102, 0.35);
    border-radius: 20px;
    padding: 5px 14px;
    font-size: 11px;
    font-weight: 700;
}

/* ── ROLE BADGES ───────────────────────────── */
QLabel#customerBadge {
    background: rgba(76, 201, 240, 0.18);
    color: #66d4f4;
    border: 1px solid rgba(76, 201, 240, 0.35);
    border-radius: 20px;
    padding: 5px 14px;
    font-size: 11px;
    font-weight: 700;
}
QLabel#sellerBadge {
    background: rgba(6, 214, 160, 0.18);
    color: #2ee8b0;
    border: 1px solid rgba(6, 214, 160, 0.35);
    border-radius: 20px;
    padding: 5px 14px;
    font-size: 11px;
    font-weight: 700;
}

)";

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_platform = Platform::getInstance();
    setWindowTitle("ShopNova — Multi-Shop E-Commerce Platform");
    resize(1280, 800);
    applyStyle();

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    m_loginPage = new LoginPage;
    connect(m_loginPage, &LoginPage::loginRequested,              this, &MainWindow::onLoginRequested);
    connect(m_loginPage, &LoginPage::registerCustomerRequested,   this, &MainWindow::onRegisterCustomer);
    connect(m_loginPage, &LoginPage::registerSellerRequested,     this, &MainWindow::onRegisterSeller);
    m_stack->addWidget(m_loginPage); // index 0
    m_stack->setCurrentIndex(PAGE_LOGIN);
}

MainWindow::~MainWindow() {}

void MainWindow::applyStyle() {
    qApp->setStyleSheet(APP_STYLE);
}

void MainWindow::goToPage(int index) {
    m_stack->setCurrentIndex(index);
}

void MainWindow::onLoginRequested(const QString& email, const QString& password) {
    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login", "Please enter email and password."); return;
    }
    User* user = m_platform->login(email, password);
    if (!user) {
        QMessageBox::warning(this, "Login Failed", "Invalid email or password."); return;
    }

    switch (user->getRole()) {
    case UserRole::ADMIN: {
        Admin* admin = dynamic_cast<Admin*>(user);
        if (m_adminPage) { m_stack->removeWidget(m_adminPage); delete m_adminPage; }
        m_adminPage = new AdminDashboard(admin, m_platform);
        connect(m_adminPage, &AdminDashboard::logoutRequested, this, &MainWindow::onLogout);
        m_stack->addWidget(m_adminPage);
        m_adminPage->refresh();
        goToPage(m_stack->indexOf(m_adminPage));
        break;
    }
    case UserRole::SELLER: {
        Seller* seller = dynamic_cast<Seller*>(user);
        if (m_sellerPage) { m_stack->removeWidget(m_sellerPage); delete m_sellerPage; }
        m_sellerPage = new SellerDashboard(seller, m_platform);
        connect(m_sellerPage, &SellerDashboard::logoutRequested, this, &MainWindow::onLogout);
        m_stack->addWidget(m_sellerPage);
        m_sellerPage->refresh();
        goToPage(m_stack->indexOf(m_sellerPage));
        break;
    }
    case UserRole::CUSTOMER: {
        Customer* customer = dynamic_cast<Customer*>(user);
        if (m_customerPage) { m_stack->removeWidget(m_customerPage); delete m_customerPage; }
        m_customerPage = new CustomerDashboard(customer, m_platform);
        connect(m_customerPage, &CustomerDashboard::logoutRequested, this, &MainWindow::onLogout);
        m_stack->addWidget(m_customerPage);
        m_customerPage->refresh();
        goToPage(m_stack->indexOf(m_customerPage));
        break;
    }
    }
}

void MainWindow::onRegisterCustomer(const QString& name, const QString& email,
                                    const QString& password, const QString& phone)
{
    if (name.isEmpty() || email.isEmpty() || password.isEmpty() || phone.isEmpty()) {
        QMessageBox::warning(this, "Register", "All fields are required."); return;
    }
    try {
        Customer* c = m_platform->registerCustomer(name, email, password, phone);
        QMessageBox::information(this, "Registered!",
                                 "Account created! Your ID: " + QString::number(c->getId()) +
                                     "\nYou can now login with your email.");
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void MainWindow::onRegisterSeller(const QString& name, const QString& email,
                                  const QString& password, const QString& phone, const QString& storeName)
{
    if (name.isEmpty() || email.isEmpty() || password.isEmpty() || storeName.isEmpty()) {
        QMessageBox::warning(this, "Register", "All fields are required."); return;
    }
    Seller* s = m_platform->registerSeller(name, email, password, phone, storeName);
    QMessageBox::information(this, "Shop Created!",
                             "Seller account created!\nStore: " + s->getStoreName() +
                                 "\nID: " + QString::number(s->getId()) +
                                 "\nAn admin will verify your account soon.");
}

void MainWindow::onLogout() {
    goToPage(PAGE_LOGIN);
}
