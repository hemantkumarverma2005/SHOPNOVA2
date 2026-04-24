#include "cartpanel.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPixmap>
#include <QDialog>
#include <QDialogButtonBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QScrollArea>
#include <QInputDialog>
#include <QMessageBox>
#include <QSet>
#include <QDir>
#include <QResizeEvent>
#include <QFormLayout>
#include <QLineEdit>

namespace PalCart {
    static const QString BG_PANEL  = "#1e1840";
    static const QString BG_ITEM   = "rgba(255,255,255,0.07)";
    static const QString BG_FOOT   = "#252048";
    static const QString ACCENT    = "#7c5cfc";
    static const QString ACCENT2   = "#a855f7";
    static const QString TEXT_HI   = "#ffffff";
    static const QString TEXT_LO   = "#9590b8";
}

// ============================================================
// CartItemRow
// ============================================================
CartItemRow::CartItemRow(const CartItem& item, Platform* platform, QWidget* parent)
    : QFrame(parent), m_item(item), m_qty(item.quantity)
{
    setStyleSheet(R"(
        CartItemRow {
            background: rgba(255,255,255,0.07);
            border-radius: 12px;
            border: 1px solid rgba(255,255,255,0.06);
        }
    )");

    QHBoxLayout* hl = new QHBoxLayout(this);
    hl->setContentsMargins(14, 14, 14, 14);
    hl->setSpacing(12);

    // Thumbnail
    QLabel* thumb = new QLabel(this);
    thumb->setFixedSize(56, 56);
    thumb->setStyleSheet("border-radius: 8px; background: #2d1b69;");
    thumb->setScaledContents(true);

    QPixmap px;
    Product* prod = platform ? platform->getProductById(item.productId) : nullptr;
    if (prod && !prod->getImagePath().isEmpty())
        px.load(prod->getImagePath());

    if (px.isNull()) {
        px = QPixmap(56, 56);
        QPainter p(&px);
        QLinearGradient g(0, 0, 56, 56);
        g.setColorAt(0, QColor("#2d1b69"));
        g.setColorAt(1, QColor("#1a1535"));
        p.fillRect(px.rect(), g);
        p.setPen(Qt::white);
        QFont f; f.setPixelSize(16); f.setBold(true); p.setFont(f);
        p.drawText(px.rect(), Qt::AlignCenter, item.productName.left(2));
    }
    thumb->setPixmap(px);
    hl->addWidget(thumb);

    // Name + shop + price
    QVBoxLayout* info = new QVBoxLayout();
    info->setSpacing(2);

    QLabel* nameLbl = new QLabel(item.productName, this);
    nameLbl->setStyleSheet("color: white; font-size: 14px; font-weight: 700; background: transparent;");

    QLabel* shopLbl = new QLabel(item.sellerName.toUpper(), this);
    shopLbl->setStyleSheet(QString(
        "color: %1; font-size: 10px; font-weight: 700;"
        "letter-spacing: 1px; background: transparent;").arg(PalCart::ACCENT));

    double finalPrice = item.price * (1.0 - item.discountPercent / 100.0) * item.quantity;
    QLabel* priceLbl = new QLabel(QString("₹%1").arg(finalPrice, 0, 'f', 2), this);
    priceLbl->setStyleSheet("color: white; font-size: 15px; font-weight: 800; background: transparent;");

    info->addWidget(nameLbl);
    info->addWidget(shopLbl);
    info->addWidget(priceLbl);
    hl->addLayout(info, 1);

    // +/- control
    QVBoxLayout* qtyCol = new QVBoxLayout();
    qtyCol->setSpacing(4);
    qtyCol->setAlignment(Qt::AlignCenter);

    auto makeBtn = [&](const QString& txt) {
        QPushButton* b = new QPushButton(txt, this);
        b->setFixedSize(24, 24);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(QString(R"(
            QPushButton { background: rgba(124,92,252,0.2); color: white;
                border: none; border-radius: 12px; font-size: 16px; font-weight: 700; }
            QPushButton:hover { background: %1; }
        )").arg(PalCart::ACCENT));
        return b;
    };

    QPushButton* plusBtn  = makeBtn("+");
    m_qtyLabel = new QLabel(QString::number(m_qty), this);
    m_qtyLabel->setAlignment(Qt::AlignCenter);
    m_qtyLabel->setFixedWidth(24);
    m_qtyLabel->setStyleSheet("color: white; font-size: 14px; font-weight: 700; background: transparent;");
    QPushButton* minusBtn = makeBtn("−");

    qtyCol->addWidget(plusBtn,  0, Qt::AlignHCenter);
    qtyCol->addWidget(m_qtyLabel, 0, Qt::AlignHCenter);
    qtyCol->addWidget(minusBtn, 0, Qt::AlignHCenter);
    hl->addLayout(qtyCol);

    connect(plusBtn,  &QPushButton::clicked, this, [this]() { updateQty(+1); });
    connect(minusBtn, &QPushButton::clicked, this, [this]() { updateQty(-1); });
}

void CartItemRow::updateQty(int delta)
{
    int newQty = m_qty + delta;
    if (newQty < 0) return;
    m_qty = newQty;
    m_qtyLabel->setText(QString::number(m_qty));
    emit quantityChanged(m_item.productId, m_qty);
}

// ============================================================
// CartPanel
// ============================================================
CartPanel::CartPanel(Platform* platform, Customer* customer, QWidget* parent)
    : QWidget(parent), m_platform(platform), m_customer(customer)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("CartPanel");
    buildPanel();
    m_anim = new QPropertyAnimation(this, "panelX", this);
    m_anim->setDuration(280);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);
}

