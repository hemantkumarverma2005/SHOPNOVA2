#pragma once
#include <QString>
#include <QVector>
#include <QMap>
#include <QDateTime>
#include <memory>
#include <algorithm>

// ============================================================
//  ENUMS
// ============================================================
enum class UserRole     { ADMIN, SELLER, CUSTOMER };
enum class OrderStatus  { PENDING, CONFIRMED, SHIPPED, DELIVERED, CANCELLED, RETURNED };
enum class PaymentMethod{ COD, UPI, CARD, NETBANKING, WALLET };
enum class PaymentStatus{ PENDING, SUCCESS, FAILED, REFUNDED };

// ============================================================
//  ADDRESS
// ============================================================
struct Address {
    QString street, city, state, pincode, country;
    QString toString() const {
        return street + ", " + city + ", " + state + " - " + pincode + ", " + country;
    }
    bool isEmpty() const { return street.isEmpty(); }
};

// ============================================================
//  REVIEW
// ============================================================
struct Review {
    int     customerId;
    QString customerName;
    int     rating;      // 1-5
    QString comment;
    QString date;
};

// ============================================================
//  PRODUCT (base class)
// ============================================================
class Product {
protected:
    int     m_id;
    QString m_name;
    QString m_description;
    QString m_category;
    QString m_brand;
    double  m_price;
    double  m_discountPercent;
    int     m_stock;
    int     m_sellerId;
    QString m_sellerName;
    bool    m_isActive;
    QString m_imagePath;
    QVector<Review> m_reviews;

public:
    Product() : m_id(0), m_price(0), m_discountPercent(0), m_stock(0), m_sellerId(0), m_isActive(true) {}
    Product(int id, const QString& name, const QString& desc,
            const QString& category, const QString& brand,
            double price, int stock, int sellerId, const QString& sellerName)
        : m_id(id), m_name(name), m_description(desc), m_category(category), m_brand(brand),
          m_price(price), m_discountPercent(0), m_stock(stock),
          m_sellerId(sellerId), m_sellerName(sellerName), m_isActive(true) {}

    virtual ~Product() = default;

    int     getId()          const { return m_id; }
    QString getName()        const { return m_name; }
    QString getDescription() const { return m_description; }
    QString getCategory()    const { return m_category; }
    QString getBrand()       const { return m_brand; }
    double  getPrice()       const { return m_price; }
    double  getDiscount()    const { return m_discountPercent; }
    int     getStock()       const { return m_stock; }
    int     getSellerId()    const { return m_sellerId; }
    QString getSellerName()  const { return m_sellerName; }
    bool    getIsActive()    const { return m_isActive; }
    QString getImagePath()   const { return m_imagePath; }

    double getDiscountedPrice() const {
        return m_price * (1.0 - m_discountPercent / 100.0);
    }

    double getAverageRating() const {
        if (m_reviews.isEmpty()) return 0.0;
        double sum = 0;
        for (auto& r : m_reviews) sum += r.rating;
        return sum / m_reviews.size();
    }

    void setPrice(double p)      { m_price = p; }
    void setDiscount(double d)   { m_discountPercent = d; }
    void setStock(int s)         { m_stock = s; }
    void setActive(bool a)       { m_isActive = a; }
    void setName(const QString& n)        { m_name = n; }
    void setDescription(const QString& d) { m_description = d; }
    void setSellerId(int id)     { m_sellerId = id; }
    void setSellerName(const QString& n)  { m_sellerName = n; }
    void setId(int id)           { m_id = id; }
    void setImagePath(const QString& p) { m_imagePath = p; }

    bool reduceStock(int qty) {
        if (m_stock < qty) return false;
        m_stock -= qty;
        return true;
    }
    void restoreStock(int qty) { m_stock += qty; }

    void addReview(const Review& r) { m_reviews.append(r); }
    const QVector<Review>& getReviews() const { return m_reviews; }

    virtual QString getType() const { return "General"; }
};

// ============================================================
//  PRODUCT SUBTYPES
// ============================================================
class Electronics : public Product {
    int     m_warrantyMonths;
    QString m_specifications;
public:
    Electronics(int id, const QString& name, const QString& desc,
                const QString& brand, double price, int stock,
                int sellerId, const QString& sellerName, int warranty)
        : Product(id, name, desc, "Electronics", brand, price, stock, sellerId, sellerName),
          m_warrantyMonths(warranty) {}

    QString getType() const override { return "Electronics"; }
    int getWarranty() const { return m_warrantyMonths; }
};

class Clothing : public Product {
    QString m_material;
    QVector<QString> m_sizes;
public:
    Clothing(int id, const QString& name, const QString& desc,
             const QString& brand, double price, int stock,
             int sellerId, const QString& sellerName, const QString& material)
        : Product(id, name, desc, "Clothing", brand, price, stock, sellerId, sellerName),
          m_material(material) {}

