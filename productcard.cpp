#include "productcard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

ProductCard::ProductCard(const Product* product, QWidget *parent)
    : QFrame(parent), m_productId(product->getId())
{
    setFixedSize(260, 380);
    setCursor(Qt::PointingHandCursor);
    setStyleSheet(
        "ProductCard {"
        "  background-color: #1a1a2e;"
        "  border-radius: 12px;"
        "  border: 1px solid #252540;"
        "}"
    );

    // Setup drop shadow effect
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 100));
    shadow->setOffset(0, 6);
    setGraphicsEffect(shadow);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- Image Container ---
    QFrame* imageContainer = new QFrame(this);
    imageContainer->setFixedHeight(160);

    QString imagePath = product->getImagePath();
    if (!imagePath.isEmpty()) {
        imagePath.replace("\\", "/");
        imageContainer->setStyleSheet(QString(
            "QFrame {"
            "  border-image: url('%1') 0 0 0 0 stretch stretch;"
            "  border-top-left-radius: 12px;"
            "  border-top-right-radius: 12px;"
            "}"
        ).arg(imagePath));
    } else {
        QString bgColor = generatePlaceholderColor(product->getId());
        imageContainer->setStyleSheet(QString(
            "QFrame {"
            "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
            "  stop:0 %1, stop:1 #1a1a2e);"
            "  border-top-left-radius: 12px;"
            "  border-top-right-radius: 12px;"
            "}"
        ).arg(bgColor));
    }

    // Discount Badge
    if (product->getDiscount() > 0) {
        QLabel* discountBadge = new QLabel(QString("-%1%").arg((int)product->getDiscount()), imageContainer);
        discountBadge->setAlignment(Qt::AlignCenter);
        discountBadge->setStyleSheet(
            "QLabel {"
            "  background-color: #ff5e3a;"
            "  color: white;"
            "  font-weight: bold;"
            "  font-size: 11px;"
            "  border-radius: 12px;"
            "  padding: 4px 10px;"
            "}"
        );
        discountBadge->move(12, 12);
        discountBadge->setFixedSize(50, 26);
    }

    // Stock badge
    if (product->getStock() == 0) {
        QLabel* oosLabel = new QLabel("OUT OF STOCK", imageContainer);
        oosLabel->setAlignment(Qt::AlignCenter);
        oosLabel->setStyleSheet(
            "background: rgba(255,40,40,0.85); color: white; font-size: 10px;"
            "font-weight: 800; border-radius: 4px; padding: 4px 10px; letter-spacing: 0.5px;");
        oosLabel->move(12, 130);
    }

    mainLayout->addWidget(imageContainer);

    // --- Content Container ---
    QFrame* contentContainer = new QFrame(this);
    contentContainer->setStyleSheet("background: transparent; border: none;");
    QVBoxLayout* contentLayout = new QVBoxLayout(contentContainer);
    contentLayout->setContentsMargins(16, 14, 16, 14);
    contentLayout->setSpacing(6);

    // Brand
    QLabel* brandLabel = new QLabel(product->getBrand());
    brandLabel->setStyleSheet("color: #7878a0; font-size: 11px; font-weight: 500; border: none;");
    contentLayout->addWidget(brandLabel);

    // Title
    QLabel* titleLabel = new QLabel(product->getName());
    titleLabel->setWordWrap(true);
    titleLabel->setStyleSheet("color: #e8e8f0; font-size: 14px; font-weight: 700; border: none;");
    titleLabel->setFixedHeight(36);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    contentLayout->addWidget(titleLabel);

    // Rating
    QHBoxLayout* ratingLayout = new QHBoxLayout();
    ratingLayout->setSpacing(6);
    QLabel* starIcon = new QLabel(QString::fromUtf8("★ ") + QString::number(product->getAverageRating(), 'f', 1));
    starIcon->setStyleSheet(
        "background-color: rgba(13,130,149,0.8);"
        "color: white;"
        "border-radius: 6px;"
        "padding: 2px 8px;"
        "font-size: 11px;"
        "font-weight: bold;"
    );

    int reviewCount = (product->getId() * 17) % 500 + 10;
    QLabel* reviewLabel = new QLabel(QString("(%1)").arg(reviewCount));
    reviewLabel->setStyleSheet("color: #7878a0; font-size: 11px; border: none;");

    ratingLayout->addWidget(starIcon);
    ratingLayout->addWidget(reviewLabel);
    ratingLayout->addStretch();
    contentLayout->addLayout(ratingLayout);

    contentLayout->addSpacing(4);

    // Price Row
    QHBoxLayout* priceLayout = new QHBoxLayout();
    priceLayout->setSpacing(8);
    QLabel* newPrice = new QLabel(QString::fromUtf8("₹%1").arg((int)product->getDiscountedPrice()));
    newPrice->setStyleSheet("color: #ff6b2b; font-size: 18px; font-weight: 800; border: none;");

    priceLayout->addWidget(newPrice);
    if (product->getDiscount() > 0) {
        QLabel* oldPrice = new QLabel(QString::fromUtf8("₹%1").arg((int)product->getPrice()));
        oldPrice->setStyleSheet("color: #5e5e7a; font-size: 12px; text-decoration: line-through; border: none;");
        priceLayout->addWidget(oldPrice);
        priceLayout->setAlignment(oldPrice, Qt::AlignBottom);
    }
    priceLayout->addStretch();
    contentLayout->addLayout(priceLayout);

    contentLayout->addSpacing(4);

    // Add to Cart Button — more prominent
    QPushButton* addToCartBtn = new QPushButton(QString::fromUtf8("🛒  Add to Cart"));
    addToCartBtn->setCursor(Qt::PointingHandCursor);
    addToCartBtn->setMinimumHeight(36);
    addToCartBtn->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "  stop:0 rgba(255,107,43,0.15), stop:1 rgba(255,107,43,0.05));"
        "  color: #ff9a5c;"
        "  border: 1px solid rgba(255,107,43,0.3);"
        "  border-radius: 8px;"
        "  padding: 8px;"
        "  font-weight: 700;"
        "  font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "  stop:0 #ff6b2b, stop:1 #e85d20);"
        "  color: white;"
        "  border-color: #ff6b2b;"
        "}"
    );

    connect(addToCartBtn, &QPushButton::clicked, this, [this](bool) {
        emit addToCartClicked(m_productId);
    });

    contentLayout->addWidget(addToCartBtn);

    mainLayout->addWidget(contentContainer);
}

void ProductCard::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit cardClicked(m_productId);
    }
    QFrame::mousePressEvent(event);
}

void ProductCard::enterEvent(QEnterEvent *event)
{
    QFrame::enterEvent(event);
    setStyleSheet(
        "ProductCard {"
        "  background-color: #202038;"
        "  border-radius: 12px;"
        "  border: 1px solid #3a3a55;"
        "}"
    );
}

void ProductCard::leaveEvent(QEvent *event)
{
    QFrame::leaveEvent(event);
    setStyleSheet(
        "ProductCard {"
        "  background-color: #1a1a2e;"
        "  border-radius: 12px;"
        "  border: 1px solid #252540;"
        "}"
    );
}

QString ProductCard::generatePlaceholderColor(int productId)
{
    const QString colors[] = {
        "#ffd12a", "#1c1c1e", "#d91428",
        "#e5e5e5", "#2db39d", "#8a2be2"
    };
    return colors[productId % 6];
}
