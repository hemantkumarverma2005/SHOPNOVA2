#include "sellerdashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>

// ── Helper: create a stat card widget ─────────────────────────
static QFrame* makeStatCard(const QString& icon, const QString& value,
                            const QString& label, const QString& accent)
{
    QFrame* card = new QFrame;
    card->setMinimumHeight(120);
    card->setStyleSheet(QString(
        "QFrame { background: #14142a; border: 1px solid #1e1e3a;"
        "border-radius: 16px; }"));
    QVBoxLayout* cl = new QVBoxLayout(card);
    cl->setContentsMargins(20, 18, 20, 18);
    cl->setSpacing(8);

    QLabel* ic = new QLabel(icon);
    ic->setFixedSize(40, 40);
    ic->setAlignment(Qt::AlignCenter);
    ic->setStyleSheet(QString("background: %1; border-radius: 12px; font-size: 20px;").arg(accent));
    QLabel* ttl = new QLabel(label);
    ttl->setStyleSheet("font-size: 12px; color: #5a5a80; font-weight: 600;");
    QLabel* val = new QLabel(value);
    val->setObjectName("cardValue");
    val->setStyleSheet("font-size: 28px; font-weight: 800; color: #e0e0f0;");

    cl->addWidget(ic);
    cl->addWidget(ttl);
    cl->addWidget(val);
    return card;
}

// ── Helper: create a status pill QLabel ────────────────────────
static QWidget* makeStatusPill(const QString& text, const QString& objectName) {
    QWidget* container = new QWidget;
    QHBoxLayout* hlay = new QHBoxLayout(container);
    hlay->setContentsMargins(0, 2, 0, 2);
    hlay->setAlignment(Qt::AlignLeft);
    QLabel* pill = new QLabel(text);
    pill->setObjectName(objectName);
    pill->setAlignment(Qt::AlignCenter);
    pill->setFixedHeight(24);
    hlay->addWidget(pill);
    return container;
}

static QWidget* makeOrderStatusPill(OrderStatus status, const QString& text) {
    QString objName;
    switch (status) {
        case OrderStatus::PENDING:   objName = "statusPending"; break;
        case OrderStatus::CONFIRMED: objName = "statusConfirmed"; break;
        case OrderStatus::SHIPPED:   objName = "statusShipped"; break;
        case OrderStatus::DELIVERED: objName = "statusDelivered"; break;
        case OrderStatus::CANCELLED: objName = "statusCancelled"; break;
        case OrderStatus::RETURNED:  objName = "statusReturned"; break;
    }
    return makeStatusPill(text, objName);
}

SellerDashboard::SellerDashboard(Seller* seller, Platform* platform, QWidget* parent)
    : QWidget(parent), m_seller(seller), m_platform(platform)
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    m_shell = new ShellWidget(this);
    root->addWidget(m_shell);

    connect(m_shell, &ShellWidget::logoutRequested,
            this, &SellerDashboard::logoutRequested);

    QVector<NavItem> nav = {
        {"",                         "Store",        nullptr},
        {QString::fromUtf8("🏠"),    "Overview",     buildOverviewPage()},
        {QString::fromUtf8("📦"),    "My Products",  buildProductsPage()},
        {"",                         "Management",   nullptr},
        {QString::fromUtf8("📋"),    "Orders",       buildOrdersPage()},
        {QString::fromUtf8("👤"),    "Profile",      buildProfilePage()},
    };

    m_shell->configure("ShopNova", m_seller->getName(), "Seller", nav);
}

// ─── Page builders ───────────────────────────────────────────────────────────