    QString getType() const override { return "Clothing"; }
    QString getMaterial() const { return m_material; }
};

class Books : public Product {
    QString m_author;
    QString m_isbn;
public:
    Books(int id, const QString& name, const QString& desc,
          const QString& brand, double price, int stock,
          int sellerId, const QString& sellerName,
          const QString& author, const QString& isbn)
        : Product(id, name, desc, "Books", brand, price, stock, sellerId, sellerName),
          m_author(author), m_isbn(isbn) {}

    QString getType() const override { return "Books"; }
    QString getAuthor() const { return m_author; }
    QString getIsbn()   const { return m_isbn; }
};

// ============================================================
//  CART
// ============================================================
struct CartItem {
    int     productId;
    QString productName;
    double  price;
    double  discountPercent;
    int     quantity;
    QString selectedSize;

    double getTotal() const {
        return price * (1.0 - discountPercent / 100.0) * quantity;
    }
};

class Cart {
    int              m_customerId;
    QVector<CartItem> m_items;
public:
    explicit Cart(int cid = 0) : m_customerId(cid) {}

    void addItem(Product* p, int qty = 1) {
        for (auto& item : m_items) {
            if (item.productId == p->getId()) {
                item.quantity += qty;
                return;
            }
        }
        m_items.append({p->getId(), p->getName(), p->getPrice(), p->getDiscount(), qty, ""});
    }

    bool removeItem(int productId) {
        for (int i = 0; i < m_items.size(); ++i) {
            if (m_items[i].productId == productId) {
                m_items.remove(i);
                return true;
            }
        }
        return false;
    }

    void clear()                         { m_items.clear(); }
    bool isEmpty()                 const { return m_items.isEmpty(); }
    const QVector<CartItem>& getItems() const { return m_items; }

    double getSubtotal() const {
        double t = 0;
        for (auto& i : m_items) t += i.getTotal();
        return t;
    }
    double getDeliveryCharge() const { return getSubtotal() >= 499 ? 0 : 40; }
    double getTotal()          const { return getSubtotal() + getDeliveryCharge(); }
    int    getItemCount()      const {
        int c = 0;
        for (auto& i : m_items) c += i.quantity;
        return c;
    }
};

// ============================================================
//  ORDER
// ============================================================
struct OrderItem {
    int     productId;
    QString productName;
    QString brand;
    int     quantity;
    double  priceAtPurchase;
    double  discountAtPurchase;
    double  getTotal() const {
        return priceAtPurchase * (1.0 - discountAtPurchase / 100.0) * quantity;
    }
};

struct Payment {
    int           orderId;
    PaymentMethod method;
    PaymentStatus status;
    double        amount;
    QString       transactionId;
    QString       timestamp;

    QString methodString() const {
        switch(method) {
            case PaymentMethod::COD:        return "Cash on Delivery";
            case PaymentMethod::UPI:        return "UPI";
            case PaymentMethod::CARD:       return "Credit/Debit Card";
            case PaymentMethod::NETBANKING: return "Net Banking";
            case PaymentMethod::WALLET:     return "Wallet";
        }
        return "Unknown";
    }
    QString statusString() const {
        switch(status) {
            case PaymentStatus::PENDING:  return "Pending";
            case PaymentStatus::SUCCESS:  return "Success";
            case PaymentStatus::FAILED:   return "Failed";
            case PaymentStatus::REFUNDED: return "Refunded";
        }
        return "Unknown";
    }
};

class Order {
    static int s_nextId;
    int              m_id;
    int              m_customerId;
    QString          m_customerName;
    QVector<OrderItem> m_items;
    Address          m_deliveryAddress;
    OrderStatus      m_status;
    Payment          m_payment;
    double           m_totalAmount;
    QString          m_orderDate;
    QString          m_trackingId;

public:
    Order() : m_id(0), m_customerId(0), m_status(OrderStatus::PENDING), m_totalAmount(0) {}

    Order(int customerId, const QString& customerName,
          const QVector<CartItem>& cartItems,
          const Address& address, PaymentMethod method)
        : m_id(++s_nextId), m_customerId(customerId), m_customerName(customerName),
          m_deliveryAddress(address), m_status(OrderStatus::PENDING)
    {
        m_totalAmount = 0;
        for (auto& ci : cartItems) {
            OrderItem oi;
            oi.productId          = ci.productId;
            oi.productName        = ci.productName;
            oi.brand              = QString();
            oi.quantity           = ci.quantity;
            oi.priceAtPurchase    = ci.price;
            oi.discountAtPurchase = ci.discountPercent;
            m_totalAmount += oi.getTotal();
            m_items.append(oi);
        }
        if (m_totalAmount < 499) m_totalAmount += 40;

        m_payment.orderId       = m_id;
        m_payment.method        = method;
        m_payment.amount        = m_totalAmount;
        m_payment.status        = (method == PaymentMethod::COD)
                                  ? PaymentStatus::PENDING : PaymentStatus::SUCCESS;
        m_payment.transactionId = "TXN" + QString::number(m_id);
        m_orderDate             = QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm");
        m_trackingId            = "TRK" + QString::number(m_id * 1000 + rand() % 999);
    }

