#include "datastore.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <stdexcept>

// ── Order static id counter ──
int Order::s_nextId = 1000;

Platform* Platform::s_instance = nullptr;

Platform* Platform::getInstance() {
    if (!s_instance) s_instance = new Platform();
    return s_instance;
}

Platform::Platform() {
    DataStore::ensureDir();
    loadFromDisk();
    if (m_admins.empty() && m_customers.empty()) {
        seedData();
    }
}

// ── Seed default data ─────────────────────────────────────────
void Platform::seedData() {
    // Admin
    auto admin = std::make_unique<Admin>(m_nextUserId++, "Super Admin",
        "admin@shopnova.com", "admin123", "9999999999", 3);
    m_admins.push_back(std::move(admin));

    // Sellers
    auto s1 = std::make_unique<Seller>(m_nextUserId++, "Ravi Electronics",
        "ravi@seller.com", "pass123", "9876543210", "RaviTech Store");
    s1->setVerified(true); s1->setGST("27AAACR5055K1ZS");
    m_sellers.push_back(std::move(s1));

    auto s2 = std::make_unique<Seller>(m_nextUserId++, "Fashion Hub",
        "fashion@seller.com", "pass123", "9123456780", "Fashion Hub India");
    s2->setVerified(true);
    m_sellers.push_back(std::move(s2));

    auto s3 = std::make_unique<Seller>(m_nextUserId++, "Book World",
        "books@seller.com", "pass123", "9000000003", "The Book World");
    s3->setVerified(true);
    m_sellers.push_back(std::move(s3));

    // Customers
    auto c1 = std::make_unique<Customer>(m_nextUserId++, "Arjun Sharma",
        "arjun@gmail.com", "pass123", "8888888888");
    c1->addAddress({"MG Road, Block 4", "Guwahati", "Assam", "781001", "India"});
    c1->addWallet(500);
    m_customers.push_back(std::move(c1));

    auto c2 = std::make_unique<Customer>(m_nextUserId++, "Priya Nair",
        "priya@gmail.com", "pass123", "7777777777");
    c2->addAddress({"Anna Nagar", "Chennai", "Tamil Nadu", "600040", "India"});
    m_customers.push_back(std::move(c2));

    // Products
    addElectronics("Samsung Galaxy S24", "Latest flagship with 200MP camera",
        "Samsung", 74999, 50, 2, "RaviTech Store", 12);
    addElectronics("Apple iPhone 15", "A16 Bionic chip, Dynamic Island",
        "Apple", 79999, 30, 2, "RaviTech Store", 12);
    addElectronics("Sony WH-1000XM5", "Industry-leading noise cancellation headphones",
        "Sony", 29990, 100, 2, "RaviTech Store", 12);
    addElectronics("Dell XPS 15 Laptop", "Intel i9, 32GB RAM, 1TB SSD",
        "Dell", 149999, 20, 2, "RaviTech Store", 24);
    addElectronics("LG OLED 55 TV", "4K OLED, Dolby Vision, WebOS",
        "LG", 89999, 15, 2, "RaviTech Store", 12);

    addClothing("Levi's 511 Slim Fit Jeans", "Premium denim, slim fit",
        "Levi's", 2999, 200, 3, "Fashion Hub India", "Denim");
    addClothing("Nike Dri-FIT T-Shirt", "Performance running tee",
        "Nike", 1499, 300, 3, "Fashion Hub India", "Polyester");
    addClothing("Woodland Leather Jacket", "Genuine leather, winter wear",
        "Woodland", 4999, 50, 3, "Fashion Hub India", "Leather");

    addBook("Clean Code", "A handbook of agile software craftsmanship",
        "Pearson", 499, 500, 4, "The Book World", "Robert C. Martin", "978-0132350884");
    addBook("The Alchemist", "A philosophical novel about following your dreams",
        "HarperCollins", 299, 1000, 4, "The Book World", "Paulo Coelho", "978-0062315007");

    m_products[0]->setDiscount(10);
    m_products[1]->setDiscount(5);
    m_products[2]->setDiscount(15);
    m_products[5]->setDiscount(20);

    m_coupons.append({"SAVE100", 10, 100, 500,  true, 100, 0, "2026-12-31"});
    m_coupons.append({"NEWUSER", 15, 200, 300,  true,  50, 0, "2026-12-31"});
    m_coupons.append({"FLAT50",   5,  50, 999,  true, 500, 0, "2026-06-30"});

    saveToDisk();
}

