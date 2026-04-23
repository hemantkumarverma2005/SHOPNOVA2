#pragma once

// ============================================================
// ShopNavController
//
// Drop this into your CustomerDashboard.  It owns a QStackedWidget
// with 2 pages:
//   Page 0 – ShopBrowserWidget   (Browse Premium Shops)
//   Page 1 – ShopDetailWidget    (Items from a shop)
//
// and a CartPanel overlay.
//
// Usage inside CustomerDashboard constructor:
//
//   auto* nav = new ShopNavController(platform, currentCustomer, this);
//   someLayout->addWidget(nav);
// ============================================================

#include <QWidget>
#include <QStackedWidget>
#include "shopbrowserwidget.h"
#include "shopdetailwidget.h"
#include "cartpanel.h"
#include "models.h"
#include "datastore.h"

class ShopNavController : public QWidget {
    Q_OBJECT

public:
    explicit ShopNavController(Platform* platform,
                               Customer*  customer,
                               QWidget*   parent = nullptr);

signals:
    void checkoutRequested();   // emitted so parent can handle order placement

private slots:
    void goToShop(Seller* seller);
    void goBack();
    void openCart();
    void onCartUpdated(Product* product, int qty);
    void onCheckout();

protected:
    void resizeEvent(QResizeEvent* e) override;

private:
    Platform*  m_platform;
    Customer*  m_customer;

    QStackedWidget*    m_stack;
    ShopBrowserWidget* m_browserPage;
    ShopDetailWidget*  m_detailPage;   // recreated each time a shop is entered
    CartPanel*         m_cartPanel;

    void rebuildDetailPage(Seller* seller);
    void syncCartBadge();
};