    // For loading from file
    void setId(int id)               { m_id = id; if (id > s_nextId) s_nextId = id; }
    void setCustomerId(int id)       { m_customerId = id; }
    void setCustomerName(const QString& n) { m_customerName = n; }
    void setStatus(OrderStatus s)    { m_status = s; }
    void setTotalAmount(double a)    { m_totalAmount = a; }
    void setOrderDate(const QString& d)  { m_orderDate = d; }
    void setTrackingId(const QString& t) { m_trackingId = t; }
    void setPayment(const Payment& p)    { m_payment = p; }
    void setAddress(const Address& a)    { m_deliveryAddress = a; }
    void addItem(const OrderItem& oi)    { m_items.append(oi); }

    int          getId()          const { return m_id; }
    int          getCustomerId()  const { return m_customerId; }
    QString      getCustomerName()const { return m_customerName; }
    OrderStatus  getStatus()      const { return m_status; }
    double       getTotalAmount() const { return m_totalAmount; }
    QString      getOrderDate()   const { return m_orderDate; }
    QString      getTrackingId()  const { return m_trackingId; }
    const Payment&     getPayment() const { return m_payment; }
    const Address&     getAddress() const { return m_deliveryAddress; }
    const QVector<OrderItem>& getItems() const { return m_items; }

    bool canCancel() const {
        return m_status == OrderStatus::PENDING || m_status == OrderStatus::CONFIRMED;
    }

    QString statusString() const {
        switch(m_status) {
            case OrderStatus::PENDING:   return "Pending";
            case OrderStatus::CONFIRMED: return "Confirmed";
            case OrderStatus::SHIPPED:   return "Shipped";
            case OrderStatus::DELIVERED: return "Delivered";
            case OrderStatus::CANCELLED: return "Cancelled";
            case OrderStatus::RETURNED:  return "Returned";
        }
        return "Unknown";
    }
};

// ============================================================
//  NOTIFICATION
// ============================================================
struct Notification {
    int     userId;
    QString message;
    QString type; // order, promo, system
    bool    isRead;
    QString timestamp;
};

// ============================================================
//  COUPON
// ============================================================
struct Coupon {
    QString code;
    double  discountPercent;
    double  maxDiscount;
    double  minOrderValue;
    bool    isActive;
    int     usageLimit;
    int     usedCount;
    QString expiryDate;

    bool isValid(double orderValue) const {
        return isActive && orderValue >= minOrderValue && usedCount < usageLimit;
    }
    double getDiscount(double orderValue) const {
        if (!isValid(orderValue)) return 0;
        double disc = orderValue * discountPercent / 100.0;
        return std::min(disc, maxDiscount);
    }
};

// ============================================================
//  USER BASE CLASS
// ============================================================
class User {
protected:
    int      m_id;
    QString  m_name;
    QString  m_email;
    QString  m_password;
    QString  m_phone;
    UserRole m_role;
    bool     m_isActive;
    QString  m_createdAt;

public:
    User(int id, const QString& name, const QString& email,
         const QString& password, const QString& phone, UserRole role)
        : m_id(id), m_name(name), m_email(email), m_password(password),
          m_phone(phone), m_role(role), m_isActive(true)
    {
        m_createdAt = QDateTime::currentDateTime().toString("dd MMM yyyy");
    }
    virtual ~User() = default;

    int     getId()       const { return m_id; }
    QString getName()     const { return m_name; }
    QString getEmail()    const { return m_email; }
    QString getPhone()    const { return m_phone; }
    UserRole getRole()    const { return m_role; }
    bool    getIsActive() const { return m_isActive; }
    QString getPassword() const { return m_password; }

    void setName(const QString& n)     { m_name = n; }
    void setPhone(const QString& p)    { m_phone = p; }
    void setActive(bool a)             { m_isActive = a; }
    void setPassword(const QString& p) { m_password = p; }
    void setId(int id)                 { m_id = id; }

    bool authenticate(const QString& pwd) const { return m_password == pwd; }

    virtual QString getRoleString() const = 0;
};

// ============================================================
//  CUSTOMER
// ============================================================
class Wishlist {
    QVector<int> m_productIds;
public:
    void add(int pid)    { if (!contains(pid)) m_productIds.append(pid); }
    void remove(int pid) { m_productIds.removeAll(pid); }
    bool contains(int pid) const { return m_productIds.contains(pid); }
    const QVector<int>& getIds() const { return m_productIds; }
};

