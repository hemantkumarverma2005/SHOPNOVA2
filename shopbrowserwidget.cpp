#include "shopbrowserwidget.h"

#include <QApplication>
#include <QScrollArea>
#include <QPixmap>
#include <QPainter>
#include <QLinearGradient>
#include <QTimer>
#include <QGraphicsDropShadowEffect>

// ============================================================
// Colour palette (matches screenshots)
// ============================================================
namespace Pal {
    static const QString BG_DARK   = "#1a1535";
    static const QString BG_MID    = "#221d45";
    static const QString BG_HEADER = "#1e1840";
    static const QString ACCENT    = "#7c5cfc";
    static const QString ACCENT2   = "#a855f7";
    static const QString TEXT_HI   = "#ffffff";
    static const QString TEXT_LO   = "#9590b8";
    static const QString CARD_BG   = "#252048";
    static const QString BADGE     = "#ef4444";
    static const QString STAR      = "#7c5cfc";
}

// ============================================================
// ShopCard
// ============================================================
ShopCard::ShopCard(Seller* seller, QWidget* parent)
    : QFrame(parent), m_seller(seller)
{
    buildUi();
}

void ShopCard::buildUi()
{
    setFixedSize(340, 320);
    setCursor(Qt::PointingHandCursor);
    setStyleSheet(QString(R"(
        ShopCard {
            background: %1;
            border-radius: 16px;
            border: 1px solid rgba(124,92,252,0.15);
        }
    )").arg(Pal::CARD_BG));

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(24);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0, 0, 0, 80));
    setGraphicsEffect(shadow);

    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(0);

    // ── banner image area ───────────────────────────────────
    QLabel* imgLbl = new QLabel(this);
    imgLbl->setFixedHeight(200);
    imgLbl->setAlignment(Qt::AlignCenter);

    // Generate a nice gradient placeholder
    QPixmap banner(340, 200);
    QPainter p(&banner);
    p.setRenderHint(QPainter::Antialiasing);
    QLinearGradient grad(0, 0, 340, 200);
    QString cat = m_seller->getStoreName();
    if (cat.contains("Tech", Qt::CaseInsensitive) || cat.contains("Ravi", Qt::CaseInsensitive)) {
        grad.setColorAt(0, QColor("#1a1a2e"));
        grad.setColorAt(1, QColor("#16213e"));
    } else if (cat.contains("Fashion", Qt::CaseInsensitive) || cat.contains("Hub", Qt::CaseInsensitive)) {
        grad.setColorAt(0, QColor("#2d1b69"));
        grad.setColorAt(1, QColor("#11998e"));
    } else {
        grad.setColorAt(0, QColor("#0f3460"));
        grad.setColorAt(1, QColor("#16213e"));
    }
    p.fillRect(banner.rect(), grad);
    // Draw store initials
    p.setPen(Qt::white);
    QFont f; f.setPixelSize(56); f.setBold(true); p.setFont(f);
    p.drawText(banner.rect(), Qt::AlignCenter,
               m_seller->getStoreName().left(2).toUpper());

    imgLbl->setPixmap(banner.scaled(340, 200, Qt::KeepAspectRatioByExpanding,
                                    Qt::SmoothTransformation));
    imgLbl->setScaledContents(true);
    imgLbl->setStyleSheet("border-radius: 16px 16px 0 0; background: transparent;");

    // Overlay: category + shop name on the image
    QWidget* overlay = new QWidget(imgLbl);
    overlay->setGeometry(0, 0, 340, 200);
    overlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    overlay->setStyleSheet(
        "background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "stop:0 rgba(0,0,0,0), stop:0.5 rgba(0,0,0,0.1), stop:1 rgba(20,15,50,0.85));"
        "border-radius: 16px 16px 0 0;");

    QVBoxLayout* ol = new QVBoxLayout(overlay);
    ol->setContentsMargins(16, 0, 16, 14);
    ol->addStretch();

    // Payment badge (🔵 UPI | 🟡 COD only)
    QHBoxLayout* badgeRow = new QHBoxLayout;
    if (m_seller->hasQr()) {
        QLabel* badge = new QLabel("🔵 UPI Available", overlay);
        badge->setStyleSheet(
            "color: #a5f3fc; font-size: 10px; font-weight: 700;"
            "background: rgba(6,182,212,0.25); border-radius: 8px;"
            "padding: 2px 8px; border: 1px solid rgba(6,182,212,0.4);");
        badgeRow->addWidget(badge);
    } else {
        QLabel* badge = new QLabel("🟡 COD Only", overlay);
        badge->setStyleSheet(
            "color: #fde68a; font-size: 10px; font-weight: 700;"
            "background: rgba(251,191,36,0.2); border-radius: 8px;"
            "padding: 2px 8px; border: 1px solid rgba(251,191,36,0.35);");
        badgeRow->addWidget(badge);
    }
    badgeRow->addStretch();
    ol->addLayout(badgeRow);

    // Store name
    QLabel* nameLbl = new QLabel(m_seller->getStoreName(), overlay);
    nameLbl->setStyleSheet(
        "color: white; font-size: 22px; font-weight: 700;"
        "background: transparent; letter-spacing: -0.3px;");
    ol->addWidget(nameLbl);

    vl->addWidget(imgLbl);

    // ── bottom info bar ─────────────────────────────────────
    QWidget* bar = new QWidget(this);
    bar->setFixedHeight(56);
    bar->setStyleSheet(QString("background: %1; border-radius: 0 0 16px 16px;")
                           .arg(Pal::CARD_BG));

    QHBoxLayout* hl = new QHBoxLayout(bar);
    hl->setContentsMargins(16, 0, 16, 0);

    // Star + rating
    double rating = m_seller->getRating();
    if (rating < 3.0) rating = 4.0 + (rand() % 10) * 0.1; // fallback demo

    QLabel* starLbl = new QLabel(
        QString("★  %1").arg(rating, 0, 'f', 1), bar);
    starLbl->setStyleSheet(QString(
        "color: white; font-size: 13px; font-weight: 600;"
        "background: %1; border-radius: 10px; padding: 3px 10px;")
        .arg(Pal::ACCENT));

    // Shop Now CTA
    QLabel* ctaLbl = new QLabel("Shop Now →", bar);
    ctaLbl->setStyleSheet(QString(
        "color: %1; font-size: 12px; font-weight: 700;"
        "background: transparent;").arg(Pal::ACCENT));

    hl->addWidget(starLbl);
    hl->addStretch();
    hl->addWidget(ctaLbl);

    vl->addWidget(bar);
    setLayout(vl);
}

