#include "sellerdashboard.h"
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
#include <QPushButton>
#include <QLineEdit>

SellerDashboard::SellerDashboard(Seller* seller, Platform* platform, QWidget* parent)
    : QWidget(parent), m_seller(seller), m_platform(platform)
{
    buildUI();
}

void SellerDashboard::buildUI() {
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(10);

    // Header
    QHBoxLayout* header = new QHBoxLayout;
    QLabel* title = new QLabel("🏪  " + m_seller->getStoreName() +
                                (m_seller->getIsVerified() ? "  ✅" : "  ⏳ Pending"));
    title->setObjectName("pageTitle");
    QPushButton* logoutBtn = new QPushButton("Logout");
    logoutBtn->setObjectName("logoutBtn");
    connect(logoutBtn, &QPushButton::clicked, this, &SellerDashboard::logoutRequested);
    header->addWidget(title);
    header->addStretch();
    header->addWidget(logoutBtn);
    root->addLayout(header);

    // Dashboard stats
    m_dashLabel = new QLabel;
    m_dashLabel->setObjectName("statsLabel");
    root->addWidget(m_dashLabel);

    // Tabs
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
        connect(updateBtn, &QPushButton::clicked, this, &SellerDashboard::onUpdateOrderStatus);
        btns->addWidget(updateBtn);
        btns->addStretch();

        m_orderTable = new QTableWidget(0, 5);
        m_orderTable->setHorizontalHeaderLabels({"Order#", "Customer", "Product", "Amount (₹)", "Status"});
        m_orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_orderTable->setAlternatingRowColors(true);

        lay->addLayout(btns);
        lay->addWidget(m_orderTable);
        tabs->addTab(tab, "📋 My Orders");
    }
}

void SellerDashboard::refresh() {
    m_dashLabel->setText(
        QString("🏪 Store: %1  |  📦 Products: %2  |  💰 Revenue: ₹%3  |  ⭐ Rating: %4")
        .arg(m_seller->getStoreName())
        .arg(m_platform->getProductsBySeller(m_seller->getId()).size())
        .arg((int)m_seller->getTotalRevenue())
        .arg(m_seller->getRating(), 0, 'f', 1));
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
        m_productTable->setItem(row, 4, new QTableWidgetItem(QString::number((int)p->getDiscount()) + "%"));
        auto* sItem = new QTableWidgetItem(QString::number(p->getStock()));
        if (p->getStock() == 0) sItem->setForeground(QColor("#f38ba8"));
        m_productTable->setItem(row, 5, sItem);
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
                m_orderTable->setItem(row, 3, new QTableWidgetItem("₹" + QString::number((int)o->getTotalAmount())));
                auto* s = new QTableWidgetItem(o->statusString());
                s->setForeground(o->getStatus() == OrderStatus::DELIVERED ? QColor("#a6e3a1") :
                                 o->getStatus() == OrderStatus::CANCELLED  ? QColor("#f38ba8") : QColor("#fab387"));
                m_orderTable->setItem(row, 4, s);
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

    QLineEdit* nameEdit  = new QLineEdit;
    QLineEdit* descEdit  = new QLineEdit;
    QLineEdit* brandEdit = new QLineEdit;
    QDoubleSpinBox* price = new QDoubleSpinBox;
    price->setRange(1, 9999999); price->setDecimals(0); price->setPrefix("₹");
    QSpinBox* stock = new QSpinBox; stock->setRange(0, 999999);
    QPushButton* ok = new QPushButton("Add Product"); ok->setObjectName("successBtn");
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
