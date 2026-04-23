#include "admindashboard.h"
#include "ui_admindashboard.h"
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
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QPixmap>

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

// ── Helper: create a quick-action tile ────────────────────────
static QPushButton* makeActionTile(const QString& icon, const QString& label) {
    QPushButton* btn = new QPushButton(icon + "\n" + label);
    btn->setObjectName("actionCard");
    btn->setMinimumSize(140, 90);
    btn->setCursor(Qt::PointingHandCursor);
    return btn;
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
    : QWidget(parent), ui(new Ui::AdminDashboard), m_admin(admin), m_platform(platform)
{
    ui->setupUi(this);

    connect(ui->shell, &ShellWidget::logoutRequested, this, &AdminDashboard::logoutRequested);
    ui->shell->setSearchVisible(false);  // Hide search in admin

    // Setup headers for tables
    ui->a_productTable->setColumnCount(7);
    ui->a_productTable->setHorizontalHeaderLabels({"ID", "Name", "Type", "Brand", "Price", "Stock", "Status"});
    ui->a_productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->a_productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->a_productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->a_productTable->setAlternatingRowColors(true);
    ui->a_productTable->verticalHeader()->setVisible(false);
    ui->a_productTable->setShowGrid(false);

    ui->a_orderTable->setColumnCount(6);
    ui->a_orderTable->setHorizontalHeaderLabels({"Order #", "Customer", "Items", "Amount", "Payment", "Status"});
    ui->a_orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->a_orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->a_orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->a_orderTable->setAlternatingRowColors(true);
    ui->a_orderTable->verticalHeader()->setVisible(false);
    ui->a_orderTable->setShowGrid(false);

    ui->a_sellerTable->setColumnCount(5);
    ui->a_sellerTable->setHorizontalHeaderLabels({"ID", "Name", "Store", "Email", "Status"});
    ui->a_sellerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->a_sellerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->a_sellerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->a_sellerTable->setAlternatingRowColors(true);
    ui->a_sellerTable->verticalHeader()->setVisible(false);
    ui->a_sellerTable->setShowGrid(false);

    ui->a_customerTable->setColumnCount(5);
    ui->a_customerTable->setHorizontalHeaderLabels({"ID", "Name", "Email", "Tier", "Status"});
    ui->a_customerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->a_customerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->a_customerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->a_customerTable->setAlternatingRowColors(true);
    ui->a_customerTable->verticalHeader()->setVisible(false);
    ui->a_customerTable->setShowGrid(false);

    // Setup Stat Cards
    ui->a_statsRow->addWidget(makeStatCard(QString::fromUtf8("📦"), "0", "Total Products", "rgba(255,107,43,0.20)"));
    ui->a_statsRow->addWidget(makeStatCard(QString::fromUtf8("🛒"), "0", "Total Orders",   "rgba(76,201,240,0.20)"));
    ui->a_statsRow->addWidget(makeStatCard(QString::fromUtf8("👥"), "0", "Customers",      "rgba(6,214,160,0.20)"));
    ui->a_statsRow->addWidget(makeStatCard(QString::fromUtf8("🏪"), "0", "Sellers",        "rgba(123,94,167,0.20)"));
    ui->a_statsRow->addWidget(makeStatCard(QString::fromUtf8("💰"), QString::fromUtf8("₹0"), "Revenue", "rgba(255,209,102,0.20)"));

    // Replace verbose greeting with a clean welcome banner
    ui->greetLbl->setText(QString::fromUtf8("👋  Hello, %1").arg(m_admin->getName()));
    ui->greetLbl->setStyleSheet("font-size: 22px; font-weight: 800; color: #f0f0f5; border: none; background: transparent;");

    // Add quick-action tiles below stats
    QHBoxLayout* actionsRow = new QHBoxLayout;
    actionsRow->setSpacing(14);

    QPushButton* tileAddProd = makeActionTile(QString::fromUtf8("➕"), "Add Product");
    QPushButton* tileOrders  = makeActionTile(QString::fromUtf8("📋"), "View Orders");
    QPushButton* tileSellers = makeActionTile(QString::fromUtf8("🏪"), "Manage Sellers");
    QPushButton* tileUsers   = makeActionTile(QString::fromUtf8("👥"), "Manage Users");

    actionsRow->addWidget(tileAddProd);
    actionsRow->addWidget(tileOrders);
    actionsRow->addWidget(tileSellers);
    actionsRow->addWidget(tileUsers);
    actionsRow->addStretch();

    // Insert actions row into dashboard layout after stats
    ui->dashLayout->addLayout(actionsRow);
    ui->dashLayout->addStretch();

    // Connect action tiles to navigate to respective pages
    connect(tileAddProd, &QPushButton::clicked, this, &AdminDashboard::onAddProduct);

    // ── Platform QR Card ──────────────────────────────────────────
    {
        QFrame* qrCard = new QFrame(ui->dashboardPage);
        qrCard->setStyleSheet(
            "QFrame { background: #1a1a2e; border: 1px solid #252540;"
            "border-radius: 16px; }");
        QVBoxLayout* qrVl = new QVBoxLayout(qrCard);
        qrVl->setContentsMargins(24, 20, 24, 20);
        qrVl->setSpacing(14);

        QLabel* qrTitle = new QLabel(
            QString::fromUtf8("🔲  Platform Payment QR"), qrCard);
        qrTitle->setStyleSheet(
            "font-size: 15px; font-weight: 700; color: #f0f0f5;"
            "background: transparent; border: none;");
        QLabel* qrSubtitle = new QLabel(
            "Shown at checkout when a customer orders from multiple shops.", qrCard);
        qrSubtitle->setWordWrap(true);
        qrSubtitle->setStyleSheet(
            "font-size: 12px; color: #7878a0; background: transparent; border: none;");

        m_adminQrDisplay = new QLabel(qrCard);
        m_adminQrDisplay->setFixedSize(180, 180);
        m_adminQrDisplay->setAlignment(Qt::AlignCenter);
        m_adminQrDisplay->setStyleSheet(
            "background: #252540; border-radius: 12px; border: none;");
        refreshAdminQrDisplay();

        m_adminQrStatus = new QLabel(qrCard);
        m_adminQrStatus->setWordWrap(true);
        m_adminQrStatus->setStyleSheet("background: transparent; border: none; font-size: 12px;");
        updateAdminQrStatus();

        QPushButton* setQrBtn = new QPushButton(
            QString::fromUtf8("📷  Set Platform QR"), qrCard);
        setQrBtn->setObjectName("successBtn");
        setQrBtn->setFixedHeight(40);
        setQrBtn->setCursor(Qt::PointingHandCursor);
        connect(setQrBtn, &QPushButton::clicked, this, &AdminDashboard::onSetAdminQr);

        QHBoxLayout* qrRow = new QHBoxLayout;
        QVBoxLayout* qrLeft = new QVBoxLayout;
        qrLeft->setSpacing(10);
        qrLeft->addWidget(m_adminQrDisplay);
        qrLeft->addWidget(setQrBtn);
        QVBoxLayout* qrRight = new QVBoxLayout;
        qrRight->setSpacing(8);
        qrRight->addWidget(qrTitle);
        qrRight->addWidget(qrSubtitle);
        qrRight->addWidget(m_adminQrStatus);
        qrRight->addStretch();
        qrRow->addLayout(qrLeft);
        qrRow->addSpacing(20);
        qrRow->addLayout(qrRight, 1);
        qrVl->addLayout(qrRow);

        ui->dashLayout->insertWidget(0, qrCard);
    }

    // Wrapper scroll areas
    QScrollArea *d_scroll = new QScrollArea; d_scroll->setWidgetResizable(true); d_scroll->setWidget(ui->dashboardPage); d_scroll->setFrameShape(QFrame::NoFrame);
    QScrollArea *p_scroll = new QScrollArea; p_scroll->setWidgetResizable(true); p_scroll->setWidget(ui->productsPage); p_scroll->setFrameShape(QFrame::NoFrame);
    QScrollArea *o_scroll = new QScrollArea; o_scroll->setWidgetResizable(true); o_scroll->setWidget(ui->ordersPage); o_scroll->setFrameShape(QFrame::NoFrame);
    QScrollArea *s_scroll = new QScrollArea; s_scroll->setWidgetResizable(true); s_scroll->setWidget(ui->sellersPage); s_scroll->setFrameShape(QFrame::NoFrame);
    QScrollArea *c_scroll = new QScrollArea; c_scroll->setWidgetResizable(true); c_scroll->setWidget(ui->customersPage); c_scroll->setFrameShape(QFrame::NoFrame);

    connect(ui->a_addProdBtn,    &QPushButton::clicked, this, &AdminDashboard::onAddProduct);
    connect(ui->a_toggleProdBtn, &QPushButton::clicked, this, &AdminDashboard::onToggleProduct);
    connect(ui->a_deleteProdBtn, &QPushButton::clicked, this, &AdminDashboard::onDeleteProduct);
    connect(ui->a_updateOrderBtn,&QPushButton::clicked, this, &AdminDashboard::onUpdateOrderStatus);
    connect(ui->a_verifySellerBtn,&QPushButton::clicked, this, &AdminDashboard::onVerifySeller);
    connect(ui->a_toggleUserBtn, &QPushButton::clicked, this, &AdminDashboard::onToggleUser);

    QVector<NavItem> nav = {
        {"",                         "Overview",     nullptr},
        {QString::fromUtf8("🏠"),    "Dashboard",    d_scroll},
        {"",                         "Management",   nullptr},
        {QString::fromUtf8("📦"),    "Products",     p_scroll},
        {QString::fromUtf8("📋"),    "All Orders",   o_scroll},
        {QString::fromUtf8("🏪"),    "Sellers",      s_scroll},
        {QString::fromUtf8("👥"),    "Customers",    c_scroll},
    };

    ui->shell->configure("ShopNova", m_admin->getName(), "Admin", nav);
}

AdminDashboard::~AdminDashboard() {
    delete ui;
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
    auto updateCard = [](QWidget* card, const QString& val) {
        if (!card) return;
        QLabel* valLbl = card->findChild<QLabel*>("cardValue");
        if (valLbl) valLbl->setText(val);
    };
    updateCard(ui->a_statsRow->itemAt(0)->widget(), QString::number(s.totalProducts));
    updateCard(ui->a_statsRow->itemAt(1)->widget(), QString::number(s.totalOrders));
    updateCard(ui->a_statsRow->itemAt(2)->widget(), QString::number(s.totalCustomers));
    updateCard(ui->a_statsRow->itemAt(3)->widget(), QString::number(s.totalSellers));
    updateCard(ui->a_statsRow->itemAt(4)->widget(), QString::fromUtf8("₹") + QString::number((int)s.totalRevenue));
}

void AdminDashboard::refreshProducts() {
    ui->a_productTable->setRowCount(0);
    for (auto& prod : m_platform->getAllProducts()) {
        int row = ui->a_productTable->rowCount();
        ui->a_productTable->insertRow(row);
        ui->a_productTable->setItem(row, 0, new QTableWidgetItem(QString::number(prod->getId())));
        ui->a_productTable->setItem(row, 1, new QTableWidgetItem(prod->getName()));
        ui->a_productTable->setItem(row, 2, new QTableWidgetItem(prod->getType()));
        ui->a_productTable->setItem(row, 3, new QTableWidgetItem(prod->getBrand()));
        ui->a_productTable->setItem(row, 4, new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)prod->getDiscountedPrice())));
        ui->a_productTable->setItem(row, 5, new QTableWidgetItem(QString::number(prod->getStock())));

        // Status pill
        QString statusText = prod->getIsActive() ? QString::fromUtf8("● Active") : QString::fromUtf8("● Inactive");
        QString objName = prod->getIsActive() ? "statusActive" : "statusInactive";
        ui->a_productTable->setCellWidget(row, 6, makeStatusPill(statusText, objName));
        ui->a_productTable->setRowHeight(row, 40);
    }
}