void Platform::addElectronics(const QString& name, const QString& desc, const QString& brand,
    double price, int stock, int sellerId, const QString& sellerName, int warranty)
{
    auto p = std::make_unique<Electronics>(m_nextProductId++, name, desc, brand, price, stock, sellerId, sellerName, warranty);
    m_products.push_back(std::move(p));
}
void Platform::addClothing(const QString& name, const QString& desc, const QString& brand,
    double price, int stock, int sellerId, const QString& sellerName, const QString& material)
{
    auto p = std::make_unique<Clothing>(m_nextProductId++, name, desc, brand, price, stock, sellerId, sellerName, material);
    m_products.push_back(std::move(p));
}
void Platform::addBook(const QString& name, const QString& desc, const QString& brand,
    double price, int stock, int sellerId, const QString& sellerName,
    const QString& author, const QString& isbn)
{
    auto p = std::make_unique<Books>(m_nextProductId++, name, desc, brand, price, stock, sellerId, sellerName, author, isbn);
    m_products.push_back(std::move(p));
}

// ── USER MANAGEMENT ──────────────────────────────────────────
Customer* Platform::registerCustomer(const QString& name, const QString& email,
    const QString& password, const QString& phone)
{
    for (auto& c : m_customers)
        if (c->getEmail() == email)
            throw std::runtime_error("Email already registered");
    auto c = std::make_unique<Customer>(m_nextUserId++, name, email, password, phone);
    Customer* ptr = c.get();
    m_customers.push_back(std::move(c));
    notify(ptr->getId(), "Welcome to " + m_platformName + "! Happy Shopping 🛍️", "system");
    saveToDisk();
    return ptr;
}

Seller* Platform::registerSeller(const QString& name, const QString& email,
    const QString& password, const QString& phone, const QString& storeName)
{
    auto s = std::make_unique<Seller>(m_nextUserId++, name, email, password, phone, storeName);
    Seller* ptr = s.get();
    m_sellers.push_back(std::move(s));
    notify(ptr->getId(), "Your seller account is under review.", "system");
    saveToDisk();
    return ptr;
}

User* Platform::login(const QString& email, const QString& password) {
    for (auto& a : m_admins)
        if (a->getEmail() == email && a->authenticate(password)) return a.get();
    for (auto& s : m_sellers)
        if (s->getEmail() == email && s->authenticate(password)) return s.get();
    for (auto& c : m_customers)
        if (c->getEmail() == email && c->authenticate(password)) return c.get();
    return nullptr;
}

// ── PRODUCTS ─────────────────────────────────────────────────
Product* Platform::getProductById(int id) {
    for (auto& p : m_products)
        if (p->getId() == id) return p.get();
    return nullptr;
}

QVector<Product*> Platform::searchProducts(const QString& query) {
    QVector<Product*> results;
    QString q = query.toLower();
    for (auto& p : m_products) {
        if (!p->getIsActive()) continue;
        if (p->getName().toLower().contains(q) ||
            p->getCategory().toLower().contains(q) ||
            p->getBrand().toLower().contains(q))
            results.append(p.get());
    }
    return results;
}

QVector<Product*> Platform::getProductsByCategory(const QString& category) {
    QVector<Product*> results;
    for (auto& p : m_products)
        if (p->getIsActive() && p->getCategory() == category)
            results.append(p.get());
    return results;
}

QVector<Product*> Platform::getProductsBySeller(int sellerId) {
    QVector<Product*> results;
    for (auto& p : m_products)
        if (p->getSellerId() == sellerId)
            results.append(p.get());
    return results;
}

int Platform::addProduct(std::unique_ptr<Product> p) {
    p->setId(m_nextProductId++);
    int id = p->getId();
    m_products.push_back(std::move(p));
    saveToDisk();
    return id;
}

void Platform::toggleProductStatus(int productId) {
    Product* p = getProductById(productId);
    if (p) { p->setActive(!p->getIsActive()); saveToDisk(); }
}

void Platform::deleteProduct(int productId) {
    auto it = std::remove_if(m_products.begin(), m_products.end(),
                             [productId](const std::unique_ptr<Product>& p) {
                                 return p->getId() == productId;
                             });
    if (it != m_products.end()) {
        m_products.erase(it, m_products.end());
        saveToDisk();
    }
}

