#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QFrame>
#include <QComboBox>
#include "shellwidget.h"
#include "datastore.h"
#include "flowlayout.h"
#include "shopnavcontroller.h"
#include "shopdetailwidget.h"

namespace Ui {
class CustomerDashboard;
}

class CustomerDashboard : public QWidget {
    Q_OBJECT
public:
    explicit CustomerDashboard(Customer* customer, Platform* platform, QWidget* parent = nullptr);
    ~CustomerDashboard();
    void refresh();

signals:
    void logoutRequested();

private slots:
    void onItemCardQtyChanged(Product* product, int qty);
    void onRemoveFromCart();
    void onCheckout();
    void onCancelOrder();
    void onSearch();
    void onClearSearch();

private:
    void refreshProducts(QVector<Product*> products = {});
    void refreshCart();
    void refreshOrders();
    void refreshProfile();

    Ui::CustomerDashboard* ui;
    Customer*           m_customer;
    Platform*           m_platform;
    ShopNavController*  m_shopNav = nullptr;   // Browse Shops mode
};
