#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QFrame>
#include "shellwidget.h"
#include "datastore.h"

class SellerDashboard : public QWidget {
    Q_OBJECT
public:
    explicit SellerDashboard(Seller* seller, Platform* platform, QWidget* parent = nullptr);
    void refresh();

signals:
    void logoutRequested();

private slots:
    void onAddProduct();
    void onSetDiscount();
    void onUpdateStock();
    void onUpdateOrderStatus();

private:
    void refreshProducts();
    void refreshOrders();
    void refreshStats();

    QWidget *buildOverviewPage();
    QWidget *buildProductsPage();
    QWidget *buildOrdersPage();
    QWidget *buildProfilePage();

    Seller*   m_seller;
    Platform* m_platform;
    ShellWidget *m_shell;

    QFrame*       m_statCards[4];
    QTableWidget* m_productTable;
    QTableWidget* m_orderTable;
};
