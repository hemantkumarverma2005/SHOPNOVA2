#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include "datastore.h"

class AdminDashboard : public QWidget {
    Q_OBJECT
public:
    explicit AdminDashboard(Admin* admin, Platform* platform, QWidget* parent = nullptr);
    void refresh();

signals:
    void logoutRequested();

private slots:
    void onAddProduct();
    void onToggleProduct();
    void onVerifySeller();
    void onUpdateOrderStatus();
    void onToggleUser();

private:
    void buildUI();
    void refreshStats();
    void refreshProducts();
    void refreshOrders();
    void refreshSellers();
    void refreshCustomers();

    Admin*    m_admin;
    Platform* m_platform;

    QLabel*       m_statsLabel;
    QTableWidget* m_productTable;
    QTableWidget* m_orderTable;
    QTableWidget* m_sellerTable;
    QTableWidget* m_customerTable;
};
