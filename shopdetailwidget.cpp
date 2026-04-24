#include "shopdetailwidget.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QLinearGradient>
#include <QPixmap>

// ── same palette ─────────────────────────────────────────────
namespace Pal2 {
    static const QString BG_DARK   = "#1a1535";
    static const QString BG_MID    = "#221d45";
    static const QString BG_HEADER = "#1e1840";
    static const QString ACCENT    = "#7c5cfc";
    static const QString ACCENT2   = "#a855f7";
    static const QString TEXT_HI   = "#ffffff";
    static const QString TEXT_LO   = "#9590b8";
    static const QString CARD_BG   = "#252048";
    static const QString BADGE     = "#ef4444";
}

// ============================================================
// ItemCard
// ============================================================
ItemCard::ItemCard(Product* product, int currentQty, QWidget* parent)
    : QFrame(parent), m_product(product), m_qty(currentQty)
{
    setFixedWidth(200);
    setStyleSheet(QString(R"(
        ItemCard {
            background: %1;
            border-radius: 14px;
            border: 1px solid rgba(124,92,252,0.1);
        }
    )").arg(Pal2::CARD_BG));

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(16);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 60));
    setGraphicsEffect(shadow);

    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->setContentsMargins(0, 0, 0, 12);
    vl->setSpacing(6);

    // Product image
    QLabel* imgLbl = new QLabel(this);
    imgLbl->setFixedHeight(140);
    imgLbl->setScaledContents(true);
    imgLbl->setStyleSheet("border-radius: 14px 14px 0 0; background: #1a1535;");

    QPixmap px;
    if (!product->getImagePath().isEmpty())
        px.load(product->getImagePath());

    if (px.isNull()) {
        px = QPixmap(200, 140);
        QPainter p(&px);
        QLinearGradient g(0, 0, 200, 140);
        g.setColorAt(0, QColor("#2d1b69"));
        g.setColorAt(1, QColor("#1a1535"));
        p.fillRect(px.rect(), g);
        p.setPen(Qt::white);
        QFont f; f.setPixelSize(28); f.setBold(true); p.setFont(f);
        p.drawText(px.rect(), Qt::AlignCenter, product->getName().left(3));
    }
    imgLbl->setPixmap(px.scaled(200, 140, Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation));
    vl->addWidget(imgLbl);

    // Name + price row
    QHBoxLayout* nameRow = new QHBoxLayout();
    nameRow->setContentsMargins(10, 4, 10, 0);

    QLabel* nameLbl = new QLabel(product->getName(), this);
    nameLbl->setStyleSheet(
        "color: white; font-size: 13px; font-weight: 700; background: transparent;");
    nameLbl->setWordWrap(true);

    QLabel* priceLbl = new QLabel(
        QString("₹%1").arg(product->getDiscountedPrice(), 0, 'f', 2), this);
    priceLbl->setStyleSheet(
        "color: white; font-size: 13px; font-weight: 700; background: transparent;");
    priceLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    nameRow->addWidget(nameLbl, 1);
    nameRow->addWidget(priceLbl);
    vl->addLayout(nameRow);

    // Description
    QLabel* descLbl = new QLabel(product->getDescription().left(50), this);
    descLbl->setStyleSheet(
        "color: #9590b8; font-size: 11px; background: transparent;");
    descLbl->setWordWrap(true);
    descLbl->setContentsMargins(10, 0, 10, 0);
    vl->addWidget(descLbl);

    vl->addStretch();

    // ── quantity control / add button ────────────────────────
    QWidget* ctrlArea = new QWidget(this);
    ctrlArea->setContentsMargins(10, 0, 10, 0);
    QVBoxLayout* ctrlVL = new QVBoxLayout(ctrlArea);
    ctrlVL->setContentsMargins(0, 0, 0, 0);

    // +/- row (shown when qty > 0)
    m_qtyControl = new QWidget(ctrlArea);
    QHBoxLayout* qhl = new QHBoxLayout(m_qtyControl);
    qhl->setContentsMargins(0, 0, 0, 0);
    qhl->setSpacing(0);

    m_minusBtn = new QPushButton("-", m_qtyControl);
    m_qtyLabel  = new QLabel("0",  m_qtyControl);
    m_plusBtn  = new QPushButton("+", m_qtyControl);

    QString btnStyle = QString(R"(
        QPushButton {
            background: %1;
            color: white;
            border: none;
            border-radius: 0;
            font-size: 16px;
            font-weight: 700;
            min-width: 32px;
            min-height: 32px;
        }
        QPushButton:hover { background: %2; }
    )").arg(Pal2::ACCENT).arg(Pal2::ACCENT2);

    m_minusBtn->setStyleSheet(btnStyle +
        "QPushButton { border-radius: 6px 0 0 6px; }");
    m_plusBtn->setStyleSheet(btnStyle +
        "QPushButton { border-radius: 0 6px 6px 0; }");
    m_qtyLabel->setAlignment(Qt::AlignCenter);
    m_qtyLabel->setStyleSheet(QString(
        "background: %1; color: white; font-size: 14px; font-weight: 700;"
        "min-width: 40px; min-height: 32px;").arg(Pal2::ACCENT));

    qhl->addWidget(m_minusBtn);
    qhl->addWidget(m_qtyLabel, 1);
    qhl->addWidget(m_plusBtn);

    // "Add to Cart" button (shown when qty == 0)
    m_addBtn = new QPushButton("ADD TO CART", ctrlArea);
    m_addBtn->setStyleSheet(QString(R"(
        QPushButton {
            background: %1;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 12px;
            font-weight: 700;
            letter-spacing: 0.8px;
            padding: 8px 0;
        }
        QPushButton:hover { background: %2; }
    )").arg(Pal2::ACCENT).arg(Pal2::ACCENT2));

    ctrlVL->addWidget(m_addBtn);
    ctrlVL->addWidget(m_qtyControl);
    vl->addWidget(ctrlArea);

    updateQtyDisplay();

    connect(m_plusBtn,  &QPushButton::clicked, this, &ItemCard::increment);
    connect(m_minusBtn, &QPushButton::clicked, this, &ItemCard::decrement);
    connect(m_addBtn,   &QPushButton::clicked, this, &ItemCard::increment);
}