int CartPanel::panelX() const  { return m_panel->x(); }
void CartPanel::setPanelX(int x) { m_panel->move(x, 0); }

void CartPanel::buildPanel()
{
    setStyleSheet(R"(#CartPanel { background: rgba(0,0,0,0.55); })");

    m_panel = new QWidget(this);
    m_panel->setFixedWidth(420);
    m_panel->setStyleSheet(QString(R"(
        background: %1;
        border-left: 1px solid rgba(124,92,252,0.2);
    )").arg(PalCart::BG_PANEL));

    auto* shadow = new QGraphicsDropShadowEffect(m_panel);
    shadow->setBlurRadius(40); shadow->setOffset(-10, 0);
    shadow->setColor(QColor(0,0,0,120));
    m_panel->setGraphicsEffect(shadow);

    QVBoxLayout* vl = new QVBoxLayout(m_panel);
    vl->setContentsMargins(0,0,0,0); vl->setSpacing(0);

    // Header
    QWidget* hdr = new QWidget(m_panel);
    hdr->setFixedHeight(80);
    hdr->setStyleSheet("background: transparent;");
    QHBoxLayout* hhl = new QHBoxLayout(hdr);
    hhl->setContentsMargins(28,20,28,10);

    QVBoxLayout* titleCol = new QVBoxLayout();
    QLabel* titleLbl = new QLabel("Your Order", hdr);
    titleLbl->setStyleSheet("color: white; font-size: 24px; font-weight: 800; background: transparent;");
    m_countLabel = new QLabel("0 ITEMS SELECTED", hdr);
    m_countLabel->setStyleSheet("color: #9590b8; font-size: 11px; font-weight: 600; background: transparent;");
    titleCol->addWidget(titleLbl);
    titleCol->addWidget(m_countLabel);
    hhl->addLayout(titleCol);
    hhl->addStretch();

    QPushButton* closeBtn = new QPushButton("Close", hdr);
    closeBtn->setFixedSize(60,36);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(R"(
        QPushButton { background: rgba(255,255,255,0.1); border: 1px solid rgba(255,255,255,0.15);
            border-radius: 18px; color: white; font-size: 13px; font-weight: 600; }
        QPushButton:hover { background: rgba(255,255,255,0.2); }
    )");
    connect(closeBtn, &QPushButton::clicked, this, &CartPanel::closeRequested);
    hhl->addWidget(closeBtn);
    vl->addWidget(hdr);

    // Scroll area
    QScrollArea* scroll = new QScrollArea(m_panel);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(R"(
        QScrollArea { background: transparent; border: none; }
        QScrollBar:vertical { background: rgba(255,255,255,0.05); width: 4px; border-radius: 2px; }
        QScrollBar::handle:vertical { background: rgba(124,92,252,0.4); border-radius: 2px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )");
    QWidget* scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");
    m_itemsLayout = new QVBoxLayout(scrollContent);
    m_itemsLayout->setContentsMargins(24,8,24,8);
    m_itemsLayout->setSpacing(10);
    scroll->setWidget(scrollContent);
    vl->addWidget(scroll, 1);

    // Footer
    QWidget* footer = new QWidget(m_panel);
    footer->setFixedHeight(160);
    footer->setStyleSheet(QString(R"(
        background: %1; border-top: 1px solid rgba(255,255,255,0.07);
    )").arg(PalCart::BG_FOOT));
    QVBoxLayout* fvl = new QVBoxLayout(footer);
    fvl->setContentsMargins(24,16,24,20); fvl->setSpacing(6);

    auto makeRow = [&](const QString& label, QLabel*& valLbl, bool large = false) {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel(label, footer);
        lbl->setStyleSheet(large
            ? "color: #7c5cfc; font-size: 12px; font-weight: 700; letter-spacing: 1px; background: transparent;"
            : "color: #9590b8; font-size: 13px; background: transparent;");
        valLbl = new QLabel("₹0.00", footer);
        valLbl->setStyleSheet(large
            ? "color: white; font-size: 22px; font-weight: 800; background: transparent;"
            : "color: white; font-size: 13px; font-weight: 600; background: transparent;");
        row->addWidget(lbl); row->addStretch(); row->addWidget(valLbl);
        fvl->addLayout(row);
    };

    makeRow("SUBTOTAL", m_subtotalLabel, false);
    QFrame* div = new QFrame(footer); div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("color: rgba(255,255,255,0.07); margin: 4px 0;");
    fvl->addWidget(div);
    makeRow("TOTAL PAYABLE", m_totalLabel, true);

    m_checkoutBtn = new QPushButton("PROCEED TO CHECKOUT", footer);
    m_checkoutBtn->setFixedHeight(48);
    m_checkoutBtn->setCursor(Qt::PointingHandCursor);
    m_checkoutBtn->setStyleSheet(QString(R"(
        QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 %1,stop:1 %2);
            color: white; border: none; border-radius: 24px;
            font-size: 14px; font-weight: 700; letter-spacing: 1.5px; }
        QPushButton:hover { opacity: 0.9; }
    )").arg(PalCart::ACCENT).arg(PalCart::ACCENT2));
    connect(m_checkoutBtn, &QPushButton::clicked, this, &CartPanel::onCheckout);
    fvl->addWidget(m_checkoutBtn);
    vl->addWidget(footer);
}

void CartPanel::refreshCart()   { rebuildItems(); updateTotals(); }

void CartPanel::rebuildItems()
{
    while (QLayoutItem* item = m_itemsLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    if (!m_customer) return;
    const QVector<CartItem>& items = m_customer->getCart().getItems();
    if (items.isEmpty()) {
        QLabel* emptyLbl = new QLabel("Your cart is empty", m_itemsLayout->parentWidget());
        emptyLbl->setAlignment(Qt::AlignCenter);
        emptyLbl->setStyleSheet("color: #9590b8; font-size: 15px;");
        m_itemsLayout->addWidget(emptyLbl);
        m_itemsLayout->addStretch();
        return;
    }
    m_countLabel->setText(QString("%1 ITEM%2 SELECTED")
        .arg(items.size()).arg(items.size() != 1 ? "S" : ""));
    for (const CartItem& ci : items) {
        CartItemRow* row = new CartItemRow(ci, m_platform, m_itemsLayout->parentWidget());
        connect(row, &CartItemRow::quantityChanged, this, &CartPanel::onQtyChanged);
        m_itemsLayout->addWidget(row);
    }
    m_itemsLayout->addStretch();
}

void CartPanel::updateTotals()
{
    if (!m_customer) return;
    const Cart& cart = m_customer->getCart();
    m_subtotalLabel->setText(QString("₹%1").arg(cart.getSubtotal(), 0, 'f', 2));
    m_totalLabel->setText(   QString("₹%1").arg(cart.getTotal(),    0, 'f', 2));
}

void CartPanel::onQtyChanged(int productId, int newQty)
{
    if (!m_customer) return;
    Cart& cart = m_customer->getCart();
    if (newQty == 0) {
        cart.removeItem(productId);
    } else {
        for (CartItem& ci : const_cast<QVector<CartItem>&>(cart.getItems())) {
            if (ci.productId == productId) { ci.quantity = newQty; break; }
        }
    }
    updateTotals();
    emit cartChanged();
}

// ─── QR Checkout Dialog ───────────────────────────────────────
QString CartPanel::determineQrPath() const
{
    if (!m_customer || !m_platform) return QString();
    const QVector<CartItem>& items = m_customer->getCart().getItems();

    // Collect unique seller IDs
    QSet<int> sellerIds;
    for (const CartItem& ci : items) sellerIds.insert(ci.sellerId);

    if (sellerIds.size() > 1) {
        // Multi-shop → admin QR
        return m_platform->getAdminQrPath();
    } else if (sellerIds.size() == 1) {
        // Single shop → seller's QR
        Seller* s = m_platform->getSellerById(*sellerIds.begin());
        return s ? s->getQrImagePath() : QString();
    }
    return QString();
}

void CartPanel::onCheckout()
{
    if (!m_customer || m_customer->getCart().isEmpty()) return;
    if (!m_customer->getDefaultAddress()) {
        QDialog addrDlg(this);
        addrDlg.setWindowTitle("Add Delivery Address");
        addrDlg.setFixedWidth(400);
        addrDlg.setStyleSheet("QDialog { background: #1e1840; color: white; border-radius: 16px; }"
                              "QLabel { color: #9590b8; font-weight: 600; }"
                              "QLineEdit { background: rgba(255,255,255,0.07); color: white; border: 1px solid rgba(255,255,255,0.1); border-radius: 8px; padding: 6px; }");
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
        saveBtn->setStyleSheet("background: #7c5cfc; color: white; padding: 10px; border-radius: 8px; font-weight: bold;");
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
        } else {
            return;
        }
    }

    const QVector<CartItem>& cartItems = m_customer->getCart().getItems();
    QString qrPath = determineQrPath();

    // Determine unique sellers for per-seller split display
    QSet<int> sellerIds;
    QMap<int, QString> sellerNames;
    QMap<int, double>  sellerTotals;
    for (const CartItem& ci : cartItems) {
        sellerIds.insert(ci.sellerId);
        sellerNames[ci.sellerId] = ci.sellerName;
        sellerTotals[ci.sellerId] += ci.getTotal();
    }
    bool isMultiShop = sellerIds.size() > 1;

    // ── Build checkout dialog ─────────────────────────────────
    QDialog dlg(this);
    dlg.setWindowTitle("Checkout");
    dlg.setFixedWidth(520);
    dlg.setStyleSheet("QDialog { background: #1e1840; color: white; border-radius: 16px; }");

    QVBoxLayout* root = new QVBoxLayout(&dlg);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(16);

    // Title
    QLabel* title = new QLabel(isMultiShop ? "🛍️  Multi-Shop Order" : "🛍️  Order Summary", &dlg);
    title->setStyleSheet("font-size: 20px; font-weight: 800; color: white; background: transparent;");
    root->addWidget(title);

    // Per-seller breakdown
    if (isMultiShop) {
        QLabel* note = new QLabel("Your cart has items from multiple shops. Payment goes to the platform.", &dlg);
        note->setWordWrap(true);
        note->setStyleSheet("font-size: 11px; color: #9590b8; background: rgba(124,92,252,0.12);"
                            "border-radius: 8px; padding: 8px 12px;");
        root->addWidget(note);
    }

    // Seller breakdown table
    QFrame* breakdown = new QFrame(&dlg);
    breakdown->setStyleSheet("background: rgba(255,255,255,0.06); border-radius: 12px;");
    QVBoxLayout* bvl = new QVBoxLayout(breakdown);
    bvl->setContentsMargins(16, 12, 16, 12); bvl->setSpacing(8);
    for (int sid : sellerIds) {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* sn = new QLabel("🏪  " + sellerNames[sid], breakdown);
        sn->setStyleSheet("color: white; font-size: 13px; font-weight: 600; background: transparent;");
        QLabel* amt = new QLabel(QString("₹%1").arg(sellerTotals[sid], 0, 'f', 2), breakdown);
        amt->setStyleSheet("color: #a78bfa; font-size: 13px; font-weight: 700; background: transparent;");
        amt->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        row->addWidget(sn); row->addStretch(); row->addWidget(amt);
        bvl->addLayout(row);
    }
    QFrame* sep = new QFrame(breakdown); sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: rgba(255,255,255,0.1);");
    bvl->addWidget(sep);
    QHBoxLayout* totalRow = new QHBoxLayout();
    QLabel* tl = new QLabel("TOTAL PAYABLE", breakdown);
    tl->setStyleSheet("color: #9590b8; font-size: 11px; font-weight: 700; letter-spacing: 1px; background: transparent;");
    QLabel* tv = new QLabel(QString("₹%1").arg(m_customer->getCart().getTotal(), 0, 'f', 2), breakdown);
    tv->setStyleSheet("color: white; font-size: 18px; font-weight: 800; background: transparent;");
    tv->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    totalRow->addWidget(tl); totalRow->addStretch(); totalRow->addWidget(tv);
    bvl->addLayout(totalRow);
    root->addWidget(breakdown);

    // Payment method selection
    QLabel* pmLabel = new QLabel("Payment Method", &dlg);
    pmLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #9590b8; background: transparent;");
    root->addWidget(pmLabel);

    QButtonGroup* btnGroup = new QButtonGroup(&dlg);
    auto makeRadio = [&](const QString& label, bool enabled = true) {
        QRadioButton* rb = new QRadioButton(label, &dlg);
        rb->setEnabled(enabled);
        rb->setStyleSheet(enabled
            ? "color: white; font-size: 13px; background: transparent;"
            : "color: #555575; font-size: 13px; background: transparent;");
        btnGroup->addButton(rb);
        root->addWidget(rb);
        return rb;
    };

    QRadioButton* rbCOD = makeRadio("💵  Cash on Delivery");
    rbCOD->setChecked(true);
    QRadioButton* rbUPI = nullptr;

    bool hasQr = !qrPath.isEmpty();
    if (hasQr) {
        rbUPI = makeRadio("📱  UPI / QR Payment");

        // QR image
        QLabel* qrImg = new QLabel(&dlg);
        qrImg->setFixedSize(200, 200);
        qrImg->setAlignment(Qt::AlignCenter);
        qrImg->setStyleSheet("background: white; border-radius: 12px;");
        QPixmap px;
        if (px.load(qrPath)) {
            qrImg->setPixmap(px.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            qrImg->setText("QR unavailable");
            qrImg->setStyleSheet("color: #9590b8; font-size: 13px; background: #252048; border-radius: 12px;");
        }
        QHBoxLayout* qrRow = new QHBoxLayout();
        qrRow->addStretch(); qrRow->addWidget(qrImg); qrRow->addStretch();
        root->addLayout(qrRow);

        QString qrNote = isMultiShop
            ? "Scan this platform QR to pay the full amount."
            : "Scan this shop's UPI QR to pay.";
        QLabel* qrNoteL = new QLabel(qrNote, &dlg);
        qrNoteL->setAlignment(Qt::AlignCenter);
        qrNoteL->setStyleSheet("font-size: 11px; color: #9590b8; background: transparent;");
        root->addWidget(qrNoteL);
    } else {
        // No QR available note
        QLabel* noQr = new QLabel("⚠️  No UPI QR available — only COD offered.", &dlg);
        noQr->setStyleSheet("font-size: 11px; color: #fde68a; background: rgba(251,191,36,0.1);"
                            "border-radius: 8px; padding: 8px 12px;");
        root->addWidget(noQr);
    }

    // Confirm button
    QPushButton* confirmBtn = new QPushButton("✅  Confirm & Place Order", &dlg);
    confirmBtn->setFixedHeight(48);
    confirmBtn->setCursor(Qt::PointingHandCursor);
    confirmBtn->setStyleSheet(R"(
        QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #7c5cfc,stop:1 #a855f7);
            color: white; border: none; border-radius: 24px;
            font-size: 14px; font-weight: 700; letter-spacing: 1px; }
        QPushButton:hover { opacity: 0.9; }
    )");
    connect(confirmBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

    QPushButton* cancelBtn = new QPushButton("Cancel", &dlg);
    cancelBtn->setFixedHeight(36);
    cancelBtn->setStyleSheet(R"(
        QPushButton { background: transparent; color: #9590b8; border: 1px solid rgba(255,255,255,0.15);
            border-radius: 18px; font-size: 13px; }
        QPushButton:hover { color: white; border-color: rgba(255,255,255,0.3); }
    )");
    connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);

    root->addWidget(confirmBtn);
    root->addWidget(cancelBtn);

    if (dlg.exec() != QDialog::Accepted) return;

    // Determine payment method
    PaymentMethod pm = PaymentMethod::COD;
    if (rbUPI && rbUPI->isChecked()) pm = PaymentMethod::UPI;

    try {
        Order* order = m_platform->placeOrder(m_customer, *m_customer->getDefaultAddress(), pm);
        refreshCart();
        emit cartChanged();
        QMessageBox::information(this, "Order Placed! 🎉",
            QString("Order #%1 placed!\nTotal: ₹%2\nTracking: %3\nPayment: %4")
                .arg(order->getId())
                .arg((int)order->getTotalAmount())
                .arg(order->getTrackingId())
                .arg(order->getPayment().methodString()));
        emit checkoutCompleted();
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Order Failed", QString::fromStdString(e.what()));
    }
}

void CartPanel::slideIn()
{
    if (!parentWidget()) return;
    int pw = parentWidget()->width();
    m_panel->setFixedHeight(parentWidget()->height());
    m_panel->move(pw, 0);
    show();
    m_anim->stop();
    m_anim->setStartValue(pw);
    m_anim->setEndValue(pw - 420);
    m_anim->start();
}

void CartPanel::slideOut()
{
    if (!parentWidget()) return;
    int pw = parentWidget()->width();
    m_anim->stop();
    m_anim->setStartValue(m_panel->x());
    m_anim->setEndValue(pw);
    connect(m_anim, &QPropertyAnimation::finished, this, [this]() { hide(); },
            Qt::SingleShotConnection);
    m_anim->start();
}

void CartPanel::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    if (m_panel && isVisible())
        m_panel->setFixedHeight(height());
}
