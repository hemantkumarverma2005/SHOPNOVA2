#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>

class LoginPage : public QWidget {
    Q_OBJECT
public:
    explicit LoginPage(QWidget* parent = nullptr);

signals:
    void loginRequested(const QString& email, const QString& password);
    void registerCustomerRequested(const QString& name, const QString& email,
                                   const QString& password, const QString& phone);
    void registerSellerRequested(const QString& name, const QString& email,
                                 const QString& password, const QString& phone,
                                 const QString& storeName, const QString& qrImagePath);

private slots:
    void onLoginClicked();
    void onRegisterCustomerClicked();
    void onRegisterSellerClicked();

private:
    void buildUI();

    // Login panel
    QLineEdit* m_emailEdit;
    QLineEdit* m_passwordEdit;

    // Register Customer panel
    QLineEdit* m_regName;
    QLineEdit* m_regEmail;
    QLineEdit* m_regPassword;
    QLineEdit* m_regPhone;

    // Register Seller panel
    QLineEdit* m_selName;
    QLineEdit* m_selEmail;
    QLineEdit* m_selPassword;
    QLineEdit* m_selPhone;
    QLineEdit* m_selStoreName;
    QString    m_selQrPath;       // path picked by file dialog
    QLabel*    m_selQrLabel;      // shows filename or "No QR selected"

    QStackedWidget* m_formStack;
};
