#include "loginpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QTabWidget>
#include <QFrame>

LoginPage::LoginPage(QWidget* parent) : QWidget(parent) {
    buildUI();
}

void LoginPage::buildUI() {
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setAlignment(Qt::AlignCenter);
    root->setSpacing(0);
    root->setContentsMargins(40, 40, 40, 40);

    // ─── Brand Header ────────────────────────────────────
    QLabel* title = new QLabel("ShopNova");
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "QLabel { font-size: 42px; font-weight: 800; letter-spacing: -1.5px; "
        "color: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #ff6b2b, stop:1 #ff9a5c); }");

    QLabel* sub = new QLabel("Your Centralized Multi-Shop E-Commerce Platform");
    sub->setObjectName("subtitleLabel");
    sub->setAlignment(Qt::AlignCenter);

    // Version badge
    QWidget* badgeRow = new QWidget;
    QHBoxLayout* badgeLay = new QHBoxLayout(badgeRow);
    badgeLay->setAlignment(Qt::AlignCenter);
    badgeLay->setContentsMargins(0, 8, 0, 0);
    QLabel* versionBadge = new QLabel("✦  v2.0 — Qt Edition  ✦");
    versionBadge->setObjectName("versionBadge");
    badgeLay->addWidget(versionBadge);

    root->addWidget(title);
    root->addSpacing(4);
    root->addWidget(sub);
    root->addWidget(badgeRow);
    root->addSpacing(28);

    // ─── Auth Card Container ─────────────────────────────
    QFrame* card = new QFrame;
    card->setFixedWidth(460);
    card->setStyleSheet(
        "QFrame { background: #111118; "
        "border: 1px solid rgba(255,255,255,0.07); "
        "border-radius: 16px; }"
    );
    QVBoxLayout* cardLay = new QVBoxLayout(card);
    cardLay->setContentsMargins(0, 0, 0, 0);
    cardLay->setSpacing(0);

    // Tabs inside card
    QTabWidget* tabs = new QTabWidget;
    cardLay->addWidget(tabs);

    // ─── LOGIN TAB ───────────────────────────────────────
    QWidget* loginTab = new QWidget;
    QVBoxLayout* loginLay = new QVBoxLayout(loginTab);
    loginLay->setSpacing(14);
    loginLay->setContentsMargins(28, 28, 28, 28);

    QLabel* loginTitle = new QLabel("Welcome back");
    loginTitle->setStyleSheet("font-size: 20px; font-weight: 700; color: #f0f0f5; letter-spacing: -0.5px;");

    QLabel* loginSub = new QLabel("Sign in to access your dashboard");
    loginSub->setStyleSheet("font-size: 13px; color: #5a5a70; margin-bottom: 8px;");

    QLabel* emailLabel = new QLabel("Email");
    emailLabel->setStyleSheet("font-size: 13px; font-weight: 500; color: #9494a8;");
    m_emailEdit = new QLineEdit;
    m_emailEdit->setPlaceholderText("Enter your email address");
    m_emailEdit->setMinimumHeight(42);

    QLabel* pwdLabel = new QLabel("Password");
    pwdLabel->setStyleSheet("font-size: 13px; font-weight: 500; color: #9494a8;");
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setPlaceholderText("Enter your password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMinimumHeight(42);

    QPushButton* loginBtn = new QPushButton("Sign In  →");
    loginBtn->setObjectName("primaryBtn");
    loginBtn->setMinimumHeight(46);
    loginBtn->setStyleSheet(loginBtn->styleSheet() +
        "QPushButton { font-size: 15px; font-weight: 700; letter-spacing: -0.3px; }");
    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::onLoginClicked);

    QLabel* hint = new QLabel(
        "Demo Accounts\n"
        "Admin: admin@shopnova.com / admin123\n"
        "Customer: arjun@gmail.com / pass123\n"
        "Seller: ravi@seller.com / pass123");
    hint->setObjectName("hintLabel");
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet(hint->styleSheet() +
        "QLabel { background: rgba(255,107,43,0.06); "
        "border: 1px solid rgba(255,107,43,0.12); "
        "border-radius: 8px; padding: 10px; "
        "color: #5a5a70; font-size: 11px; }");

    loginLay->addWidget(loginTitle);
    loginLay->addWidget(loginSub);
    loginLay->addSpacing(6);
    loginLay->addWidget(emailLabel);
    loginLay->addWidget(m_emailEdit);
    loginLay->addWidget(pwdLabel);
    loginLay->addWidget(m_passwordEdit);
    loginLay->addSpacing(6);
    loginLay->addWidget(loginBtn);
    loginLay->addSpacing(10);
    loginLay->addWidget(hint);
    loginLay->addStretch();

    tabs->addTab(loginTab, "  Sign In  ");

    // ─── REGISTER CUSTOMER TAB ───────────────────────────
    QWidget* regTab = new QWidget;
    QVBoxLayout* regLay = new QVBoxLayout(regTab);
    regLay->setSpacing(12);
    regLay->setContentsMargins(28, 28, 28, 28);

    QLabel* regTitle = new QLabel("Create Account");
    regTitle->setStyleSheet("font-size: 20px; font-weight: 700; color: #f0f0f5;");
    QLabel* regSub = new QLabel("Join ShopNova and start shopping");
    regSub->setStyleSheet("font-size: 13px; color: #5a5a70; margin-bottom: 8px;");

    m_regName     = new QLineEdit; m_regName->setPlaceholderText("Full name");     m_regName->setMinimumHeight(42);
    m_regEmail    = new QLineEdit; m_regEmail->setPlaceholderText("Email address"); m_regEmail->setMinimumHeight(42);
    m_regPassword = new QLineEdit; m_regPassword->setPlaceholderText("Create password"); m_regPassword->setMinimumHeight(42);
    m_regPassword->setEchoMode(QLineEdit::Password);
    m_regPhone    = new QLineEdit; m_regPhone->setPlaceholderText("Phone number"); m_regPhone->setMinimumHeight(42);

    QPushButton* regBtn = new QPushButton("Create Customer Account  →");
    regBtn->setObjectName("successBtn");
    regBtn->setMinimumHeight(46);
    connect(regBtn, &QPushButton::clicked, this, &LoginPage::onRegisterCustomerClicked);

    auto addField = [](QVBoxLayout* lay, const QString& label, QLineEdit* edit) {
        QLabel* lbl = new QLabel(label);
        lbl->setStyleSheet("font-size: 13px; font-weight: 500; color: #9494a8;");
        lay->addWidget(lbl);
        lay->addWidget(edit);
    };

    regLay->addWidget(regTitle);
    regLay->addWidget(regSub);
    regLay->addSpacing(4);
    addField(regLay, "Full Name", m_regName);
    addField(regLay, "Email", m_regEmail);
    addField(regLay, "Password", m_regPassword);
    addField(regLay, "Phone", m_regPhone);
    regLay->addSpacing(6);
    regLay->addWidget(regBtn);
    regLay->addStretch();

    tabs->addTab(regTab, "  Customer Signup  ");

    // ─── REGISTER SELLER TAB ─────────────────────────────
    QWidget* selTab = new QWidget;
    QVBoxLayout* selLay = new QVBoxLayout(selTab);
    selLay->setSpacing(12);
    selLay->setContentsMargins(28, 28, 28, 28);

    QLabel* selTitle = new QLabel("Open Your Shop");
    selTitle->setStyleSheet("font-size: 20px; font-weight: 700; color: #f0f0f5;");
    QLabel* selSub = new QLabel("Start selling on ShopNova today");
    selSub->setStyleSheet("font-size: 13px; color: #5a5a70; margin-bottom: 8px;");

    m_selName      = new QLineEdit; m_selName->setPlaceholderText("Your full name"); m_selName->setMinimumHeight(42);
    m_selEmail     = new QLineEdit; m_selEmail->setPlaceholderText("Email address"); m_selEmail->setMinimumHeight(42);
    m_selPassword  = new QLineEdit; m_selPassword->setPlaceholderText("Create password"); m_selPassword->setMinimumHeight(42);
    m_selPassword->setEchoMode(QLineEdit::Password);
    m_selPhone     = new QLineEdit; m_selPhone->setPlaceholderText("Phone number"); m_selPhone->setMinimumHeight(42);
    m_selStoreName = new QLineEdit; m_selStoreName->setPlaceholderText("Your store / shop name"); m_selStoreName->setMinimumHeight(42);

    QPushButton* selBtn = new QPushButton("Open My Shop  →");
    selBtn->setObjectName("warnBtn");
    selBtn->setMinimumHeight(46);
    connect(selBtn, &QPushButton::clicked, this, &LoginPage::onRegisterSellerClicked);

    selLay->addWidget(selTitle);
    selLay->addWidget(selSub);
    selLay->addSpacing(4);
    addField(selLay, "Full Name", m_selName);
    addField(selLay, "Email", m_selEmail);
    addField(selLay, "Password", m_selPassword);
    addField(selLay, "Phone", m_selPhone);
    addField(selLay, "Store Name", m_selStoreName);
    selLay->addSpacing(6);
    selLay->addWidget(selBtn);
    selLay->addStretch();

    tabs->addTab(selTab, "  Seller Signup  ");

    root->addWidget(card, 0, Qt::AlignCenter);
    root->addStretch();
}

void LoginPage::onLoginClicked() {
    emit loginRequested(m_emailEdit->text().trimmed(), m_passwordEdit->text());
    m_passwordEdit->clear();
}

void LoginPage::onRegisterCustomerClicked() {
    emit registerCustomerRequested(
        m_regName->text().trimmed(),
        m_regEmail->text().trimmed(),
        m_regPassword->text(),
        m_regPhone->text().trimmed());
    m_regName->clear(); m_regEmail->clear();
    m_regPassword->clear(); m_regPhone->clear();
}

void LoginPage::onRegisterSellerClicked() {
    emit registerSellerRequested(
        m_selName->text().trimmed(),
        m_selEmail->text().trimmed(),
        m_selPassword->text(),
        m_selPhone->text().trimmed(),
        m_selStoreName->text().trimmed());
    m_selName->clear(); m_selEmail->clear();
    m_selPassword->clear(); m_selPhone->clear(); m_selStoreName->clear();
}
