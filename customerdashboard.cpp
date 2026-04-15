#include "customerdashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>

CustomerDashboard::CustomerDashboard(Customer* customer, Platform* platform, QWidget* parent)
    : QWidget(parent), m_customer(customer), m_platform(platform)
{
    buildUI();
}

void CustomerDashboard::buildUI() {
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(10);

    // ─── Header ─────────────────────────────────────────
    QHBoxLayout* header = new QHBoxLayout;
    QLabel* title = new QLabel("🛍️  " + m_customer->getName() + "'s ShopNova");
    title->setObjectName("pageTitle");

    m_notifLabel = new QLabel;
    m_notifLabel->setObjectName("notifLabel");

    QPushButton* logoutBtn = new QPushButton("Logout");
    logoutBtn->setObjectName("logoutBtn");
    connect(logoutBtn, &QPushButton::clicked, this, &CustomerDashboard::logoutRequested);

    header->addWidget(title);
    header->addStretch();
    header->addWidget(m_notifLabel);
    header->addWidget(logoutBtn);
    root->addLayout(header);

    // ─── Tabs ─────────────────────────────────────────────
    QTabWidget* tabs = new QTabWidget;
    root->addWidget(tabs);

    // ── Browse Products ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);

        // Search bar
        QHBoxLayout* searchRow = new QHBoxLayout;
        m_searchEdit = new QLineEdit;
        m_searchEdit->setPlaceholderText("🔍  Search products, brands, categories...");
        QPushButton* searchBtn  = new QPushButton("Search");  searchBtn->setObjectName("primaryBtn");
        QPushButton* clearBtn   = new QPushButton("All");     clearBtn->setObjectName("logoutBtn");

        // Category filter
        QComboBox* catCombo = new QComboBox;
        catCombo->addItems({"All", "Electronics", "Clothing", "Books"});

        connect(searchBtn, &QPushButton::clicked, this, &CustomerDashboard::onSearch);
        connect(m_searchEdit, &QLineEdit::returnPressed, this, &CustomerDashboard::onSearch);
        connect(clearBtn, &QPushButton::clicked, this, &CustomerDashboard::onClearSearch);
        connect(catCombo, &QComboBox::currentTextChanged, this, [=](const QString& cat) {
            if (cat == "All") onClearSearch();
            else refreshProducts(m_platform->getProductsByCategory(cat));
        });

        searchRow->addWidget(m_searchEdit);
        searchRow->addWidget(searchBtn);
        searchRow->addWidget(clearBtn);
        searchRow->addWidget(new QLabel("Category:"));
        searchRow->addWidget(catCombo);

        lay->addLayout(searchRow);

        m_productTable = new QTableWidget(0, 7);
        m_productTable->setHorizontalHeaderLabels({"ID", "Name", "Brand", "Category", "Price (₹)", "Disc%", "Stock"});
        m_productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_productTable->setAlternatingRowColors(true);

        QHBoxLayout* btns = new QHBoxLayout;
        QPushButton* addCartBtn = new QPushButton("🛒 Add to Cart");
        addCartBtn->setObjectName("successBtn");
        connect(addCartBtn, &QPushButton::clicked, this, &CustomerDashboard::onAddToCart);
        btns->addWidget(addCartBtn);
        btns->addStretch();

        lay->addWidget(m_productTable);
        lay->addLayout(btns);
        tabs->addTab(tab, "📦 Shop");
    }

    // ── Cart ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);

        m_cartTable = new QTableWidget(0, 4);
        m_cartTable->setHorizontalHeaderLabels({"Product", "Unit Price (₹)", "Qty", "Subtotal (₹)"});
        m_cartTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_cartTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_cartTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_cartTable->setAlternatingRowColors(true);

        QHBoxLayout* btns = new QHBoxLayout;
        QPushButton* removeBtn   = new QPushButton("🗑 Remove");    removeBtn->setObjectName("dangerBtn");
        QPushButton* checkoutBtn = new QPushButton("💳 Checkout");  checkoutBtn->setObjectName("warnBtn");
        m_cartTotalLabel = new QLabel("Total: ₹0");
        m_cartTotalLabel->setObjectName("balanceLabel");

        connect(removeBtn,   &QPushButton::clicked, this, &CustomerDashboard::onRemoveFromCart);
        connect(checkoutBtn, &QPushButton::clicked, this, &CustomerDashboard::onCheckout);

        btns->addWidget(removeBtn);
        btns->addWidget(checkoutBtn);
        btns->addStretch();
        btns->addWidget(m_cartTotalLabel);

        lay->addWidget(m_cartTable);
        lay->addLayout(btns);
        tabs->addTab(tab, "🛒 My Cart");
    }

    // ── Orders ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);

        m_orderTable = new QTableWidget(0, 5);
        m_orderTable->setHorizontalHeaderLabels({"Order#", "Date", "Amount (₹)", "Payment", "Status"});
        m_orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_orderTable->setAlternatingRowColors(true);

        QHBoxLayout* btns = new QHBoxLayout;
        QPushButton* cancelBtn = new QPushButton("❌ Cancel Order");
        cancelBtn->setObjectName("dangerBtn");
        connect(cancelBtn, &QPushButton::clicked, this, &CustomerDashboard::onCancelOrder);
        btns->addWidget(cancelBtn);
        btns->addStretch();

        lay->addWidget(m_orderTable);
        lay->addLayout(btns);
        tabs->addTab(tab, "📋 My Orders");
    }

    // ── Profile ──
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(tab);
        m_profileLabel = new QLabel;
        m_profileLabel->setObjectName("profileLabel");
        m_profileLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        m_profileLabel->setWordWrap(true);
        lay->addWidget(m_profileLabel);
        lay->addStretch();
        tabs->addTab(tab, "👤 Profile");
    }
}

