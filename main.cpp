#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <memory>

using namespace std;

unsigned int nextGuestId = 0;
unsigned int nextReservationId = 0;
unsigned int nextPaymentId = 0;

class Hotel;
class Room;
class Guest;
class Reservation;
class Payment;

class Hotel {
private:
    string name;
    string address;
    vector<Guest*> guests;
    vector<Room*> rooms;

public:
    Hotel(string n, string a) : name(n), address(a) {}

    void addGuest(Guest& guest) { guests.push_back(&guest); }
    void addRoom(Room& room) { rooms.push_back(&room); }

    vector<Room*> getAvailableRooms(time_t startDate, time_t endDate, unsigned int peopleCount);
    vector<Reservation*> getAllReservations();
    void displayRooms();
    void displayReservations();
};

class Room {
private:
    string roomNumber;
    string standard;
    unsigned int pricePerNight;
    unsigned int maxAmountOfPeople;
    vector<pair<time_t, time_t>> bookings;

public:
    Room(string n, string s, unsigned int p, unsigned int m)
        : roomNumber(n), standard(s), pricePerNight(p), maxAmountOfPeople(m) {}

    void setPrice(unsigned int price) { pricePerNight = price; }
    void setMaxAmountOfPeople(unsigned int max) { maxAmountOfPeople = max; }

    string getRoomNumber() const { return roomNumber; }
    string getStandard() const { return standard; }
    unsigned int getPricePerNight() const { return pricePerNight; }
    unsigned int getMaxAmountOfPeople() const { return maxAmountOfPeople; }

    bool checkAvailability(time_t startDate, time_t endDate) {
        for (const auto& booking : bookings) {
            if (startDate < booking.second && endDate > booking.first) {
                return false;
            }
        }
        return true;
    }

    void bookRoom(time_t startDate, time_t endDate) {
        bookings.push_back({startDate, endDate});
    }

    void displayDetails() {
        cout << "Pokój " << roomNumber << " (" << standard << "), Cena: " << pricePerNight
            << " zł, Max osób: " << maxAmountOfPeople << "\n";
    }
};

class Guest {
private:
    string name;
    string email;
    string password;
    vector<Reservation*> reservations;

public:
    unsigned int guestId;

    Guest(string guestName, string guestEmail, string guestPassword)
        : guestId(nextGuestId++), name(guestName), email(guestEmail), password(guestPassword) {}

    string getName() const { return name; }
    string getEmail() const { return email; }
    vector<Reservation*> getReservations() { return reservations; }


    bool verifyPassword(const string& inputPassword) const { return password == inputPassword; }
    void addReservation(Reservation* reservation) { reservations.push_back(reservation); }
    
    void displayReservations();
};

class Reservation {
private:
    time_t startDate;
    time_t endDate;
    Room* room;
    unsigned int totalPrice;
    bool isPaidFor;
    string status;

public:
    unsigned int reservationId;
    Guest* guest;

    Reservation(Guest* g, time_t start, time_t end, Room* r)
        : reservationId(nextReservationId++), guest(g), startDate(start), endDate(end), room(r), isPaidFor(false) {
        totalPrice = (endDate - startDate) / 86400 * room->getPricePerNight();
        status = "Zarezerwowany";
    }

    void displayDetails() {
        char start[64], end[64];
        
        struct tm* datetime1 = localtime(&startDate);
        strftime(start, 64, "%Y-%m-%d %H:%M", datetime1);   

        struct tm* datetime2 = localtime(&endDate);
        strftime(end, 64, "%Y-%m-%d %H:%M", datetime2);

        cout << "Rezerwacja " << reservationId
            << " dla " << guest->getName() << ". Pokój: " << room->getRoomNumber() 
            << ", Cena: " << totalPrice << " zł, \nod " 
            << start << " do " << end
            << ",\nstatus: " << status << "\n\n";
    }

    void cancel() {
        status = "cancelled";
        cout << "Rezerwacja anulowana.\n";
    }

    void markAsPaid() {
        isPaidFor = true;
        cout << "Rezerwacja opłacona.\n";
    }
};

class Payment {
protected:
    double amount;
    time_t paymentDate;
    string method;

public:
    unsigned int paymentId;

    Payment(double a) : paymentId(nextPaymentId++), amount(a) {}

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


vector<Room*> Hotel::getAvailableRooms(time_t startDate, time_t endDate, unsigned int peopleCount) {
    vector<Room*> availableRooms;
    for (auto& room : rooms) {
        if (room->getMaxAmountOfPeople() >= peopleCount && room->checkAvailability(startDate, endDate)) {
            availableRooms.push_back(room);
        }
    }
    return availableRooms;
}

vector<Reservation*> Hotel::getAllReservations() {
    vector<Reservation*> allReservations;
    vector<Reservation*> guestReservations;
    for (auto& guest : guests) {
        guestReservations = guest->getReservations();
        allReservations.insert(allReservations.end(), guestReservations.begin(), guestReservations.end());
    }
    return allReservations;
}

void Hotel::displayRooms() {
    cout << "Lista wszystkich pokoi:\n";
    for (auto& room : rooms) {
        room->displayDetails();
    }
}

void Hotel::displayReservations() {
    vector<Reservation*> allReservations = this->getAllReservations();
    cout << "Lista rezerwacji:\n";
    for (auto& reservation : allReservations) {
        reservation->displayDetails();
    }
}

void Guest::displayReservations() {
    if (reservations.empty()) {
        cout << "Brak rezerwacji.\n";
    } else {
        cout << "Twoje rezerwacje:\n";
        for (auto& reservation : reservations) {
            reservation->displayDetails();
        }
    }
}


int main() {
    // Testowanie kodu
    Room room1("101", "Standard", 200, 2);
    Room room2("102", "Deluxe", 300, 3);
    Guest guest1("Jan Kowalski", "jankowalski@gmail.com", "haslo123");
    Hotel hotel("Słoneczny młyn", "Portowa 5");

    hotel.addRoom(room1);
    hotel.addRoom(room2);
    hotel.addGuest(guest1);

    // Zarządzenie datami
    // doba hotelowa
    string checkOutTime = " 15:00:00";
    string checkInTime = " 10:00:00";

    // uzytkownik podaje date jako string
    // TODO: sprawdzanie regex daty, prosba o ponowne podanie
    
    // od
    // string from = "2025-01-02";
    // string year = from.substr(0, 4);
    // string month = from.substr(5, 2);
    // string day = from.substr(8, 2);
    // cout << year << " " << month << " " << day << "\n";
    // cout << stoi(year) << " " << stoi(month) << " " << stoi(day) << "\n";

    struct tm* start;
    struct tm* end;

    string format = "%Y-%m-%d %H:%M:%S";
    char* from = "2025-01-02";
    strcat(from, checkInTime);
    strptime(from, format, &start);
    char* to = "2025-01-06";
    strcat(from, checkOutTime);
    strptime(to, format, &end);
    
    time_t startDate = mktime(start);
    time_t endDate = mktime(end);

    auto availableRooms = hotel.getAvailableRooms(startDate, endDate, 2);

    if (!availableRooms.empty()) {
        auto reservation1 = Reservation(&guest1, startDate, endDate, availableRooms[0]);
        auto reservation2 = Reservation(&guest1, startDate, endDate, availableRooms[0]);
        guest1.addReservation(&reservation1);
        guest1.addReservation(&reservation2);

        cout << "Rezerwacja pomyślna!\n";
        // reservation.displayDetails();
    }

    hotel.displayReservations();

    return 0;
}