QWidget *SellerDashboard::buildOverviewPage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);

    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(24);

    // Header with store name and badge
    QHBoxLayout *headerRow = new QHBoxLayout;
    QLabel *title = new QLabel(QString::fromUtf8("🏪  ") + m_seller->getStoreName());
    title->setStyleSheet("font-size: 26px; font-weight: 800; color: #e0e0f0;");
    headerRow->addWidget(title);
    headerRow->addSpacing(12);

    QLabel* badge;
    if (m_seller->getIsVerified()) {
        badge = new QLabel(QString::fromUtf8("  ✓ VERIFIED  "));
        badge->setObjectName("verifiedBadge");
    } else {
        badge = new QLabel(QString::fromUtf8("  ⏳ PENDING  "));
        badge->setObjectName("pendingBadge");
    }
    headerRow->addWidget(badge);
    headerRow->addStretch();
    lay->addLayout(headerRow);

    // Stat cards row
    QHBoxLayout *statsRow = new QHBoxLayout;
    statsRow->setSpacing(16);

    m_statCards[0] = makeStatCard(QString::fromUtf8("🏪"), m_seller->getStoreName(), "Store Name",  "rgba(255,107,43,0.15)");
    m_statCards[1] = makeStatCard(QString::fromUtf8("📦"), "0", "Products",  "rgba(76,201,240,0.15)");
    m_statCards[2] = makeStatCard(QString::fromUtf8("💰"), QString::fromUtf8("₹0"), "Revenue",  "rgba(6,214,160,0.15)");
    m_statCards[3] = makeStatCard(QString::fromUtf8("⭐"), "0.0", "Rating",   "rgba(255,209,102,0.15)");

    for (int i = 0; i < 4; ++i)
        statsRow->addWidget(m_statCards[i]);
    lay->addLayout(statsRow);

    QLabel *hint = new QLabel("Manage your products and orders from the sidebar navigation.");
    hint->setStyleSheet("font-size: 13px; color: #5a5a70;");
    hint->setWordWrap(true);
    lay->addWidget(hint);

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget *SellerDashboard::buildProductsPage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);

    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(20);

    QLabel *h = new QLabel(QString::fromUtf8("📦  My Products"));
    h->setStyleSheet("font-size: 24px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(h);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton* addBtn      = new QPushButton(QString::fromUtf8("➕ Add Product"));
    addBtn->setObjectName("successBtn");
    QPushButton* discountBtn = new QPushButton(QString::fromUtf8("🏷 Set Discount"));
    discountBtn->setObjectName("warnBtn");
    QPushButton* stockBtn    = new QPushButton(QString::fromUtf8("📦 Update Stock"));
    stockBtn->setObjectName("primaryBtn");
    addBtn->setMinimumHeight(36);
    discountBtn->setMinimumHeight(36);
    stockBtn->setMinimumHeight(36);

    connect(addBtn,      &QPushButton::clicked, this, &SellerDashboard::onAddProduct);
    connect(discountBtn, &QPushButton::clicked, this, &SellerDashboard::onSetDiscount);
    connect(stockBtn,    &QPushButton::clicked, this, &SellerDashboard::onUpdateStock);

    btns->addWidget(addBtn);
    btns->addWidget(discountBtn);
    btns->addWidget(stockBtn);
    btns->addStretch();
    lay->addLayout(btns);

    m_productTable = new QTableWidget(0, 6);
    m_productTable->setHorizontalHeaderLabels({"ID", "Name", "Brand", QString::fromUtf8("Price (₹)"), "Disc%", "Stock"});
    m_productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_productTable->setAlternatingRowColors(true);
    m_productTable->verticalHeader()->setVisible(false);
    m_productTable->setShowGrid(false);
    m_productTable->setMinimumHeight(400);
    lay->addWidget(m_productTable);

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget *SellerDashboard::buildOrdersPage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);

    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(20);

    QLabel *h = new QLabel(QString::fromUtf8("📋  My Orders"));
    h->setStyleSheet("font-size: 24px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(h);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton* updateBtn = new QPushButton(QString::fromUtf8("✏️ Update Status"));
    updateBtn->setObjectName("primaryBtn");
    updateBtn->setMinimumHeight(36);
    connect(updateBtn, &QPushButton::clicked, this, &SellerDashboard::onUpdateOrderStatus);
    btns->addWidget(updateBtn);
    btns->addStretch();
    lay->addLayout(btns);

    m_orderTable = new QTableWidget(0, 5);
    m_orderTable->setHorizontalHeaderLabels({"Order#", "Customer", "Product", QString::fromUtf8("Amount (₹)"), "Status"});
    m_orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_orderTable->setAlternatingRowColors(true);
    m_orderTable->verticalHeader()->setVisible(false);
    m_orderTable->setShowGrid(false);
    m_orderTable->setMinimumHeight(400);
    lay->addWidget(m_orderTable);

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget *SellerDashboard::buildProfilePage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);

    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(16);

    QLabel *h = new QLabel(QString::fromUtf8("👤  Seller Profile"));
    h->setStyleSheet("font-size: 24px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(h);

    QLabel *profile = new QLabel;
    profile->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    profile->setWordWrap(true);
    profile->setStyleSheet(
        "font-size: 14px; color: #f0f0f5; padding: 24px;"
        "background-color: #111118; border: 1px solid rgba(255,255,255,0.07);"
        "border-radius: 16px; line-height: 1.8;");
    profile->setText(
        QString("<div style='line-height: 2.2;'>"
                "<span style='font-size: 22px; font-weight: 800; color: #f0f0f5;'>🏪  %1</span>"
                "<br>"
                "<span style='background: rgba(6,214,160,0.15); color: #06d6a0; "
                "border-radius: 12px; padding: 3px 10px; font-size: 11px; font-weight: 700;'>"
                "%2</span>"
                "<br><br>"
                "<span style='color: #5a5a70; font-size: 12px; text-transform: uppercase; "
                "letter-spacing: 1px;'>CONTACT</span><br>"
                "📧  <span style='color: #9494a8;'>%3</span><br>"
                "📱  <span style='color: #9494a8;'>%4</span>"
                "<br><br>"
                "<span style='color: #5a5a70; font-size: 12px; text-transform: uppercase; "
                "letter-spacing: 1px;'>STORE INFO</span><br>"
                "💰  Revenue: <span style='color: #06d6a0; font-weight: 700;'>₹%5</span><br>"
                "⭐  Rating: <span style='color: #ffd166; font-weight: 700;'>%6</span><br>"
                "📦  Products: <span style='color: #4cc9f0; font-weight: 700;'>%7</span>"
                "</div>")
        .arg(m_seller->getStoreName())
        .arg(m_seller->getIsVerified() ? "Verified Seller" : "Pending Verification")
        .arg(m_seller->getEmail())
        .arg(m_seller->getPhone())
        .arg((int)m_seller->getTotalRevenue())
        .arg(m_seller->getRating(), 0, 'f', 1)
        .arg(m_seller->getProductIds().size()));
    lay->addWidget(profile);

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ─── Refresh logic ───────────────────────────────────────────────────────────

void SellerDashboard::refresh() {
    refreshStats();
    refreshProducts();
    refreshOrders();
}

void SellerDashboard::refreshStats() {
    auto updateCard = [](QFrame* card, const QString& val) {
        QLabel* valLbl = card->findChild<QLabel*>("cardValue");
        if (valLbl) valLbl->setText(val);
    };
    auto prods = m_platform->getProductsBySeller(m_seller->getId());
    updateCard(m_statCards[0], m_seller->getStoreName());
    updateCard(m_statCards[1], QString::number(prods.size()));
    updateCard(m_statCards[2], QString::fromUtf8("₹") + QString::number((int)m_seller->getTotalRevenue()));
    updateCard(m_statCards[3], QString::number(m_seller->getRating(), 'f', 1));
}

void SellerDashboard::refreshProducts() {
    m_productTable->setRowCount(0);
    for (auto* p : m_platform->getProductsBySeller(m_seller->getId())) {
        int row = m_productTable->rowCount();
        m_productTable->insertRow(row);
        m_productTable->setItem(row, 0, new QTableWidgetItem(QString::number(p->getId())));
        m_productTable->setItem(row, 1, new QTableWidgetItem(p->getName()));
        m_productTable->setItem(row, 2, new QTableWidgetItem(p->getBrand()));
        m_productTable->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)p->getDiscountedPrice())));

        // Discount with color
        auto* discItem = new QTableWidgetItem(QString::number((int)p->getDiscount()) + "%");
        if (p->getDiscount() > 0) discItem->setForeground(QColor("#06d6a0"));
        m_productTable->setItem(row, 4, discItem);

        // Stock with color coding
        auto* sItem = new QTableWidgetItem(QString::number(p->getStock()));
        if (p->getStock() == 0) { sItem->setForeground(QColor("#ff4444")); sItem->setText("Out"); }
        else if (p->getStock() <= 5) { sItem->setForeground(QColor("#ffd166")); }
        m_productTable->setItem(row, 5, sItem);
        m_productTable->setRowHeight(row, 38);
    }
}

