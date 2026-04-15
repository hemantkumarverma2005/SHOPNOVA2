#include "customerdashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
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

CustomerDashboard::CustomerDashboard(Customer* customer, Platform* platform, QWidget* parent)
    : QWidget(parent), m_customer(customer), m_platform(platform)
{
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    m_shell = new ShellWidget(this);
    root->addWidget(m_shell);

    connect(m_shell, &ShellWidget::logoutRequested,
            this, &CustomerDashboard::logoutRequested);

    // Build nav structure with section headers
    QVector<NavItem> nav = {
        {"",                         "Main",      nullptr},
        {QString::fromUtf8("🏠"),    "Home",      buildHomePage()},
        {QString::fromUtf8("🛍️"),   "Shop",      buildShopPage()},
        {QString::fromUtf8("🛒"),    "Cart",      buildCartPage()},
        {"",                         "Account",   nullptr},
        {QString::fromUtf8("📦"),    "My Orders", buildOrdersPage()},
        {QString::fromUtf8("👤"),    "Profile",   buildProfilePage()},
    };

    m_shell->configure("ShopNova", m_customer->getName(), "Customer", nav);
}

// ─── Page builders ───────────────────────────────────────────────────────────

QWidget *CustomerDashboard::buildHomePage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);

    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(24);

    QLabel *greet = new QLabel(QString::fromUtf8("Welcome back, %1 👋").arg(m_customer->getName()));
    greet->setStyleSheet("font-size: 26px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(greet);

    // Stat cards row
    QHBoxLayout *statsRow = new QHBoxLayout;
    statsRow->setSpacing(16);

    m_statCards[0] = makeStatCard(QString::fromUtf8("💰"), QString::fromUtf8("₹0"), "Wallet Balance", "rgba(6,214,160,0.15)");
    m_statCards[1] = makeStatCard(QString::fromUtf8("🏆"), "0",  "Reward Points",  "rgba(255,209,102,0.15)");
    m_statCards[2] = makeStatCard(QString::fromUtf8("📦"), "0",  "Total Orders",   "rgba(76,201,240,0.15)");
    m_statCards[3] = makeStatCard(QString::fromUtf8("⭐"), "-",  "Membership",     "rgba(123,94,167,0.15)");

    for (int i = 0; i < 4; ++i)
        statsRow->addWidget(m_statCards[i]);
    lay->addLayout(statsRow);

    // Notification area
    m_notifLabel = new QLabel;
    m_notifLabel->setObjectName("notifLabel");
    m_notifLabel->setStyleSheet("font-size: 14px; font-weight: 600; color: #ffd166;");
    lay->addWidget(m_notifLabel);

    // Recent activity label
    QLabel *recent = new QLabel("Recent Activity");
    recent->setStyleSheet("font-size: 18px; font-weight: 700; color: #e0e0f0; margin-top: 8px;");
    lay->addWidget(recent);

    QLabel *hint = new QLabel("Browse the Shop tab to explore products, add items to your cart, and place orders.");
    hint->setStyleSheet("font-size: 13px; color: #5a5a70;");
    hint->setWordWrap(true);
    lay->addWidget(hint);

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget *CustomerDashboard::buildShopPage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);
    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(20);

    QLabel *h = new QLabel("Browse Products");
    h->setStyleSheet("font-size: 24px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(h);

    // Search bar
    QHBoxLayout* searchRow = new QHBoxLayout;
    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText(QString::fromUtf8("🔍  Search products, brands, categories..."));
    m_searchEdit->setMinimumHeight(40);
    m_searchEdit->setStyleSheet(
        "QLineEdit { border-radius: 20px; padding-left: 18px; font-size: 14px; }");

    QPushButton* searchBtn  = new QPushButton("Search");  searchBtn->setObjectName("primaryBtn");
    QPushButton* clearBtn   = new QPushButton("All");     clearBtn->setObjectName("logoutBtn");
    searchBtn->setMinimumHeight(38);
    clearBtn->setMinimumHeight(38);

    QComboBox* catCombo = new QComboBox;
    catCombo->addItems({"All", "Electronics", "Clothing", "Books"});

    connect(searchBtn, &QPushButton::clicked, this, &CustomerDashboard::onSearch);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &CustomerDashboard::onSearch);
    connect(clearBtn, &QPushButton::clicked, this, &CustomerDashboard::onClearSearch);
    connect(catCombo, &QComboBox::currentTextChanged, this, [=](const QString& cat) {
        if (cat == "All") onClearSearch();
        else refreshProducts(m_platform->getProductsByCategory(cat));
    });

    QLabel* catLabel = new QLabel("Category:");
    catLabel->setStyleSheet("color: #9494a8; font-size: 13px;");

    searchRow->addWidget(m_searchEdit, 1);
    searchRow->addWidget(searchBtn);
    searchRow->addWidget(clearBtn);
    searchRow->addSpacing(8);
    searchRow->addWidget(catLabel);
    searchRow->addWidget(catCombo);
    lay->addLayout(searchRow);

    m_productTable = new QTableWidget(0, 7);
    m_productTable->setHorizontalHeaderLabels({QString::fromUtf8("ID"), "Name", "Brand", "Category",
                                               QString::fromUtf8("Price (₹)"), "Disc%", "Stock"});
    m_productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_productTable->setAlternatingRowColors(true);
    m_productTable->verticalHeader()->setVisible(false);
    m_productTable->setShowGrid(false);
    m_productTable->setMinimumHeight(400);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton* addCartBtn = new QPushButton(QString::fromUtf8("🛒 Add to Cart"));
    addCartBtn->setObjectName("successBtn");
    addCartBtn->setMinimumHeight(38);
    connect(addCartBtn, &QPushButton::clicked, this, &CustomerDashboard::onAddToCart);
    btns->addWidget(addCartBtn);
    btns->addStretch();

    lay->addWidget(m_productTable);
    lay->addLayout(btns);
    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget *CustomerDashboard::buildCartPage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);
    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(20);

    QLabel *h = new QLabel(QString::fromUtf8("🛒  My Cart"));
    h->setStyleSheet("font-size: 24px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(h);

    m_cartTable = new QTableWidget(0, 4);
    m_cartTable->setHorizontalHeaderLabels({QString::fromUtf8("Product"),
                                            QString::fromUtf8("Unit Price (₹)"), "Qty",
                                            QString::fromUtf8("Subtotal (₹)")});
    m_cartTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_cartTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_cartTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_cartTable->setAlternatingRowColors(true);
    m_cartTable->verticalHeader()->setVisible(false);
    m_cartTable->setShowGrid(false);
    m_cartTable->setMinimumHeight(300);
    lay->addWidget(m_cartTable);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton* removeBtn   = new QPushButton(QString::fromUtf8("🗑 Remove"));
    removeBtn->setObjectName("dangerBtn");
    QPushButton* checkoutBtn = new QPushButton(QString::fromUtf8("💳 Checkout"));
    checkoutBtn->setObjectName("warnBtn");
    removeBtn->setMinimumHeight(38);
    checkoutBtn->setMinimumHeight(38);
    m_cartTotalLabel = new QLabel(QString::fromUtf8("Total: ₹0"));
    m_cartTotalLabel->setObjectName("balanceLabel");
    m_cartTotalLabel->setStyleSheet("font-size: 16px; font-weight: 700; color: #06d6a0;");

    connect(removeBtn,   &QPushButton::clicked, this, &CustomerDashboard::onRemoveFromCart);
    connect(checkoutBtn, &QPushButton::clicked, this, &CustomerDashboard::onCheckout);

    btns->addWidget(removeBtn);
    btns->addWidget(checkoutBtn);
    btns->addStretch();
    btns->addWidget(m_cartTotalLabel);
    lay->addLayout(btns);

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget *CustomerDashboard::buildOrdersPage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);
    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(20);

    QLabel *h = new QLabel(QString::fromUtf8("📦  My Orders"));
    h->setStyleSheet("font-size: 24px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(h);

    m_orderTable = new QTableWidget(0, 5);
    m_orderTable->setHorizontalHeaderLabels({"Order#", "Date", QString::fromUtf8("Amount (₹)"), "Payment", "Status"});
    m_orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_orderTable->setAlternatingRowColors(true);
    m_orderTable->verticalHeader()->setVisible(false);
    m_orderTable->setShowGrid(false);
    m_orderTable->setMinimumHeight(400);
    lay->addWidget(m_orderTable);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton* cancelBtn = new QPushButton(QString::fromUtf8("❌ Cancel Order"));
    cancelBtn->setObjectName("dangerBtn");
    cancelBtn->setMinimumHeight(38);
    connect(cancelBtn, &QPushButton::clicked, this, &CustomerDashboard::onCancelOrder);
    btns->addWidget(cancelBtn);
    btns->addStretch();
    lay->addLayout(btns);

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget *CustomerDashboard::buildProfilePage() {
    QScrollArea *scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);
    QWidget *page = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 32, 32, 32);
    lay->setSpacing(16);

    QLabel *h = new QLabel(QString::fromUtf8("👤  My Profile"));
    h->setStyleSheet("font-size: 24px; font-weight: 800; color: #e0e0f0;");
    lay->addWidget(h);

    m_profileLabel = new QLabel;
    m_profileLabel->setObjectName("profileLabel");
    m_profileLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_profileLabel->setWordWrap(true);
    m_profileLabel->setStyleSheet(
        "font-size: 14px; color: #f0f0f5; padding: 24px;"
        "background-color: #111118; border: 1px solid rgba(255,255,255,0.07);"
        "border-radius: 16px; line-height: 1.8;");
    lay->addWidget(m_profileLabel);

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

