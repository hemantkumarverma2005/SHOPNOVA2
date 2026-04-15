#pragma once
#include "models.h"
#include <memory>
#include <vector>

// ============================================================
//  PLATFORM STATS
// ============================================================
struct PlatformStats {
    int    totalCustomers;
    int    totalSellers;
    int    totalProducts;
    int    totalOrders;
    double totalRevenue;
    int    pendingOrders;
    int    pendingSellers;
};

// ============================================================
//  PLATFORM  (Singleton)
// ============================================================
class Platform {
    static Platform* s_instance;

    std::vector<std::unique_ptr<Customer>>  m_customers;
    std::vector<std::unique_ptr<Seller>>    m_sellers;
    std::vector<std::unique_ptr<Admin>>     m_admins;
    std::vector<std::unique_ptr<Product>>   m_products;
    std::vector<std::unique_ptr<Order>>     m_orders;
    QVector<Coupon>                         m_coupons;
    QVector<Notification>                   m_notifications;

    int    m_nextUserId    = 1;
    int    m_nextProductId = 1;
    double m_totalRevenue  = 0;
    QString m_platformName = "ShopNova";

    Platform();
    void seedData();
    void addElectronics(const QString& name, const QString& desc, const QString& brand,
                        double price, int stock, int sellerId, const QString& sellerName, int warranty);
    void addClothing(const QString& name, const QString& desc, const QString& brand,
                     double price, int stock, int sellerId, const QString& sellerName, const QString& material);
    void addBook(const QString& name, const QString& desc, const QString& brand,
                 double price, int stock, int sellerId, const QString& sellerName,
                 const QString& author, const QString& isbn);

public:
    static Platform* getInstance();
    QString getPlatformName() const { return m_platformName; }

    // USER MANAGEMENT
    Customer* registerCustomer(const QString& name, const QString& email,
                               const QString& password, const QString& phone);
    Seller*   registerSeller(const QString& name, const QString& email,
                             const QString& password, const QString& phone,
                             const QString& storeName);
    User*     login(const QString& email, const QString& password);

    // PRODUCT OPERATIONS
    const std::vector<std::unique_ptr<Product>>& getAllProducts() const { return m_products; }
    Product*              getProductById(int id);
    QVector<Product*>     searchProducts(const QString& query);
    QVector<Product*>     getProductsByCategory(const QString& category);
    QVector<Product*>     getProductsBySeller(int sellerId);
    int                   addProduct(std::unique_ptr<Product> p);
    void                  toggleProductStatus(int productId);

    // ORDER OPERATIONS
    Order*                placeOrder(Customer* customer, const Address& address, PaymentMethod method);
    Order*                getOrderById(int id);
    QVector<Order*>       getOrdersByCustomer(int customerId);
    const std::vector<std::unique_ptr<Order>>& getAllOrders() const { return m_orders; }
    bool                  cancelOrder(int orderId, int customerId);
    bool                  updateOrderStatus(int orderId, OrderStatus newStatus);

    // ADMIN
    PlatformStats getStats() const;
    void verifySeller(int sellerId, bool status);
    void toggleUserStatus(int userId);

    const std::vector<std::unique_ptr<Customer>>& getCustomers() const { return m_customers; }
    const std::vector<std::unique_ptr<Seller>>&   getSellers()   const { return m_sellers; }
    const std::vector<std::unique_ptr<Admin>>&    getAdmins()    const { return m_admins; }

    // NOTIFICATIONS
    void notify(int userId, const QString& msg, const QString& type);
    QVector<Notification*> getNotifications(int userId);

    // COUPONS
    Coupon* getCoupon(const QString& code);
    const QVector<Coupon>& getCoupons() const { return m_coupons; }

    // PERSISTENCE
    void saveToDisk();
    void loadFromDisk();

    friend class DataStore;
};

// ============================================================
//  FILE I/O
// ============================================================
class DataStore {
public:
    static void save(Platform* platform);
    static void load(Platform* platform);
    static void ensureDir();
};
