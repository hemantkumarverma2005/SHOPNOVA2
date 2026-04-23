#include "shopnavcontroller.h"
#include <QVBoxLayout>
#include <QResizeEvent>

ShopNavController::ShopNavController(Platform* platform,
                                     Customer*  customer,
                                     QWidget*   parent)
    : QWidget(parent), m_platform(platform), m_customer(customer),
      m_detailPage(nullptr)
{
    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->setContentsMargins(0, 0, 0, 0);

    m_stack = new QStackedWidget(this);

    m_browserPage = new ShopBrowserWidget(
        m_platform,
        customer ? customer->getName() : "Customer",
        this);

    m_stack->addWidget(m_browserPage);   // index 0

    vl->addWidget(m_stack);

    // Cart panel overlay (hidden initially)
    m_cartPanel = new CartPanel(m_platform, m_customer, this);
    m_cartPanel->hide();

    // Connections
    connect(m_browserPage, &ShopBrowserWidget::shopSelected,
            this, &ShopNavController::goToShop);
    connect(m_browserPage, &ShopBrowserWidget::cartClicked,
            this, &ShopNavController::openCart);

    connect(m_cartPanel, &CartPanel::closeRequested,
            this, [this]() { m_cartPanel->slideOut(); });
    connect(m_cartPanel, &CartPanel::checkoutCompleted,
            this, [this]() {
                m_cartPanel->slideOut();
                emit checkoutRequested();
            });
    connect(m_cartPanel, &CartPanel::cartChanged,
            this, &ShopNavController::syncCartBadge);

    syncCartBadge();
}

void ShopNavController::goToShop(Seller* seller)
{
    rebuildDetailPage(seller);
    m_stack->setCurrentIndex(1);
}

void ShopNavController::goBack()
{
    m_stack->setCurrentIndex(0);
    syncCartBadge();
}

void ShopNavController::openCart()
{
    m_cartPanel->resize(size());
    m_cartPanel->refreshCart();
    m_cartPanel->slideIn();
    m_cartPanel->raise();
}

void ShopNavController::onCartUpdated(Product* product, int qty)
{
    if (!m_customer || !product) return;

    Cart& cart = m_customer->getCart();
    if (qty == 0) {
        cart.removeItem(product->getId());
    } else {
        bool found = false;
        for (CartItem& ci : const_cast<QVector<CartItem>&>(cart.getItems())) {
            if (ci.productId == product->getId()) {
                ci.quantity = qty;
                found = true;
                break;
            }
        }
        if (!found) {
            cart.addItem(product, qty);
        }
    }

    syncCartBadge();

    // Keep cart panel in sync if open
    if (m_cartPanel->isVisible())
        m_cartPanel->refreshCart();
}

void ShopNavController::onCheckout()
{
    // Handled via CartPanel::checkoutCompleted signal
}

void ShopNavController::rebuildDetailPage(Seller* seller)
{
    // Remove old detail page if any
    if (m_detailPage) {
        m_stack->removeWidget(m_detailPage);
        m_detailPage->deleteLater();
    }

    m_detailPage = new ShopDetailWidget(seller, m_platform, this);

    // Restore current cart quantities so +/- show correct state
    QMap<int, int> qtys;
    if (m_customer) {
        for (const CartItem& ci : m_customer->getCart().getItems())
            qtys[ci.productId] = ci.quantity;
    }
    m_detailPage->setCartQuantities(qtys);

    connect(m_detailPage, &ShopDetailWidget::backClicked,
            this, &ShopNavController::goBack);
    connect(m_detailPage, &ShopDetailWidget::cartUpdated,
            this, &ShopNavController::onCartUpdated);
    connect(m_detailPage, &ShopDetailWidget::cartBtnClicked,
            this, &ShopNavController::openCart);

    m_stack->addWidget(m_detailPage);   // index 1
}

void ShopNavController::syncCartBadge()
{
    int count = m_customer ? m_customer->getCart().getItemCount() : 0;
    m_browserPage->refreshCartBadge(count);
}

void ShopNavController::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    if (m_cartPanel && m_cartPanel->isVisible())
        m_cartPanel->resize(size());
}
