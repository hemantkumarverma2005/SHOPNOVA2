#include "customerdashboard.h"
#include "ui_customerdashboard.h"
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
#include <QFormLayout>
#include <QLineEdit>
#include "flowlayout.h"

// ── Helper: create a stat card widget ─────────────────────────
static QFrame* makeStatCard(const QString& icon, const QString& value,
                            const QString& label, const QString& accent)
{
    QFrame* card = new QFrame;
    card->setMinimumHeight(120);
    card->setStyleSheet(QString(
        "QFrame { background: #1a1a2e; border: 1px solid #252540;"
        "border-radius: 16px; }"));
    QVBoxLayout* cl = new QVBoxLayout(card);
    cl->setContentsMargins(20, 18, 20, 18);
    cl->setSpacing(8);

    QLabel* ic = new QLabel(icon);
    ic->setFixedSize(40, 40);
    ic->setAlignment(Qt::AlignCenter);
    ic->setStyleSheet(QString("background: %1; border-radius: 12px; font-size: 20px; border: none;").arg(accent));
    QLabel* ttl = new QLabel(label);
    ttl->setStyleSheet("font-size: 12px; color: #8888a8; font-weight: 600; border: none; background: transparent;");
    QLabel* val = new QLabel(value);
    val->setObjectName("cardValue");
    val->setStyleSheet("font-size: 28px; font-weight: 800; color: #e0e0f0; border: none; background: transparent;");

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

// ── Helper: quick-link card for home page ─────────────────────
static QPushButton* makeQuickLink(const QString& icon, const QString& label) {
    QPushButton* btn = new QPushButton(icon + "\n" + label);
    btn->setObjectName("actionCard");
    btn->setMinimumSize(140, 90);
    btn->setCursor(Qt::PointingHandCursor);
    return btn;
}

CustomerDashboard::CustomerDashboard(Customer* customer, Platform* platform, QWidget* parent)
    : QWidget(parent), ui(new Ui::CustomerDashboard), m_customer(customer), m_platform(platform)
{
    ui->setupUi(this);

    connect(ui->shell, &ShellWidget::logoutRequested, this, &CustomerDashboard::logoutRequested);

    // Setup Stat Cards for Home Page
    ui->c_statsRow->addWidget(makeStatCard(QString::fromUtf8("💰"), QString::fromUtf8("₹0"), "Wallet Balance", "rgba(6,214,160,0.20)"));
    ui->c_statsRow->addWidget(makeStatCard(QString::fromUtf8("🏆"), "0",  "Reward Points",  "rgba(255,209,102,0.20)"));
    ui->c_statsRow->addWidget(makeStatCard(QString::fromUtf8("📦"), "0",  "Total Orders",   "rgba(76,201,240,0.20)"));
    ui->c_statsRow->addWidget(makeStatCard(QString::fromUtf8("⭐"), "-",  "Membership",     "rgba(123,94,167,0.20)"));

    ui->greetLbl->setText(QString::fromUtf8("👋  Welcome back, %1").arg(m_customer->getName()));

    // Add quick-link tiles on home page (after notification label, before spacer)
    QHBoxLayout* quickLinks = new QHBoxLayout;
    quickLinks->setSpacing(14);

    QPushButton* qlShop   = makeQuickLink(QString::fromUtf8("🛍️"), "Shop Now");
    QPushButton* qlCart   = makeQuickLink(QString::fromUtf8("🛒"), "My Cart");
    QPushButton* qlOrders = makeQuickLink(QString::fromUtf8("📦"), "Track Orders");
    QPushButton* qlProfile = makeQuickLink(QString::fromUtf8("👤"), "Profile");

    quickLinks->addWidget(qlShop);
    quickLinks->addWidget(qlCart);
    quickLinks->addWidget(qlOrders);
    quickLinks->addWidget(qlProfile);
    quickLinks->addStretch();

    // Insert after notifLabel (index 2 in homeLay)
    ui->homeLay->insertLayout(3, quickLinks);

    // Setup Shop Page Combo and layout
    ui->catCombo->addItems({"All", "Electronics", "Clothing", "Books"});
    new FlowLayout(ui->productsContainer, 0, 20, 20); // Add FlowLayout to the container
    
    // Setup tables
    ui->cartTable->setColumnCount(4);
    ui->cartTable->setHorizontalHeaderLabels({QString::fromUtf8("Product"), QString::fromUtf8("Unit Price (₹)"), "Qty", QString::fromUtf8("Subtotal (₹)")});
    ui->cartTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->cartTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->cartTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->cartTable->setAlternatingRowColors(true);
    ui->cartTable->verticalHeader()->setVisible(false);
    ui->cartTable->setShowGrid(false);

    ui->orderTable->setColumnCount(5);
    ui->orderTable->setHorizontalHeaderLabels({"Order#", "Date", QString::fromUtf8("Amount (₹)"), "Payment", "Status"});
    ui->orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->orderTable->setAlternatingRowColors(true);
    ui->orderTable->verticalHeader()->setVisible(false);
    ui->orderTable->setShowGrid(false);

    // Scroll areas for Navigation wrap
    QScrollArea *h_scroll = new QScrollArea; h_scroll->setWidgetResizable(true); h_scroll->setWidget(ui->homePage); h_scroll->setFrameShape(QFrame::NoFrame);
    QScrollArea *s_scroll = new QScrollArea; s_scroll->setWidgetResizable(true); s_scroll->setWidget(ui->shopPage); s_scroll->setFrameShape(QFrame::NoFrame);
    QScrollArea *c_scroll = new QScrollArea; c_scroll->setWidgetResizable(true); c_scroll->setWidget(ui->cartPage); c_scroll->setFrameShape(QFrame::NoFrame);
    QScrollArea *o_scroll = new QScrollArea; o_scroll->setWidgetResizable(true); o_scroll->setWidget(ui->ordersPage); o_scroll->setFrameShape(QFrame::NoFrame);
    QScrollArea *p_scroll = new QScrollArea; p_scroll->setWidgetResizable(true); p_scroll->setWidget(ui->profilePage); p_scroll->setFrameShape(QFrame::NoFrame);

    connect(ui->searchBtn, &QPushButton::clicked, this, &CustomerDashboard::onSearch);
    connect(ui->searchEdit, &QLineEdit::returnPressed, this, &CustomerDashboard::onSearch);
    connect(ui->clearBtn, &QPushButton::clicked, this, &CustomerDashboard::onClearSearch);
    connect(ui->catCombo, &QComboBox::currentTextChanged, this, [=](const QString& cat) {
        if (cat == "All") onClearSearch();
        else refreshProducts(m_platform->getProductsByCategory(cat));
    });
    connect(ui->removeBtn,   &QPushButton::clicked, this, &CustomerDashboard::onRemoveFromCart);
    connect(ui->checkoutBtn, &QPushButton::clicked, this, &CustomerDashboard::onCheckout);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &CustomerDashboard::onCancelOrder);

    // ── Dual-browse: Shop-by-Store nav ───────────────────────────
    m_shopNav = new ShopNavController(m_platform, m_customer, this);
    // When checkout completes inside the nav, refresh orders/cart
    connect(m_shopNav, &ShopNavController::checkoutRequested,
            this, &CustomerDashboard::refresh);

    // Build nav structure with section headers
    QVector<NavItem> nav = {
        {"",                         "Main",          nullptr},
        {QString::fromUtf8("🏠"),    "Home",          h_scroll},
        {QString::fromUtf8("🏪"),    "Browse Shops",  m_shopNav},
        {QString::fromUtf8("🛍️"),   "All Products",  s_scroll},
        {QString::fromUtf8("🛒"),    "Cart",          c_scroll},
        {"",                         "Account",       nullptr},
        {QString::fromUtf8("📦"),    "My Orders",     o_scroll},
        {QString::fromUtf8("👤"),    "Profile",       p_scroll},
    };

    ui->shell->configure("ShopNova", m_customer->getName(), "Customer", nav);
}

