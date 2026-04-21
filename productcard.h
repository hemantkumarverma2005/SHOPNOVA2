#ifndef PRODUCTCARD_H
#define PRODUCTCARD_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QEnterEvent>
#include "models.h"

class ProductCard : public QFrame
{
    Q_OBJECT
public:
    explicit ProductCard(const Product* product, QWidget *parent = nullptr);

signals:
    void addToCartClicked(int productId);
    void cardClicked(int productId);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_productId;
    QString generatePlaceholderColor(int productId);
};

#endif // PRODUCTCARD_H
