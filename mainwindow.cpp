#include "mainwindow.h"
#include <QMessageBox>
#include <QApplication>

static const char* APP_STYLE = R"(
QMainWindow, QWidget, QDialog {
    background-color: #1e1e2e;
    color: #cdd6f4;
    font-family: "Segoe UI", "Ubuntu", sans-serif;
    font-size: 13px;
}
QGroupBox {
    border: 1px solid #45475a;
    border-radius: 6px;
    margin-top: 10px;
    padding: 8px;
    font-weight: bold;
    color: #cba6f7;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 4px;
}
QPushButton {
    background-color: #89b4fa;
    color: #1e1e2e;
    border: none;
    border-radius: 6px;
    padding: 7px 18px;
    font-weight: bold;
}
QPushButton:hover  { background-color: #b4d0ff; }
QPushButton:pressed { background-color: #6c9fd8; }
QPushButton#primaryBtn {
    background-color: #89b4fa;
    color: #1e1e2e;
}
QPushButton#successBtn {
    background-color: #a6e3a1;
    color: #1e1e2e;
}
QPushButton#successBtn:hover { background-color: #b8f0b4; }
QPushButton#dangerBtn {
    background-color: #f38ba8;
    color: #1e1e2e;
}
QPushButton#dangerBtn:hover { background-color: #ff96aa; }
QPushButton#warnBtn {
    background-color: #fab387;
    color: #1e1e2e;
}
QPushButton#warnBtn:hover { background-color: #ffc49e; }
QPushButton#logoutBtn {
    background-color: #45475a;
    color: #cdd6f4;
}
QPushButton#logoutBtn:hover { background-color: #585b70; }
QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
    background-color: #313244;
    border: 1px solid #45475a;
    border-radius: 5px;
    padding: 5px 8px;
    color: #cdd6f4;
}
QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
    border: 1px solid #89b4fa;
}
QComboBox::drop-down { border: none; }
QComboBox QAbstractItemView {
    background-color: #313244;
    color: #cdd6f4;
    selection-background-color: #45475a;
}
QTableWidget {
    background-color: #181825;
    border: 1px solid #45475a;
    gridline-color: #313244;
    border-radius: 5px;
    alternate-background-color: #1e1e2e;
}
QTableWidget::item { padding: 4px 8px; }
QTableWidget::item:selected {
    background-color: #45475a;
    color: #89b4fa;
}
QHeaderView::section {
    background-color: #313244;
    color: #89b4fa;
    padding: 6px;
    border: none;
    font-weight: bold;
}
QTabWidget::pane {
    border: 1px solid #45475a;
    border-radius: 6px;
}
QTabBar::tab {
    background: #313244;
    color: #cdd6f4;
    padding: 8px 20px;
    border-top-left-radius: 5px;
    border-top-right-radius: 5px;
    margin-right: 2px;
}
QTabBar::tab:selected {
    background: #89b4fa;
    color: #1e1e2e;
    font-weight: bold;
}
QLabel#titleLabel {
    font-size: 30px;
    font-weight: bold;
    color: #cba6f7;
}
QLabel#subtitleLabel {
    font-size: 13px;
    color: #6c7086;
}
QLabel#pageTitle {
    font-size: 20px;
    font-weight: bold;
    color: #cba6f7;
}
QLabel#statsLabel {
    background-color: #313244;
    border: 1px solid #45475a;
    border-radius: 6px;
    padding: 6px 12px;
    color: #89b4fa;
    font-size: 12px;
}
QLabel#balanceLabel {
    font-size: 15px;
    font-weight: bold;
    color: #a6e3a1;
}
QLabel#notifLabel {
    color: #fab387;
    font-weight: bold;
}
QLabel#hintLabel {
    color: #585b70;
    font-size: 11px;
}
QLabel#profileLabel {
    font-size: 14px;
    line-height: 1.6;
    color: #cdd6f4;
    padding: 20px;
}
QScrollBar:vertical {
    background: #1e1e2e;
    width: 8px;
}
QScrollBar::handle:vertical {
    background: #45475a;
    border-radius: 4px;
}
QMessageBox {
    background-color: #1e1e2e;
}
QInputDialog {
    background-color: #1e1e2e;
}
)";

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_platform = Platform::getInstance();
    setWindowTitle("ShopNova — Multi-Shop E-Commerce Platform");
    resize(1100, 720);
    applyStyle();

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    m_loginPage = new LoginPage;
    connect(m_loginPage, &LoginPage::loginRequested,              this, &MainWindow::onLoginRequested);
    connect(m_loginPage, &LoginPage::registerCustomerRequested,   this, &MainWindow::onRegisterCustomer);
    connect(m_loginPage, &LoginPage::registerSellerRequested,     this, &MainWindow::onRegisterSeller);
    m_stack->addWidget(m_loginPage); // index 0
    m_stack->setCurrentIndex(PAGE_LOGIN);
}