void SellerDashboard::refreshOrders() {
    m_orderTable->setRowCount(0);
    auto myProds = m_platform->getProductsBySeller(m_seller->getId());
    QVector<int> myPids;
    for (auto* p : myProds) myPids.append(p->getId());

    for (auto& o : m_platform->getAllOrders()) {
        for (auto& item : o->getItems()) {
            if (myPids.contains(item.productId)) {
                int row = m_orderTable->rowCount();
                m_orderTable->insertRow(row);
                m_orderTable->setItem(row, 0, new QTableWidgetItem("#" + QString::number(o->getId())));
                m_orderTable->setItem(row, 1, new QTableWidgetItem(o->getCustomerName()));
                m_orderTable->setItem(row, 2, new QTableWidgetItem(item.productName + " x" + QString::number(item.quantity)));
                auto* amtItem = new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)o->getTotalAmount()));
                amtItem->setForeground(QColor("#ff9a5c"));
                m_orderTable->setItem(row, 3, amtItem);

                // Status pill
                m_orderTable->setCellWidget(row, 4, makeOrderStatusPill(o->getStatus(), o->statusString()));
                m_orderTable->setRowHeight(row, 40);
                break;
            }
        }
    }
}

// ─── Slot implementations ────────────────────────────────────────────────────

void SellerDashboard::onAddProduct() {
    QDialog dlg(this);
    dlg.setWindowTitle("Add Product to " + m_seller->getStoreName());
    dlg.setFixedWidth(360);
    QFormLayout* form = new QFormLayout(&dlg);
    form->setContentsMargins(16, 16, 16, 16); form->setSpacing(10);

    QLineEdit* nameEdit  = new QLineEdit;  nameEdit->setMinimumHeight(36);
    QLineEdit* descEdit  = new QLineEdit;  descEdit->setMinimumHeight(36);
    QLineEdit* brandEdit = new QLineEdit;  brandEdit->setMinimumHeight(36);
    QDoubleSpinBox* price = new QDoubleSpinBox;
    price->setRange(1, 9999999); price->setDecimals(0); price->setPrefix(QString::fromUtf8("₹"));
    price->setMinimumHeight(36);
    QSpinBox* stock = new QSpinBox; stock->setRange(0, 999999);
    stock->setMinimumHeight(36);
    QPushButton* ok = new QPushButton("Add Product"); ok->setObjectName("successBtn");
    ok->setMinimumHeight(40);
    connect(ok, &QPushButton::clicked, &dlg, &QDialog::accept);

    form->addRow("Name:",    nameEdit);
    form->addRow("Desc:",    descEdit);
    form->addRow("Brand:",   brandEdit);
    form->addRow("Price:",   price);
    form->addRow("Stock:",   stock);
    form->addRow(ok);

    if (dlg.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) return;
        auto prod = std::make_unique<Electronics>(0, name, descEdit->text(),
            brandEdit->text(), price->value(), stock->value(),
            m_seller->getId(), m_seller->getStoreName(), 12);
        int pid = m_platform->addProduct(std::move(prod));
        m_seller->addProductId(pid);
        refresh();
        QMessageBox::information(this, "Added", "Product added! ID: " + QString::number(pid));
    }
}

