#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <algorithm>
using namespace std;

// ---------- Abstract Base Class ----------
class Vehicle {
protected:
    int id;
    string name, number;
    double rate;
    bool available;

public:
    Vehicle(int i, string n, string no, double r, bool a = true)
        : id(i), name(n), number(no), rate(r), available(a) {}

    virtual ~Vehicle() = default;

    int getID() const { return id; }
    string getName() const { return name; }
    string getNumber() const { return number; }
    double getRate() const { return rate; }
    bool isAvailable() const { return available; }
    void setAvailable(bool a) { available = a; }

    virtual string getType() const = 0;

    virtual double calculateRent(int days) const {
        double total = rate * days;

        if (days >= 7)
            total *= 0.90;       // 10% weekly discount
        else if (days >= 3)
            total *= 0.95;       // 5% discount

        return total;
    }

    virtual void display() const {
        cout << left << setw(6) << id
             << setw(14) << name
             << setw(16) << number
             << setw(10) << getType()
             << setw(12) << fixed << setprecision(2) << rate
             << (available ? "Available" : "Rented") << '\n';
    }

    // Operator overloading
    bool operator<(const Vehicle& other) const {
        return rate < other.rate;
    }

    virtual void save(ofstream& out) const {
        out << getType() << '|'
            << id << '|'
            << name << '|'
            << number << '|'
            << rate << '|'
            << available << '\n';
    }
};

// ---------- Derived Classes ----------
class Car : public Vehicle {
public:
    using Vehicle::Vehicle;
    string getType() const override { return "Car"; }
};

class Bike : public Vehicle {
public:
    using Vehicle::Vehicle;
    string getType() const override { return "Bike"; }
};

class SUV : public Vehicle {
public:
    using Vehicle::Vehicle;
    string getType() const override { return "SUV"; }
};

// ---------- Customer Class ----------
class Customer {
    int id;
    string name;
    string phone;
    bool premium;

public:
    Customer(int i, string n, string p, bool pr = false)
        : id(i), name(n), phone(p), premium(pr) {}

    int getID() const { return id; }
    string getName() const { return name; }
    bool isPremium() const { return premium; }

    void display() const {
        cout << "Customer ID: " << id
             << "\nName: " << name
             << "\nPhone: " << phone
             << "\nType: " << (premium ? "Premium" : "Regular")
             << '\n';
    }
};

// ---------- Rental Class ----------
class Rental {
    int rentalID, customerID, vehicleID, days;
    double amount;
    bool returned;

public:
    Rental(int r, int c, int v, int d, double a)
        : rentalID(r), customerID(c), vehicleID(v),
          days(d), amount(a), returned(false) {}

    int getRentalID() const { return rentalID; }
    int getVehicleID() const { return vehicleID; }
    bool isReturned() const { return returned; }
    void markReturned() { returned = true; }

    void display() const {
        cout << "Rental ID: " << rentalID
             << " | Customer ID: " << customerID
             << " | Vehicle ID: " << vehicleID
             << " | Days: " << days
             << " | Amount: Rs. " << amount
             << " | Status: " << (returned ? "Returned" : "Active")
             << '\n';
    }
};

// ---------- Template Function ----------
template <typename T>
void displayCollection(const vector<T>& collection) {
    for (const auto& item : collection)
        item->display();
}

// ---------- Rental Management System ----------
class RentalManager {
private:
    vector<unique_ptr<Vehicle>> vehicles;
    vector<Customer> customers;
    vector<Rental> rentals;

    static int totalRentals;
    int nextRentalID = 1001;

    // Friend function can access private members
    friend void showStatistics(const RentalManager& manager);

    Vehicle* findVehicle(int id) {
        for (auto& v : vehicles)
            if (v->getID() == id)
                return v.get();
        return nullptr;
    }

    Customer* findCustomer(int id) {
        for (auto& c : customers)
            if (c.getID() == id)
                return &c;
        return nullptr;
    }

    Rental* findRental(int id) {
        for (auto& r : rentals)
            if (r.getRentalID() == id)
                return &r;
        return nullptr;
    }

    void saveVehicles() const {
        ofstream out("vehicles.txt");
        if (!out)
            throw runtime_error("Cannot save vehicle records.");

        for (const auto& v : vehicles)
            v->save(out);
    }

    void saveCustomers() const {
        ofstream out("customers.txt");
        if (!out)
            throw runtime_error("Cannot save customer records.");

        for (const auto& c : customers) {
            // Customer information is stored below in a simple format.
            // Phone numbers should contain no spaces or '|'.
            c.display();
        }
    }

