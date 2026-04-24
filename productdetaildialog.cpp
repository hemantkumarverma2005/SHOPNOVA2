#include "productdetaildialog.h"
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>

ProductDetailDialog::ProductDetailDialog(const Product* product, QWidget *parent)
    : QDialog(parent), m_productId(product->getId())
{
    setWindowTitle(product->getName());
    setMinimumSize(720, 520);
    setMaximumSize(900, 650);
    setStyleSheet(
        "QDialog { background: #10101c; border: 1px solid #252540; border-radius: 16px; }"
    );
    buildUI(product);
}

void ProductDetailDialog::buildUI(const Product* product) {
    QHBoxLayout* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── LEFT: Image area ────────────────────────────────
    QFrame* imageFrame = new QFrame;
    imageFrame->setFixedWidth(300);
    QString imagePath = product->getImagePath();
    if (!imagePath.isEmpty()) {
        QString path = imagePath;
        path.replace("\\", "/");
        imageFrame->setStyleSheet(QString(
            "QFrame { border-image: url('%1') 0 0 0 0 stretch stretch;"
            "border-top-left-radius: 16px; border-bottom-left-radius: 16px;"
            "border: none; }").arg(path));
    } else {
        QString bgColor = generatePlaceholderColor(product->getId());
        imageFrame->setStyleSheet(QString(
            "QFrame { background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
            "stop:0 %1, stop:1 #10101c);"
            "border-top-left-radius: 16px; border-bottom-left-radius: 16px;"
            "border: none; }").arg(bgColor));

        QVBoxLayout* imgLay = new QVBoxLayout(imageFrame);
        imgLay->setAlignment(Qt::AlignCenter);
        QLabel* emoji = new QLabel(QString::fromUtf8("📦"));
        emoji->setStyleSheet("font-size: 72px; background: transparent; border: none;");
        emoji->setAlignment(Qt::AlignCenter);
        imgLay->addWidget(emoji);
    }

    // Discount badge overlay
    if (product->getDiscount() > 0) {
        QLabel* badge = new QLabel(QString("-%1%").arg((int)product->getDiscount()), imageFrame);
        badge->setStyleSheet(
            "background: #ff5e3a; color: white; font-weight: 800; font-size: 13px;"
            "border-radius: 14px; padding: 5px 14px; border: none;");
        badge->move(16, 16);
    }

    root->addWidget(imageFrame);

    // ── RIGHT: Details ──────────────────────────────────
    QWidget* rightPanel = new QWidget;
    rightPanel->setStyleSheet("background: transparent;");
    QVBoxLayout* detailLay = new QVBoxLayout(rightPanel);
    detailLay->setContentsMargins(32, 32, 32, 28);
    detailLay->setSpacing(0);

    // Category / Type pill
    QLabel* typePill = new QLabel(product->getType().toUpper());
    typePill->setFixedHeight(26);
    typePill->setStyleSheet(
        "background: rgba(76,201,240,0.15); color: #4cc9f0; border: 1px solid rgba(76,201,240,0.3);"
        "border-radius: 13px; padding: 3px 14px; font-size: 10px; font-weight: 700;"
        "letter-spacing: 1px;");
    QHBoxLayout* pillRow = new QHBoxLayout;
    pillRow->setAlignment(Qt::AlignLeft);
    pillRow->addWidget(typePill);
    detailLay->addLayout(pillRow);
    detailLay->addSpacing(12);

    // Product name
    QLabel* nameLabel = new QLabel(product->getName());
    nameLabel->setWordWrap(true);
    nameLabel->setStyleSheet(
        "font-size: 24px; font-weight: 800; color: #f0f0f5;"
        "letter-spacing: -0.5px; border: none; background: transparent;");
    detailLay->addWidget(nameLabel);
    detailLay->addSpacing(6);

    // Brand
    QLabel* brandLabel = new QLabel(QString::fromUtf8("by ") + product->getBrand());
    brandLabel->setStyleSheet("font-size: 13px; color: #8888a8; border: none; background: transparent;");
    detailLay->addWidget(brandLabel);
    detailLay->addSpacing(12);

    // Rating row
    QHBoxLayout* ratingRow = new QHBoxLayout;
    ratingRow->setAlignment(Qt::AlignLeft);
    ratingRow->setSpacing(8);
    double rating = product->getAverageRating();
    QString stars;
    for (int i = 0; i < 5; i++)
        stars += (i < (int)rating) ? QString::fromUtf8("★") : QString::fromUtf8("☆");
    QLabel* starLbl = new QLabel(stars);
    starLbl->setStyleSheet("font-size: 16px; color: #ffd166; border: none; background: transparent;");
    QLabel* ratingVal = new QLabel(QString::number(rating, 'f', 1));
    ratingVal->setStyleSheet("font-size: 14px; font-weight: 700; color: #e0e0f0; border: none; background: transparent;");
    int reviewCount = product->getReviews().size();
    QLabel* reviewLbl = new QLabel(QString("(%1 reviews)").arg(reviewCount > 0 ? reviewCount : ((product->getId() * 17) % 500 + 10)));
    reviewLbl->setStyleSheet("font-size: 12px; color: #7878a0; border: none; background: transparent;");
    ratingRow->addWidget(starLbl);
    ratingRow->addWidget(ratingVal);
    ratingRow->addWidget(reviewLbl);
    detailLay->addLayout(ratingRow);
    detailLay->addSpacing(16);

    // Price section
    QFrame* priceCard = new QFrame;
    priceCard->setStyleSheet(
        "QFrame { background: #1a1a2e; border: 1px solid #252540;"
        "border-radius: 12px; }");
    QHBoxLayout* priceLay = new QHBoxLayout(priceCard);
    priceLay->setContentsMargins(20, 16, 20, 16);
    priceLay->setSpacing(12);

    QLabel* priceLabel = new QLabel(QString::fromUtf8("₹%1").arg((int)product->getDiscountedPrice()));
    priceLabel->setStyleSheet("font-size: 28px; font-weight: 800; color: #ff6b2b; border: none; background: transparent;");
    priceLay->addWidget(priceLabel);

    if (product->getDiscount() > 0) {
        QLabel* oldPrice = new QLabel(QString::fromUtf8("₹%1").arg((int)product->getPrice()));
        oldPrice->setStyleSheet("font-size: 16px; color: #5e5e7a; text-decoration: line-through; border: none; background: transparent;");
        priceLay->addWidget(oldPrice);
        priceLay->setAlignment(oldPrice, Qt::AlignBottom);

        QLabel* saveLbl = new QLabel(QString("Save %1%").arg((int)product->getDiscount()));
        saveLbl->setStyleSheet(
            "background: rgba(6,214,160,0.15); color: #2ee8b0; border: 1px solid rgba(6,214,160,0.3);"
            "border-radius: 10px; padding: 3px 10px; font-size: 11px; font-weight: 700;");
        priceLay->addWidget(saveLbl);
    }
    priceLay->addStretch();
    detailLay->addWidget(priceCard);
    detailLay->addSpacing(16);

    // Info grid
    QGridLayout* infoGrid = new QGridLayout;
    infoGrid->setSpacing(12);

    auto addInfoItem = [&](int row, const QString& icon, const QString& label, const QString& value) {
        QLabel* icn = new QLabel(icon);
        icn->setFixedSize(32, 32);
        icn->setAlignment(Qt::AlignCenter);
        icn->setStyleSheet("background: rgba(255,107,43,0.1); border-radius: 8px; font-size: 14px; border: none;");
        QLabel* lbl = new QLabel(label);
        lbl->setStyleSheet("font-size: 11px; color: #7070a0; font-weight: 600; border: none; background: transparent;");
        QLabel* val = new QLabel(value);
        val->setStyleSheet("font-size: 13px; color: #e0e0f0; font-weight: 600; border: none; background: transparent;");

        QVBoxLayout* textCol = new QVBoxLayout;
        textCol->setSpacing(2);
        textCol->addWidget(lbl);
        textCol->addWidget(val);

        QHBoxLayout* itemLay = new QHBoxLayout;
        itemLay->setSpacing(10);
        itemLay->addWidget(icn);
        itemLay->addLayout(textCol);
        itemLay->addStretch();

        QWidget* itemWidget = new QWidget;
        itemWidget->setLayout(itemLay);
        itemWidget->setStyleSheet("background: transparent; border: none;");
        infoGrid->addWidget(itemWidget, row / 2, row % 2);
    };

    addInfoItem(0, QString::fromUtf8("🏪"), "SELLER", product->getSellerName());
    addInfoItem(1, QString::fromUtf8("📦"), "STOCK",
                product->getStock() > 0 ? QString::number(product->getStock()) + " available" : "Out of Stock");
    addInfoItem(2, QString::fromUtf8("🏷️"), "CATEGORY", product->getCategory());

    // Add type-specific info
    if (auto elec = dynamic_cast<const Electronics*>(product))
        addInfoItem(3, QString::fromUtf8("🛡️"), "WARRANTY", QString::number(elec->getWarranty()) + " months");
    else if (auto cloth = dynamic_cast<const Clothing*>(product))
        addInfoItem(3, QString::fromUtf8("🧵"), "MATERIAL", cloth->getMaterial().isEmpty() ? "Premium Fabric" : cloth->getMaterial());
    else if (auto book = dynamic_cast<const Books*>(product))
        addInfoItem(3, QString::fromUtf8("✍️"), "AUTHOR", book->getAuthor().isEmpty() ? "Various" : book->getAuthor());
    else
        addInfoItem(3, QString::fromUtf8("📋"), "TYPE", product->getType());

    detailLay->addLayout(infoGrid);
    detailLay->addSpacing(12);

    // Description
    if (!product->getDescription().isEmpty()) {
        QLabel* descTitle = new QLabel("Description");
        descTitle->setStyleSheet("font-size: 12px; font-weight: 700; color: #7070a0; letter-spacing: 0.8px; border: none; background: transparent;");
        QLabel* descText = new QLabel(product->getDescription());
        descText->setWordWrap(true);
        descText->setStyleSheet("font-size: 13px; color: #b0b0c8; line-height: 1.5; border: none; background: transparent;");
        detailLay->addWidget(descTitle);
        detailLay->addSpacing(4);
        detailLay->addWidget(descText);
        detailLay->addSpacing(12);
    }

    detailLay->addStretch();

    // Bottom buttons
    QHBoxLayout* btnRow = new QHBoxLayout;
    btnRow->setSpacing(12);

    QPushButton* addCartBtn = new QPushButton(QString::fromUtf8("🛒  Add to Cart"));
    addCartBtn->setMinimumHeight(46);
    addCartBtn->setCursor(Qt::PointingHandCursor);
    addCartBtn->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
        "stop:0 #ff6b2b, stop:1 #e85d20); color: white; border: none;"
        "border-radius: 12px; font-size: 14px; font-weight: 700; padding: 0 28px; }"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
        "stop:0 #ff9a5c, stop:1 #ff6b2b); }");
    connect(addCartBtn, &QPushButton::clicked, this, [this]() {
        emit addToCartRequested(m_productId);
        accept();
    });

    QPushButton* closeBtn = new QPushButton("Close");
    closeBtn->setMinimumHeight(46);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { background: #1a1a2e; color: #b0b0c8; border: 1px solid #252540;"
        "border-radius: 12px; font-size: 14px; font-weight: 600; padding: 0 28px; }"
        "QPushButton:hover { background: #252538; color: #f0f0f5; border-color: #3a3a55; }");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);

    btnRow->addWidget(addCartBtn, 2);
    btnRow->addWidget(closeBtn, 1);
    detailLay->addLayout(btnRow);

    root->addWidget(rightPanel);
}

QString ProductDetailDialog::generatePlaceholderColor(int productId) {
    const QString colors[] = {
        "#ffd12a", "#1c1c1e", "#d91428",
        "#e5e5e5", "#2db39d", "#8a2be2"
    };
    return colors[productId % 6];
}
