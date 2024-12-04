#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <memory>
#include <iomanip>
#include <regex>

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
    // doba hotelowa
    string checkInTime;
    string checkOutTime;
    string password;

public:
    Hotel(string n, string a, string chkInTime, string chkOutTime, string p)
        : name(n), address(a), checkInTime(chkInTime), checkOutTime(chkOutTime),password(p) {}

    void addGuest(Guest& guest) { guests.push_back(&guest); }
    void addRoom(Room& room) { rooms.push_back(&room); }

    vector<Room*> getAvailableRooms(time_t startDate, time_t endDate, unsigned int peopleCount);
    vector<Reservation*> getAllReservations();
    string getCheckInTime() { return checkInTime; }
    string getCheckOutTime() { return checkOutTime; }
    string getPassword() { return password; }

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
            // FIXME? JESLI datetime albo konczy sie, miesci sie w calosci, lub zaczyna się w zajetym zakresie ZWROC FALSE
            // doba hotelowa nie przewiduje ze rezerwacja1 konczy sie np o 15 a rezerwacja2 zaczyna sie o 15, bo musi byc przerwa
            bool isContained = startDate <= booking.second && endDate >= booking.first;
            bool endsWithin = startDate < booking.first && (endDate >= booking.first && endDate <= booking.second);
            bool startsWithin = (startDate >= booking.first && startDate <= booking.second) && endDate > booking.second;
            if (isContained || endsWithin || startsWithin) {
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
        totalPrice = ((endDate - startDate) / 86400 + 1) * room->getPricePerNight();
        status = "potwierdzona";
    }

    void displayDetails() {
        char start[64], end[64];

        // v TODO: mozna by ujednolicic z get_time, czyli tutaj put_time
        struct tm* datetime1 = localtime(&startDate);
        strftime(start, 64, "%Y-%m-%d %H:%M", datetime1);

        struct tm* datetime2 = localtime(&endDate);
        strftime(end, 64, "%Y-%m-%d %H:%M", datetime2);
        // ^

        cout << "Rezerwacja #" << reservationId
            << " dla " << guest->getName() << ". Pokój: " << room->getRoomNumber()
            << ", Cena: " << totalPrice << " zł, \nod "
            << start << " do " << end
            << ",\nstatus: " << status << "\n\n";
    }

    void cancel() {
        status = "anulowana";
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
        bool isBigEnough = room->getMaxAmountOfPeople() >= peopleCount;
        bool isAvailable = room->checkAvailability(startDate, endDate);
        if (isBigEnough && isAvailable) {
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

string inputDate() {
    // uzytkownik podaje date jako string
    string date;
    // TODO: nie wiem jak wykrywac nieprawidlowe daty, np. 2025-02-31, 2025-31-31
    // trzeba by pewnie zlaczyc inputDate() i convertDate()
    regex datePattern(R"(\d{4}-\d{2}-\d{2})"); // YYYY-MM-DD

    // sprawdzanie regex daty, ewentualna prosba o ponowne podanie
    while (true) {
        cin >> date;
        if (regex_match(date, datePattern)) {
            return date;
        } else {
            cout << "Nieprawidłowy format daty. Spróbuj ponownie.\n";
        }
    }
}

time_t convertDate(string datetime) {
    tm dt = {};
    istringstream(datetime) >> get_time(&dt, "%Y-%m-%d %H:%M:%S");

    return mktime(&dt);
}

void manageHotelProfile(Hotel* hotel);
void manageGuestLogging(Hotel* hotel);
void manageGuestProfile(Guest* guest);


void displayMainManu(Hotel* hotel) {
    int choice;

    while (choice!=3) {
        cout << "\n=== Witaj w aplikacji hotelowej ===\n";
        cout << "1. Profil hotelu\n";
        cout << "2. Profil gościa\n";
        cout << "3. Zakończ\n";
        cout << "Twój wybór: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Profil hotelu\n";
                manageHotelProfile(hotel);
            break;
            case 2:
                cout << "Profil gościa\n";
                //manageGuestProfile(hotel);
            break;
            case 3:
                cout << "Do widzenia!\n";
            break;
            default:
                cout << "Nieprawidłowy wybór. Spróbuj ponownie.\n";
        }
    }
}

void manageHotelProfile(Hotel* hotel) {
    string password;
    string adminPassword = hotel->getPassword();
    int choice;
    int attempts = 3;
    while (attempts>0) {
        cout << "\nPodaj hasło administratora: ";
        cin >> password;

        if (password != adminPassword) {
            cout << "Nieprawidłowe hasło! Liczba pozostałych prób: " << --attempts << "\n";
            if (attempts == 0) {
                cout << "Powrót do menu!\n";
                return;
            }
        } else {
            break;

        }
    }
    // TODO: Dodać resztę możliwych działań administratora hotelu.
    while (choice != 4){
        cout << "\n=== Profil hotelu ===\n";
        cout << "1. Wyświetl wszystkie pokoje\n";
        cout << "2. Sprawdź dostępność pokoi\n";
        cout << "3. Wyświetl rezerwacje\n";
        cout << "4. Powrót do głównego menu\n";
        cout << "Twój wybór: ";
        cin >> choice;

        switch (choice) {
            case 1:
                hotel->displayRooms();
                break;
            case 2: {
                cout << "Podaj datę początku rezerwacji\n";
                time_t startDate = convertDate(inputDate());
                cout << "Podaj datę końca rezerwacji\n";
                time_t endDate = convertDate(inputDate());

                auto rooms = hotel->getAvailableRooms(startDate,endDate,1);
                cout << "Lista wszystkich pokoi:\n";
                for (auto& room : rooms) {
                    room->displayDetails();
                    }
                break;
                }
            case 3:
                hotel->displayReservations();
                break;
            case 4:
                cout << "Powrót do głównego menu\n";
                return;
        }
    }
}
// TODO Stworzyć wybór dodanie użytkownika/logowania
void manageGuestLogging(Hotel* hotel) {
    cout << "Logowanie użytkownika\n";
    // manageGuestProfile(&guest);
}

// TODO Stworzyć wybór działań dla użytkownika
void manageGuestProfile(Guest* guest) {
    cout << "Zarządzenie gośćmi\n";
}


int main() {
    // Testowanie kodu
    Room room1("101", "Standard", 200, 2);
    Room room2("102", "Deluxe", 300, 3);
    Room room3("103", "Standard", 300, 4);
    Room room4("104", "Deluxe", 600, 5);
    Guest guest1("Jan Kowalski", "jankowalski@gmail.com", "haslo123");
    Hotel hotel("Słoneczny młyn", "Portowa 5", "15:00:00", "10:00:00","admin123");

    hotel.addRoom(room1);
    hotel.addRoom(room2);
    hotel.addRoom(room3);
    hotel.addRoom(room4);
    hotel.addGuest(guest1);

    // nwm czy to ma sens chcialem ukryc dobe hotelowa w klasie
    string checkInTime = hotel.getCheckInTime();
    string checkOutTime = hotel.getCheckOutTime();
    // string from = inputDate();
    // string to = inputDate();
    string from = "2025-01-02";
    string to = "2025-01-06";
    time_t startDate = convertDate(from + string(" ") + checkInTime);
    time_t endDate = convertDate(to + string(" ") + checkOutTime);

    auto availableRooms = hotel.getAvailableRooms(startDate, endDate, 2);
    // debug
    cout << "dostepne: ";
    for (auto room : availableRooms) {
        cout << room->getRoomNumber() << " ";
    }
    cout << "\n";
    //
    // TODO? mozna zrobic wybor z listy dostepnych pokoi
    if (!availableRooms.empty()) {
        auto chosenRoom = availableRooms[0];
        auto reservation1 = new Reservation(&guest1, startDate, endDate, chosenRoom);
        guest1.addReservation(reservation1);
        chosenRoom->bookRoom(startDate, endDate);

        // cout << "Rezerwacja pomyślna!\n";
    }
    hotel.displayReservations();

    availableRooms = hotel.getAvailableRooms(startDate, endDate, 2);
    // debug
    cout << "dostepne: ";
    for (auto room : availableRooms) {
        cout << room->getRoomNumber() << " ";
    }
    cout << "\n";
    //
    if (!availableRooms.empty()) {
        auto chosenRoom = availableRooms[0];
        auto reservation2 = new Reservation(&guest1, startDate, endDate, chosenRoom);
        guest1.addReservation(reservation2);
        chosenRoom->bookRoom(startDate, endDate);

        // cout << "Rezerwacja pomyślna!\n";
    }

    hotel.displayReservations();

    displayMainManu(&hotel);

    return 0;
}