void AdminDashboard::refreshOrders() {
    ui->a_orderTable->setRowCount(0);
    for (auto& o : m_platform->getAllOrders()) {
        int row = ui->a_orderTable->rowCount();
        ui->a_orderTable->insertRow(row);
        ui->a_orderTable->setItem(row, 0, new QTableWidgetItem("#" + QString::number(o->getId())));
        ui->a_orderTable->setItem(row, 1, new QTableWidgetItem(o->getCustomerName()));
        ui->a_orderTable->setItem(row, 2, new QTableWidgetItem(QString::number(o->getItems().size()) + " item(s)"));
        ui->a_orderTable->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)o->getTotalAmount())));
        ui->a_orderTable->setItem(row, 4, new QTableWidgetItem(o->getPayment().methodString()));

        // Status pill
        ui->a_orderTable->setCellWidget(row, 5, makeOrderStatusPill(o->getStatus(), o->statusString()));
        ui->a_orderTable->setRowHeight(row, 40);
    }
}

void AdminDashboard::refreshSellers() {
    ui->a_sellerTable->setRowCount(0);
    for (auto& s : m_platform->getSellers()) {
        int row = ui->a_sellerTable->rowCount();
        ui->a_sellerTable->insertRow(row);
        ui->a_sellerTable->setItem(row, 0, new QTableWidgetItem(QString::number(s->getId())));
        ui->a_sellerTable->setItem(row, 1, new QTableWidgetItem(s->getName()));
        ui->a_sellerTable->setItem(row, 2, new QTableWidgetItem(s->getStoreName()));
        ui->a_sellerTable->setItem(row, 3, new QTableWidgetItem(s->getEmail()));

        // Status pill
        QString statusText = s->getIsVerified() ? QString::fromUtf8("✓ Verified") : QString::fromUtf8("⏳ Pending");
        QString objName = s->getIsVerified() ? "statusVerified" : "statusPending";
        ui->a_sellerTable->setCellWidget(row, 4, makeStatusPill(statusText, objName));
        ui->a_sellerTable->setRowHeight(row, 40);
    }
}

