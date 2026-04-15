#include "admindashboard.h"
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
#include <QComboBox>
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

AdminDashboard::AdminDashboard(Admin* admin, Platform* platform, QWidget* parent)
    : QWidget(parent), m_admin(admin), m_platform(platform)
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    m_shell = new ShellWidget(this);
    root->addWidget(m_shell);

    connect(m_shell, &ShellWidget::logoutRequested,
            this, &AdminDashboard::logoutRequested);

    QVector<NavItem> nav = {
        {"",                         "Overview",     nullptr},
        {QString::fromUtf8("🏠"),    "Dashboard",    buildDashboardPage()},
        {"",                         "Management",   nullptr},
        {QString::fromUtf8("📦"),    "Products",     buildProductsPage()},
        {QString::fromUtf8("📋"),    "All Orders",   buildOrdersPage()},
        {QString::fromUtf8("🏪"),    "Sellers",      buildSellersPage()},
        {QString::fromUtf8("👥"),    "Customers",    buildCustomersPage()},
    };

    m_shell->configure("ShopNova", m_admin->getName(), "Admin", nav);
}

// ─── Page builders ───────────────────────────────────────────────────────────

QWidget *AdminDashboard::buildDashboardPage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);

    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(24);

    QLabel *greet = new QLabel(QString::fromUtf8("Admin Dashboard"));
    greet->setStyleSheet("font-size: 26px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(greet);

    QLabel *welcome = new QLabel(QString::fromUtf8("Welcome, %1 — manage your platform from here.").arg(m_admin->getName()));
    welcome->setStyleSheet("font-size: 14px; color: #5a5a70;");
    lay->addWidget(welcome);

    // Stat cards row
    QHBoxLayout *statsRow = new QHBoxLayout;
    statsRow->setSpacing(16);

    m_statCards[0] = makeStatCard(QString::fromUtf8("📦"), "0", "Total Products", "rgba(255,107,43,0.15)");
    m_statCards[1] = makeStatCard(QString::fromUtf8("🛒"), "0", "Total Orders",   "rgba(76,201,240,0.15)");
    m_statCards[2] = makeStatCard(QString::fromUtf8("👥"), "0", "Customers",      "rgba(6,214,160,0.15)");
    m_statCards[3] = makeStatCard(QString::fromUtf8("🏪"), "0", "Sellers",        "rgba(123,94,167,0.15)");
    m_statCards[4] = makeStatCard(QString::fromUtf8("💰"), QString::fromUtf8("₹0"), "Revenue", "rgba(255,209,102,0.15)");

    for (int i = 0; i < 5; ++i)
        statsRow->addWidget(m_statCards[i]);
    lay->addLayout(statsRow);

    QLabel *hint = new QLabel("Navigate using the sidebar to manage products, orders, sellers, and customers.");
    hint->setStyleSheet("font-size: 13px; color: #5a5a70;");
    hint->setWordWrap(true);
    lay->addWidget(hint);

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget *AdminDashboard::buildProductsPage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);

    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(20);

    QLabel *h = new QLabel(QString::fromUtf8("📦  Products"));
    h->setStyleSheet("font-size: 24px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(h);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton* addBtn = new QPushButton("  Add Product  ");
    addBtn->setObjectName("successBtn");
    QPushButton* toggleBtn = new QPushButton("  Toggle Active  ");
    toggleBtn->setObjectName("ghostBtn");
    addBtn->setMinimumHeight(38);
    toggleBtn->setMinimumHeight(38);
    connect(addBtn,    &QPushButton::clicked, this, &AdminDashboard::onAddProduct);
    connect(toggleBtn, &QPushButton::clicked, this, &AdminDashboard::onToggleProduct);
    btns->addWidget(addBtn);
    btns->addWidget(toggleBtn);
    btns->addStretch();
    lay->addLayout(btns);

    m_productTable = new QTableWidget(0, 7);
    m_productTable->setHorizontalHeaderLabels({"ID", "Name", "Type", "Brand", "Price", "Stock", "Status"});
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

QWidget *AdminDashboard::buildOrdersPage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);

    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(20);

    QLabel *h = new QLabel(QString::fromUtf8("📋  All Orders"));
    h->setStyleSheet("font-size: 24px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(h);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton* updateBtn = new QPushButton("  Update Status  ");
    updateBtn->setObjectName("primaryBtn");
    updateBtn->setMinimumHeight(38);
    connect(updateBtn, &QPushButton::clicked, this, &AdminDashboard::onUpdateOrderStatus);
    btns->addWidget(updateBtn);
    btns->addStretch();
    lay->addLayout(btns);

    m_orderTable = new QTableWidget(0, 6);
    m_orderTable->setHorizontalHeaderLabels({"Order #", "Customer", "Items", "Amount", "Payment", "Status"});
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

QWidget *AdminDashboard::buildSellersPage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);

    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(20);

    QLabel *h = new QLabel(QString::fromUtf8("🏪  Sellers"));
    h->setStyleSheet("font-size: 24px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(h);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton* verifyBtn = new QPushButton("  Verify Seller  ");
    verifyBtn->setObjectName("successBtn");
    verifyBtn->setMinimumHeight(38);
    connect(verifyBtn, &QPushButton::clicked, this, &AdminDashboard::onVerifySeller);
    btns->addWidget(verifyBtn);
    btns->addStretch();
    lay->addLayout(btns);

    m_sellerTable = new QTableWidget(0, 5);
    m_sellerTable->setHorizontalHeaderLabels({"ID", "Name", "Store", "Email", "Status"});
    m_sellerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_sellerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_sellerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_sellerTable->setAlternatingRowColors(true);
    m_sellerTable->verticalHeader()->setVisible(false);
    m_sellerTable->setShowGrid(false);
    m_sellerTable->setMinimumHeight(400);
    lay->addWidget(m_sellerTable);

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget *AdminDashboard::buildCustomersPage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);

    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(20);

    QLabel *h = new QLabel(QString::fromUtf8("👥  Customers"));
    h->setStyleSheet("font-size: 24px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(h);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton* toggleUserBtn = new QPushButton("  Toggle Active  ");
    toggleUserBtn->setObjectName("dangerBtn");
    toggleUserBtn->setMinimumHeight(38);
    connect(toggleUserBtn, &QPushButton::clicked, this, &AdminDashboard::onToggleUser);
    btns->addWidget(toggleUserBtn);
    btns->addStretch();
    lay->addLayout(btns);

    m_customerTable = new QTableWidget(0, 5);
    m_customerTable->setHorizontalHeaderLabels({"ID", "Name", "Email", "Tier", "Status"});
    m_customerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_customerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_customerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_customerTable->setAlternatingRowColors(true);
    m_customerTable->verticalHeader()->setVisible(false);
    m_customerTable->setShowGrid(false);
    m_customerTable->setMinimumHeight(400);
    lay->addWidget(m_customerTable);

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ─── Refresh logic ───────────────────────────────────────────────────────────

void AdminDashboard::refresh() {
    refreshStats();
    refreshProducts();
    refreshOrders();
    refreshSellers();
    refreshCustomers();
}

void AdminDashboard::refreshStats() {
    auto s = m_platform->getStats();
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
        QString statusText = prod->getIsActive() ? QString::fromUtf8("● Active") : QString::fromUtf8("● Inactive");
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
        QString statusText = s->getIsVerified() ? QString::fromUtf8("✓ Verified") : QString::fromUtf8("⏳ Pending");
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
        QString statusText = c->getIsActive() ? QString::fromUtf8("● Active") : QString::fromUtf8("● Banned");
        QString objName = c->getIsActive() ? "statusActive" : "statusInactive";
        m_customerTable->setCellWidget(row, 4, makeStatusPill(statusText, objName));
        m_customerTable->setRowHeight(row, 40);
    }
}

// ─── Slot implementations ────────────────────────────────────────────────────

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
        m_admin->logAction("Updated order #" + QString::number(oid) + QString::fromUtf8(" → ") + chosen);
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