void CustomerDashboard::refresh() {
    // Notifications
    auto notifs = m_platform->getNotifications(m_customer->getId());
    if (!notifs.isEmpty())
        m_notifLabel->setText("🔔 " + QString::number(notifs.size()) + " new");
    else
        m_notifLabel->clear();

    refreshProducts();
    refreshCart();
    refreshOrders();
    refreshProfile();
}

void CustomerDashboard::refreshProducts(QVector<Product*> products) {
    if (products.isEmpty()) {
        for (auto& p : m_platform->getAllProducts())
            if (p->getIsActive()) products.append(p.get());
    }
    m_productTable->setRowCount(0);
    for (auto* p : products) {
        if (!p->getIsActive()) continue;
        int row = m_productTable->rowCount();
        m_productTable->insertRow(row);
        m_productTable->setItem(row, 0, new QTableWidgetItem(QString::number(p->getId())));
        m_productTable->setItem(row, 1, new QTableWidgetItem(p->getName()));
        m_productTable->setItem(row, 2, new QTableWidgetItem(p->getBrand()));
        m_productTable->setItem(row, 3, new QTableWidgetItem(p->getCategory()));
        m_productTable->setItem(row, 4, new QTableWidgetItem("₹" + QString::number((int)p->getDiscountedPrice())));
        QString discStr = p->getDiscount() > 0 ? QString::number((int)p->getDiscount()) + "%" : "-";
        auto* dItem = new QTableWidgetItem(discStr);
        if (p->getDiscount() > 0) dItem->setForeground(QColor("#a6e3a1"));
        m_productTable->setItem(row, 5, dItem);
        auto* sItem = new QTableWidgetItem(QString::number(p->getStock()));
        if (p->getStock() == 0) { sItem->setForeground(QColor("#f38ba8")); sItem->setText("Out"); }
        m_productTable->setItem(row, 6, sItem);
    }
}

void CustomerDashboard::refreshCart() {
    m_cartTable->setRowCount(0);
    for (auto& item : m_customer->getCart().getItems()) {
        int row = m_cartTable->rowCount();
        m_cartTable->insertRow(row);
        m_cartTable->setItem(row, 0, new QTableWidgetItem(item.productName));
        m_cartTable->setItem(row, 1, new QTableWidgetItem("₹" + QString::number((int)(item.price * (1 - item.discountPercent/100)))));
        m_cartTable->setItem(row, 2, new QTableWidgetItem(QString::number(item.quantity)));
        m_cartTable->setItem(row, 3, new QTableWidgetItem("₹" + QString::number((int)item.getTotal())));
    }
    double total = m_customer->getCart().getTotal();
    double delivery = m_customer->getCart().getDeliveryCharge();
    m_cartTotalLabel->setText(QString("Total: ₹%1  (Delivery: ₹%2)")
        .arg((int)total).arg((int)delivery));
}

void CustomerDashboard::refreshOrders() {
    m_orderTable->setRowCount(0);
    for (auto* o : m_platform->getOrdersByCustomer(m_customer->getId())) {
        int row = m_orderTable->rowCount();
        m_orderTable->insertRow(row);
        m_orderTable->setItem(row, 0, new QTableWidgetItem("#" + QString::number(o->getId())));
        m_orderTable->setItem(row, 1, new QTableWidgetItem(o->getOrderDate()));
        m_orderTable->setItem(row, 2, new QTableWidgetItem("₹" + QString::number((int)o->getTotalAmount())));
        m_orderTable->setItem(row, 3, new QTableWidgetItem(o->getPayment().methodString()));
        auto* s = new QTableWidgetItem(o->statusString());
        s->setForeground(o->getStatus() == OrderStatus::DELIVERED ? QColor("#a6e3a1") :
                         o->getStatus() == OrderStatus::CANCELLED  ? QColor("#f38ba8") : QColor("#fab387"));
        m_orderTable->setItem(row, 4, s);
    }
}