void SellerDashboard::onSetDiscount() {
    int row = m_productTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select a product."); return; }
    int pid = m_productTable->item(row, 0)->text().toInt();
    Product* p = m_platform->getProductById(pid);
    if (!p || p->getSellerId() != m_seller->getId()) return;
    bool ok;
    int disc = QInputDialog::getInt(this, "Set Discount",
        "Discount % for " + p->getName(), (int)p->getDiscount(), 0, 90, 1, &ok);
    if (ok) { p->setDiscount(disc); m_platform->saveToDisk(); refreshProducts(); }
}

void SellerDashboard::onUpdateStock() {
    int row = m_productTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select a product."); return; }
    int pid = m_productTable->item(row, 0)->text().toInt();
    Product* p = m_platform->getProductById(pid);
    if (!p || p->getSellerId() != m_seller->getId()) return;
    bool ok;
    int newStock = QInputDialog::getInt(this, "Update Stock",
        "New stock for " + p->getName(), p->getStock(), 0, 999999, 1, &ok);
    if (ok) { p->setStock(newStock); m_platform->saveToDisk(); refreshProducts(); }
}

void SellerDashboard::onUpdateOrderStatus() {
    int row = m_orderTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select an order."); return; }
    int oid = m_orderTable->item(row, 0)->text().remove('#').toInt();
    QStringList statuses = {"Confirmed", "Shipped", "Delivered"};
    bool ok;
    QString chosen = QInputDialog::getItem(this, "Update Status", "New status:", statuses, 0, false, &ok);
    if (!ok) return;
    OrderStatus ns = (chosen == "Confirmed") ? OrderStatus::CONFIRMED :
                     (chosen == "Shipped")   ? OrderStatus::SHIPPED   : OrderStatus::DELIVERED;
    m_platform->updateOrderStatus(oid, ns);
    refreshOrders();
}
