#include "shellwidget.h"
#include <QScrollArea>
#include <QPainter>
#include <QPainterPath>

// ── Custom logout button that draws a door+arrow icon via QPainter ──
class LogoutIconButton : public QPushButton {
public:
    explicit LogoutIconButton(QWidget* parent = nullptr) : QPushButton(parent) {
        setFixedSize(34, 34);
        setToolTip("Logout");
        setCursor(Qt::PointingHandCursor);
        setStyleSheet(
            "QPushButton { background: rgba(231,76,60,0.10); border-radius: 10px;"
            "border: 1px solid rgba(231,76,60,0.15); }"
            "QPushButton:hover { background: rgba(231,76,60,0.22);"
            "border-color: rgba(231,76,60,0.35); }");
    }

protected:
    void paintEvent(QPaintEvent* e) override {
        QPushButton::paintEvent(e);
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        QPen pen(QColor("#e74c3c"), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);

        // Center offset
        qreal cx = width() / 2.0;
        qreal cy = height() / 2.0;

        // Door frame (open rectangle on the right)
        QPainterPath door;
        door.moveTo(cx + 2, cy - 7);
        door.lineTo(cx - 5, cy - 7);
        door.lineTo(cx - 5, cy + 7);
        door.lineTo(cx + 2, cy + 7);
        p.drawPath(door);

        // Arrow shaft (pointing right, exiting the door)
        p.drawLine(QPointF(cx - 2, cy), QPointF(cx + 7, cy));

        // Arrow head
        p.drawLine(QPointF(cx + 4, cy - 3.5), QPointF(cx + 7, cy));
        p.drawLine(QPointF(cx + 4, cy + 3.5), QPointF(cx + 7, cy));
    }
};

ShellWidget::ShellWidget(QWidget *parent) : QWidget(parent) {
    buildSkeleton();
}

void ShellWidget::buildSkeleton() {
    // Root: horizontal — sidebar | content column
    QHBoxLayout *root = new QHBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // ── Sidebar ───────────────────────────────────────
    m_sidebar = new QFrame;
    m_sidebar->setFixedWidth(220);
    m_sidebar->setStyleSheet(
        "QFrame { background: #0a0a12; border-right: 1px solid #1a1a28; }");

    QVBoxLayout *sideL = new QVBoxLayout(m_sidebar);
    sideL->setContentsMargins(0, 0, 0, 0);
    sideL->setSpacing(0);

    // Logo area
    QWidget *logoArea = new QWidget;
    logoArea->setFixedHeight(64);
    logoArea->setStyleSheet("background: #0a0a12;");
    QHBoxLayout *logoL = new QHBoxLayout(logoArea);
    logoL->setContentsMargins(20, 0, 16, 0);
    QLabel *logo = new QLabel(QString::fromUtf8("⚡ ShopNova"));
    logo->setStyleSheet(
        "font-size: 18px; font-weight: 800; letter-spacing: -0.5px;"
        "color: #ff6b2b;");
    logoL->addWidget(logo);
    logoL->addStretch();
    sideL->addWidget(logoArea);

    // Divider
    QFrame *div1 = new QFrame;
    div1->setFrameShape(QFrame::HLine);
    div1->setStyleSheet("color: #1a1a28;");
    sideL->addWidget(div1);

    // Nav items container
    QWidget *navContainer = new QWidget;
    navContainer->setStyleSheet("background: transparent;");
    m_navLayout = new QVBoxLayout(navContainer);
    m_navLayout->setContentsMargins(12, 16, 12, 16);
    m_navLayout->setSpacing(4);

    QScrollArea *navScroll = new QScrollArea;
    navScroll->setWidget(navContainer);
    navScroll->setWidgetResizable(true);
    navScroll->setFrameShape(QFrame::NoFrame);
    navScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    navScroll->setStyleSheet("background: transparent;");
    sideL->addWidget(navScroll, 1);

    // Bottom user card
    QFrame *div2 = new QFrame;
    div2->setFrameShape(QFrame::HLine);
    div2->setStyleSheet("color: #1a1a28;");
    sideL->addWidget(div2);

    QWidget *userCard = new QWidget;
    userCard->setFixedHeight(80);
    userCard->setStyleSheet("background: #0a0a12;");
    QHBoxLayout *userL = new QHBoxLayout(userCard);
    userL->setContentsMargins(16, 12, 16, 12);
    userL->setSpacing(12);

    QLabel *avatar = new QLabel(QString::fromUtf8("👤"));
    avatar->setFixedSize(38, 38);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet(
        "background: rgba(255,107,43,0.15); border-radius: 19px;"
        "font-size: 18px;");

    QVBoxLayout *nameStack = new QVBoxLayout;
    nameStack->setSpacing(2);
    m_userNameLbl = new QLabel("User");
    m_userNameLbl->setStyleSheet("font-size: 13px; font-weight: 700; color: #e0e0f0;");
    m_roleLbl = new QLabel("role");
    m_roleLbl->setStyleSheet(
        "font-size: 11px; font-weight: 600; color: #ff6b2b;"
        "background: rgba(255,107,43,0.12); border-radius: 6px; padding: 1px 7px;");
    nameStack->addWidget(m_userNameLbl);
    nameStack->addWidget(m_roleLbl);

    LogoutIconButton *logoutBtn = new LogoutIconButton;
    connect(logoutBtn, &QPushButton::clicked, this, &ShellWidget::logoutRequested);

    userL->addWidget(avatar);
    userL->addLayout(nameStack, 1);
    userL->addWidget(logoutBtn);
    sideL->addWidget(userCard);

    root->addWidget(m_sidebar);

    // ── Right column: header + content ───────────────
    QVBoxLayout *rightCol = new QVBoxLayout;
    rightCol->setContentsMargins(0,0,0,0);
    rightCol->setSpacing(0);

    // Header
    m_header = new QWidget;
    m_header->setFixedHeight(64);
    m_header->setStyleSheet(
        "background: #0d0d14; border-bottom: 1px solid #1a1a28;");
    QHBoxLayout *headerL = new QHBoxLayout(m_header);
    headerL->setContentsMargins(28, 0, 24, 0);
    headerL->addStretch();   // right content injected via setHeaderRight()

    rightCol->addWidget(m_header);

    // Content stack
    m_contentStack = new QStackedWidget;
    m_contentStack->setStyleSheet("background: #0d0d14;");
    rightCol->addWidget(m_contentStack, 1);

    root->addLayout(rightCol, 1);
}

