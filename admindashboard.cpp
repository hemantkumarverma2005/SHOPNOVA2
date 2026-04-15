#include "admindashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>
#include <QGridLayout>

AdminDashboard::AdminDashboard(Admin* admin, Platform* platform, QWidget* parent)
    : QWidget(parent), m_admin(admin), m_platform(platform)
{
    buildUI();
}

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

void AdminDashboard::buildUI() {
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(16);

    // ─── Header ─────────────────────────────────────────
    QHBoxLayout* header = new QHBoxLayout;
    QLabel* title = new QLabel("Admin Dashboard");
    title->setObjectName("pageTitle");

    QLabel* badge = new QLabel("  ADMIN  ");
    badge->setObjectName("roleBadge");
    badge->setStyleSheet(
        "QLabel { background: rgba(255,107,43,0.15); color: #ff9a5c; "
        "border: 1px solid rgba(255,107,43,0.3); border-radius: 20px; "
        "padding: 5px 14px; font-size: 11px; font-weight: 700; }");

    QLabel* welcomeText = new QLabel("  Welcome, " + m_admin->getName());
    welcomeText->setStyleSheet("color: #5a5a70; font-size: 13px;");

    QPushButton* logoutBtn = new QPushButton("Logout");
    logoutBtn->setObjectName("logoutBtn");
    connect(logoutBtn, &QPushButton::clicked, this, &AdminDashboard::logoutRequested);

    header->addWidget(title);
    header->addSpacing(10);
    header->addWidget(badge);
    header->addWidget(welcomeText);
    header->addStretch();
    header->addWidget(logoutBtn);
    root->addLayout(header);

    // ─── Stats Grid ──────────────────────────────────────
    m_statsLabel = new QLabel;
    m_statsLabel->setVisible(false); // hidden, replaced by cards

    QGridLayout* statsGrid = new QGridLayout;
    statsGrid->setSpacing(12);
    m_statCards[0] = makeStatCard("📦", "0", "Total Products", "#ff6b2b");
    m_statCards[1] = makeStatCard("🛒", "0", "Total Orders",   "#4cc9f0");
    m_statCards[2] = makeStatCard("👥", "0", "Customers",      "#06d6a0");
    m_statCards[3] = makeStatCard("🏪", "0", "Sellers",        "#7b5ea7");
    m_statCards[4] = makeStatCard("💰", "₹0", "Revenue",       "#ffd166");

    for (int i = 0; i < 5; ++i)
        statsGrid->addWidget(m_statCards[i], 0, i);

    root->addLayout(statsGrid);

    // ─── Tabs ────────────────────────────────────────────
    QTabWidget* tabs = new QTabWidget;
    root->addWidget(tabs);

    // ── Products tab ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);
        lay->setContentsMargins(8, 12, 8, 8);
        QHBoxLayout* btns = new QHBoxLayout;
        QPushButton* addBtn = new QPushButton("  Add Product  ");
        addBtn->setObjectName("successBtn");
        QPushButton* toggleBtn = new QPushButton("  Toggle Active  ");
        toggleBtn->setObjectName("ghostBtn");
        connect(addBtn,    &QPushButton::clicked, this, &AdminDashboard::onAddProduct);
        connect(toggleBtn, &QPushButton::clicked, this, &AdminDashboard::onToggleProduct);
        btns->addWidget(addBtn);
        btns->addWidget(toggleBtn);
        btns->addStretch();

        m_productTable = new QTableWidget(0, 7);
        m_productTable->setHorizontalHeaderLabels({"ID", "Name", "Type", "Brand", "Price", "Stock", "Status"});
        m_productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_productTable->setAlternatingRowColors(true);
        m_productTable->verticalHeader()->setVisible(false);
        m_productTable->setShowGrid(false);

        lay->addLayout(btns);
        lay->addWidget(m_productTable);
        tabs->addTab(tab, "  Products  ");
    }

    // ── Orders tab ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);
        lay->setContentsMargins(8, 12, 8, 8);
        QHBoxLayout* btns = new QHBoxLayout;
        QPushButton* updateBtn = new QPushButton("  Update Status  ");
        updateBtn->setObjectName("primaryBtn");
        connect(updateBtn, &QPushButton::clicked, this, &AdminDashboard::onUpdateOrderStatus);
        btns->addWidget(updateBtn);
        btns->addStretch();

        m_orderTable = new QTableWidget(0, 6);
        m_orderTable->setHorizontalHeaderLabels({"Order #", "Customer", "Items", "Amount", "Payment", "Status"});
        m_orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_orderTable->setAlternatingRowColors(true);
        m_orderTable->verticalHeader()->setVisible(false);
        m_orderTable->setShowGrid(false);

        lay->addLayout(btns);
        lay->addWidget(m_orderTable);
        tabs->addTab(tab, "  Orders  ");
    }

    // ── Sellers tab ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);
        lay->setContentsMargins(8, 12, 8, 8);
        QHBoxLayout* btns = new QHBoxLayout;
        QPushButton* verifyBtn = new QPushButton("  Verify Seller  ");
        verifyBtn->setObjectName("successBtn");
        connect(verifyBtn, &QPushButton::clicked, this, &AdminDashboard::onVerifySeller);
        btns->addWidget(verifyBtn);
        btns->addStretch();

        m_sellerTable = new QTableWidget(0, 5);
        m_sellerTable->setHorizontalHeaderLabels({"ID", "Name", "Store", "Email", "Status"});
        m_sellerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_sellerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_sellerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_sellerTable->setAlternatingRowColors(true);
        m_sellerTable->verticalHeader()->setVisible(false);
        m_sellerTable->setShowGrid(false);

        lay->addLayout(btns);
        lay->addWidget(m_sellerTable);
        tabs->addTab(tab, "  Sellers  ");
    }

    // ── Customers tab ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);
        lay->setContentsMargins(8, 12, 8, 8);
        QHBoxLayout* btns = new QHBoxLayout;
        QPushButton* toggleUserBtn = new QPushButton("  Toggle Active  ");
        toggleUserBtn->setObjectName("dangerBtn");
        connect(toggleUserBtn, &QPushButton::clicked, this, &AdminDashboard::onToggleUser);
        btns->addWidget(toggleUserBtn);
        btns->addStretch();

        m_customerTable = new QTableWidget(0, 5);
        m_customerTable->setHorizontalHeaderLabels({"ID", "Name", "Email", "Tier", "Status"});
        m_customerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_customerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_customerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_customerTable->setAlternatingRowColors(true);
        m_customerTable->verticalHeader()->setVisible(false);
        m_customerTable->setShowGrid(false);

        lay->addLayout(btns);
        lay->addWidget(m_customerTable);
        tabs->addTab(tab, "  Customers  ");
    }
}

