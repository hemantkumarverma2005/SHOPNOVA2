#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "datastore.h"
#include "loginpage.h"
#include "admindashboard.h"
#include "sellerdashboard.h"
#include "customerdashboard.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onLoginRequested(const QString& email, const QString& password);
    void onRegisterCustomer(const QString& name, const QString& email,
                            const QString& password, const QString& phone);
    void onRegisterSeller(const QString& name, const QString& email,
                          const QString& password, const QString& phone,
                          const QString& storeName);
    void onLogout();

private:
    void applyStyle();
    void goToPage(int index);

    Platform* m_platform;
    QStackedWidget* m_stack;

    enum Pages { PAGE_LOGIN = 0 };

    LoginPage*         m_loginPage;
    AdminDashboard*    m_adminPage    = nullptr;
    SellerDashboard*   m_sellerPage   = nullptr;
    CustomerDashboard* m_customerPage = nullptr;
};