CustomerDashboard::~CustomerDashboard() {
    delete ui;
}

// ─── Refresh logic ───────────────────────────────────────────────────────────

void CustomerDashboard::refresh() {
    // Notifications
    auto notifs = m_platform->getNotifications(m_customer->getId());
    if (!notifs.isEmpty())
        ui->notifLabel->setText(QString::fromUtf8("🔔 ") + QString::number(notifs.size()) + " new notifications");
    else
        ui->notifLabel->clear();

    // Update stat cards
    auto updateCard = [](QWidget* card, const QString& val) {
        if (!card) return;
        QLabel* valLbl = card->findChild<QLabel*>("cardValue");
        if (valLbl) valLbl->setText(val);
    };
    updateCard(ui->c_statsRow->itemAt(0)->widget(), QString::fromUtf8("₹") + QString::number((int)m_customer->getWalletBalance()));
    updateCard(ui->c_statsRow->itemAt(1)->widget(), QString::number(m_customer->getRewardPoints()));
    updateCard(ui->c_statsRow->itemAt(2)->widget(), QString::number(m_customer->getOrderIds().size()));
    updateCard(ui->c_statsRow->itemAt(3)->widget(), m_customer->getMembershipTier());

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
    
    // Clear the existing cards
    QLayoutItem *child;
    while ((child = ui->productsContainer->layout()->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    for (auto* p : products) {
        if (!p->getIsActive()) continue;

        int currentQty = 0;
        for (const CartItem& ci : m_customer->getCart().getItems()) {
            if (ci.productId == p->getId()) { currentQty = ci.quantity; break; }
        }

        ItemCard* card = new ItemCard(p, currentQty, ui->productsContainer);
        connect(card, &ItemCard::quantityChanged,
                this, &CustomerDashboard::onItemCardQtyChanged);
        
        ui->productsContainer->layout()->addWidget(card);
    }
}

void CustomerDashboard::refreshCart() {
    ui->cartTable->setRowCount(0);
    for (auto& item : m_customer->getCart().getItems()) {
        int row = ui->cartTable->rowCount();
        ui->cartTable->insertRow(row);
        ui->cartTable->setItem(row, 0, new QTableWidgetItem(item.productName));
        ui->cartTable->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)(item.price * (1 - item.discountPercent/100)))));
        ui->cartTable->setItem(row, 2, new QTableWidgetItem(QString::number(item.quantity)));
        auto* totalItem = new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)item.getTotal()));
        totalItem->setForeground(QColor("#ff9a5c"));
        ui->cartTable->setItem(row, 3, totalItem);
        ui->cartTable->setRowHeight(row, 38);
    }
    double total = m_customer->getCart().getTotal();
    double delivery = m_customer->getCart().getDeliveryCharge();
    ui->cartTotalLabel->setText(QString(QString::fromUtf8("Total: ₹%1  (Delivery: ₹%2)"))
        .arg((int)total).arg((int)delivery));
}

