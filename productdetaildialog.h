#pragma once
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include "models.h"

class ProductDetailDialog : public QDialog {
    Q_OBJECT
public:
    explicit ProductDetailDialog(const Product* product, QWidget *parent = nullptr);

signals:
    void addToCartRequested(int productId);

private:
    int m_productId;
    void buildUI(const Product* product);
    static QString generatePlaceholderColor(int productId);
};
