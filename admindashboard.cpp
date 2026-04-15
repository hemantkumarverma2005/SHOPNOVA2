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

AdminDashboard::AdminDashboard(Admin* admin, Platform* platform, QWidget* parent)
    : QWidget(parent), m_admin(admin), m_platform(platform)
{
    buildUI();
}

void AdminDashboard::buildUI() {
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(10);

    // ─── Header ─────────────────────────────────────────
    QHBoxLayout* header = new QHBoxLayout;
    QLabel* title = new QLabel("⚙️  Admin Dashboard — " + m_admin->getName());
    title->setObjectName("pageTitle");

    QPushButton* logoutBtn = new QPushButton("Logout");
    logoutBtn->setObjectName("logoutBtn");
    connect(logoutBtn, &QPushButton::clicked, this, &AdminDashboard::logoutRequested);

    header->addWidget(title);
    header->addStretch();
    header->addWidget(logoutBtn);
    root->addLayout(header);

    // ─── Stats bar ──────────────────────────────────────
    m_statsLabel = new QLabel;
    m_statsLabel->setObjectName("statsLabel");
    root->addWidget(m_statsLabel);

    // ─── Tabs ────────────────────────────────────────────
    QTabWidget* tabs = new QTabWidget;
    root->addWidget(tabs);

    // ── Products tab ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);
        QHBoxLayout* btns = new QHBoxLayout;
        QPushButton* addBtn = new QPushButton("➕ Add Product");
        addBtn->setObjectName("successBtn");
        QPushButton* toggleBtn = new QPushButton("⏸ Toggle Active");
        toggleBtn->setObjectName("warnBtn");
        connect(addBtn,    &QPushButton::clicked, this, &AdminDashboard::onAddProduct);
        connect(toggleBtn, &QPushButton::clicked, this, &AdminDashboard::onToggleProduct);
        btns->addWidget(addBtn);
        btns->addWidget(toggleBtn);
        btns->addStretch();

        m_productTable = new QTableWidget(0, 7);
        m_productTable->setHorizontalHeaderLabels({"ID", "Name", "Type", "Brand", "Price (₹)", "Stock", "Status"});
        m_productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_productTable->setAlternatingRowColors(true);

        lay->addLayout(btns);
        lay->addWidget(m_productTable);
        tabs->addTab(tab, "📦 Products");
    }

    // ── Orders tab ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);
        QHBoxLayout* btns = new QHBoxLayout;
        QPushButton* updateBtn = new QPushButton("✏️ Update Status");
        updateBtn->setObjectName("primaryBtn");
        connect(updateBtn, &QPushButton::clicked, this, &AdminDashboard::onUpdateOrderStatus);
        btns->addWidget(updateBtn);
        btns->addStretch();

        m_orderTable = new QTableWidget(0, 6);
        m_orderTable->setHorizontalHeaderLabels({"Order#", "Customer", "Items", "Total (₹)", "Payment", "Status"});
        m_orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_orderTable->setAlternatingRowColors(true);

        lay->addLayout(btns);
        lay->addWidget(m_orderTable);
        tabs->addTab(tab, "📋 Orders");
    }

    // ── Sellers tab ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);
        QHBoxLayout* btns = new QHBoxLayout;
        QPushButton* verifyBtn = new QPushButton("✅ Verify Seller");
        verifyBtn->setObjectName("successBtn");
        connect(verifyBtn, &QPushButton::clicked, this, &AdminDashboard::onVerifySeller);
        btns->addWidget(verifyBtn);
        btns->addStretch();

        m_sellerTable = new QTableWidget(0, 5);
        m_sellerTable->setHorizontalHeaderLabels({"ID", "Name", "Store", "Email", "Verified"});
        m_sellerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_sellerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_sellerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_sellerTable->setAlternatingRowColors(true);

        lay->addLayout(btns);
        lay->addWidget(m_sellerTable);
        tabs->addTab(tab, "🏪 Sellers");
    }

    // ── Customers tab ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);
        QHBoxLayout* btns = new QHBoxLayout;
        QPushButton* toggleUserBtn = new QPushButton("🔒 Toggle Active");
        toggleUserBtn->setObjectName("dangerBtn");
        connect(toggleUserBtn, &QPushButton::clicked, this, &AdminDashboard::onToggleUser);
        btns->addWidget(toggleUserBtn);
        btns->addStretch();

        m_customerTable = new QTableWidget(0, 5);
        m_customerTable->setHorizontalHeaderLabels({"ID", "Name", "Email", "Tier", "Active"});
        m_customerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_customerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_customerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_customerTable->setAlternatingRowColors(true);

        lay->addLayout(btns);
        lay->addWidget(m_customerTable);
        tabs->addTab(tab, "👥 Customers");
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
    m_statsLabel->setText(
        QString("📦 Products: %1  |  🛒 Orders: %2  |  👥 Customers: %3  "
                "|  🏪 Sellers: %4  |  💰 Revenue: ₹%5")
        .arg(s.totalProducts).arg(s.totalOrders).arg(s.totalCustomers)
        .arg(s.totalSellers).arg((int)s.totalRevenue));
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
        m_productTable->setItem(row, 4, new QTableWidgetItem("₹" + QString::number((int)prod->getDiscountedPrice())));
        m_productTable->setItem(row, 5, new QTableWidgetItem(QString::number(prod->getStock())));
        auto* statusItem = new QTableWidgetItem(prod->getIsActive() ? "Active" : "Inactive");
        statusItem->setForeground(prod->getIsActive() ? QColor("#a6e3a1") : QColor("#f38ba8"));
        m_productTable->setItem(row, 6, statusItem);
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
        m_orderTable->setItem(row, 3, new QTableWidgetItem("₹" + QString::number((int)o->getTotalAmount())));
        m_orderTable->setItem(row, 4, new QTableWidgetItem(o->getPayment().methodString()));
        auto* s = new QTableWidgetItem(o->statusString());
        s->setForeground(o->getStatus() == OrderStatus::DELIVERED ? QColor("#a6e3a1") :
                         o->getStatus() == OrderStatus::CANCELLED  ? QColor("#f38ba8") :
                                                                      QColor("#fab387"));
        m_orderTable->setItem(row, 5, s);
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
        auto* v = new QTableWidgetItem(s->getIsVerified() ? "✅ Verified" : "⏳ Pending");
        v->setForeground(s->getIsVerified() ? QColor("#a6e3a1") : QColor("#fab387"));
        m_sellerTable->setItem(row, 4, v);
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
        auto* a = new QTableWidgetItem(c->getIsActive() ? "Active" : "Banned");
        a->setForeground(c->getIsActive() ? QColor("#a6e3a1") : QColor("#f38ba8"));
        m_customerTable->setItem(row, 4, a);
    }
}

