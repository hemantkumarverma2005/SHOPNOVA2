#include "shellwidget.h"
#include "ui_shellwidget.h"
#include <QScrollArea>
#include <QLineEdit>

ShellWidget::ShellWidget(QWidget *parent) : QWidget(parent), ui(new Ui::ShellWidget) {
    ui->setupUi(this);
    connect(ui->logoutBtn, &QPushButton::clicked, this, &ShellWidget::onLogoutClicked);
}

ShellWidget::~ShellWidget() {
    delete ui;
}

void ShellWidget::onLogoutClicked() {
    emit logoutRequested();
}

void ShellWidget::setSearchVisible(bool visible) {
    ui->searchBar->setVisible(visible);
}

void ShellWidget::configure(const QString &/*appTitle*/,
                             const QString &userLabel,
                             const QString &roleTag,
                             const QVector<NavItem> &navItems) {
    ui->userNameLbl->setText(userLabel);
    ui->roleLbl->setText(roleTag);

    // Set avatar initial
    if (!userLabel.isEmpty()) {
        ui->avatarLbl->setText(userLabel.left(1).toUpper());
    }

    // Clear old nav buttons
    for (auto *b : m_navBtns) { b->deleteLater(); }
    m_navBtns.clear();

    // Remove old pages
    while (ui->contentStack->count() > 0)
        ui->contentStack->removeWidget(ui->contentStack->widget(0));

    // Section label helper
    auto addSection = [&](const QString &text) {
        QLabel *sec = new QLabel(text.toUpper());
        sec->setStyleSheet(
            "font-size: 10px; font-weight: 700; color: #7070a0;"
            "letter-spacing: 1.2px; padding: 12px 8px 4px 8px;");
        ui->navLayout->addWidget(sec);
    };

    bool firstSection = true;
    for (int i = 0; i < navItems.size(); ++i) {
        const NavItem &item = navItems[i];

        // If icon is empty string treat as section header
        if (item.icon.isEmpty()) {
            if (!firstSection) ui->navLayout->addSpacing(8);
            addSection(item.label);
            firstSection = false;
            continue;
        }
        firstSection = false;

        QPushButton *btn = new QPushButton(item.icon + "  " + item.label);
        btn->setObjectName("navBtn");
        btn->setCheckable(true);
        btn->setFixedHeight(44);
        btn->setAutoExclusive(false);  // we handle manually
        ui->navLayout->addWidget(btn);
        m_navBtns.append(btn);
        
        if (item.page) {
            ui->contentStack->addWidget(item.page);
        } else {
            ui->contentStack->addWidget(new QWidget);
        }

        int idx = m_navBtns.size() - 1;
        connect(btn, &QPushButton::clicked, this, [this, idx]() {
            selectNav(idx);
        });
    }

    ui->navLayout->addStretch();
    if (!m_navBtns.isEmpty()) selectNav(0);
}

void ShellWidget::selectNav(int index) {
    for (int i = 0; i < m_navBtns.size(); ++i)
        m_navBtns[i]->setChecked(i == index);
    if (index >= 0 && index < ui->contentStack->count())
        ui->contentStack->setCurrentIndex(index);
}

void ShellWidget::setHeaderRight(QWidget *w) {
    ui->headerL->addWidget(w);
}
