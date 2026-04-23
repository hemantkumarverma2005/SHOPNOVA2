#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include "models.h"
#include "datastore.h"

// ── forward declare the shop detail page ──────────────────
class ShopDetailWidget;

// ============================================================
// ShopCard  – one clickable shop tile
// ============================================================
class ShopCard : public QFrame {
    Q_OBJECT
public:
    explicit ShopCard(Seller* seller, QWidget* parent = nullptr);

signals:
    void clicked(Seller* seller);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void enterEvent(QEnterEvent* e) override;
    void leaveEvent(QEvent* e) override;

private:
    Seller* m_seller;
    void buildUi();
};

// ============================================================
// ShopBrowserWidget  – "Browse Premium Shops" landing screen
// ============================================================
class ShopBrowserWidget : public QWidget {
    Q_OBJECT
public:
    explicit ShopBrowserWidget(Platform* platform,
                               const QString& customerName,
                               QWidget* parent = nullptr);

    // call this after cart changes so badge stays current
    void refreshCartBadge(int count);

signals:
    void cartClicked();
    // emitted when customer navigates into a shop
    void shopSelected(Seller* seller);

private slots:
    void filterShops(const QString& text);
    void onShopCardClicked(Seller* seller);

private:
    Platform*    m_platform;
    QString      m_customerName;

    QLineEdit*   m_searchBox;
    QWidget*     m_gridContainer;
    QGridLayout* m_grid;
    QPushButton* m_cartBtn;
    QLabel*      m_cartBadge;

    QList<Seller*> m_allSellers;

    void buildHeader();
    void buildShopGrid(const QString& filter = "");

    static QString globalStyle();
};