void AdminDashboard::onAddProduct() {
    QDialog dlg(this);
    dlg.setWindowTitle("Add New Product");
    dlg.setFixedWidth(360);
    QFormLayout* form = new QFormLayout(&dlg);
    form->setContentsMargins(16, 16, 16, 16);
    form->setSpacing(10);

    QLineEdit* nameEdit  = new QLineEdit;
    QLineEdit* descEdit  = new QLineEdit;
    QLineEdit* brandEdit = new QLineEdit;
    QComboBox* typeCombo = new QComboBox;
    typeCombo->addItems({"Electronics", "Clothing", "Books"});
    QDoubleSpinBox* priceSpin = new QDoubleSpinBox;
    priceSpin->setRange(0.01, 9999999.0); priceSpin->setDecimals(0); priceSpin->setPrefix("₹");
    QSpinBox* stockSpin = new QSpinBox;
    stockSpin->setRange(0, 999999);

    // Seller combo
    QComboBox* sellerCombo = new QComboBox;
    for (auto& s : m_platform->getSellers())
        sellerCombo->addItem(s->getStoreName(), s->getId());

    QPushButton* okBtn = new QPushButton("Add Product");
    okBtn->setObjectName("successBtn");
    connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

    form->addRow("Name:",     nameEdit);
    form->addRow("Desc:",     descEdit);
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
    // Get order id from display (strip '#')
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