void CustomerDashboard::refreshProfile() {
    Address* addr = m_customer->getDefaultAddress();
    m_profileLabel->setText(
        QString("👤  <b>%1</b>   [%2 Member]<br><br>"
                "📧  %3<br>"
                "📱  %4<br>"
                "💰  Wallet: ₹%5<br>"
                "🏆  Points: %6<br>"
                "🛒  Orders: %7<br>"
                "📍  %8")
        .arg(m_customer->getName())
        .arg(m_customer->getMembershipTier())
        .arg(m_customer->getEmail())
        .arg(m_customer->getPhone())
        .arg((int)m_customer->getWalletBalance())
        .arg(m_customer->getRewardPoints())
        .arg(m_customer->getOrderIds().size())
        .arg(addr ? addr->toString() : "No address on file"));
}

void CustomerDashboard::onSearch() {
    QString q = m_searchEdit->text().trimmed();
    if (q.isEmpty()) { onClearSearch(); return; }
    refreshProducts(m_platform->searchProducts(q));
}

void CustomerDashboard::onClearSearch() {
    m_searchEdit->clear();
    refreshProducts();
}

void CustomerDashboard::onAddToCart() {
    int row = m_productTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Cart", "Select a product first."); return; }
    int pid = m_productTable->item(row, 0)->text().toInt();
    Product* p = m_platform->getProductById(pid);
    if (!p || p->getStock() == 0) {
        QMessageBox::warning(this, "Cart", "Product is out of stock."); return;
    }
    bool ok;
    int qty = QInputDialog::getInt(this, "Add to Cart",
        QString("How many '%1'? (Available: %2)").arg(p->getName()).arg(p->getStock()),
        1, 1, p->getStock(), 1, &ok);
    if (!ok) return;
    m_customer->getCart().addItem(p, qty);
    refreshCart();
    QMessageBox::information(this, "Cart", "✅ Added " + p->getName() + " × " + QString::number(qty));
}

void CustomerDashboard::onRemoveFromCart() {
    int row = m_cartTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Cart", "Select a cart item."); return; }
    int pid = m_customer->getCart().getItems()[row].productId;
    m_customer->getCart().removeItem(pid);
    refreshCart();
}

void CustomerDashboard::onCheckout() {
    if (m_customer->getCart().isEmpty()) {
        QMessageBox::information(this, "Checkout", "Your cart is empty!"); return;
    }

    Address* addr = m_customer->getDefaultAddress();
    if (!addr) {
        QMessageBox::warning(this, "Checkout",
            "No delivery address on file. Add one in your profile."); return;
    }

    QStringList methods = {"Cash on Delivery (COD)", "UPI", "Credit/Debit Card",
                           "Net Banking", "Wallet (₹" + QString::number((int)m_customer->getWalletBalance()) + ")"};
    bool ok;
    QString chosen = QInputDialog::getItem(this, "Payment Method",
        QString("Cart Total: ₹%1\nDelivery Address: %2\n\nPayment Method:")
            .arg((int)m_customer->getCart().getTotal())
            .arg(addr->toString()),
        methods, 0, false, &ok);
    if (!ok) return;

    PaymentMethod pm;
    if (chosen.startsWith("UPI"))          pm = PaymentMethod::UPI;
    else if (chosen.startsWith("Credit"))  pm = PaymentMethod::CARD;
    else if (chosen.startsWith("Net"))     pm = PaymentMethod::NETBANKING;
    else if (chosen.startsWith("Wallet"))  pm = PaymentMethod::WALLET;
    else                                   pm = PaymentMethod::COD;

    try {
        Order* order = m_platform->placeOrder(m_customer, *addr, pm);
        refresh();
        QMessageBox::information(this, "Order Placed! 🎉",
            QString("Order #%1 placed successfully!\n"
                    "Total: ₹%2\n"
                    "Tracking ID: %3\n"
                    "Payment: %4")
            .arg(order->getId())
            .arg((int)order->getTotalAmount())
            .arg(order->getTrackingId())
            .arg(order->getPayment().methodString()));
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Order Failed", QString::fromStdString(e.what()));
    }
}

void CustomerDashboard::onCancelOrder() {
    int row = m_orderTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Orders", "Select an order."); return; }
    int oid = m_orderTable->item(row, 0)->text().remove('#').toInt();
    if (QMessageBox::question(this, "Cancel Order",
            "Cancel order #" + QString::number(oid) + "?") != QMessageBox::Yes) return;
    if (m_platform->cancelOrder(oid, m_customer->getId())) {
        refreshOrders();
        refreshProducts();
        QMessageBox::information(this, "Cancelled", "Order cancelled successfully.");
    } else {
        QMessageBox::warning(this, "Cannot Cancel", "Order cannot be cancelled (already shipped/delivered).");
    }
}
