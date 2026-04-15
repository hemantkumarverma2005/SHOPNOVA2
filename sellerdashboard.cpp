#include "sellerdashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>

// ── Helper: create a stat card widget ─────────────────────────
static QFrame* makeStatCard(const QString& icon, const QString& value,
                            const QString& label, const QString& glowColor)
{
    QFrame* card = new QFrame;
    card->setStyleSheet(
        QString("QFrame { background: #111118; border: 1px solid rgba(255,255,255,0.07); "
                "border-radius: 16px; padding: 20px; }"
                "QFrame:hover { border-color: %1; }").arg(glowColor));

    QVBoxLayout* lay = new QVBoxLayout(card);
    lay->setSpacing(6);

    QLabel* iconLbl = new QLabel(icon);
    iconLbl->setStyleSheet("font-size: 26px; background: transparent; border: none;");

    QLabel* valLbl = new QLabel(value);
    valLbl->setObjectName("cardValue");
    valLbl->setStyleSheet("font-size: 26px; font-weight: 800; color: #f0f0f5; "
                          "letter-spacing: -0.5px; background: transparent; border: none;");

    QLabel* lblLbl = new QLabel(label);
    lblLbl->setStyleSheet("font-size: 12px; color: #5a5a70; background: transparent; border: none;");

    lay->addWidget(iconLbl);
    lay->addWidget(valLbl);
    lay->addWidget(lblLbl);
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
    buildUI();
}

void SellerDashboard::buildUI() {
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(16);

    // ─── Header ─────────────────────────────────────────
    QHBoxLayout* header = new QHBoxLayout;
    QLabel* title = new QLabel("🏪  " + m_seller->getStoreName());
    title->setObjectName("pageTitle");

    // Verified / Pending badge
    QLabel* badge;
    if (m_seller->getIsVerified()) {
        badge = new QLabel("  ✓ VERIFIED  ");
        badge->setObjectName("verifiedBadge");
    } else {
        badge = new QLabel("  ⏳ PENDING  ");
        badge->setObjectName("pendingBadge");
    }

    QLabel* sellerName = new QLabel("  " + m_seller->getName());
    sellerName->setStyleSheet("color: #5a5a70; font-size: 13px;");

    QPushButton* logoutBtn = new QPushButton("Logout");
    logoutBtn->setObjectName("logoutBtn");
    connect(logoutBtn, &QPushButton::clicked, this, &SellerDashboard::logoutRequested);

    header->addWidget(title);
    header->addSpacing(8);
    header->addWidget(badge);
    header->addWidget(sellerName);
    header->addStretch();
    header->addWidget(logoutBtn);
    root->addLayout(header);

    // ─── Stats Grid (replaces text label) ────────────────
    m_dashLabel = new QLabel;
    m_dashLabel->setVisible(false); // hidden, replaced by cards

    QGridLayout* statsGrid = new QGridLayout;
    statsGrid->setSpacing(12);
    m_statCards[0] = makeStatCard("🏪", m_seller->getStoreName(), "Store Name",  "#ff6b2b");
    m_statCards[1] = makeStatCard("📦", "0", "Products",  "#4cc9f0");
    m_statCards[2] = makeStatCard("💰", "₹0", "Revenue",  "#06d6a0");
    m_statCards[3] = makeStatCard("⭐", "0.0", "Rating",   "#ffd166");

    for (int i = 0; i < 4; ++i)
        statsGrid->addWidget(m_statCards[i], 0, i);
    root->addLayout(statsGrid);

    // ─── Tabs ────────────────────────────────────────────
    QTabWidget* tabs = new QTabWidget;
    root->addWidget(tabs);

    // ── My Products ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);
        QHBoxLayout* btns = new QHBoxLayout;

        QPushButton* addBtn      = new QPushButton("➕ Add Product");      addBtn->setObjectName("successBtn");
        QPushButton* discountBtn = new QPushButton("🏷 Set Discount");     discountBtn->setObjectName("warnBtn");
        QPushButton* stockBtn    = new QPushButton("📦 Update Stock");     stockBtn->setObjectName("primaryBtn");
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

        m_productTable = new QTableWidget(0, 6);
        m_productTable->setHorizontalHeaderLabels({"ID", "Name", "Brand", "Price (₹)", "Disc%", "Stock"});
        m_productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_productTable->setAlternatingRowColors(true);
        m_productTable->verticalHeader()->setVisible(false);
        m_productTable->setShowGrid(false);

        lay->addLayout(btns);
        lay->addWidget(m_productTable);
        tabs->addTab(tab, "📦 My Products");
    }

    // ── My Orders ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);
        QHBoxLayout* btns = new QHBoxLayout;

        QPushButton* updateBtn = new QPushButton("✏️ Update Status");
        updateBtn->setObjectName("primaryBtn");
        updateBtn->setMinimumHeight(36);
        connect(updateBtn, &QPushButton::clicked, this, &SellerDashboard::onUpdateOrderStatus);
        btns->addWidget(updateBtn);
        btns->addStretch();

        m_orderTable = new QTableWidget(0, 5);
        m_orderTable->setHorizontalHeaderLabels({"Order#", "Customer", "Product", "Amount (₹)", "Status"});
        m_orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_orderTable->setAlternatingRowColors(true);
        m_orderTable->verticalHeader()->setVisible(false);
        m_orderTable->setShowGrid(false);

        lay->addLayout(btns);
        lay->addWidget(m_orderTable);
        tabs->addTab(tab, "📋 My Orders");
    }
}

void SellerDashboard::refresh() {
    // Update stat card values
    auto updateCard = [](QFrame* card, const QString& val) {
        QLabel* valLbl = card->findChild<QLabel*>("cardValue");
        if (valLbl) valLbl->setText(val);
    };
    auto prods = m_platform->getProductsBySeller(m_seller->getId());
    updateCard(m_statCards[0], m_seller->getStoreName());
    updateCard(m_statCards[1], QString::number(prods.size()));
    updateCard(m_statCards[2], "₹" + QString::number((int)m_seller->getTotalRevenue()));
    updateCard(m_statCards[3], QString::number(m_seller->getRating(), 'f', 1));

    refreshProducts();
    refreshOrders();
}

void SellerDashboard::refreshProducts() {
    m_productTable->setRowCount(0);
    for (auto* p : m_platform->getProductsBySeller(m_seller->getId())) {
        int row = m_productTable->rowCount();
        m_productTable->insertRow(row);
        m_productTable->setItem(row, 0, new QTableWidgetItem(QString::number(p->getId())));
        m_productTable->setItem(row, 1, new QTableWidgetItem(p->getName()));
        m_productTable->setItem(row, 2, new QTableWidgetItem(p->getBrand()));
        m_productTable->setItem(row, 3, new QTableWidgetItem("₹" + QString::number((int)p->getDiscountedPrice())));

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
                auto* amtItem = new QTableWidgetItem("₹" + QString::number((int)o->getTotalAmount()));
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
    price->setRange(1, 9999999); price->setDecimals(0); price->setPrefix("₹");
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
    int stock = QInputDialog::getInt(this, "Update Stock",
        "New stock for " + p->getName(), p->getStock(), 0, 999999, 1, &ok);
    if (ok) { p->setStock(stock); m_platform->saveToDisk(); refreshProducts(); }
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