void AdminDashboard::refresh() {
    refreshStats();
    refreshProducts();
    refreshOrders();
    refreshSellers();
    refreshCustomers();
}

void AdminDashboard::refreshStats() {
    auto s = m_platform->getStats();
    // Update stat card values
    auto updateCard = [](QFrame* card, const QString& val) {
        QLabel* valLbl = card->findChild<QLabel*>("cardValue");
        if (valLbl) valLbl->setText(val);
    };
    updateCard(m_statCards[0], QString::number(s.totalProducts));
    updateCard(m_statCards[1], QString::number(s.totalOrders));
    updateCard(m_statCards[2], QString::number(s.totalCustomers));
    updateCard(m_statCards[3], QString::number(s.totalSellers));
    updateCard(m_statCards[4], QString::fromUtf8("₹") + QString::number((int)s.totalRevenue));
}

void AdminDashboard::refreshProducts() {
    m_productTable->setRowCount(0);
    for (auto& prod : m_platform->getAllProducts()) {
        int row = m_productTable->rowCount();
        m_productTable->insertRow(row);
        m_productTable->setItem(row, 0, new QTableWidgetItem(QString::number(prod->getId())));
        m_productTable->setItem(row, 1, new QTableWidgetItem(prod->getName()));
        m_productTable->setItem(row, 2, new QTableWidgetItem(prod->getType()));
        m_productTable->setItem(row, 3, new QTableWidgetItem(prod->getBrand()));
        m_productTable->setItem(row, 4, new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)prod->getDiscountedPrice())));
        m_productTable->setItem(row, 5, new QTableWidgetItem(QString::number(prod->getStock())));

        // Status pill
        QString statusText = prod->getIsActive() ? "● Active" : "● Inactive";
        QString objName = prod->getIsActive() ? "statusActive" : "statusInactive";
        m_productTable->setCellWidget(row, 6, makeStatusPill(statusText, objName));
        m_productTable->setRowHeight(row, 40);
    }
}