// ── ORDERS ───────────────────────────────────────────────────
Order* Platform::placeOrder(Customer* customer, const Address& address, PaymentMethod method) {
    if (customer->getCart().isEmpty())
        throw std::runtime_error("Cart is empty");
    for (auto& ci : customer->getCart().getItems()) {
        Product* p = getProductById(ci.productId);
        if (!p || p->getStock() < ci.quantity)
            throw std::runtime_error("Insufficient stock for: " + (p ? p->getName() : QString("unknown")).toStdString());
    }
    for (auto& ci : customer->getCart().getItems()) {
        Product* p = getProductById(ci.productId);
        if (p) p->reduceStock(ci.quantity);
    }

    auto order = std::make_unique<Order>(
        customer->getId(), customer->getName(),
        customer->getCart().getItems(), address, method);

    Order* ptr = order.get();
    m_orders.push_back(std::move(order));

    customer->addOrderId(ptr->getId());
    customer->getCart().clear();
    customer->updateMembership();
    customer->addPoints((int)(ptr->getTotalAmount() / 100));
    m_totalRevenue += ptr->getTotalAmount();

    notify(customer->getId(),
           "Order #" + QString::number(ptr->getId()) + " placed! ₹" +
           QString::number((int)ptr->getTotalAmount()), "order");
    saveToDisk();
    return ptr;
}

Order* Platform::getOrderById(int id) {
    for (auto& o : m_orders)
        if (o->getId() == id) return o.get();
    return nullptr;
}

QVector<Order*> Platform::getOrdersByCustomer(int customerId) {
    QVector<Order*> result;
    for (auto& o : m_orders)
        if (o->getCustomerId() == customerId) result.append(o.get());
    return result;
}

bool Platform::cancelOrder(int orderId, int customerId) {
    Order* o = getOrderById(orderId);
    if (!o || o->getCustomerId() != customerId || !o->canCancel()) return false;
    o->setStatus(OrderStatus::CANCELLED);
    for (auto& item : o->getItems()) {
        Product* p = getProductById(item.productId);
        if (p) p->restoreStock(item.quantity);
    }
    notify(customerId, "Order #" + QString::number(orderId) + " cancelled.", "order");
    saveToDisk();
    return true;
}

bool Platform::updateOrderStatus(int orderId, OrderStatus newStatus) {
    Order* o = getOrderById(orderId);
    if (!o) return false;
    o->setStatus(newStatus);
    notify(o->getCustomerId(),
           "Order #" + QString::number(orderId) + " is now " + o->statusString(), "order");
    saveToDisk();
    return true;
}

// ── ADMIN ─────────────────────────────────────────────────────
PlatformStats Platform::getStats() const {
    int pending = 0, pendingSellers = 0;
    for (auto& o : m_orders) if (o->getStatus() == OrderStatus::PENDING) ++pending;
    for (auto& s : m_sellers) if (!s->getIsVerified()) ++pendingSellers;
    return {(int)m_customers.size(), (int)m_sellers.size(), (int)m_products.size(),
            (int)m_orders.size(), m_totalRevenue, pending, pendingSellers};
}

void Platform::verifySeller(int sellerId, bool status) {
    for (auto& s : m_sellers)
        if (s->getId() == sellerId) { s->setVerified(status); saveToDisk(); return; }
}

void Platform::toggleUserStatus(int userId) {
    for (auto& c : m_customers)
        if (c->getId() == userId) { c->setActive(!c->getIsActive()); saveToDisk(); return; }
}

// ── NOTIFICATIONS ─────────────────────────────────────────────
void Platform::notify(int userId, const QString& msg, const QString& type) {
    m_notifications.append({userId, msg, type, false,
        QDateTime::currentDateTime().toString("hh:mm dd/MM")});
}

QVector<Notification*> Platform::getNotifications(int userId) {
    QVector<Notification*> result;
    for (auto& n : m_notifications)
        if (n.userId == userId && !n.isRead) result.append(&n);
    return result;
}

Coupon* Platform::getCoupon(const QString& code) {
    for (auto& c : m_coupons)
        if (c.code == code && c.isActive) return &c;
    return nullptr;
}

// ── PERSISTENCE ───────────────────────────────────────────────
void DataStore::ensureDir() { 
    QDir().mkpath("data"); 
    QDir().mkpath("data/images");
}

