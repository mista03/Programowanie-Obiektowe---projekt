#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <memory>

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
    vector<Guest*> guests;
    vector<Reservation*> reservations;
    vector<Room*> rooms;

public:
    Hotel(string n, string a) : name(n), address(a) {}

    void addGuest(Guest& guest) { guests.push_back(&guest); }
    void addReservation(Reservation& reservation) { reservations.push_back(&reservation); }
    void addRoom(Room& room) { rooms.push_back(&room); }

    vector<Room*> getAvailableRooms(time_t startDate, time_t endDate, unsigned int peopleCount);
    vector<Reservation*> getAllGuestReservations(Guest* guest);
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
    Room(string n, string s, unsigned int price, unsigned int max)
        : roomNumber(n), standard(s), pricePerNight(price), maxAmountOfPeople(max) {}

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
    Guest(string guestName, string guestEmail, string guestPassword)
        : name(guestName), email(guestEmail), password(guestPassword) {}

    string getName() const { return name; }
    string getEmail() const { return email; }
    bool verifyPassword(const string& inputPassword) const { return password == inputPassword; }

    void addReservation(Reservation* reservation) { reservations.push_back(reservation); }
    void displayReservations();
    void cancelReservation();
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
    Guest* guest;

    Reservation(Guest* g, time_t start, time_t end, Room* r)
        : guest(g), startDate(start), endDate(end), room(r), isPaidFor(false) {
        totalPrice = (endDate - startDate) / 86400 * room->getPricePerNight();
        status = "Zarezerwowany";
    }

    void displayDetails() {
        cout << "Rezerwacja dla " << guest->getName() << ". Pokój: " << room->getRoomNumber()
            << ", Cena: " << totalPrice << " zł, Termin: od " << ctime(&startDate)
            << " do " << ctime(&endDate) << "\n";
    }

    void cancel() {
        status = "Odwołany";
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

vector<Room*> Hotel::getAvailableRooms(time_t startDate, time_t endDate, unsigned int peopleCount) {
    vector<Room*> availableRooms;
    for (auto& room : rooms) {
        if (room->getMaxAmountOfPeople() >= peopleCount && room->checkAvailability(startDate, endDate)) {
            availableRooms.push_back(room);
        }
    }
    return availableRooms;
}

vector<Reservation*> Hotel::getAllGuestReservations(Guest* guest) {
    vector<Reservation*> guestReservations;
    for (auto& reservation : reservations) {
        if (reservation->guest == guest) {
            guestReservations.push_back(reservation);
        }
    }
    return guestReservations;
}

void Hotel::displayRooms() {
    cout << "Lista wszystkich pokoi:\n";
    for (auto& room : rooms) {
        room->displayDetails();
    }
}

void Hotel::displayReservations() {
    cout << "Lista rezerwacji:\n";
    for (auto& reservation : reservations) {
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
// Do implementacji
void Guest::cancelReservation() {

    displayReservations();

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

    //Zarządzenie datami do przemyślenia/zmiany - moja propozycja
    // ustalenie doby hotelowej i wybieranie po samych dniach
    time_t startDate = time(0) + 86400; // Jutro
    time_t endDate = time(0) + 3 * 86400; // Dwa dni później
    auto availableRooms = hotel.getAvailableRooms(startDate, endDate, 2);

    if (!availableRooms.empty()) {
        auto reservation = Reservation(&guest1, startDate, endDate, availableRooms[0]);
        guest1.addReservation(&reservation);
        hotel.addReservation(reservation);

        cout << "Rezerwacja pomyślna!\n";
        reservation.displayDetails();
    }

    hotel.displayReservations();

    return 0;
}