// ─── Refresh logic (preserved from original) ─────────────────────────────────

void CustomerDashboard::refresh() {
    // Notifications
    auto notifs = m_platform->getNotifications(m_customer->getId());
    if (!notifs.isEmpty())
        m_notifLabel->setText(QString::fromUtf8("🔔 ") + QString::number(notifs.size()) + " new");
    else
        m_notifLabel->clear();

    // Update stat cards
    auto updateCard = [](QFrame* card, const QString& val) {
        QLabel* valLbl = card->findChild<QLabel*>("cardValue");
        if (valLbl) valLbl->setText(val);
    };
    updateCard(m_statCards[0], QString::fromUtf8("₹") + QString::number((int)m_customer->getWalletBalance()));
    updateCard(m_statCards[1], QString::number(m_customer->getRewardPoints()));
    updateCard(m_statCards[2], QString::number(m_customer->getOrderIds().size()));
    updateCard(m_statCards[3], m_customer->getMembershipTier());

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
        m_productTable->setItem(row, 4, new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)p->getDiscountedPrice())));

        // Discount column with accent color
        QString discStr = p->getDiscount() > 0 ? QString::number((int)p->getDiscount()) + "%" : "-";
        auto* dItem = new QTableWidgetItem(discStr);
        if (p->getDiscount() > 0) dItem->setForeground(QColor("#06d6a0"));
        m_productTable->setItem(row, 5, dItem);

        // Stock with color coding
        auto* sItem = new QTableWidgetItem(QString::number(p->getStock()));
        if (p->getStock() == 0) { sItem->setForeground(QColor("#ff4444")); sItem->setText("Out"); }
        else if (p->getStock() <= 5) { sItem->setForeground(QColor("#ffd166")); }
        m_productTable->setItem(row, 6, sItem);
        m_productTable->setRowHeight(row, 38);
    }
}