void ShopCard::mousePressEvent(QMouseEvent* e)
{
    QFrame::mousePressEvent(e);
    emit clicked(m_seller);
}

void ShopCard::enterEvent(QEnterEvent* e)
{
    QFrame::enterEvent(e);
    setStyleSheet(QString(R"(
        ShopCard {
            background: %1;
            border-radius: 16px;
            border: 1px solid rgba(124,92,252,0.55);
        }
    )").arg(Pal::CARD_BG));
    auto* s = qobject_cast<QGraphicsDropShadowEffect*>(graphicsEffect());
    if (s) { s->setBlurRadius(36); s->setOffset(0, 10); }
}

void ShopCard::leaveEvent(QEvent* e)
{
    QFrame::leaveEvent(e);
    setStyleSheet(QString(R"(
        ShopCard {
            background: %1;
            border-radius: 16px;
            border: 1px solid rgba(124,92,252,0.15);
        }
    )").arg(Pal::CARD_BG));
    auto* s = qobject_cast<QGraphicsDropShadowEffect*>(graphicsEffect());
    if (s) { s->setBlurRadius(24); s->setOffset(0, 6); }
}

// ============================================================
// ShopBrowserWidget
// ============================================================
ShopBrowserWidget::ShopBrowserWidget(Platform* platform,
                                     const QString& customerName,
                                     QWidget* parent)
    : QWidget(parent), m_platform(platform), m_customerName(customerName)
{
    setStyleSheet(globalStyle());
    setObjectName("ShopBrowserWidget");

    // Collect all sellers
    for (auto& s : m_platform->getSellers())
        m_allSellers.append(s.get());

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    buildHeader();

    // ── welcome + search row ─────────────────────────────────
    QWidget* heroArea = new QWidget(this);
    heroArea->setStyleSheet(QString("background: %1;").arg(Pal::BG_DARK));
    QHBoxLayout* heroHL = new QHBoxLayout(heroArea);
    heroHL->setContentsMargins(48, 32, 48, 16);

    QVBoxLayout* titleCol = new QVBoxLayout();
    QLabel* welcomeLbl = new QLabel("WELCOME BACK", heroArea);
    welcomeLbl->setStyleSheet(
        "color: #9590b8; font-size: 11px; font-weight: 700; letter-spacing: 2px;"
        "background: transparent;");
    QLabel* headLbl = new QLabel("Browse Premium Shops", heroArea);
    headLbl->setStyleSheet(
        "color: white; font-size: 28px; font-weight: 800; background: transparent;");
    titleCol->addWidget(welcomeLbl);
    titleCol->addWidget(headLbl);
    heroHL->addLayout(titleCol);
    heroHL->addStretch();

    // Search box
    m_searchBox = new QLineEdit(heroArea);
    m_searchBox->setPlaceholderText("Search for shops...");
    m_searchBox->setFixedWidth(300);
    m_searchBox->setStyleSheet(QString(R"(
        QLineEdit {
            background: rgba(255,255,255,0.08);
            border: 1px solid rgba(255,255,255,0.12);
            border-radius: 22px;
            color: white;
            padding: 10px 20px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 1px solid %1;
            background: rgba(124,92,252,0.12);
        }
    )").arg(Pal::ACCENT));
    heroHL->addWidget(m_searchBox, 0, Qt::AlignVCenter);
    connect(m_searchBox, &QLineEdit::textChanged,
            this, &ShopBrowserWidget::filterShops);

    root->addWidget(heroArea);

    // ── scrollable grid ──────────────────────────────────────
    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(QString("background: %1; border: none;").arg(Pal::BG_DARK));

    m_gridContainer = new QWidget();
    m_gridContainer->setStyleSheet(QString("background: %1;").arg(Pal::BG_DARK));
    m_grid = new QGridLayout(m_gridContainer);
    m_grid->setContentsMargins(48, 16, 48, 48);
    m_grid->setSpacing(24);

    scroll->setWidget(m_gridContainer);
    root->addWidget(scroll, 1);

    buildShopGrid();
}

void ShopBrowserWidget::buildHeader()
{
    QWidget* header = new QWidget(this);
    header->setFixedHeight(60);
    header->setStyleSheet(QString(R"(
        background: %1;
        border-bottom: 1px solid rgba(255,255,255,0.07);
    )").arg(Pal::BG_HEADER));

    QHBoxLayout* hl = new QHBoxLayout(header);
    hl->setContentsMargins(24, 0, 24, 0);

    // Logo
    QLabel* logoIcon = new QLabel("S", header);
    logoIcon->setFixedSize(34, 34);
    logoIcon->setAlignment(Qt::AlignCenter);
    logoIcon->setStyleSheet(QString(R"(
        background: %1;
        border-radius: 8px;
        color: white;
        font-size: 16px;
        font-weight: 800;
    )").arg(Pal::ACCENT));

    QLabel* logoText = new QLabel("<b>SHOPNOVA</b><span style='color:#7c5cfc;'>2</span>", header);
    logoText->setTextFormat(Qt::RichText);
    logoText->setStyleSheet(
        "color: white; font-size: 17px; font-weight: 700; background: transparent;");

    hl->addWidget(logoIcon);
    hl->addSpacing(8);
    hl->addWidget(logoText);
    hl->addStretch();

    // Cart button with badge
    QWidget* cartWrapper = new QWidget(header);
    cartWrapper->setFixedSize(44, 44);
    m_cartBtn = new QPushButton("🛒", cartWrapper);
    m_cartBtn->setFixedSize(44, 44);
    m_cartBtn->setCursor(Qt::PointingHandCursor);
    m_cartBtn->setStyleSheet(QString(R"(
        QPushButton {
            background: rgba(255,255,255,0.08);
            border: 1px solid rgba(255,255,255,0.15);
            border-radius: 22px;
            color: white;
            font-size: 18px;
        }
        QPushButton:hover {
            background: rgba(124,92,252,0.25);
            border: 1px solid %1;
        }
    )").arg(Pal::ACCENT));
    connect(m_cartBtn, &QPushButton::clicked, this, &ShopBrowserWidget::cartClicked);

    m_cartBadge = new QLabel("0", cartWrapper);
    m_cartBadge->setFixedSize(18, 18);
    m_cartBadge->move(26, 0);
    m_cartBadge->setAlignment(Qt::AlignCenter);
    m_cartBadge->setStyleSheet(QString(R"(
        background: %1;
        color: white;
        border-radius: 9px;
        font-size: 10px;
        font-weight: 700;
    )").arg(Pal::BADGE));
    m_cartBadge->hide();

    hl->addWidget(cartWrapper);

    // Add header to parent's layout
    qobject_cast<QVBoxLayout*>(layout())->addWidget(header);
}

void ShopBrowserWidget::buildShopGrid(const QString& filter)
{
    // Clear existing cards
    while (QLayoutItem* item = m_grid->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    int col = 0, row = 0;
    const int COLS = 3;

    for (Seller* s : m_allSellers) {
        if (!filter.isEmpty() &&
            !s->getStoreName().contains(filter, Qt::CaseInsensitive))
            continue;

        ShopCard* card = new ShopCard(s, m_gridContainer);
        connect(card, &ShopCard::clicked, this, &ShopBrowserWidget::onShopCardClicked);

        m_grid->addWidget(card, row, col);
        if (++col >= COLS) { col = 0; ++row; }
    }

    // Fill remaining columns with stretch
    if (col > 0) {
        for (int c = col; c < COLS; ++c)
            m_grid->setColumnStretch(c, 1);
    }
    m_grid->setRowStretch(row + 1, 1);
}

void ShopBrowserWidget::filterShops(const QString& text)
{
    buildShopGrid(text);
}

void ShopBrowserWidget::onShopCardClicked(Seller* seller)
{
    emit shopSelected(seller);
}

void ShopBrowserWidget::refreshCartBadge(int count)
{
    if (count > 0) {
        m_cartBadge->setText(QString::number(count));
        m_cartBadge->show();
    } else {
        m_cartBadge->hide();
    }
}

QString ShopBrowserWidget::globalStyle()
{
    return QString(R"(
        #ShopBrowserWidget {
            background: %1;
        }
        QScrollBar:vertical {
            background: %2;
            width: 6px;
            border-radius: 3px;
        }
        QScrollBar::handle:vertical {
            background: rgba(124,92,252,0.5);
            border-radius: 3px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )").arg(Pal::BG_DARK).arg(Pal::BG_MID);
}
