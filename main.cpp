#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <ctime>

using namespace std;

// mozna jeszcze dodac globalnie te wszystkie guestId, paymentId i je inkrementowac przy tworzeniu

class Hotel;
class Room;
class Guest;
class Reservation;
class Payment;

class Hotel {
private:
    string name;
    string address;
    vector<Guest> guests;
    vector<Reservation> reservations;
    vector<Room> rooms;

public:
    Hotel(string n, string a) : name(n), address(a) {}

    void addGuest(Guest &guest) { guests.push_back(guest); }
    void addReservation(Reservation &reservation) { reservations.push_back(reservation); }
    void addRoom(Room &room) { rooms.push_back(room); }

    vector<Room> getAvailableRooms();
    vector<Reservation> getAllGuestResrvations();
};

class Room {
private:
    string roomNumber;
    string standard;
    unsigned int pricePerNight;
    unsigned int maxAmountOfPeople;

public:
    Room(string n, string s) : roomNumber(n), standard(s) {}

    void setPrice(unsigned int price) { pricePerNight = price; }
    void setMaxAmountOfPeople(unsigned int max) { maxAmountOfPeople = max; }
    
    string getStandard();

    bool checkAvailability();
    double calculatePrice();
};

class Guest {
private:
    string name;

public:
    Guest(string guestName) : name(guestName) {}

    void makeReservation();
    void cancelReservation();
    void payForTheReservation();
};

class Reservation {
private:
    //Hotel &hotel; // zakladamy ze jest 1 hotel?
    Guest *guest;
    time_t startDate;
    time_t endDate;
    vector<weak_ptr<Room>> rooms;
    shared_ptr<Payment> payment;
    unsigned int totalPrice;
    bool isPaidFor;
    string status;

public:
    Reservation(Guest *g, time_t start, time_t end) 
        : guest(g), startDate(start), endDate(end) {}
};

class Payment {
protected:
    double amount;
    time_t paymentDate;
    string method;

public:
    Payment(double a) : amount(a) {}

    virtual void processPayment() = 0;
};

class CardPayment : public Payment {
private:
    string cardNumber;

public:
    CardPayment(double amount, string cardNo) : Payment(amount), cardNumber(cardNo) {}

    void processPayment() override {
        cout << "Płatność kartą. Kwota: " << amount << " zł\n";
    }
};

class CashPayment : public Payment {
public:
    CashPayment(double amount) : Payment(amount) {}

    void processPayment() override {
        cout << "Płatność gotówką. Kwota: " << amount << " zł\n";
    }
};


int main() { 
    system("cls");

    return 0;
}