void CustomerDashboard::refreshOrders() {
    ui->orderTable->setRowCount(0);
    for (auto* o : m_platform->getOrdersByCustomer(m_customer->getId())) {
        int row = ui->orderTable->rowCount();
        ui->orderTable->insertRow(row);
        ui->orderTable->setItem(row, 0, new QTableWidgetItem("#" + QString::number(o->getId())));
        ui->orderTable->setItem(row, 1, new QTableWidgetItem(o->getOrderDate()));
        auto* amtItem = new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)o->getTotalAmount()));
        amtItem->setForeground(QColor("#ff9a5c"));
        ui->orderTable->setItem(row, 2, amtItem);
        ui->orderTable->setItem(row, 3, new QTableWidgetItem(o->getPayment().methodString()));

        // Status pill
        ui->orderTable->setCellWidget(row, 4, makeOrderStatusPill(o->getStatus(), o->statusString()));
        ui->orderTable->setRowHeight(row, 40);
    }
}

void CustomerDashboard::refreshProfile() {
    Address* addr = m_customer->getDefaultAddress();
    ui->profileLabel->setText(
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

// ─── Slot implementations ────────────────────────────────────────────────────

void CustomerDashboard::onSearch() {
    QString q = ui->searchEdit->text().trimmed();
    if (q.isEmpty()) { onClearSearch(); return; }
    refreshProducts(m_platform->searchProducts(q));
}

void CustomerDashboard::onClearSearch() {
    ui->searchEdit->clear();
    refreshProducts();
}

void CustomerDashboard::onItemCardQtyChanged(Product* product, int qty) {
    if (!product || !m_customer) return;
    
    Cart& cart = m_customer->getCart();
    if (qty == 0) {
        cart.removeItem(product->getId());
    } else {
        bool found = false;
        for (CartItem& ci : const_cast<QVector<CartItem>&>(cart.getItems())) {
            if (ci.productId == product->getId()) {
                ci.quantity = qty;
                found = true;
                break;
            }
        }
        if (!found) cart.addItem(product, qty);
    }
    refreshCart();
    
    // Quick hack to force ShopBrowserWidget's badge to sync if possible
    if (m_shopNav) {
        // Just trigger a resize event or something? Or maybe we don't need to tightly couple it,
        // Since if the user clicks "Browse Shops" it might read the customer cart again.
        // Actually, ShopNavController reads the Cart size on back/forward.
    }
}

void CustomerDashboard::onRemoveFromCart() {
    int row = ui->cartTable->currentRow();
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
        QDialog addrDlg(this);
        addrDlg.setWindowTitle("Add Delivery Address");
        addrDlg.setFixedWidth(400);
        addrDlg.setStyleSheet("QDialog { background: #1a1a2e; color: white; border-radius: 16px; }"
                              "QLabel { color: #9590b8; font-weight: 600; }"
                              "QLineEdit { background: #161630; color: white; border: 1px solid #2a2a42; border-radius: 8px; padding: 6px; }");
        QFormLayout* form = new QFormLayout(&addrDlg);
        
        QLineEdit* streetEdit = new QLineEdit(&addrDlg);
        QLineEdit* cityEdit = new QLineEdit(&addrDlg);
        QLineEdit* stateEdit = new QLineEdit(&addrDlg);
        QLineEdit* pinEdit = new QLineEdit(&addrDlg);
        QLineEdit* countryEdit = new QLineEdit(&addrDlg);
        
        form->addRow("Street:", streetEdit);
        form->addRow("City:", cityEdit);
        form->addRow("State:", stateEdit);
        form->addRow("Pincode:", pinEdit);
        form->addRow("Country:", countryEdit);
        
        QPushButton* saveBtn = new QPushButton("Save Address", &addrDlg);
        saveBtn->setStyleSheet("background: #ff6b2b; color: white; padding: 10px; border-radius: 8px; font-weight: bold;");
        form->addRow(saveBtn);
        
        connect(saveBtn, &QPushButton::clicked, &addrDlg, &QDialog::accept);
        
        if (addrDlg.exec() == QDialog::Accepted) {
            QString street = streetEdit->text().trimmed();
            QString city = cityEdit->text().trimmed();
            if (street.isEmpty() || city.isEmpty()) {
                QMessageBox::warning(this, "Error", "Street and City are required!");
                return;
            }
            Address newAddr{street, city, stateEdit->text().trimmed(), pinEdit->text().trimmed(), countryEdit->text().trimmed()};
            m_customer->addAddress(newAddr);
            m_platform->saveToDisk();
            addr = m_customer->getDefaultAddress();
        } else {
            return;
        }
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
    int row = ui->orderTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Orders", "Select an order."); return; }
    int oid = ui->orderTable->item(row, 0)->text().remove('#').toInt();
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
