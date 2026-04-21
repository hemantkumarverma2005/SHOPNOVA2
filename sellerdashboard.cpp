#include "sellerdashboard.h"
#include "ui_sellerdashboard.h"
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
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QColor>
#include <QLayoutItem>

// ── Helper: create a stat card widget ─────────────────────────
static QFrame* makeStatCard(const QString& label, const QString& value,
                            const QString& subtitle, const QString& valueColor)
{
    QFrame* card = new QFrame;
    card->setMinimumHeight(130);
    card->setStyleSheet(QString(
        "QFrame { background: #1a1a2e; border: 1px solid #252540;"
        "border-radius: 12px; }"));
    QVBoxLayout* cl = new QVBoxLayout(card);
    cl->setContentsMargins(20, 20, 20, 20);
    cl->setSpacing(8);

    QLabel* ttl = new QLabel(label);
    ttl->setStyleSheet("font-size: 13px; color: #8888a8; font-weight: 500; border: none; background: transparent;");
    QLabel* val = new QLabel(value);
    val->setObjectName("cardValue");
    val->setStyleSheet(QString("font-size: 28px; font-weight: 800; color: %1; border: none; background: transparent;").arg(valueColor));
    QLabel* sub = new QLabel(subtitle);
    sub->setObjectName("cardSubtitle");
    sub->setStyleSheet("font-size: 11px; color: #7878a0; border: none; background: transparent;");

    cl->addWidget(ttl);
    cl->addStretch();
    cl->addWidget(val);
    cl->addWidget(sub);
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

// ── Helper: quick-action tile ─────────────────────────────────
static QPushButton* makeActionTile(const QString& icon, const QString& label) {
    QPushButton* btn = new QPushButton(icon + "\n" + label);
    btn->setObjectName("actionCard");
    btn->setMinimumSize(140, 90);
    btn->setCursor(Qt::PointingHandCursor);
    return btn;
}

SellerDashboard::SellerDashboard(Seller* seller, Platform* platform, QWidget* parent)
    : QWidget(parent), ui(new Ui::SellerDashboard), m_seller(seller), m_platform(platform)
{
    ui->setupUi(this);

    connect(ui->shell, &ShellWidget::logoutRequested, this, &SellerDashboard::logoutRequested);
    ui->shell->setSearchVisible(false);  // Hide search in seller

    // Wire up buttons
    connect(ui->p_addBtn, &QPushButton::clicked, this, &SellerDashboard::onAddProduct);
    connect(ui->p_discountBtn, &QPushButton::clicked, this, &SellerDashboard::onSetDiscount);
    connect(ui->p_stockBtn, &QPushButton::clicked, this, &SellerDashboard::onUpdateStock);
    connect(ui->or_updateBtn, &QPushButton::clicked, this, &SellerDashboard::onUpdateOrderStatus);

    // Setup headers for tables
    ui->productTable->setColumnCount(6);
    ui->productTable->setHorizontalHeaderLabels({"ID", "Name", "Brand", QString::fromUtf8("Price (₹)"), "Disc%", "Stock"});
    ui->productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->productTable->setAlternatingRowColors(true);
    ui->productTable->verticalHeader()->setVisible(false);
    ui->productTable->setShowGrid(false);

    ui->orderTable->setColumnCount(5);
    ui->orderTable->setHorizontalHeaderLabels({"Order#", "Customer", "Product", QString::fromUtf8("Amount (₹)"), "Status"});
    ui->orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->orderTable->setAlternatingRowColors(true);
    ui->orderTable->verticalHeader()->setVisible(false);
    ui->orderTable->setShowGrid(false);

    // Refill stat cards layout
    QLayoutItem *child;
    while((child = ui->o_statsRow->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    ui->o_statsRow->addWidget(makeStatCard("Total Revenue", QString::fromUtf8("₹0"), "+20.1% from last month", "#ff6b2b"));
    ui->o_statsRow->addWidget(makeStatCard("Total Orders", "+0", "+15% from last month", "#4cc9f0"));
    ui->o_statsRow->addWidget(makeStatCard("Active Products", "0", "In your store", "#06d6a0"));
    ui->o_statsRow->addWidget(makeStatCard("Pending Orders", "0", "Need processing", "#ffd166"));

    // Add quick-action tiles to overview
    QPushButton* tileAdd    = makeActionTile(QString::fromUtf8("➕"), "Add Product");
    QPushButton* tileDisc   = makeActionTile(QString::fromUtf8("🏷️"), "Set Discount");
    QPushButton* tileStock  = makeActionTile(QString::fromUtf8("📦"), "Update Stock");
    QPushButton* tileOrders = makeActionTile(QString::fromUtf8("📋"), "View Orders");

    ui->o_actionRow->addWidget(tileAdd);
    ui->o_actionRow->addWidget(tileDisc);
    ui->o_actionRow->addWidget(tileStock);
    ui->o_actionRow->addWidget(tileOrders);
    ui->o_actionRow->addStretch();

    connect(tileAdd, &QPushButton::clicked, this, &SellerDashboard::onAddProduct);
    connect(tileDisc, &QPushButton::clicked, this, &SellerDashboard::onSetDiscount);
    connect(tileStock, &QPushButton::clicked, this, &SellerDashboard::onUpdateStock);

    // Profile page text
    ui->profileLbl->setText(
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
        .arg(m_seller->getProductIds().size())
    );

    // Extract pages out to wrapper scroll areas
    QScrollArea *o_scroll = new QScrollArea; o_scroll->setWidgetResizable(true); o_scroll->setWidget(ui->overviewPage); o_scroll->setFrameShape(QFrame::NoFrame);
    QScrollArea *p_scroll = new QScrollArea; p_scroll->setWidgetResizable(true); p_scroll->setWidget(ui->productsPage); p_scroll->setFrameShape(QFrame::NoFrame);
    QScrollArea *or_scroll = new QScrollArea; or_scroll->setWidgetResizable(true); or_scroll->setWidget(ui->ordersPage); or_scroll->setFrameShape(QFrame::NoFrame);
    QScrollArea *pr_scroll = new QScrollArea; pr_scroll->setWidgetResizable(true); pr_scroll->setWidget(ui->profilePage); pr_scroll->setFrameShape(QFrame::NoFrame);
    
    QVector<NavItem> nav = {
        {"",                         "Store",        nullptr},
        {QString::fromUtf8("🏠"),    "Overview",     o_scroll},
        {QString::fromUtf8("📦"),    "My Products",  p_scroll},
        {"",                         "Management",   nullptr},
        {QString::fromUtf8("📋"),    "Orders",       or_scroll},
        {QString::fromUtf8("👤"),    "Profile",      pr_scroll},
    };

    ui->shell->configure("ShopNova", m_seller->getName(), "Seller", nav);
}

SellerDashboard::~SellerDashboard() {
    delete ui;
}

// ─── Refresh logic ───────────────────────────────────────────────────────────

void SellerDashboard::refresh() {
    refreshStats();
    refreshProducts();
    refreshOrders();
}

void SellerDashboard::refreshStats() {
    auto updateCard = [](QLayoutItem* child, const QString& val) {
        if (!child || !child->widget()) return;
        QLabel* valLbl = child->widget()->findChild<QLabel*>("cardValue");
        if (valLbl) valLbl->setText(val);
    };
    auto prods = m_platform->getProductsBySeller(m_seller->getId());
    
    int totalOrders = 0;
    int pendingOrders = 0;
    QVector<int> myPids;
    for (auto* p : prods) myPids.append(p->getId());

    for (auto& o : m_platform->getAllOrders()) {
        for (auto& item : o->getItems()) {
            if (myPids.contains(item.productId)) {
                totalOrders++;
                if (o->getStatus() == OrderStatus::PENDING) pendingOrders++;
                break; // count order only once for this seller
            }
        }
    }
    
    updateCard(ui->o_statsRow->itemAt(0), QString::fromUtf8("₹") + QString::number((int)m_seller->getTotalRevenue()));
    updateCard(ui->o_statsRow->itemAt(1), QString("+") + QString::number(totalOrders));
    updateCard(ui->o_statsRow->itemAt(2), QString::number(prods.size()));
    updateCard(ui->o_statsRow->itemAt(3), QString::number(pendingOrders));
}

void SellerDashboard::refreshProducts() {
    ui->productTable->setRowCount(0);
    for (auto* p : m_platform->getProductsBySeller(m_seller->getId())) {
        int row = ui->productTable->rowCount();
        ui->productTable->insertRow(row);
        ui->productTable->setItem(row, 0, new QTableWidgetItem(QString::number(p->getId())));
        ui->productTable->setItem(row, 1, new QTableWidgetItem(p->getName()));
        ui->productTable->setItem(row, 2, new QTableWidgetItem(p->getBrand()));
        ui->productTable->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)p->getDiscountedPrice())));

        // Discount with color
        auto* discItem = new QTableWidgetItem(QString::number((int)p->getDiscount()) + "%");
        if (p->getDiscount() > 0) discItem->setForeground(QColor("#06d6a0"));
        ui->productTable->setItem(row, 4, discItem);

        // Stock with color coding
        auto* sItem = new QTableWidgetItem(QString::number(p->getStock()));
        if (p->getStock() == 0) { sItem->setForeground(QColor("#ff4444")); sItem->setText("Out"); }
        else if (p->getStock() <= 5) { sItem->setForeground(QColor("#ffd166")); }
        ui->productTable->setItem(row, 5, sItem);
        ui->productTable->setRowHeight(row, 38);
    }
}