void CustomerDashboard::refreshCart() {
    m_cartTable->setRowCount(0);
    for (auto& item : m_customer->getCart().getItems()) {
        int row = m_cartTable->rowCount();
        m_cartTable->insertRow(row);
        m_cartTable->setItem(row, 0, new QTableWidgetItem(item.productName));
        m_cartTable->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)(item.price * (1 - item.discountPercent/100)))));
        m_cartTable->setItem(row, 2, new QTableWidgetItem(QString::number(item.quantity)));
        auto* totalItem = new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)item.getTotal()));
        totalItem->setForeground(QColor("#ff9a5c"));
        m_cartTable->setItem(row, 3, totalItem);
        m_cartTable->setRowHeight(row, 38);
    }
    double total = m_customer->getCart().getTotal();
    double delivery = m_customer->getCart().getDeliveryCharge();
    m_cartTotalLabel->setText(QString(QString::fromUtf8("Total: ₹%1  (Delivery: ₹%2)"))
        .arg((int)total).arg((int)delivery));
}

void CustomerDashboard::refreshOrders() {
    m_orderTable->setRowCount(0);
    for (auto* o : m_platform->getOrdersByCustomer(m_customer->getId())) {
        int row = m_orderTable->rowCount();
        m_orderTable->insertRow(row);
        m_orderTable->setItem(row, 0, new QTableWidgetItem("#" + QString::number(o->getId())));
        m_orderTable->setItem(row, 1, new QTableWidgetItem(o->getOrderDate()));
        auto* amtItem = new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)o->getTotalAmount()));
        amtItem->setForeground(QColor("#ff9a5c"));
        m_orderTable->setItem(row, 2, amtItem);
        m_orderTable->setItem(row, 3, new QTableWidgetItem(o->getPayment().methodString()));

        // Status pill
        m_orderTable->setCellWidget(row, 4, makeOrderStatusPill(o->getStatus(), o->statusString()));
        m_orderTable->setRowHeight(row, 40);
    }
}

