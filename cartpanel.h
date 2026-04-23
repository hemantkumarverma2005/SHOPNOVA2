#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPropertyAnimation>
#include "models.h"
#include "datastore.h"

// ============================================================
// CartItemRow  – one item in the cart panel
// ============================================================
class CartItemRow : public QFrame {
    Q_OBJECT
public:
    explicit CartItemRow(const CartItem& item,
                         Platform* platform,
                         QWidget* parent = nullptr);

signals:
    void quantityChanged(int productId, int newQty);

private:
    CartItem m_item;
    QLabel*  m_qtyLabel;
    int      m_qty;

    void updateQty(int delta);
};

// ============================================================
// CartPanel  – slide-in panel from the right
// ============================================================
class CartPanel : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int panelX READ panelX WRITE setPanelX)

public:
    explicit CartPanel(Platform* platform, Customer* customer, QWidget* parent = nullptr);

    void refreshCart();
    void slideIn();
    void slideOut();

    int  panelX() const;
    void setPanelX(int x);

signals:
    void closeRequested();
    void checkoutCompleted();   // emitted after successful order placement
    void cartChanged();

private slots:
    void onQtyChanged(int productId, int newQty);
    void onCheckout();

private:
    Platform*  m_platform;
    Customer*  m_customer;

    QWidget*     m_panel;
    QVBoxLayout* m_itemsLayout;
    QLabel*      m_countLabel;
    QLabel*      m_subtotalLabel;
    QLabel*      m_totalLabel;
    QPushButton* m_checkoutBtn;

    QPropertyAnimation* m_anim;

    void buildPanel();
    void rebuildItems();
    void updateTotals();

    // QR checkout dialog helper
    QString determineQrPath() const;

protected:
    void resizeEvent(QResizeEvent* e) override;
};
