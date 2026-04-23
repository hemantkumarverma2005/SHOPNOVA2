#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include "shellwidget.h"
#include "datastore.h"

namespace Ui {
class AdminDashboard;
}

class AdminDashboard : public QWidget {
    Q_OBJECT
public:
    explicit AdminDashboard(Admin* admin, Platform* platform, QWidget* parent = nullptr);
    ~AdminDashboard();
    void refresh();

signals:
    void logoutRequested();

private slots:
    void onAddProduct();
    void onToggleProduct();
    void onDeleteProduct();
    void onVerifySeller();
    void onUpdateOrderStatus();
    void onToggleUser();
    void onSetAdminQr();

private:
    void refreshStats();
    void refreshProducts();
    void refreshOrders();
    void refreshSellers();
    void refreshCustomers();
    void refreshAdminQrDisplay();
    void updateAdminQrStatus();

    Ui::AdminDashboard* ui;
    Admin*    m_admin;
    Platform* m_platform;

    QLabel* m_adminQrDisplay = nullptr;
    QLabel* m_adminQrStatus  = nullptr;
};