void AdminDashboard::refreshCustomers() {
    ui->a_customerTable->setRowCount(0);
    for (auto& c : m_platform->getCustomers()) {
        int row = ui->a_customerTable->rowCount();
        ui->a_customerTable->insertRow(row);
        ui->a_customerTable->setItem(row, 0, new QTableWidgetItem(QString::number(c->getId())));
        ui->a_customerTable->setItem(row, 1, new QTableWidgetItem(c->getName()));
        ui->a_customerTable->setItem(row, 2, new QTableWidgetItem(c->getEmail()));
        ui->a_customerTable->setItem(row, 3, new QTableWidgetItem(c->getMembershipTier()));

        // Status pill
        QString statusText = c->getIsActive() ? QString::fromUtf8("● Active") : QString::fromUtf8("● Banned");
        QString objName = c->getIsActive() ? "statusActive" : "statusInactive";
        ui->a_customerTable->setCellWidget(row, 4, makeStatusPill(statusText, objName));
        ui->a_customerTable->setRowHeight(row, 40);
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
    int row = ui->a_productTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select a product first."); return; }
    int pid = ui->a_productTable->item(row, 0)->text().toInt();
    m_platform->toggleProductStatus(pid);
    m_admin->logAction("Toggled product #" + QString::number(pid));
    refreshProducts();
}

void AdminDashboard::onDeleteProduct() {
    int row = ui->a_productTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select a product first."); return; }
    int pid = ui->a_productTable->item(row, 0)->text().toInt();
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delele Product", "Are you sure you want to permanently delete this product?", 
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_platform->deleteProduct(pid);
        m_admin->logAction("Deleted product #" + QString::number(pid));
        refreshProducts();
    }
}

