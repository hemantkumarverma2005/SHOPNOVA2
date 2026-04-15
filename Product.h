#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>

class Product {
public:
    int id;
    QString name;
    int price;

    Product(int id, QString name, int price);
};

#endif