/*
 * TikTok Shop Simulator - Interactive console system (C++)
 * Theme: TikTok Social Commerce
 *
 * Features:
 * - Browse products
 * - Add to cart
 * - View/remove items
 * - Change cart quantity
 * - Promo codes
 * - Live stream discount
 * - "For You" product recommendation
 * - Checkout with receipt
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <limits>
#include <ctime>
#include <algorithm>
#include <cctype>

using namespace std;

// ---------- Data structures ----------
struct Product {
    string name;
    double price;
    int stock;
};

struct CartItem {
    int index;
    int qty;
};

vector<Product> products = {
    {"Phone Case",       15.90, 20},
    {"Wireless Earbuds", 59.00, 10},
    {"Skincare Set",     45.50, 15},
    {"LED Ring Light",   32.00,  8},
    {"Mini Tote Bag",    27.90, 12}
};

vector<CartItem> cart;

const double LIVE_DISCOUNT     = 0.05;
const double TIKTOK10_RATE     = 0.10;
const double FYP15_RATE        = 0.15;
const double FREE_SHIPPING_MIN = 100.00;
const double SHIPPING_FEE      = 6.00;


// ---------- Input helpers ----------
int getInt(const string& prompt, int low, int high) {
    int value;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (value >= low && value <= high)
                return value;

            cout << "Please enter a number between "
                 << low << " and " << high << ".\n";
        }
        else {
            if (cin.eof())
                exit(0);

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Invalid input. Please enter a number.\n";
        }
    }
}


bool getYesNo(const string& prompt) {
    string answer;

    while (true) {
        cout << prompt;

        if (!getline(cin, answer))
            exit(0);

        transform(
            answer.begin(),
            answer.end(),
            answer.begin(),
            [](unsigned char c) {
                return tolower(c);
            }
        );

        if (answer == "y" || answer == "yes")
            return true;

        if (answer == "n" || answer == "no")
            return false;

        cout << "Please type y or n.\n";
    }
}


// ---------- Main menu ----------
void showMenu() {
    cout << "\n===== TikTok Shop =====\n"
         << "1. Browse products\n"
         << "2. Add to cart\n"
         << "3. View cart\n"
         << "4. Remove item from cart\n"
         << "5. Change item quantity\n"
         << "6. Checkout\n"
         << "7. For You recommendation\n"
         << "8. Exit\n";
}


// ---------- Product functions ----------
void displayProducts() {
    cout << "\n--- Products ---\n"
         << fixed << setprecision(2);

    for (size_t i = 0; i < products.size(); i++) {
        cout << i + 1 << ". "
             << left << setw(18) << products[i].name
             << right
             << " RM" << setw(6) << products[i].price
             << "   stock: " << products[i].stock
             << "\n";
    }
}


int cartQuantityOf(int index) {
    int total = 0;

    for (const CartItem& item : cart) {
        if (item.index == index)
            total += item.qty;
    }

    return total;
}


// ---------- For You recommendation ----------
void recommendProduct() {
    cout << "\n===== For You Recommendation =====\n";

    int budget = getInt(
        "Enter your shopping budget in RM (1-500): ",
        1,
        500
    );

    int bestIndex = -1;

    for (size_t i = 0; i < products.size(); i++) {

        int availableStock =
            products[i].stock - cartQuantityOf((int)i);

        if (availableStock > 0 &&
            products[i].price <= budget) {

            if (bestIndex == -1 ||
                products[i].price > products[bestIndex].price) {

                bestIndex = (int)i;
            }
        }
    }

    cout << fixed << setprecision(2);

    if (bestIndex == -1) {
        cout << "\nSorry, no available products fit your budget.\n";
    }
    else {
        cout << "\nRecommended for you:\n";

        cout << products[bestIndex].name
             << " - RM"
             << products[bestIndex].price
             << "\n";

        cout << "This product fits within your RM"
             << budget
             << " budget.\n";
    }
}


// ---------- Cart functions ----------
void addToCart() {
    displayProducts();

    int choice = getInt(
        "Product number (0 to cancel): ",
        0,
        (int)products.size()
    );

    if (choice == 0)
        return;

    int index = choice - 1;

    int available =
        products[index].stock - cartQuantityOf(index);

    if (available <= 0) {
        cout << "Sorry, no more stock available for this item.\n";
        return;
    }

    int qty = getInt(
        "Quantity (1-" + to_string(available) + "): ",
        1,
        available
    );

    bool found = false;

    for (CartItem& item : cart) {
        if (item.index == index) {
            item.qty += qty;
            found = true;
            break;
        }
    }

    if (!found) {
        cart.push_back({index, qty});
    }

    cout << "Added "
         << qty
         << " x "
         << products[index].name
         << " to cart.\n";
}


double subtotal() {
    double sum = 0;

    for (const CartItem& item : cart) {
        sum += products[item.index].price * item.qty;
    }

    return sum;
}


void viewCart() {
    if (cart.empty()) {
        cout << "\nYour cart is empty.\n";
        return;
    }

    cout << "\n--- Your Cart ---\n"
         << fixed << setprecision(2);

    for (size_t i = 0; i < cart.size(); i++) {

        const Product& p =
            products[cart[i].index];

        cout << i + 1 << ". "
             << left << setw(18) << p.name
             << " x" << setw(3) << cart[i].qty
             << right
             << " RM"
             << setw(7)
             << p.price * cart[i].qty
             << "\n";
    }

    cout << "Subtotal: RM"
         << subtotal()
         << "\n";
}


void removeFromCart() {
    if (cart.empty()) {
        cout << "\nYour cart is empty.\n";
        return;
    }

    viewCart();

    int choice = getInt(
        "Item number to remove (0 to cancel): ",
        0,
        (int)cart.size()
    );

    if (choice == 0)
        return;

    cout << "Removed "
         << products[cart[choice - 1].index].name
         << " from cart.\n";

    cart.erase(
        cart.begin() + (choice - 1)
    );
}


// ---------- NEW FEATURE: Change cart quantity ----------
void changeCartQuantity() {

    if (cart.empty()) {
        cout << "\nYour cart is empty.\n";
        return;
    }

    viewCart();

    int choice = getInt(
        "Item number to change (0 to cancel): ",
        0,
        (int)cart.size()
    );

    if (choice == 0)
        return;

    int cartIndex = choice - 1;
    int productIndex = cart[cartIndex].index;

    int oldQuantity = cart[cartIndex].qty;
    int maxQuantity = products[productIndex].stock;

    cout << "\nSelected: "
         << products[productIndex].name
         << "\n";

    cout << "Current quantity: "
         << oldQuantity
         << "\n";

    int newQuantity = getInt(
        "New quantity (1-" +
        to_string(maxQuantity) +
        "): ",
        1,
        maxQuantity
    );

    cart[cartIndex].qty = newQuantity;

    cout << "Quantity updated from "
         << oldQuantity
         << " to "
         << newQuantity
         << ".\n";

    cout << "New subtotal: RM"
         << fixed
         << setprecision(2)
         << subtotal()
         << "\n";
}


// ---------- Discount system ----------
double askDiscount(vector<string>& notes) {
    double rate = 0.0;

    if (getYesNo(
        "Did you watch our live stream? (y/n): "
    )) {

        rate += LIVE_DISCOUNT;

        notes.push_back(
            "Live stream viewer (5%)"
        );
    }


    if (getYesNo(
        "Do you have a promo code? (y/n): "
    )) {

        string code;

        cout << "Enter code: ";
        getline(cin, code);

        transform(
            code.begin(),
            code.end(),
            code.begin(),
            [](unsigned char c) {
                return toupper(c);
            }
        );


        if (code == "TIKTOK10") {

            rate += TIKTOK10_RATE;

            notes.push_back(
                "Promo TIKTOK10 (10%)"
            );
        }

        else if (code == "FYP15") {

            rate += FYP15_RATE;

            notes.push_back(
                "Promo FYP15 (15%)"
            );
        }

        else {
            cout << "Invalid promo code.\n";
        }
    }

    return rate;
}


// ---------- Payment ----------
string choosePayment() {
    cout << "\nPayment method:\n"
         << "1. TikTok Wallet\n"
         << "2. Credit/Debit Card\n"
         << "3. Online Banking\n"
         << "4. Cash on Delivery\n";

    int choice =
        getInt("Choice: ", 1, 4);

    switch (choice) {

        case 1:
            return "TikTok Wallet";

        case 2:
            return "Credit/Debit Card";

        case 3:
            return "Online Banking";

        default:
            return "Cash on Delivery";
    }
}


// ---------- Checkout ----------
void checkout() {

    if (cart.empty()) {
        cout << "\nYour cart is empty. Add something first.\n";
        return;
    }

    viewCart();

    if (!getYesNo(
        "Proceed to checkout? (y/n): "
    ))
        return;


    double sub = subtotal();

    vector<string> notes;

    double rate =
        askDiscount(notes);

    double discount =
        sub * rate;

    double afterDiscount =
        sub - discount;

    double shipping =
        (afterDiscount >= FREE_SHIPPING_MIN)
        ? 0.0
        : SHIPPING_FEE;

    double total =
        afterDiscount + shipping;

    string payment =
        choosePayment();


    // Current date/time
    time_t now =
        time(nullptr);

    char buf[32];

    strftime(
        buf,
        sizeof(buf),
        "%d %b %Y  %H:%M",
        localtime(&now)
    );


    // ---------- Receipt ----------
    cout << "\n"
         << string(36, '=')
         << "\n";

    cout << "          TIKTOK SHOP RECEIPT\n";

    cout << string(36, '=')
         << "\n";

    cout << buf << "\n";

    cout << string(36, '-')
         << "\n"
         << fixed
         << setprecision(2);


    for (const CartItem& item : cart) {

        const Product& p =
            products[item.index];

        cout << left
             << setw(18)
             << p.name

             << " x"
             << setw(3)
             << item.qty

             << right
             << " RM"
             << setw(7)
             << p.price * item.qty
             << "\n";
    }


    cout << string(36, '-')
         << "\n";


    cout << left
         << setw(24)
         << "Subtotal"
         << right
         << " RM"
         << setw(7)
         << sub
         << "\n";


    for (const string& note : notes) {
        cout << "  "
             << note
             << "\n";
    }


    cout << left
         << setw(24)
         << "Discount"
         << right
         << "-RM"
         << setw(7)
         << discount
         << "\n";


    cout << left
         << setw(24)
         << "Shipping"
         << right
         << " RM"
         << setw(7)
         << shipping
         << "\n";


    cout << left
         << setw(24)
         << "TOTAL"
         << right
         << " RM"
         << setw(7)
         << total
         << "\n";


    cout << "Paid via: "
         << payment
         << "\n";


    cout << string(36, '=')
         << "\n";

    cout << "Thank you for shopping with us!\n";


    // Update stock
    for (const CartItem& item : cart) {
        products[item.index].stock -= item.qty;
    }

    cart.clear();
}


// ---------- Main ----------
int main() {

    cout << "Welcome to the TikTok Shop Simulator!\n";

    cout << "Free shipping on orders over RM"
         << fixed
         << setprecision(0)
         << FREE_SHIPPING_MIN
         << " (after discounts).\n";


    int choice;

    do {

        showMenu();

        choice =
            getInt("Choice: ", 1, 8);


        switch (choice) {

            case 1:
                displayProducts();
                break;

            case 2:
                addToCart();
                break;

            case 3:
                viewCart();
                break;

            case 4:
                removeFromCart();
                break;

            case 5:
                changeCartQuantity();
                break;

            case 6:
                checkout();
                break;

            case 7:
                recommendProduct();
                break;

            case 8:
                cout << "Thanks for visiting. Goodbye!\n";
                break;
        }

    } while (choice != 8);


    return 0;
}
