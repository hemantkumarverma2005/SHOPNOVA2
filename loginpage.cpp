#include "loginpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QTabWidget>
#include <QFrame>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QTimer>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>

LoginPage::LoginPage(QWidget* parent) : QWidget(parent) {
    buildUI();
}

void LoginPage::buildUI() {
    // ── Root: full-screen horizontal split ────────────────────────
    QHBoxLayout* rootH = new QHBoxLayout(this);
    rootH->setContentsMargins(0, 0, 0, 0);
    rootH->setSpacing(0);

    // ═══════════════════════════════════════════════════════════════
    //  LEFT PANEL — Hero / Brand panel
    // ═══════════════════════════════════════════════════════════════
    QFrame* leftPanel = new QFrame;
    leftPanel->setMinimumWidth(480);
    leftPanel->setStyleSheet(
        "QFrame {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
        "    stop:0 #1a0f08, stop:0.3 #0f0a14, stop:0.7 #0a0f1a, stop:1 #0a0a0f);"
        "  border: none;"
        "}");

    QVBoxLayout* leftLay = new QVBoxLayout(leftPanel);
    leftLay->setContentsMargins(48, 48, 48, 48);
    leftLay->setSpacing(0);

    // Logo image
    QLabel* logoImg = new QLabel;
    QPixmap pix(":/shopnova_logo.png");
    if (!pix.isNull()) {
        logoImg->setPixmap(pix.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        logoImg->setText(QString::fromUtf8("⚡"));
        logoImg->setStyleSheet("font-size: 72px;");
    }
    logoImg->setAlignment(Qt::AlignLeft);
    logoImg->setStyleSheet(logoImg->styleSheet() + "background: transparent; border: none;");
    leftLay->addWidget(logoImg);
    leftLay->addSpacing(24);

    // Brand name
    QLabel* brandName = new QLabel("ShopNova");
    brandName->setStyleSheet(
        "font-size: 48px; font-weight: 900; letter-spacing: -2px;"
        "color: #ff6b2b; background: transparent; border: none;");
    leftLay->addWidget(brandName);

    leftLay->addSpacing(8);

    // Tagline
    QLabel* tagline = new QLabel("Your Multi-Shop\nE-Commerce Platform");
    tagline->setStyleSheet(
        "font-size: 22px; font-weight: 600; color: #f0f0f5;"
        "line-height: 1.4; letter-spacing: -0.5px;"
        "background: transparent; border: none;");
    tagline->setWordWrap(true);
    leftLay->addWidget(tagline);

    leftLay->addSpacing(16);

    QLabel* desc = new QLabel(
        "Manage shops, browse products, track orders —\n"
        "all in one unified platform built with Qt.");
    desc->setStyleSheet(
        "font-size: 14px; color: #7878a0; line-height: 1.6;"
        "background: transparent; border: none;");
    desc->setWordWrap(true);
    leftLay->addWidget(desc);

    leftLay->addSpacing(32);

    // Feature pills
    auto makePill = [](const QString& icon, const QString& text) -> QWidget* {
        QWidget* pill = new QWidget;
        pill->setStyleSheet("background: transparent; border: none;");
        QHBoxLayout* pl = new QHBoxLayout(pill);
        pl->setContentsMargins(0, 6, 0, 6);
        pl->setSpacing(12);

        QLabel* icLbl = new QLabel(icon);
        icLbl->setFixedSize(36, 36);
        icLbl->setAlignment(Qt::AlignCenter);
        icLbl->setStyleSheet(
            "background: rgba(255,107,43,0.12); border-radius: 10px;"
            "font-size: 16px; border: none;");

        QLabel* txtLbl = new QLabel(text);
        txtLbl->setStyleSheet(
            "font-size: 13px; font-weight: 500; color: #b0b0c8;"
            "background: transparent; border: none;");

        pl->addWidget(icLbl);
        pl->addWidget(txtLbl);
        pl->addStretch();
        return pill;
    };

    leftLay->addWidget(makePill(QString::fromUtf8("🛒"), "Multi-vendor marketplace"));
    leftLay->addWidget(makePill(QString::fromUtf8("📦"), "Real-time order tracking"));
    leftLay->addWidget(makePill(QString::fromUtf8("🔒"), "Secure authentication"));
    leftLay->addWidget(makePill(QString::fromUtf8("⚡"), "Lightning-fast performance"));

    leftLay->addStretch();

    // Version badge at bottom
    QLabel* version = new QLabel(QString::fromUtf8("✦  v2.0 — Qt Edition  ✦"));
    version->setObjectName("versionBadge");
    version->setAlignment(Qt::AlignLeft);
    leftLay->addWidget(version);

    rootH->addWidget(leftPanel, 5);

    // ═══════════════════════════════════════════════════════════════
    //  RIGHT PANEL — Auth forms
    // ═══════════════════════════════════════════════════════════════
    QFrame* rightPanel = new QFrame;
    rightPanel->setStyleSheet(
        "QFrame { background: #0c0c14; border: none;"
        "border-left: 1px solid #1e1e30; }");

    QVBoxLayout* rightLay = new QVBoxLayout(rightPanel);
    rightLay->setContentsMargins(0, 0, 0, 0);
    rightLay->setSpacing(0);
    rightLay->setAlignment(Qt::AlignCenter);

    // Auth card
    QFrame* card = new QFrame;
    card->setFixedWidth(420);
    card->setStyleSheet(
        "QFrame { background: #141420;"
        "border: 1px solid #1e1e30;"
        "border-radius: 20px; }");

    // Glow shadow effect
    QGraphicsDropShadowEffect* glow = new QGraphicsDropShadowEffect;
    glow->setBlurRadius(60);
    glow->setColor(QColor(255, 107, 43, 30));
    glow->setOffset(0, 8);
    card->setGraphicsEffect(glow);

    QVBoxLayout* cardLay = new QVBoxLayout(card);
    cardLay->setContentsMargins(0, 0, 0, 0);
    cardLay->setSpacing(0);

    // Tabs inside card
    QTabWidget* tabs = new QTabWidget;
    tabs->setStyleSheet(tabs->styleSheet() +
        "QTabWidget::pane { border: none; background: transparent; }"
        "QTabBar { background: transparent; }"
        "QTabBar::tab { padding: 14px 20px; }");
    cardLay->addWidget(tabs);

    // ─── LOGIN TAB ───────────────────────────────────────
    QWidget* loginTab = new QWidget;
    QVBoxLayout* loginLay = new QVBoxLayout(loginTab);
    loginLay->setSpacing(14);
    loginLay->setContentsMargins(32, 32, 32, 32);

    // Lock icon
    QLabel* lockIcon = new QLabel(QString::fromUtf8("🔐"));
    lockIcon->setAlignment(Qt::AlignCenter);
    lockIcon->setStyleSheet(
        "font-size: 36px; background: transparent; border: none;");
    loginLay->addWidget(lockIcon);
    loginLay->addSpacing(4);

    QLabel* loginTitle = new QLabel("Welcome back");
    loginTitle->setAlignment(Qt::AlignCenter);
    loginTitle->setStyleSheet(
        "font-size: 22px; font-weight: 800; color: #f0f0f5;"
        "letter-spacing: -0.5px; background: transparent; border: none;");

    QLabel* loginSub = new QLabel("Sign in to access your dashboard");
    loginSub->setAlignment(Qt::AlignCenter);
    loginSub->setStyleSheet(
        "font-size: 13px; color: #7878a0; margin-bottom: 8px;"
        "background: transparent; border: none;");

    QLabel* emailLabel = new QLabel("Email");
    emailLabel->setStyleSheet(
        "font-size: 13px; font-weight: 500; color: #9494a8;"
        "background: transparent; border: none;");
    m_emailEdit = new QLineEdit;
    m_emailEdit->setPlaceholderText("Enter your email address");
    m_emailEdit->setMinimumHeight(44);

    QLabel* pwdLabel = new QLabel("Password");
    pwdLabel->setStyleSheet(
        "font-size: 13px; font-weight: 500; color: #9494a8;"
        "background: transparent; border: none;");
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setPlaceholderText("Enter your password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMinimumHeight(44);

    QPushButton* loginBtn = new QPushButton(QString::fromUtf8("Sign In  →"));
    loginBtn->setObjectName("primaryBtn");
    loginBtn->setMinimumHeight(48);
    loginBtn->setCursor(Qt::PointingHandCursor);
    loginBtn->setStyleSheet(loginBtn->styleSheet() +
        "QPushButton { font-size: 15px; font-weight: 700; letter-spacing: -0.3px; }");
    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginPage::onLoginClicked);

    // Demo accounts hint — styled card
    QFrame* hintCard = new QFrame;
    hintCard->setStyleSheet(
        "QFrame { background: rgba(255,107,43,0.05);"
        "border: 1px solid rgba(255,107,43,0.12);"
        "border-radius: 12px; }");
    QVBoxLayout* hintLay = new QVBoxLayout(hintCard);
    hintLay->setContentsMargins(14, 12, 14, 12);
    hintLay->setSpacing(6);

    QLabel* hintTitle = new QLabel(QString::fromUtf8("🔑  Demo Accounts"));
    hintTitle->setStyleSheet(
        "font-size: 11px; font-weight: 700; color: #ff9a5c;"
        "letter-spacing: 0.5px; background: transparent; border: none;");

    auto makeHintRow = [](const QString& role, const QString& email, const QString& pwd) -> QLabel* {
        QLabel* lbl = new QLabel(
            QString("<span style='color:#7878a0;'>%1:</span> "
                    "<span style='color:#a0a0b8;'>%2</span> / "
                    "<span style='color:#a0a0b8;'>%3</span>")
                .arg(role, email, pwd));
        lbl->setStyleSheet("font-size: 11px; background: transparent; border: none;");
        return lbl;
    };

    hintLay->addWidget(hintTitle);
    hintLay->addWidget(makeHintRow("Admin",    "admin@shopnova.com", "admin123"));
    hintLay->addWidget(makeHintRow("Customer", "arjun@gmail.com",    "pass123"));
    hintLay->addWidget(makeHintRow("Seller",   "ravi@seller.com",    "pass123"));

    loginLay->addWidget(loginTitle);
    loginLay->addWidget(loginSub);
    loginLay->addSpacing(6);
    loginLay->addWidget(emailLabel);
    loginLay->addWidget(m_emailEdit);
    loginLay->addWidget(pwdLabel);
    loginLay->addWidget(m_passwordEdit);
    loginLay->addSpacing(8);
    loginLay->addWidget(loginBtn);
    loginLay->addSpacing(12);
    loginLay->addWidget(hintCard);
    loginLay->addStretch();

    tabs->addTab(loginTab, "  Sign In  ");

    // ─── REGISTER CUSTOMER TAB ───────────────────────────
    QWidget* regTab = new QWidget;
    QVBoxLayout* regLay = new QVBoxLayout(regTab);
    regLay->setSpacing(12);
    regLay->setContentsMargins(32, 28, 32, 28);

    QLabel* regIcon = new QLabel(QString::fromUtf8("👤"));
    regIcon->setAlignment(Qt::AlignCenter);
    regIcon->setStyleSheet("font-size: 32px; background: transparent; border: none;");

    QLabel* regTitle = new QLabel("Create Account");
    regTitle->setAlignment(Qt::AlignCenter);
    regTitle->setStyleSheet(
        "font-size: 20px; font-weight: 800; color: #f0f0f5;"
        "background: transparent; border: none;");
    QLabel* regSub = new QLabel("Join ShopNova and start shopping");
    regSub->setAlignment(Qt::AlignCenter);
    regSub->setStyleSheet(
        "font-size: 13px; color: #7878a0; margin-bottom: 4px;"
        "background: transparent; border: none;");

    m_regName     = new QLineEdit; m_regName->setPlaceholderText("Full name");     m_regName->setMinimumHeight(44);
    m_regEmail    = new QLineEdit; m_regEmail->setPlaceholderText("Email address"); m_regEmail->setMinimumHeight(44);
    m_regPassword = new QLineEdit; m_regPassword->setPlaceholderText("Create password"); m_regPassword->setMinimumHeight(44);
    m_regPassword->setEchoMode(QLineEdit::Password);
    m_regPhone    = new QLineEdit; m_regPhone->setPlaceholderText("Phone number"); m_regPhone->setMinimumHeight(44);

    QPushButton* regBtn = new QPushButton(QString::fromUtf8("Create Customer Account  →"));
    regBtn->setObjectName("successBtn");
    regBtn->setMinimumHeight(48);
    regBtn->setCursor(Qt::PointingHandCursor);
    connect(regBtn, &QPushButton::clicked, this, &LoginPage::onRegisterCustomerClicked);

    auto addField = [](QVBoxLayout* lay, const QString& label, QLineEdit* edit) {
        QLabel* lbl = new QLabel(label);
        lbl->setStyleSheet(
            "font-size: 13px; font-weight: 500; color: #9494a8;"
            "background: transparent; border: none;");
        lay->addWidget(lbl);
        lay->addWidget(edit);
    };

    regLay->addWidget(regIcon);
    regLay->addWidget(regTitle);
    regLay->addWidget(regSub);
    regLay->addSpacing(4);
    addField(regLay, "Full Name", m_regName);
    addField(regLay, "Email", m_regEmail);
    addField(regLay, "Password", m_regPassword);
    addField(regLay, "Phone", m_regPhone);
    regLay->addSpacing(8);
    regLay->addWidget(regBtn);
    regLay->addStretch();

    tabs->addTab(regTab, "  Customer Signup  ");

    // ─── REGISTER SELLER TAB ─────────────────────────────
    QWidget* selTab = new QWidget;
    QVBoxLayout* selLay = new QVBoxLayout(selTab);
    selLay->setSpacing(12);
    selLay->setContentsMargins(32, 28, 32, 28);

    QLabel* selIcon = new QLabel(QString::fromUtf8("🏪"));
    selIcon->setAlignment(Qt::AlignCenter);
    selIcon->setStyleSheet("font-size: 32px; background: transparent; border: none;");

    QLabel* selTitle = new QLabel("Open Your Shop");
    selTitle->setAlignment(Qt::AlignCenter);
    selTitle->setStyleSheet(
        "font-size: 20px; font-weight: 800; color: #f0f0f5;"
        "background: transparent; border: none;");
    QLabel* selSub = new QLabel("Start selling on ShopNova today");
    selSub->setAlignment(Qt::AlignCenter);
    selSub->setStyleSheet(
        "font-size: 13px; color: #7878a0; margin-bottom: 4px;"
        "background: transparent; border: none;");

    m_selName      = new QLineEdit; m_selName->setPlaceholderText("Your full name"); m_selName->setMinimumHeight(44);
    m_selEmail     = new QLineEdit; m_selEmail->setPlaceholderText("Email address"); m_selEmail->setMinimumHeight(44);
    m_selPassword  = new QLineEdit; m_selPassword->setPlaceholderText("Create password"); m_selPassword->setMinimumHeight(44);
    m_selPassword->setEchoMode(QLineEdit::Password);
    m_selPhone     = new QLineEdit; m_selPhone->setPlaceholderText("Phone number"); m_selPhone->setMinimumHeight(44);
    m_selStoreName = new QLineEdit; m_selStoreName->setPlaceholderText("Your store / shop name"); m_selStoreName->setMinimumHeight(44);

    // QR Upload row
    QHBoxLayout* qrLayout = new QHBoxLayout;
    QPushButton* qrBtn    = new QPushButton(QString::fromUtf8("📷  Browse QR Image"));
    qrBtn->setMinimumHeight(40);
    qrBtn->setObjectName("ghostBtn");
    m_selQrLabel = new QLabel("No QR selected  (optional)");
    m_selQrLabel->setStyleSheet("font-size: 11px; color: #7878a0; background: transparent; border: none;");
    qrLayout->addWidget(qrBtn);
    qrLayout->addWidget(m_selQrLabel, 1);

    connect(qrBtn, &QPushButton::clicked, [this]() {
        QString path = QFileDialog::getOpenFileName(
            this, "Select UPI QR Image", "", "Images (*.png *.jpg *.jpeg *.webp)");
        if (!path.isEmpty()) {
            m_selQrPath = path;
            m_selQrLabel->setText(QFileInfo(path).fileName());
            m_selQrLabel->setStyleSheet("font-size: 11px; color: #06d6a0; background: transparent; border: none;");
        }
    });

    // Info note
    QLabel* qrNote = new QLabel(
        QString::fromUtf8("💡  Without a QR, customers can only pay Cash on Delivery for orders from your shop."));
    qrNote->setWordWrap(true);
    qrNote->setStyleSheet(
        "font-size: 11px; color: #7878a0; background: rgba(255,107,43,0.06);"
        "border: 1px solid rgba(255,107,43,0.15); border-radius: 8px; padding: 8px 12px;");

    QPushButton* selBtn = new QPushButton(QString::fromUtf8("Open My Shop  →"));
    selBtn->setObjectName("warnBtn");
    selBtn->setMinimumHeight(48);
    selBtn->setCursor(Qt::PointingHandCursor);
    connect(selBtn, &QPushButton::clicked, this, &LoginPage::onRegisterSellerClicked);

    selLay->addWidget(selIcon);
    selLay->addWidget(selTitle);
    selLay->addWidget(selSub);
    selLay->addSpacing(4);
    addField(selLay, "Full Name", m_selName);
    addField(selLay, "Email", m_selEmail);
    addField(selLay, "Password", m_selPassword);
    addField(selLay, "Phone", m_selPhone);
    addField(selLay, "Store Name", m_selStoreName);
    selLay->addSpacing(4);
    // QR row label
    {
        QLabel* qrFieldLabel = new QLabel("UPI QR Code");
        qrFieldLabel->setStyleSheet(
            "font-size: 13px; font-weight: 500; color: #9494a8;"
            "background: transparent; border: none;");
        selLay->addWidget(qrFieldLabel);
    }
    selLay->addLayout(qrLayout);
    selLay->addWidget(qrNote);
    selLay->addSpacing(8);
    selLay->addWidget(selBtn);
    selLay->addStretch();

    tabs->addTab(selTab, "  Seller Signup  ");

    rightLay->addWidget(card);

    rootH->addWidget(rightPanel, 4);
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
        m_selStoreName->text().trimmed(),
        m_selQrPath);
    m_selName->clear(); m_selEmail->clear();
    m_selPassword->clear(); m_selPhone->clear(); m_selStoreName->clear();
    m_selQrPath.clear();
    m_selQrLabel->setText("No QR selected  (optional)");
    m_selQrLabel->setStyleSheet("font-size: 11px; color: #7878a0; background: transparent; border: none;");
}