void AdminDashboard::refreshOrders() {
    m_orderTable->setRowCount(0);
    for (auto& o : m_platform->getAllOrders()) {
        int row = m_orderTable->rowCount();
        m_orderTable->insertRow(row);
        m_orderTable->setItem(row, 0, new QTableWidgetItem("#" + QString::number(o->getId())));
        m_orderTable->setItem(row, 1, new QTableWidgetItem(o->getCustomerName()));
        m_orderTable->setItem(row, 2, new QTableWidgetItem(QString::number(o->getItems().size()) + " item(s)"));
        m_orderTable->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)o->getTotalAmount())));
        m_orderTable->setItem(row, 4, new QTableWidgetItem(o->getPayment().methodString()));

        // Status pill
        m_orderTable->setCellWidget(row, 5, makeOrderStatusPill(o->getStatus(), o->statusString()));
        m_orderTable->setRowHeight(row, 40);
    }
}

void AdminDashboard::refreshSellers() {
    m_sellerTable->setRowCount(0);
    for (auto& s : m_platform->getSellers()) {
        int row = m_sellerTable->rowCount();
        m_sellerTable->insertRow(row);
        m_sellerTable->setItem(row, 0, new QTableWidgetItem(QString::number(s->getId())));
        m_sellerTable->setItem(row, 1, new QTableWidgetItem(s->getName()));
        m_sellerTable->setItem(row, 2, new QTableWidgetItem(s->getStoreName()));
        m_sellerTable->setItem(row, 3, new QTableWidgetItem(s->getEmail()));

        // Status pill
        QString statusText = s->getIsVerified() ? "✓ Verified" : "⏳ Pending";
        QString objName = s->getIsVerified() ? "statusVerified" : "statusPending";
        m_sellerTable->setCellWidget(row, 4, makeStatusPill(statusText, objName));
        m_sellerTable->setRowHeight(row, 40);
    }
}

void AdminDashboard::refreshCustomers() {
    m_customerTable->setRowCount(0);
    for (auto& c : m_platform->getCustomers()) {
        int row = m_customerTable->rowCount();
        m_customerTable->insertRow(row);
        m_customerTable->setItem(row, 0, new QTableWidgetItem(QString::number(c->getId())));
        m_customerTable->setItem(row, 1, new QTableWidgetItem(c->getName()));
        m_customerTable->setItem(row, 2, new QTableWidgetItem(c->getEmail()));
        m_customerTable->setItem(row, 3, new QTableWidgetItem(c->getMembershipTier()));

        // Status pill
        QString statusText = c->getIsActive() ? "● Active" : "● Banned";
        QString objName = c->getIsActive() ? "statusActive" : "statusInactive";
        m_customerTable->setCellWidget(row, 4, makeStatusPill(statusText, objName));
        m_customerTable->setRowHeight(row, 40);
    }
}

