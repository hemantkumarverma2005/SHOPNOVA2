#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QFrame>
#include "shellwidget.h"
#include "datastore.h"

namespace Ui {
class SellerDashboard;
}

class SellerDashboard : public QWidget {
    Q_OBJECT
public:
    explicit SellerDashboard(Seller* seller, Platform* platform, QWidget* parent = nullptr);
    ~SellerDashboard();
    
    void refresh();

signals:
    void logoutRequested();

private slots:
    void onAddProduct();
    void onSetDiscount();
    void onUpdateStock();
    void onUpdateOrderStatus();
    void onChangeQr();

private:
    void refreshProducts();
    void refreshOrders();
    void refreshStats();
    void refreshQrDisplay();
    void updateQrStatusLabel();

    Ui::SellerDashboard* ui;
    Seller*   m_seller;
    Platform* m_platform;

    QLabel* m_qrDisplay    = nullptr;
    QLabel* m_qrStatusLbl  = nullptr;
};
