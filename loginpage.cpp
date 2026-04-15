#include "loginpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QTabWidget>

LoginPage::LoginPage(QWidget* parent) : QWidget(parent) {
    buildUI();
}

void LoginPage::buildUI() {
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setAlignment(Qt::AlignCenter);
    root->setSpacing(20);
    root->setContentsMargins(40, 40, 40, 40);

    // Title
    QLabel* title = new QLabel("🛒  ShopNova");
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);

    QLabel* sub = new QLabel("Your Centralized Multi-Shop E-Commerce Platform");
    sub->setObjectName("subtitleLabel");
    sub->setAlignment(Qt::AlignCenter);

    root->addWidget(title);
    root->addWidget(sub);
    root->addSpacing(16);

    // Tabs: Login | Register Customer | Register Seller
    QTabWidget* tabs = new QTabWidget;
    tabs->setFixedWidth(400);
    root->addWidget(tabs, 0, Qt::AlignCenter);

    // ─── LOGIN TAB ───────────────────────────────────────
    QWidget* loginTab = new QWidget;
    QVBoxLayout* loginLay = new QVBoxLayout(loginTab);
    loginLay->setSpacing(12);
    loginLay->setContentsMargins(20, 20, 20, 20);

    m_emailEdit    = new QLineEdit; m_emailEdit->setPlaceholderText("Email address");
    m_passwordEdit = new QLineEdit; m_passwordEdit->setPlaceholderText("Password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    QPushButton* loginBtn = new QPushButton("Login →");
    loginBtn->setObjectName("primaryBtn");
    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::onLoginClicked);

    QLabel* hint = new QLabel("Demo: admin@shopnova.com / admin123\n"
                               "Customer: arjun@gmail.com / pass123\n"
                               "Seller: ravi@seller.com / pass123");
    hint->setObjectName("hintLabel");
    hint->setAlignment(Qt::AlignCenter);

    loginLay->addWidget(new QLabel("Email:"));
    loginLay->addWidget(m_emailEdit);
    loginLay->addWidget(new QLabel("Password:"));
    loginLay->addWidget(m_passwordEdit);
    loginLay->addSpacing(8);
    loginLay->addWidget(loginBtn);
    loginLay->addSpacing(8);
    loginLay->addWidget(hint);
    loginLay->addStretch();

    tabs->addTab(loginTab, "🔑  Login");

    // ─── REGISTER CUSTOMER TAB ───────────────────────────
    QWidget* regTab = new QWidget;
    QFormLayout* regForm = new QFormLayout(regTab);
    regForm->setSpacing(10);
    regForm->setContentsMargins(20, 20, 20, 20);

    m_regName     = new QLineEdit; m_regName->setPlaceholderText("Full name");
    m_regEmail    = new QLineEdit; m_regEmail->setPlaceholderText("Email");
    m_regPassword = new QLineEdit; m_regPassword->setPlaceholderText("Password");
    m_regPassword->setEchoMode(QLineEdit::Password);
    m_regPhone    = new QLineEdit; m_regPhone->setPlaceholderText("Phone number");

    QPushButton* regBtn = new QPushButton("Create Customer Account");
    regBtn->setObjectName("successBtn");
    connect(regBtn, &QPushButton::clicked, this, &LoginPage::onRegisterCustomerClicked);

    regForm->addRow("Full Name:", m_regName);
    regForm->addRow("Email:",     m_regEmail);
    regForm->addRow("Password:",  m_regPassword);
    regForm->addRow("Phone:",     m_regPhone);
    regForm->addRow(regBtn);

    tabs->addTab(regTab, "👤  Customer Signup");

    // ─── REGISTER SELLER TAB ─────────────────────────────
    QWidget* selTab = new QWidget;
    QFormLayout* selForm = new QFormLayout(selTab);
    selForm->setSpacing(10);
    selForm->setContentsMargins(20, 20, 20, 20);

    m_selName      = new QLineEdit; m_selName->setPlaceholderText("Your name");
    m_selEmail     = new QLineEdit; m_selEmail->setPlaceholderText("Email");
    m_selPassword  = new QLineEdit; m_selPassword->setPlaceholderText("Password");
    m_selPassword->setEchoMode(QLineEdit::Password);
    m_selPhone     = new QLineEdit; m_selPhone->setPlaceholderText("Phone");
    m_selStoreName = new QLineEdit; m_selStoreName->setPlaceholderText("Store / Shop name");

    QPushButton* selBtn = new QPushButton("Open My Shop");
    selBtn->setObjectName("warnBtn");
    connect(selBtn, &QPushButton::clicked, this, &LoginPage::onRegisterSellerClicked);

    selForm->addRow("Full Name:",  m_selName);
    selForm->addRow("Email:",      m_selEmail);
    selForm->addRow("Password:",   m_selPassword);
    selForm->addRow("Phone:",      m_selPhone);
    selForm->addRow("Store Name:", m_selStoreName);
    selForm->addRow(selBtn);

    tabs->addTab(selTab, "🏪  Seller Signup");
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
