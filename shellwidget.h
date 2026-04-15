#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QFrame>
#include <QVector>

struct NavItem {
    QString icon;
    QString label;
    QWidget *page;
};

class ShellWidget : public QWidget {
    Q_OBJECT
public:
    explicit ShellWidget(QWidget *parent = nullptr);

    // Call before showing — sets up sidebar + content
    void configure(const QString &appTitle,
                   const QString &userLabel,
                   const QString &roleTag,
                   const QVector<NavItem> &navItems);

    void setHeaderRight(QWidget *w);  // optional top-right area

signals:
    void logoutRequested();

private:
    QFrame        *m_sidebar;
    QWidget       *m_header;
    QStackedWidget *m_contentStack;
    QVBoxLayout   *m_navLayout;
    QLabel        *m_userNameLbl;
    QLabel        *m_roleLbl;
    QVector<QPushButton*> m_navBtns;

    void buildSkeleton();
    void selectNav(int index);
};