void Platform::saveToDisk() { DataStore::save(this); }

void Platform::loadFromDisk() { DataStore::load(this); }

void DataStore::save(Platform* p) {
    ensureDir();

    // Save products
    {
        QFile f("data/products.tsv");
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&f);
            for (auto& prod : p->getAllProducts()) {
                out << prod->getId()       << "\t"
                    << prod->getType()     << "\t"
                    << prod->getName()     << "\t"
                    << prod->getDescription() << "\t"
                    << prod->getCategory() << "\t"
                    << prod->getBrand()    << "\t"
                    << prod->getPrice()    << "\t"
                    << prod->getDiscount() << "\t"
                    << prod->getStock()    << "\t"
                    << prod->getSellerId() << "\t"
                    << prod->getSellerName() << "\t"
                    << (prod->getIsActive() ? "1" : "0") << "\t"
                    << prod->getImagePath() << "\n";
            }
        }
    }

    // Save customers (basic info)
    {
        QFile f("data/customers.tsv");
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&f);
            for (auto& c : p->getCustomers()) {
                QString addr = "";
                if (c->getDefaultAddress())
                    addr = c->getDefaultAddress()->street + "|" +
                           c->getDefaultAddress()->city + "|" +
                           c->getDefaultAddress()->state + "|" +
                           c->getDefaultAddress()->pincode + "|" +
                           c->getDefaultAddress()->country;
                out << c->getId()      << "\t"
                    << c->getName()    << "\t"
                    << c->getEmail()   << "\t"
                    << c->getPassword()<< "\t"
                    << c->getPhone()   << "\t"
                    << c->getWalletBalance() << "\t"
                    << c->getRewardPoints()  << "\t"
                    << c->getMembershipTier()<< "\t"
                    << addr << "\n";
            }
        }
    }

    // Save sellers
    {
        QFile f("data/sellers.tsv");
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&f);
            for (auto& s : p->getSellers()) {
                out << s->getId()          << "\t"
                    << s->getName()        << "\t"
                    << s->getEmail()       << "\t"
                    << s->getPassword()    << "\t"
                    << s->getPhone()       << "\t"
                    << s->getStoreName()   << "\t"
                    << (s->getIsVerified() ? "1" : "0") << "\t"
                    << s->getGST()         << "\t"
                    << s->getTotalRevenue()<< "\n";
            }
        }
    }

    // Save admins
    {
        QFile f("data/admins.tsv");
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&f);
            for (auto& a : p->getAdmins()) {
                out << a->getId()      << "\t"
                    << a->getName()    << "\t"
                    << a->getEmail()   << "\t"
                    << a->getPassword()<< "\t"
                    << a->getPhone()   << "\t"
                    << a->getAdminLevel() << "\n";
            }
        }
    }

    // Save orders
    {
        QFile f("data/orders.tsv");
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&f);
            for (auto& o : p->getAllOrders()) {
                // Items packed as: pid,name,qty,price,discount;...
                QString items;
                for (auto& it : o->getItems()) {
                    if (!items.isEmpty()) items += ";";
                    items += QString::number(it.productId) + "," + it.productName + "," +
                             QString::number(it.quantity) + "," +
                             QString::number(it.priceAtPurchase) + "," +
                             QString::number(it.discountAtPurchase);
                }
                out << o->getId()          << "\t"
                    << o->getCustomerId()  << "\t"
                    << o->getCustomerName()<< "\t"
                    << (int)o->getStatus() << "\t"
                    << o->getTotalAmount() << "\t"
                    << o->getOrderDate()   << "\t"
                    << o->getTrackingId()  << "\t"
                    << items               << "\n";
            }
        }
    }
}

