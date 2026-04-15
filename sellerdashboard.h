#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
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
    void buildUI();
    void refreshProducts();
    void refreshOrders();

    Seller*   m_seller;
    Platform* m_platform;

    QLabel*       m_dashLabel;
    QTableWidget* m_productTable;
    QTableWidget* m_orderTable;
};