void SellerDashboard::refreshOrders() {
    ui->orderTable->setRowCount(0);
    auto myProds = m_platform->getProductsBySeller(m_seller->getId());
    QVector<int> myPids;
    for (auto* p : myProds) myPids.append(p->getId());

    for (auto& o : m_platform->getAllOrders()) {
        for (auto& item : o->getItems()) {
            if (myPids.contains(item.productId)) {
                int row = ui->orderTable->rowCount();
                ui->orderTable->insertRow(row);
                ui->orderTable->setItem(row, 0, new QTableWidgetItem("#" + QString::number(o->getId())));
                ui->orderTable->setItem(row, 1, new QTableWidgetItem(o->getCustomerName()));
                ui->orderTable->setItem(row, 2, new QTableWidgetItem(item.productName + " x" + QString::number(item.quantity)));
                auto* amtItem = new QTableWidgetItem(QString::fromUtf8("₹") + QString::number((int)o->getTotalAmount()));
                amtItem->setForeground(QColor("#ff9a5c"));
                ui->orderTable->setItem(row, 3, amtItem);

                // Status pill
                ui->orderTable->setCellWidget(row, 4, makeOrderStatusPill(o->getStatus(), o->statusString()));
                ui->orderTable->setRowHeight(row, 40);
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
    
    QHBoxLayout* imgLayout = new QHBoxLayout;
    QPushButton* imgBtn = new QPushButton("Browse Image");
    QLabel* imgPathLbl = new QLabel("No image selected");
    imgLayout->addWidget(imgBtn);
    imgLayout->addWidget(imgPathLbl);
    
    QString selectedImagePath;
    connect(imgBtn, &QPushButton::clicked, [&]() {
        QString path = QFileDialog::getOpenFileName(&dlg, "Select Product Image", "", "Images (*.png *.jpg *.jpeg *.webp)");
        if (!path.isEmpty()) {
            selectedImagePath = path;
            imgPathLbl->setText(QFileInfo(path).fileName());
        }
    });

    QPushButton* ok = new QPushButton("Add Product"); ok->setObjectName("successBtn");
    ok->setMinimumHeight(40);
    connect(ok, &QPushButton::clicked, &dlg, &QDialog::accept);

    form->addRow("Name:",    nameEdit);
    form->addRow("Desc:",    descEdit);
    form->addRow("Brand:",   brandEdit);
    form->addRow("Price:",   price);
    form->addRow("Stock:",   stock);
    form->addRow("Image:",   imgLayout);
    form->addRow(ok);

    if (dlg.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) return;
        auto prod = std::make_unique<Electronics>(0, name, descEdit->text(),
            brandEdit->text(), price->value(), stock->value(),
            m_seller->getId(), m_seller->getStoreName(), 12);
        
        int pid = m_platform->addProduct(std::move(prod));
        
        if (!selectedImagePath.isEmpty()) {
            Product* newlyAdded = m_platform->getProductById(pid);
            if (newlyAdded) {
                QString destName = QString("data/images/prod_%1_%2").arg(pid).arg(QFileInfo(selectedImagePath).fileName());
                if (QFile::copy(selectedImagePath, destName)) {
                    newlyAdded->setImagePath(destName);
                    m_platform->saveToDisk();
                }
            }
        }
        
        m_seller->addProductId(pid);
        refresh();
        QMessageBox::information(this, "Added", "Product added! ID: " + QString::number(pid));
    }
}

void SellerDashboard::onSetDiscount() {
    int row = ui->productTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select a product."); return; }
    int pid = ui->productTable->item(row, 0)->text().toInt();
    Product* p = m_platform->getProductById(pid);
    if (!p || p->getSellerId() != m_seller->getId()) return;
    bool ok;
    int disc = QInputDialog::getInt(this, "Set Discount",
        "Discount % for " + p->getName(), (int)p->getDiscount(), 0, 90, 1, &ok);
    if (ok) { p->setDiscount(disc); m_platform->saveToDisk(); refreshProducts(); }
}

void SellerDashboard::onUpdateStock() {
    int row = ui->productTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select a product."); return; }
    int pid = ui->productTable->item(row, 0)->text().toInt();
    Product* p = m_platform->getProductById(pid);
    if (!p || p->getSellerId() != m_seller->getId()) return;
    bool ok;
    int newStock = QInputDialog::getInt(this, "Update Stock",
        "New stock for " + p->getName(), p->getStock(), 0, 999999, 1, &ok);
    if (ok) { p->setStock(newStock); m_platform->saveToDisk(); refreshProducts(); }
}

void SellerDashboard::onUpdateOrderStatus() {
    int row = ui->orderTable->currentRow();
    if (row < 0) { QMessageBox::information(this, "Info", "Select an order."); return; }
    int oid = ui->orderTable->item(row, 0)->text().remove('#').toInt();
    QStringList statuses = {"Confirmed", "Shipped", "Delivered"};
    bool ok;
    QString chosen = QInputDialog::getItem(this, "Update Status", "New status:", statuses, 0, false, &ok);
    if (!ok) return;
    OrderStatus ns = (chosen == "Confirmed") ? OrderStatus::CONFIRMED :
                     (chosen == "Shipped")   ? OrderStatus::SHIPPED   : OrderStatus::DELIVERED;
    m_platform->updateOrderStatus(oid, ns);
    refreshOrders();
}