void AdminDashboard::onAddProduct() {
    QDialog dlg(this);
    dlg.setWindowTitle("Add New Product");
    dlg.setFixedWidth(400);
    QFormLayout* form = new QFormLayout(&dlg);
    form->setContentsMargins(24, 24, 24, 24);
    form->setSpacing(12);

    QLineEdit* nameEdit  = new QLineEdit;  nameEdit->setMinimumHeight(36);
    QLineEdit* descEdit  = new QLineEdit;  descEdit->setMinimumHeight(36);
    QLineEdit* brandEdit = new QLineEdit;  brandEdit->setMinimumHeight(36);
    QComboBox* typeCombo = new QComboBox;
    typeCombo->addItems({"Electronics", "Clothing", "Books"});
    typeCombo->setMinimumHeight(36);
    QDoubleSpinBox* priceSpin = new QDoubleSpinBox;
    priceSpin->setRange(0.01, 9999999.0); priceSpin->setDecimals(0);
    priceSpin->setPrefix(QString::fromUtf8("₹")); priceSpin->setMinimumHeight(36);
    QSpinBox* stockSpin = new QSpinBox;
    stockSpin->setRange(0, 999999); stockSpin->setMinimumHeight(36);

    QComboBox* sellerCombo = new QComboBox;
    sellerCombo->setMinimumHeight(36);
    for (auto& s : m_platform->getSellers())
        sellerCombo->addItem(s->getStoreName(), s->getId());

    QPushButton* okBtn = new QPushButton("  Add Product  ");
    okBtn->setObjectName("successBtn");
    okBtn->setMinimumHeight(40);
    connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

    form->addRow("Name:",     nameEdit);
    form->addRow("Description:", descEdit);
    form->addRow("Brand:",    brandEdit);
    form->addRow("Type:",     typeCombo);
    form->addRow("Price:",    priceSpin);
    form->addRow("Stock:",    stockSpin);
    form->addRow("Seller:",   sellerCombo);
    form->addRow(okBtn);

    if (dlg.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) { QMessageBox::warning(this, "Error", "Name is required."); return; }
        int sellerId   = sellerCombo->currentData().toInt();
        QString sName  = sellerCombo->currentText();
        QString type   = typeCombo->currentText();

        std::unique_ptr<Product> prod;
        if (type == "Electronics")
            prod = std::make_unique<Electronics>(0, name, descEdit->text(), brandEdit->text(),
                priceSpin->value(), stockSpin->value(), sellerId, sName, 12);
        else if (type == "Clothing")
            prod = std::make_unique<Clothing>(0, name, descEdit->text(), brandEdit->text(),
                priceSpin->value(), stockSpin->value(), sellerId, sName, "");
        else
            prod = std::make_unique<Books>(0, name, descEdit->text(), brandEdit->text(),
                priceSpin->value(), stockSpin->value(), sellerId, sName, "", "");

        int pid = m_platform->addProduct(std::move(prod));
        m_admin->logAction("Added product ID:" + QString::number(pid));
        refresh();
        QMessageBox::information(this, "Success", "Product added! ID: " + QString::number(pid));
    }
}

void AdminDashboard::onToggleProduct() {
    int row = m_productTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select a product first."); return; }
    int pid = m_productTable->item(row, 0)->text().toInt();
    m_platform->toggleProductStatus(pid);
    m_admin->logAction("Toggled product #" + QString::number(pid));
    refreshProducts();
}

void AdminDashboard::onVerifySeller() {
    int row = m_sellerTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select a seller first."); return; }
    int sid = m_sellerTable->item(row, 0)->text().toInt();
    m_platform->verifySeller(sid, true);
    m_admin->logAction("Verified seller #" + QString::number(sid));
    refreshSellers();
    QMessageBox::information(this, "Success", "Seller verified!");
}

void AdminDashboard::onUpdateOrderStatus() {
    int row = m_orderTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select an order first."); return; }
    int oid = m_orderTable->item(row, 0)->text().remove('#').toInt();

    QStringList statuses = {"Pending", "Confirmed", "Shipped", "Delivered", "Cancelled"};
    bool ok;
    QString chosen = QInputDialog::getItem(this, "Update Status", "New status:", statuses, 0, false, &ok);
    if (!ok) return;

    OrderStatus ns;
    if      (chosen == "Pending")   ns = OrderStatus::PENDING;
    else if (chosen == "Confirmed") ns = OrderStatus::CONFIRMED;
    else if (chosen == "Shipped")   ns = OrderStatus::SHIPPED;
    else if (chosen == "Delivered") ns = OrderStatus::DELIVERED;
    else                            ns = OrderStatus::CANCELLED;

    if (m_platform->updateOrderStatus(oid, ns)) {
        m_admin->logAction("Updated order #" + QString::number(oid) + " → " + chosen);
        refreshOrders();
        QMessageBox::information(this, "Updated", "Order status updated.");
    }
}

void AdminDashboard::onToggleUser() {
    int row = m_customerTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select a customer first."); return; }
    int uid = m_customerTable->item(row, 0)->text().toInt();
    m_platform->toggleUserStatus(uid);
    m_admin->logAction("Toggled user #" + QString::number(uid));
    refreshCustomers();
}