void ItemCard::increment()
{
    ++m_qty;
    updateQtyDisplay();
    emit quantityChanged(m_product, m_qty);
}

void ItemCard::decrement()
{
    if (m_qty > 0) {
        --m_qty;
        updateQtyDisplay();
        emit quantityChanged(m_product, m_qty);
    }
}

void ItemCard::updateQtyDisplay()
{
    m_qtyLabel->setText(QString::number(m_qty));
    m_addBtn->setVisible(m_qty == 0);
    m_qtyControl->setVisible(m_qty > 0);
}

// ============================================================
// ShopDetailWidget
// ============================================================
ShopDetailWidget::ShopDetailWidget(Seller* seller,
                                   Platform* platform,
                                   QWidget* parent)
    : QWidget(parent), m_seller(seller), m_platform(platform)
{
    setObjectName("ShopDetailWidget");
    setStyleSheet(QString(R"(
        #ShopDetailWidget { background: %1; }
        QScrollBar:vertical {
            background: %2; width: 6px; border-radius: 3px;
        }
        QScrollBar::handle:vertical {
            background: rgba(124,92,252,0.5); border-radius: 3px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )").arg(Pal2::BG_DARK).arg(Pal2::BG_MID));

    // Collect this seller's products
    for (auto& p : m_platform->getAllProducts())
        if (p->getSellerId() == seller->getId())
            m_shopProducts.append(p.get());

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    buildHeader();
    buildShopBanner();

    // ── search + category bar ────────────────────────────────
    QWidget* filterBar = new QWidget(this);
    filterBar->setStyleSheet(QString("background: %1;").arg(Pal2::BG_DARK));
    QHBoxLayout* fhl = new QHBoxLayout(filterBar);
    fhl->setContentsMargins(32, 10, 32, 10);
    fhl->setSpacing(12);

    m_searchBox = new QLineEdit(filterBar);
    m_searchBox->setPlaceholderText(QString("Search in %1...").arg(seller->getStoreName()));
    m_searchBox->setStyleSheet(QString(R"(
        QLineEdit {
            background: rgba(255,255,255,0.07);
            border: 1px solid rgba(255,255,255,0.1);
            border-radius: 20px;
            color: white;
            padding: 8px 18px;
            font-size: 13px;
        }
        QLineEdit:focus {
            border: 1px solid %1;
            background: rgba(124,92,252,0.1);
        }
    )").arg(Pal2::ACCENT));
    fhl->addWidget(m_searchBox, 1);

    // Category pills
    QStringList cats = getCategories();
    for (const QString& cat : cats) {
        QPushButton* pill = new QPushButton(cat, filterBar);
        pill->setCursor(Qt::PointingHandCursor);
        pill->setCheckable(true);
        bool isActive = (m_activeCategory == cat);
        pill->setChecked(isActive);
        pill->setStyleSheet(isActive
            ? QString(R"(QPushButton {
                background: %1; color: white; border: none;
                border-radius: 14px; padding: 5px 14px;
                font-size: 12px; font-weight: 600;
              })").arg(Pal2::ACCENT)
            : R"(QPushButton {
                background: rgba(255,255,255,0.08); color: #9590b8;
                border: 1px solid rgba(255,255,255,0.1);
                border-radius: 14px; padding: 5px 14px;
                font-size: 12px; font-weight: 600;
              }
              QPushButton:hover {
                background: rgba(124,92,252,0.2); color: white;
              })");

        connect(pill, &QPushButton::clicked, this, [this, cat]() {
            m_activeCategory = (m_activeCategory == cat) ? "" : cat;
            buildItemList(m_searchBox->text());
        });
        fhl->addWidget(pill);
    }

    root->addWidget(filterBar);

    // ── scrollable items area ────────────────────────────────
    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(QString("background: %1; border: none;").arg(Pal2::BG_DARK));

    m_itemsContainer = new QWidget();
    m_itemsContainer->setStyleSheet(QString("background: %1;").arg(Pal2::BG_DARK));
    m_itemsLayout = new QVBoxLayout(m_itemsContainer);
    m_itemsLayout->setContentsMargins(32, 16, 32, 48);
    m_itemsLayout->setSpacing(32);

    scroll->setWidget(m_itemsContainer);
    root->addWidget(scroll, 1);

    connect(m_searchBox, &QLineEdit::textChanged,
            this, &ShopDetailWidget::onSearch);

    buildItemList();
}

void ShopDetailWidget::buildHeader()
{
    QWidget* header = new QWidget(this);
    header->setFixedHeight(56);
    header->setStyleSheet(QString(R"(
        background: %1;
        border-bottom: 1px solid rgba(255,255,255,0.07);
    )").arg(Pal2::BG_HEADER));

    QHBoxLayout* hl = new QHBoxLayout(header);
    hl->setContentsMargins(20, 0, 20, 0);

    // Logo
    QLabel* logoIcon = new QLabel("S", header);
    logoIcon->setFixedSize(30, 30);
    logoIcon->setAlignment(Qt::AlignCenter);
    logoIcon->setStyleSheet(QString(R"(
        background: %1; border-radius: 7px;
        color: white; font-size: 14px; font-weight: 800;
    )").arg(Pal2::ACCENT));
    QLabel* logoText = new QLabel("<b>SHOPNOVA</b><span style='color:#7c5cfc;'>2</span>", header);
    logoText->setTextFormat(Qt::RichText);
    logoText->setStyleSheet("color: white; font-size: 15px; background: transparent;");

    hl->addWidget(logoIcon);
    hl->addSpacing(6);
    hl->addWidget(logoText);
    hl->addStretch();

    // Cart btn
    QWidget* cartWrapper = new QWidget(header);
    cartWrapper->setFixedSize(100, 44);
    QPushButton* cartBtn = new QPushButton("🛒 Cart", cartWrapper);
    cartBtn->setFixedSize(100, 44);
    cartBtn->setCursor(Qt::PointingHandCursor);
    cartBtn->setStyleSheet(QString(R"(
        QPushButton {
            background: rgba(255,255,255,0.08);
            border: 1px solid rgba(255,255,255,0.15);
            border-radius: 20px; color: white; font-size: 16px;
        }
        QPushButton:hover { background: rgba(124,92,252,0.25); border: 1px solid %1; }
    )").arg(Pal2::ACCENT));
    connect(cartBtn, &QPushButton::clicked, this, &ShopDetailWidget::cartBtnClicked);

    m_cartBadge = new QLabel("0", cartWrapper);
    m_cartBadge->setFixedSize(18, 18);
    m_cartBadge->move(82, 0); // Position at top right of the 100px wide button
    m_cartBadge->setAlignment(Qt::AlignCenter);
    m_cartBadge->setStyleSheet(QString(R"(
        background: %1; color: white; border-radius: 8px;
        font-size: 9px; font-weight: 700;
    )").arg(Pal2::BADGE));
    m_cartBadge->hide();
    hl->addWidget(cartWrapper);

    qobject_cast<QVBoxLayout*>(layout())->addWidget(header);
}

void ShopDetailWidget::buildShopBanner()
{
    QWidget* banner = new QWidget(this);
    banner->setFixedHeight(180);
    banner->setStyleSheet(QString("background: %1;").arg(Pal2::BG_MID));

    QVBoxLayout* col = new QVBoxLayout(banner);
    col->setContentsMargins(32, 12, 32, 12);

    // Back button
    QPushButton* backBtn = new QPushButton("← BACK TO SHOPS", banner);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet(QString(R"(
        QPushButton {
            background: transparent;
            color: %1;
            border: none;
            font-size: 12px;
            font-weight: 600;
            letter-spacing: 0.5px;
        }
        QPushButton:hover { color: white; }
    )").arg(Pal2::ACCENT));
    connect(backBtn, &QPushButton::clicked, this, &ShopDetailWidget::backClicked);
    col->addWidget(backBtn, 0, Qt::AlignLeft);

    QWidget* infoRow = new QWidget(banner);
    QHBoxLayout* ihl = new QHBoxLayout(infoRow);
    ihl->setContentsMargins(0, 0, 0, 0);
    ihl->setSpacing(20);

    // Shop image thumb
    QLabel* imgLbl = new QLabel(infoRow);
    imgLbl->setFixedSize(120, 100);
    imgLbl->setStyleSheet("border-radius: 12px; background: #1a1535;");
    imgLbl->setScaledContents(true);
    QPixmap px(120, 100);
    QPainter p(&px);
    QLinearGradient g(0, 0, 120, 100);
    g.setColorAt(0, QColor("#2d1b69"));
    g.setColorAt(1, QColor("#1a1535"));
    p.fillRect(px.rect(), g);
    p.setPen(Qt::white);
    QFont f; f.setPixelSize(28); f.setBold(true); p.setFont(f);
    p.drawText(px.rect(), Qt::AlignCenter,
               m_seller->getStoreName().left(2).toUpper());
    imgLbl->setPixmap(px);
    ihl->addWidget(imgLbl);

    // Shop meta
    QVBoxLayout* meta = new QVBoxLayout();
    meta->setSpacing(4);

    // Payment badge
    QLabel* payBadge = new QLabel(
        m_seller->hasQr() ? "🔵 UPI Payments Available" : "🟡 Cash on Delivery Only",
        infoRow);
    payBadge->setStyleSheet(m_seller->hasQr()
        ? "color: #a5f3fc; font-size: 10px; font-weight: 700;"
          "background: rgba(6,182,212,0.25); border-radius: 8px;"
          "padding: 2px 8px; border: 1px solid rgba(6,182,212,0.4);"
        : "color: #fde68a; font-size: 10px; font-weight: 700;"
          "background: rgba(251,191,36,0.2); border-radius: 8px;"
          "padding: 2px 8px; border: 1px solid rgba(251,191,36,0.35);");
    payBadge->setMaximumWidth(200);

    QLabel* nameL = new QLabel(m_seller->getStoreName(), infoRow);
    nameL->setStyleSheet(
        "color: white; font-size: 24px; font-weight: 800; background: transparent;");

    QHBoxLayout* ratingRow = new QHBoxLayout();
    double rating = m_seller->getRating();
    if (rating < 3.0) rating = 4.0 + (rand() % 10) * 0.1;

    QLabel* rL = new QLabel(QString("★  %1").arg(rating, 0, 'f', 1), infoRow);
    rL->setStyleSheet(QString(
        "color: white; font-size: 13px; font-weight: 700;"
        "background: %1; border-radius: 10px; padding: 2px 10px;")
        .arg(Pal2::ACCENT));

    QLabel* tL = new QLabel("🕐  20-40 min", infoRow);
    tL->setStyleSheet("color: #9590b8; font-size: 12px; background: transparent;");

    ratingRow->addWidget(rL);
    ratingRow->addSpacing(12);
    ratingRow->addWidget(tL);
    ratingRow->addStretch();

    meta->addWidget(payBadge);
    meta->addWidget(nameL);
    meta->addLayout(ratingRow);
    ihl->addLayout(meta);
    ihl->addStretch();

    col->addWidget(infoRow);
    banner->setLayout(col);

    qobject_cast<QVBoxLayout*>(layout())->addWidget(banner);
}

void ShopDetailWidget::buildItemList(const QString& searchText)
{
    // Clear
    while (QLayoutItem* item = m_itemsLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    // Group products by category
    QMap<QString, QList<Product*>> byCategory;
    for (Product* p : m_shopProducts) {
        if (!searchText.isEmpty() &&
            !p->getName().contains(searchText, Qt::CaseInsensitive))
            continue;
        if (!m_activeCategory.isEmpty() &&
            p->getCategory() != m_activeCategory)
            continue;
        byCategory[p->getCategory()].append(p);
    }

    if (byCategory.isEmpty()) {
        QLabel* emptyLbl = new QLabel("No items found.", m_itemsContainer);
        emptyLbl->setStyleSheet("color: #9590b8; font-size: 16px;");
        emptyLbl->setAlignment(Qt::AlignCenter);
        m_itemsLayout->addWidget(emptyLbl);
        return;
    }

    for (auto it = byCategory.begin(); it != byCategory.end(); ++it) {
        // Category header
        QHBoxLayout* catHdr = new QHBoxLayout();
        QLabel* catName = new QLabel(it.key(), m_itemsContainer);
        catName->setStyleSheet(
            "color: white; font-size: 20px; font-weight: 800; background: transparent;");

        QLabel* countLbl = new QLabel(
            QString("%1 ITEMS").arg(it.value().size()), m_itemsContainer);
        countLbl->setStyleSheet(
            "color: #9590b8; font-size: 11px; font-weight: 600; background: transparent;");
        countLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        catHdr->addWidget(catName);
        catHdr->addStretch();
        catHdr->addWidget(countLbl);
        m_itemsLayout->addLayout(catHdr);

        // Item cards row (wrap)
        QWidget* rowWrap = new QWidget(m_itemsContainer);
        rowWrap->setStyleSheet("background: transparent;");
        QHBoxLayout* rowHL = new QHBoxLayout(rowWrap);
        rowHL->setContentsMargins(0, 0, 0, 0);
        rowHL->setSpacing(16);

        for (Product* p : it.value()) {
            int qty = m_cartQtys.value(p->getId(), 0);
            ItemCard* card = new ItemCard(p, qty, rowWrap);
            connect(card, &ItemCard::quantityChanged,
                    this, &ShopDetailWidget::onQuantityChanged);
            rowHL->addWidget(card);
        }
        rowHL->addStretch();
        m_itemsLayout->addWidget(rowWrap);

        // Separator
        QFrame* sep = new QFrame(m_itemsContainer);
        sep->setFrameShape(QFrame::HLine);
        sep->setStyleSheet("color: rgba(255,255,255,0.06);");
        m_itemsLayout->addWidget(sep);
    }

    m_itemsLayout->addStretch();
}

QStringList ShopDetailWidget::getCategories() const
{
    QStringList cats;
    for (Product* p : m_shopProducts)
        if (!cats.contains(p->getCategory()))
            cats.append(p->getCategory());
    return cats;
}

void ShopDetailWidget::onSearch(const QString& text)
{
    buildItemList(text);
}

void ShopDetailWidget::onCategoryFilter(const QString& cat)
{
    m_activeCategory = cat;
    buildItemList(m_searchBox->text());
}

void ShopDetailWidget::onQuantityChanged(Product* product, int qty)
{
    m_cartQtys[product->getId()] = qty;

    // Update cart badge
    int total = 0;
    for (int v : m_cartQtys.values()) total += v;
    if (total > 0) {
        m_cartBadge->setText(QString::number(total));
        m_cartBadge->show();
    } else {
        m_cartBadge->hide();
    }

    emit cartUpdated(product, qty);
}

void ShopDetailWidget::setCartQuantities(const QMap<int, int>& qtys)
{
    m_cartQtys = qtys;
    buildItemList(m_searchBox ? m_searchBox->text() : "");
}