MainWindow::~MainWindow() {}

void MainWindow::applyStyle() {
    qApp->setStyleSheet(APP_STYLE);
}

void MainWindow::goToPage(int index) {
    m_stack->setCurrentIndex(index);
}

void MainWindow::onLoginRequested(const QString& email, const QString& password) {
    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login", "Please enter email and password."); return;
    }
    User* user = m_platform->login(email, password);
    if (!user) {
        QMessageBox::warning(this, "Login Failed", "Invalid email or password."); return;
    }

    switch (user->getRole()) {
    case UserRole::ADMIN: {
        Admin* admin = dynamic_cast<Admin*>(user);
        if (m_adminPage) { m_stack->removeWidget(m_adminPage); delete m_adminPage; }
        m_adminPage = new AdminDashboard(admin, m_platform);
        connect(m_adminPage, &AdminDashboard::logoutRequested, this, &MainWindow::onLogout);
        m_stack->addWidget(m_adminPage);
        m_adminPage->refresh();
        goToPage(m_stack->indexOf(m_adminPage));
        break;
    }
    case UserRole::SELLER: {
        Seller* seller = dynamic_cast<Seller*>(user);
        if (m_sellerPage) { m_stack->removeWidget(m_sellerPage); delete m_sellerPage; }
        m_sellerPage = new SellerDashboard(seller, m_platform);
        connect(m_sellerPage, &SellerDashboard::logoutRequested, this, &MainWindow::onLogout);
        m_stack->addWidget(m_sellerPage);
        m_sellerPage->refresh();
        goToPage(m_stack->indexOf(m_sellerPage));
        break;
    }
    case UserRole::CUSTOMER: {
        Customer* customer = dynamic_cast<Customer*>(user);
        if (m_customerPage) { m_stack->removeWidget(m_customerPage); delete m_customerPage; }
        m_customerPage = new CustomerDashboard(customer, m_platform);
        connect(m_customerPage, &CustomerDashboard::logoutRequested, this, &MainWindow::onLogout);
        m_stack->addWidget(m_customerPage);
        m_customerPage->refresh();
        goToPage(m_stack->indexOf(m_customerPage));
        break;
    }
    }
}

void MainWindow::onRegisterCustomer(const QString& name, const QString& email,
                                    const QString& password, const QString& phone)
{
    if (name.isEmpty() || email.isEmpty() || password.isEmpty() || phone.isEmpty()) {
        QMessageBox::warning(this, "Register", "All fields are required."); return;
    }
    try {
        Customer* c = m_platform->registerCustomer(name, email, password, phone);
        QMessageBox::information(this, "Registered!",
                                 "Account created! Your ID: " + QString::number(c->getId()) +
                                     "\nYou can now login with your email.");
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString::fromStdString(e.what()));
    }
}

void MainWindow::onRegisterSeller(const QString& name, const QString& email,
                                  const QString& password, const QString& phone, const QString& storeName)
{
    if (name.isEmpty() || email.isEmpty() || password.isEmpty() || storeName.isEmpty()) {
        QMessageBox::warning(this, "Register", "All fields are required."); return;
    }
    Seller* s = m_platform->registerSeller(name, email, password, phone, storeName);
    QMessageBox::information(this, "Shop Created!",
                             "Seller account created!\nStore: " + s->getStoreName() +
                                 "\nID: " + QString::number(s->getId()) +
                                 "\nAn admin will verify your account soon.");
}

void MainWindow::onLogout() {
    goToPage(PAGE_LOGIN);
}
