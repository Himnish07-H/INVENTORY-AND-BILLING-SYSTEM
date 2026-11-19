#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
using namespace std;

class Product {
public:
    int id, stock;
    string name;
    double price;
    Product(int i, string n, double p, int q)
        : id(i), stock(q), name(n), price(p) {}
};

class BillItem {
public:
    int id, quantity;
    string name;
    double price;
    BillItem(int i, string n, double p, int q)
        : id(i), quantity(q), name(n), price(p) {}
    double amount() const { return price * quantity; }
};

class InventorySystem {
    vector<Product> products;

    int findIndex(int id) const {
        for (int i = 0; i < (int)products.size(); ++i)
            if (products[i].id == id) return i;
        return -1;
    }
    void save() const {
        ofstream f("inventory.txt");
        for (const auto &p : products)
            f << p.id << '|' << p.name << '|'
              << p.price << '|' << p.stock << '\n';
    }
    void load() {
        ifstream f("inventory.txt");
        string line;
        while (getline(f, line)) {
            size_t a=line.find('|'), b=line.find('|',a+1),
                   c=line.find('|',b+1);
            if (a==string::npos || b==string::npos || c==string::npos) continue;
            products.emplace_back(stoi(line.substr(0,a)),
                line.substr(a+1,b-a-1), stod(line.substr(b+1,c-b-1)),
                stoi(line.substr(c+1)));
        }
    }
public:
    InventorySystem() {
        load();
        if (products.empty()) {
            products.emplace_back(101,"Notebook",55.00,30);
            products.emplace_back(102,"Pen",10.00,100);
            products.emplace_back(103,"File",35.00,40);
            save();
        }
    }
    void addProduct() {
        int id, qty; double price; string name;
        cout << "Product ID: "; cin >> id;
        if (findIndex(id)!=-1) { cout<<"Duplicate ID.\n"; return; }
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        cout << "Product name: "; getline(cin,name);
        cout << "Unit price: "; cin >> price;
        cout << "Opening stock: "; cin >> qty;
        if (name.empty() || price<=0 || qty<0) {
            cout<<"Invalid product details.\n"; return;
        }
        products.emplace_back(id,name,price,qty);
        save(); cout<<"Product added.\n";
    }
    void displayProducts() const {
        cout<<"\nID    PRODUCT              PRICE       STOCK\n";
        for (const auto &p:products)
            cout<<left<<setw(6)<<p.id<<setw(21)<<p.name
                <<right<<setw(8)<<fixed<<setprecision(2)<<p.price
                <<setw(10)<<p.stock<<'\n';
    }
    void searchProduct() const {
        int id; cout<<"Enter product ID: "; cin>>id;
        int k=findIndex(id);
        if(k<0) cout<<"Product not found.\n";
        else { const auto &p=products[k];
            cout<<p.id<<" | "<<p.name<<" | Rs. "<<p.price
                <<" | Stock: "<<p.stock<<'\n'; }
    }
    void generateBill() {
        vector<BillItem> bill; char more='y';
        while(more=='y'||more=='Y') {
            int id,qty; cout<<"Enter product ID: "; cin>>id;
            int k=findIndex(id);
            if(k<0) cout<<"Product not found.\n";
            else {
                cout<<"Available stock: "<<products[k].stock<<"\nQuantity: ";
                cin>>qty;
                if(qty<=0) cout<<"Quantity must be positive.\n";
                else if(qty>products[k].stock) cout<<"Insufficient stock.\n";
                else {
                    bill.emplace_back(id,products[k].name,products[k].price,qty);
                    products[k].stock-=qty;
                }
            }
            cout<<"Add another item? (y/n): "; cin>>more;
        }
        if(bill.empty()) { cout<<"No bill generated.\n"; return; }
        double subtotal=0;
        cout<<"\n------------- CUSTOMER BILL -------------\n";
        cout<<left<<setw(20)<<"Item"<<setw(8)<<"Qty"<<setw(10)<<"Rate"<<"Amount\n";
        for(const auto &x:bill) {
            cout<<left<<setw(20)<<x.name<<setw(8)<<x.quantity
                <<setw(10)<<fixed<<setprecision(2)<<x.price<<x.amount()<<'\n';
            subtotal+=x.amount();
        }
        double tax=subtotal*0.05, total=subtotal+tax;
        cout<<"Subtotal: Rs. "<<subtotal<<"\nTax (5%): Rs. "<<tax
            <<"\nGrand Total: Rs. "<<total<<'\n';
        save();
        ofstream history("sales.txt",ios::app);
        history<<"Subtotal="<<subtotal<<", Tax="<<tax<<", Total="<<total<<'\n';
        cout<<"Bill saved; stock updated.\n";
    }
};

int main() {
    InventorySystem shop; int choice;
    do {
        cout<<"\n====== INVENTORY & BILLING SYSTEM ======\n"
            <<"1. Add Product\n2. Display Products\n3. Search Product\n"
            <<"4. Generate Customer Bill\n5. Exit\nChoose: ";
        if(!(cin>>choice)) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(),'\n');
            cout<<"Enter a valid number.\n"; continue;
        }
        switch(choice) {
            case 1: shop.addProduct(); break;
            case 2: shop.displayProducts(); break;
            case 3: shop.searchProduct(); break;
            case 4: shop.generateBill(); break;
            case 5: cout<<"Thank you.\n"; break;
            default: cout<<"Invalid choice.\n";
        }
    } while(choice!=5);
    return 0;
}