void ShellWidget::configure(const QString &/*appTitle*/,
                             const QString &userLabel,
                             const QString &roleTag,
                             const QVector<NavItem> &navItems) {
    m_userNameLbl->setText(userLabel);
    m_roleLbl->setText(roleTag);

    // Clear old nav buttons
    for (auto *b : m_navBtns) { b->deleteLater(); }
    m_navBtns.clear();

    // Remove old pages
    while (m_contentStack->count() > 0)
        m_contentStack->removeWidget(m_contentStack->widget(0));

    // Section label helper
    auto addSection = [&](const QString &text) {
        QLabel *sec = new QLabel(text.toUpper());
        sec->setStyleSheet(
            "font-size: 10px; font-weight: 700; color: #3a3a58;"
            "letter-spacing: 1.2px; padding: 12px 8px 4px 8px;");
        m_navLayout->addWidget(sec);
    };

    bool firstSection = true;
    for (int i = 0; i < navItems.size(); ++i) {
        const NavItem &item = navItems[i];

        // If icon is empty string treat as section header
        if (item.icon.isEmpty()) {
            if (!firstSection) m_navLayout->addSpacing(8);
            addSection(item.label);
            firstSection = false;
            continue;
        }
        firstSection = false;

        QPushButton *btn = new QPushButton(item.icon + "  " + item.label);
        btn->setObjectName("navBtn");
        btn->setCheckable(true);
        btn->setFixedHeight(44);
        btn->setAutoExclusive(false);  // we handle manually
        m_navLayout->addWidget(btn);
        m_navBtns.append(btn);
        m_contentStack->addWidget(item.page);

        int idx = m_navBtns.size() - 1;
        connect(btn, &QPushButton::clicked, this, [this, idx]() {
            selectNav(idx);
        });
    }

    m_navLayout->addStretch();
    if (!m_navBtns.isEmpty()) selectNav(0);
}

void ShellWidget::selectNav(int index) {
    for (int i = 0; i < m_navBtns.size(); ++i)
        m_navBtns[i]->setChecked(i == index);
    m_contentStack->setCurrentIndex(index);
}

void ShellWidget::setHeaderRight(QWidget *w) {
    QHBoxLayout *headerL = qobject_cast<QHBoxLayout*>(m_header->layout());
    if (headerL) headerL->addWidget(w);
}