void CustomerDashboard::refreshProfile() {
    Address* addr = m_customer->getDefaultAddress();
    m_profileLabel->setText(
        QString("<div style='line-height: 2.2;'>"
                "<span style='font-size: 22px; font-weight: 800; color: #f0f0f5;'>👤  %1</span>"
                "<br>"
                "<span style='background: rgba(76,201,240,0.15); color: #4cc9f0; "
                "border-radius: 12px; padding: 3px 10px; font-size: 11px; font-weight: 700;'>"
                "%2 Member</span>"
                "<br><br>"
                "<span style='color: #5a5a70; font-size: 12px; text-transform: uppercase; "
                "letter-spacing: 1px;'>CONTACT</span><br>"
                "📧  <span style='color: #9494a8;'>%3</span><br>"
                "📱  <span style='color: #9494a8;'>%4</span>"
                "<br><br>"
                "<span style='color: #5a5a70; font-size: 12px; text-transform: uppercase; "
                "letter-spacing: 1px;'>ACCOUNT</span><br>"
                "💰  Wallet: <span style='color: #06d6a0; font-weight: 700;'>₹%5</span><br>"
                "🏆  Points: <span style='color: #ffd166; font-weight: 700;'>%6</span><br>"
                "🛒  Orders: <span style='color: #4cc9f0; font-weight: 700;'>%7</span>"
                "<br><br>"
                "<span style='color: #5a5a70; font-size: 12px; text-transform: uppercase; "
                "letter-spacing: 1px;'>ADDRESS</span><br>"
                "📍  <span style='color: #9494a8;'>%8</span>"
                "</div>")
        .arg(m_customer->getName())
        .arg(m_customer->getMembershipTier())
        .arg(m_customer->getEmail())
        .arg(m_customer->getPhone())
        .arg((int)m_customer->getWalletBalance())
        .arg(m_customer->getRewardPoints())
        .arg(m_customer->getOrderIds().size())
        .arg(addr ? addr->toString() : "No address on file"));
}

// ─── Slot implementations (preserved from original) ──────────────────────────

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
    QMessageBox::information(this, "Cart", QString::fromUtf8("✅ Added ") + p->getName() + QString::fromUtf8(" × ") + QString::number(qty));
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
                           "Net Banking", QString::fromUtf8("Wallet (₹") + QString::number((int)m_customer->getWalletBalance()) + ")"};
    bool ok;
    QString chosen = QInputDialog::getItem(this, "Payment Method",
        QString(QString::fromUtf8("Cart Total: ₹%1\nDelivery Address: %2\n\nPayment Method:"))
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
        QMessageBox::information(this, QString::fromUtf8("Order Placed! 🎉"),
            QString(QString::fromUtf8("Order #%1 placed successfully!\n"
                    "Total: ₹%2\n"
                    "Tracking ID: %3\n"
                    "Payment: %4"))
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