    void loadVehicles() {
        ifstream in("vehicles.txt");

        if (!in) {
            vehicles.push_back(make_unique<Car>(
                101, "HondaCity", "GJ06AB1234", 1500));
            vehicles.push_back(make_unique<Bike>(
                102, "Activa", "GJ06CD5678", 500));
            vehicles.push_back(make_unique<SUV>(
                103, "Creta", "GJ06EF9012", 2500));
            saveVehicles();
            return;
        }

        string line;
        while (getline(in, line)) {
            try {
                size_t p1 = line.find('|');
                size_t p2 = line.find('|', p1 + 1);
                size_t p3 = line.find('|', p2 + 1);
                size_t p4 = line.find('|', p3 + 1);
                size_t p5 = line.find('|', p4 + 1);

                if (p1 == string::npos || p5 == string::npos)
                    continue;

                string type = line.substr(0, p1);
                int id = stoi(line.substr(p1 + 1, p2 - p1 - 1));
                string name = line.substr(p2 + 1, p3 - p2 - 1);
                string number = line.substr(p3 + 1, p4 - p3 - 1);
                double rate = stod(line.substr(p4 + 1, p5 - p4 - 1));
                bool available = stoi(line.substr(p5 + 1)) != 0;

                if (type == "Car")
                    vehicles.push_back(make_unique<Car>(
                        id, name, number, rate, available));
                else if (type == "Bike")
                    vehicles.push_back(make_unique<Bike>(
                        id, name, number, rate, available));
                else if (type == "SUV")
                    vehicles.push_back(make_unique<SUV>(
                        id, name, number, rate, available));
            }
            catch (const exception&) {
                cerr << "Skipping an invalid vehicle record.\n";
            }
        }
    }

public:
    RentalManager() {
        loadVehicles();
    }

    // Function overloading
    void searchVehicle(const string& name) const {
        bool found = false;

        for (const auto& v : vehicles) {
            if (v->getName().find(name) != string::npos) {
                v->display();
                found = true;
            }
        }

        if (!found)
            cout << "No matching vehicle found.\n";
    }

    void searchVehicle(const string& name, const string& type) const {
        bool found = false;

        for (const auto& v : vehicles) {
            if (v->getName().find(name) != string::npos &&
                v->getType() == type) {
                v->display();
                found = true;
            }
        }

        if (!found)
            cout << "No matching vehicle found.\n";
    }

    void showVehicles() const {
        cout << "\nID    Name          Number          Type      Rate/day    Status\n";
        cout << string(75, '-') << '\n';
        displayCollection(vehicles);
    }

    void addVehicle() {
        int id, typeChoice;
        string name, number;
        double rate;

        cout << "Enter vehicle ID: ";
        cin >> id;

        if (findVehicle(id))
            throw runtime_error("Vehicle ID already exists.");

        cout << "Enter model name (no spaces): ";
        cin >> name;

        cout << "Enter vehicle number (no spaces): ";
        cin >> number;

        cout << "Enter rental rate per day: ";
        cin >> rate;

        if (!cin || rate <= 0)
            throw runtime_error("Invalid rental rate.");

        cout << "1. Car  2. Bike  3. SUV\nChoose type: ";
        cin >> typeChoice;

        if (typeChoice == 1)
            vehicles.push_back(make_unique<Car>(
                id, name, number, rate));
        else if (typeChoice == 2)
            vehicles.push_back(make_unique<Bike>(
                id, name, number, rate));
        else if (typeChoice == 3)
            vehicles.push_back(make_unique<SUV>(
                id, name, number, rate));
        else
            throw runtime_error("Invalid vehicle type.");

        saveVehicles();
        cout << "Vehicle added successfully.\n";
    }

    void registerCustomer() {
        int id;
        string name, phone;
        int premiumChoice;

        cout << "Enter customer ID: ";
        cin >> id;

        if (findCustomer(id))
            throw runtime_error("Customer ID already exists.");

        cout << "Enter name (no spaces): ";
        cin >> name;

        cout << "Enter phone number: ";
        cin >> phone;

        cout << "Premium customer? (1 = Yes, 0 = No): ";
        cin >> premiumChoice;

        if (!cin || (premiumChoice != 0 && premiumChoice != 1))
            throw runtime_error("Invalid customer type.");

        customers.emplace_back(id, name, phone, premiumChoice == 1);

        ofstream out("customers.txt", ios::app);
        if (!out)
            throw runtime_error("Cannot open customers.txt.");

        out << id << '|' << name << '|'
            << phone << '|' << premiumChoice << '\n';

        cout << "Customer registered successfully.\n";
    }