void DataStore::load(Platform* p) {
    // Load admins
    {
        QFile f("data/admins.tsv");
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            while (!in.atEnd()) {
                QStringList parts = in.readLine().split('\t');
                if (parts.size() >= 6) {
                    auto a = std::make_unique<Admin>(
                        parts[0].toInt(), parts[1], parts[2], parts[3], parts[4],
                        parts[5].toInt());
                    p->m_admins.push_back(std::move(a));
                    if (parts[0].toInt() >= p->m_nextUserId) p->m_nextUserId = parts[0].toInt() + 1;
                }
            }
        }
    }

    // Load sellers
    {
        QFile f("data/sellers.tsv");
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            while (!in.atEnd()) {
                QStringList parts = in.readLine().split('\t');
                if (parts.size() >= 7) {
                    auto s = std::make_unique<Seller>(
                        parts[0].toInt(), parts[1], parts[2], parts[3], parts[4], parts[5]);
                    s->setVerified(parts[6] == "1");
                    if (parts.size() > 7) s->setGST(parts[7]);
                    p->m_sellers.push_back(std::move(s));
                    if (parts[0].toInt() >= p->m_nextUserId) p->m_nextUserId = parts[0].toInt() + 1;
                }
            }
        }
    }

    // Load customers
    {
        QFile f("data/customers.tsv");
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            while (!in.atEnd()) {
                QStringList parts = in.readLine().split('\t');
                if (parts.size() >= 5) {
                    auto c = std::make_unique<Customer>(
                        parts[0].toInt(), parts[1], parts[2], parts[3], parts[4]);
                    if (parts.size() > 5) c->addWallet(parts[5].toDouble());
                    if (parts.size() > 8 && !parts[8].isEmpty()) {
                        QStringList ap = parts[8].split('|');
                        if (ap.size() >= 5)
                            c->addAddress({ap[0], ap[1], ap[2], ap[3], ap[4]});
                    }
                    p->m_customers.push_back(std::move(c));
                    if (parts[0].toInt() >= p->m_nextUserId) p->m_nextUserId = parts[0].toInt() + 1;
                }
            }
        }
    }

    // Load products
    {
        QFile f("data/products.tsv");
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            while (!in.atEnd()) {
                QStringList parts = in.readLine().split('\t');
                if (parts.size() >= 12) {
                    int id       = parts[0].toInt();
                    QString type = parts[1];
                    QString name = parts[2];
                    QString desc = parts[3];
                    QString cat  = parts[4];
                    QString brand= parts[5];
                    double price = parts[6].toDouble();
                    double disc  = parts[7].toDouble();
                    int stock    = parts[8].toInt();
                    int sid      = parts[9].toInt();
                    QString sname= parts[10];
                    bool active  = parts[11] == "1";

                    std::unique_ptr<Product> prod;
                    if (type == "Electronics")
                        prod = std::make_unique<Electronics>(id, name, desc, brand, price, stock, sid, sname, 12);
                    else if (type == "Clothing")
                        prod = std::make_unique<Clothing>(id, name, desc, brand, price, stock, sid, sname, "");
                    else if (type == "Books")
                        prod = std::make_unique<Books>(id, name, desc, brand, price, stock, sid, sname, "", "");
                    else
                        prod = std::make_unique<Electronics>(id, name, desc, brand, price, stock, sid, sname, 0);

                    prod->setDiscount(disc);
                    prod->setActive(active);
                    if (parts.size() >= 13) {
                        prod->setImagePath(parts[12]);
                    }
                    p->m_products.push_back(std::move(prod));
                    if (id >= p->m_nextProductId) p->m_nextProductId = id + 1;
                }
            }
        }
    }

    // Load orders
    {
        QFile f("data/orders.tsv");
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            while (!in.atEnd()) {
                QStringList parts = in.readLine().split('\t');
                if (parts.size() >= 7) {
                    auto o = std::make_unique<Order>();
                    o->setId(parts[0].toInt());
                    o->setCustomerId(parts[1].toInt());
                    o->setCustomerName(parts[2]);
                    o->setStatus((OrderStatus)parts[3].toInt());
                    o->setTotalAmount(parts[4].toDouble());
                    o->setOrderDate(parts[5]);
                    o->setTrackingId(parts[6]);
                    if (parts.size() > 7 && !parts[7].isEmpty()) {
                        for (const QString& itemStr : parts[7].split(';')) {
                            QStringList ip = itemStr.split(',');
                            if (ip.size() >= 5) {
                                OrderItem oi;
                                oi.productId          = ip[0].toInt();
                                oi.productName        = ip[1];
                                oi.brand              = QString();
                                oi.quantity           = ip[2].toInt();
                                oi.priceAtPurchase    = ip[3].toDouble();
                                oi.discountAtPurchase = ip[4].toDouble();
                                o->addItem(oi);
                            }
                        }
                    }
                    p->m_totalRevenue += o->getTotalAmount();
                    p->m_orders.push_back(std::move(o));
                }
            }
        }
    }
}
