#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QFrame>
#include <QComboBox>
#include "shellwidget.h"
#include "datastore.h"

class CustomerDashboard : public QWidget {
    Q_OBJECT
public:
    explicit CustomerDashboard(Customer* customer, Platform* platform, QWidget* parent = nullptr);
    void refresh();

signals:
    void logoutRequested();

private slots:
    void onAddToCart();
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

    QWidget *buildHomePage();
    QWidget *buildShopPage();
    QWidget *buildCartPage();
    QWidget *buildOrdersPage();
    QWidget *buildProfilePage();

    Customer* m_customer;
    Platform* m_platform;
    ShellWidget *m_shell;

    QFrame*       m_statCards[4];
    QTableWidget* m_productTable;
    QTableWidget* m_cartTable;
    QTableWidget* m_orderTable;
    QLabel*       m_cartTotalLabel;
    QLabel*       m_profileLabel;
    QLabel*       m_notifLabel;
    QLineEdit*    m_searchEdit;
};