    void rentVehicle() {
        int customerID, vehicleID, days;

        cout << "Enter customer ID: ";
        cin >> customerID;

        Customer* customer = findCustomer(customerID);
        if (!customer)
            throw runtime_error("Register the customer first.");

        cout << "Enter vehicle ID: ";
        cin >> vehicleID;

        Vehicle* vehicle = findVehicle(vehicleID);
        if (!vehicle)
            throw runtime_error("Vehicle not found.");

        if (!vehicle->isAvailable())
            throw runtime_error("Vehicle is already rented.");

        cout << "Enter rental days: ";
        cin >> days;

        if (!cin || days <= 0)
            throw runtime_error("Rental days must be positive.");

        double amount = vehicle->calculateRent(days);

        // Premium customer discount
        if (customer->isPremium())
            amount *= 0.95;

        int id = nextRentalID++;
        rentals.emplace_back(id, customerID, vehicleID, days, amount);
        totalRentals++;

        vehicle->setAvailable(false);
        saveVehicles();

        ofstream out("rental_history.txt", ios::app);
        if (!out)
            throw runtime_error("Cannot save rental history.");

        out << id << '|' << customerID << '|'
            << vehicleID << '|' << days << '|'
            << amount << "|Active\n";

        cout << fixed << setprecision(2);
        cout << "\nRental confirmed!\n"
             << "Rental ID: " << id
             << "\nVehicle: " << vehicle->getName()
             << "\nRental amount: Rs. " << amount << '\n';
    }

    void returnVehicle() {
        int id;
        cout << "Enter rental ID: ";
        cin >> id;

        Rental* rental = findRental(id);
        if (!rental)
            throw runtime_error("Rental not found.");

        if (rental->isReturned())
            throw runtime_error("Vehicle has already been returned.");

        Vehicle* vehicle = findVehicle(rental->getVehicleID());
        if (!vehicle)
            throw runtime_error("Associated vehicle not found.");

        rental->markReturned();
        vehicle->setAvailable(true);
        saveVehicles();

        // Save the current rental status.
        ofstream out("rental_history.txt", ios::app);
        if (!out)
            throw runtime_error("Cannot save return record.");

        out << "RETURN|" << id << "|Vehicle returned\n";

        cout << "Vehicle returned successfully.\n";
        cout << "Vehicle is now available for rental.\n";
    }

    void showRentals() const {
        if (rentals.empty()) {
            cout << "No rentals in this session.\n";
            return;
        }

        for (const auto& r : rentals)
            r.display();
    }

    void showAvailableVehicles() const {
        bool found = false;

        for (const auto& v : vehicles) {
            if (v->isAvailable()) {
                v->display();
                found = true;
            }
        }

        if (!found)
            cout << "No vehicles are currently available.\n";
    }
};

int RentalManager::totalRentals = 0;

// Friend function
void showStatistics(const RentalManager& manager) {
    int available = 0, rented = 0;

    for (const auto& v : manager.vehicles) {
        if (v->isAvailable())
            available++;
        else
            rented++;
    }

    cout << "\n--- Vehicle Statistics ---\n"
         << "Total vehicles: " << manager.vehicles.size()
         << "\nAvailable: " << available
         << "\nCurrently rented: " << rented
         << "\nRentals created this session: "
         << RentalManager::totalRentals << '\n';
}

// ---------- Main Function ----------
int main() {
    RentalManager system;
    int choice;

    do {
        cout << "\n====================================\n"
             << "   VEHICLE RENTAL MANAGEMENT SYSTEM\n"
             << "====================================\n"
             << "1. Display all vehicles\n"
             << "2. Display available vehicles\n"
             << "3. Add vehicle\n"
             << "4. Register customer\n"
             << "5. Search vehicle by name\n"
             << "6. Search vehicle by name and type\n"
             << "7. Rent vehicle\n"
             << "8. Return vehicle\n"
             << "9. View rental history (current session)\n"
             << "10. View statistics\n"
             << "0. Exit\n"
             << "Enter choice: ";

        cin >> choice;

        if (!cin) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a valid number.\n";
            continue;
        }

        try {
            switch (choice) {
            case 1:
                system.showVehicles();
                break;

            case 2:
                system.showAvailableVehicles();
                break;

            case 3:
                system.addVehicle();
                break;

            case 4:
                system.registerCustomer();
                break;

            case 5: {
                string name;
                cout << "Enter vehicle name: ";
                cin >> name;
                system.searchVehicle(name);
                break;
            }

            case 6: {
                string name, type;
                cout << "Enter vehicle name: ";
                cin >> name;
                cout << "Enter type (Car/Bike/SUV): ";
                cin >> type;
                system.searchVehicle(name, type);
                break;
            }

            case 7:
                system.rentVehicle();
                break;

            case 8:
                system.returnVehicle();
                break;

            case 9:
                system.showRentals();
                break;

            case 10:
                showStatistics(system);
                break;

            case 0:
                cout << "Thank you for using the system!\n";
                break;

            default:
                cout << "Invalid choice. Try again.\n";
            }
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << '\n';
        }

    } while (choice != 0);

    return 0;
}