#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QMap>
#include "models.h"
#include "datastore.h"

// ============================================================
// ItemCard  – one product card with +/- quantity control
// ============================================================
class ItemCard : public QFrame {
    Q_OBJECT
public:
    explicit ItemCard(Product* product, int currentQty = 0, QWidget* parent = nullptr);

signals:
    void quantityChanged(Product* product, int newQty);

private slots:
    void increment();
    void decrement();

private:
    Product* m_product;
    int      m_qty;
    QLabel*  m_qtyLabel;
    QPushButton* m_minusBtn;
    QPushButton* m_plusBtn;
    QWidget* m_qtyControl;
    QPushButton* m_addBtn;

    void updateQtyDisplay();
};

// ============================================================
// ShopDetailWidget  – Screen 2
// ============================================================
class ShopDetailWidget : public QWidget {
    Q_OBJECT
public:
    explicit ShopDetailWidget(Seller* seller,
                               Platform* platform,
                               QWidget* parent = nullptr);

    void setCartQuantities(const QMap<int, int>& qtys);

signals:
    void backClicked();
    void cartUpdated(Product* product, int qty);
    void cartBtnClicked();

private slots:
    void onSearch(const QString& text);
    void onCategoryFilter(const QString& cat);
    void onQuantityChanged(Product* product, int qty);

private:
    Seller*    m_seller;
    Platform*  m_platform;
    QMap<int, int> m_cartQtys;   // productId -> qty

    QLineEdit*   m_searchBox;
    QWidget*     m_itemsContainer;
    QVBoxLayout* m_itemsLayout;
    QLabel*      m_cartBadge;
    QString      m_activeCategory;

    QList<Product*> m_shopProducts;

    void buildHeader();
    void buildShopBanner();
    void buildCategoryBar(const QStringList& cats);
    void buildItemList(const QString& searchText = "");
    QStringList getCategories() const;
};