class Customer : public User {
    QVector<Address> m_addresses;
    QVector<int>     m_orderIds;
    Cart             m_cart;
    Wishlist         m_wishlist;
    double           m_walletBalance;
    int              m_rewardPoints;
    QString          m_membershipTier;

public:
    Customer(int id, const QString& name, const QString& email,
             const QString& password, const QString& phone)
        : User(id, name, email, password, phone, UserRole::CUSTOMER),
          m_cart(id), m_walletBalance(0), m_rewardPoints(0),
          m_membershipTier("Bronze") {}

    QString getRoleString() const override { return "Customer"; }

    Cart&           getCart()          { return m_cart; }
    const Cart&     getCart()    const { return m_cart; }
    Wishlist&       getWishlist()      { return m_wishlist; }

    void addAddress(const Address& a)  { m_addresses.append(a); }
    const QVector<Address>& getAddresses() const { return m_addresses; }
    Address* getDefaultAddress() {
        return m_addresses.isEmpty() ? nullptr : &m_addresses[0];
    }
    const Address* getDefaultAddress() const {
        return m_addresses.isEmpty() ? nullptr : &m_addresses[0];
    }

    void            addOrderId(int oid)    { m_orderIds.append(oid); }
    const QVector<int>& getOrderIds() const { return m_orderIds; }

    double  getWalletBalance() const { return m_walletBalance; }
    void    addWallet(double a)       { m_walletBalance += a; }
    bool    useWallet(double a)       { if (m_walletBalance < a) return false; m_walletBalance -= a; return true; }

    int     getRewardPoints()  const { return m_rewardPoints; }
    void    addPoints(int pts)        { m_rewardPoints += pts; }

    QString getMembershipTier() const { return m_membershipTier; }

    void updateMembership() {
        int n = m_orderIds.size();
        if (n >= 50)      m_membershipTier = "Platinum";
        else if (n >= 20) m_membershipTier = "Gold";
        else if (n >= 5)  m_membershipTier = "Silver";
        else              m_membershipTier = "Bronze";
    }
};

// ============================================================
//  SELLER
// ============================================================
class Seller : public User {
    QString      m_storeName;
    QString      m_storeDescription;
    QVector<int> m_productIds;
    QVector<int> m_orderIds;
    double       m_totalRevenue;
    double       m_rating;
    int          m_totalRatings;
    bool         m_isVerified;
    double       m_commissionRate;
    QString      m_gstNumber;

public:
    Seller(int id, const QString& name, const QString& email,
           const QString& password, const QString& phone, const QString& storeName)
        : User(id, name, email, password, phone, UserRole::SELLER),
          m_storeName(storeName), m_totalRevenue(0), m_rating(0),
          m_totalRatings(0), m_isVerified(false), m_commissionRate(10.0) {}

    QString getRoleString() const override { return "Seller"; }

    QString getStoreName()   const { return m_storeName; }
    bool    getIsVerified()  const { return m_isVerified; }
    double  getTotalRevenue()const { return m_totalRevenue; }
    double  getRating()      const { return m_rating; }
    QString getGST()         const { return m_gstNumber; }
    double  getCommission()  const { return m_commissionRate; }
    const QVector<int>& getProductIds() const { return m_productIds; }
    const QVector<int>& getOrderIds()   const { return m_orderIds; }

    void setVerified(bool v)              { m_isVerified = v; }
    void setStoreName(const QString& s)   { m_storeName = s; }
    void setGST(const QString& g)         { m_gstNumber = g; }
    void setCommissionRate(double r)      { m_commissionRate = r; }

    void addProductId(int pid)            { m_productIds.append(pid); }
    void addOrderId(int oid)              { m_orderIds.append(oid); }
    void addRevenue(double amt)           { m_totalRevenue += amt * (1.0 - m_commissionRate / 100.0); }

    void addRating(double r) {
        m_rating = (m_rating * m_totalRatings + r) / (m_totalRatings + 1);
        ++m_totalRatings;
    }
};

// ============================================================
//  ADMIN
// ============================================================
class Admin : public User {
    QVector<QString> m_actionLog;
    int              m_adminLevel;

public:
    Admin(int id, const QString& name, const QString& email,
          const QString& password, const QString& phone, int level = 3)
        : User(id, name, email, password, phone, UserRole::ADMIN),
          m_adminLevel(level) {}

    QString getRoleString() const override { return "Admin"; }

    int getAdminLevel() const { return m_adminLevel; }
    const QVector<QString>& getLogs() const { return m_actionLog; }

    void logAction(const QString& action) {
        m_actionLog.append("[" + QDateTime::currentDateTime().toString("hh:mm:ss") + "] " + action);
    }
};
