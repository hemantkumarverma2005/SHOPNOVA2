#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QFrame>
#include <QVector>
#include <QString>

namespace Ui {
class ShellWidget;
}

struct NavItem {
    QString icon;
    QString label;
    QWidget *page;
};

class ShellWidget : public QWidget {
    Q_OBJECT
public:
    explicit ShellWidget(QWidget *parent = nullptr);
    ~ShellWidget();

    // Call before showing — sets up sidebar + content
    void configure(const QString &appTitle,
                   const QString &userLabel,
                   const QString &roleTag,
                   const QVector<NavItem> &navItems);

    void setHeaderRight(QWidget *w);  // optional top-right area
    void setSearchVisible(bool visible);  // hide search bar for admin/seller

signals:
    void logoutRequested();

private slots:
    void onLogoutClicked();

private:
    Ui::ShellWidget *ui;
    QVector<QPushButton*> m_navBtns;

    void selectNav(int index);
};