void AdminDashboard::onVerifySeller() {
    int row = ui->a_sellerTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select a seller first."); return; }
    int sid = ui->a_sellerTable->item(row, 0)->text().toInt();
    m_platform->verifySeller(sid, true);
    m_admin->logAction("Verified seller #" + QString::number(sid));
    refreshSellers();
    QMessageBox::information(this, "Success", "Seller verified!");
}

void AdminDashboard::onUpdateOrderStatus() {
    int row = ui->a_orderTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select an order first."); return; }
    int oid = ui->a_orderTable->item(row, 0)->text().remove('#').toInt();

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
    int row = ui->a_customerTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select a customer first."); return; }
    int uid = ui->a_customerTable->item(row, 0)->text().toInt();
    m_platform->toggleUserStatus(uid);
    m_admin->logAction("Toggled user #" + QString::number(uid));
    refreshCustomers();
}

// ─── Admin QR Helpers ────────────────────────────────────────────────────────

void AdminDashboard::refreshAdminQrDisplay() {
    if (!m_adminQrDisplay) return;
    QPixmap px;
    if (!m_platform->getAdminQrPath().isEmpty())
        px.load(m_platform->getAdminQrPath());
    if (!px.isNull()) {
        m_adminQrDisplay->setPixmap(
            px.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_adminQrDisplay->setStyleSheet(
            "background: white; border-radius: 12px; border: none;");
    } else {
        m_adminQrDisplay->setText(QString::fromUtf8("🔲\nNot set"));
        m_adminQrDisplay->setAlignment(Qt::AlignCenter);
        m_adminQrDisplay->setStyleSheet(
            "color: #7878a0; font-size: 24px; background: #252540;"
            "border-radius: 12px; border: none;");
    }
}

void AdminDashboard::updateAdminQrStatus() {
    if (!m_adminQrStatus) return;
    if (!m_platform->getAdminQrPath().isEmpty()) {
        m_adminQrStatus->setText(
            QString::fromUtf8("<span style='color:#06d6a0; font-weight:700;'>"
                              "✅ Platform QR active</span><br>"
                              "<span style='color:#7878a0;'>"
                              "Shown when customer cart spans multiple shops.</span>"));
    } else {
        m_adminQrStatus->setText(
            QString::fromUtf8("<span style='color:#ffd166; font-weight:700;'>"
                              "⚠️ No platform QR set</span><br>"
                              "<span style='color:#7878a0;'>"
                              "Multi-shop orders will show COD only.</span>"));
    }
}

void AdminDashboard::onSetAdminQr() {
    QString path = QFileDialog::getOpenFileName(
        this, "Select Platform UPI QR", "", "Images (*.png *.jpg *.jpeg *.webp)");
    if (path.isEmpty()) return;

    QDir().mkpath("data/qr");
    QString ext  = QFileInfo(path).suffix();
    QString dest = QString("data/qr/admin_platform.%1").arg(ext);
    QFile::remove(dest);  // overwrite
    if (QFile::copy(path, dest)) {
        m_platform->setAdminQrPath(dest);
        refreshAdminQrDisplay();
        updateAdminQrStatus();
        QMessageBox::information(this, QString::fromUtf8("Platform QR Saved ✅"),
            "Platform QR saved. Multi-shop checkouts will now show this QR.");
    } else {
        QMessageBox::warning(this, "Error", "Could not copy the image.");
    }
}
