#include "HotelSystem.h"

// HotelEntity
HotelEntity::HotelEntity(string id, string name) : id(id), name(name) {}
string HotelEntity::getId() const { return id; }
string HotelEntity::getName() const { return name; }

// GuestUser
GuestUser::GuestUser(string id, string name, string phone, int points)
    : HotelEntity(id, name), phone(phone), loyaltyPoints(points) {}

string GuestUser::getPhone() const { return phone; }
int GuestUser::getLoyaltyPoints() const { return loyaltyPoints; }
void GuestUser::addLoyaltyPoints(int points) { loyaltyPoints += points; }

bool GuestUser::redeemPoints(int points) {
    if (loyaltyPoints >= points) {
        loyaltyPoints -= points;
        return true;
    }
    return false;
}

void GuestUser::displayDetails() const {
    cout << left << setw(10) << id << setw(20) << name 
         << setw(15) << phone << setw(10) << loyaltyPoints << endl;
}

string GuestUser::serialize() const {
    return id + "," + name + "," + phone + "," + to_string(loyaltyPoints);
}

GuestUser GuestUser::deserialize(const string& line) {
    stringstream ss(line);
    string id, name, phone, ptsStr;
    getline(ss, id, ',');
    getline(ss, name, ',');
    getline(ss, phone, ',');
    getline(ss, ptsStr, ',');
    return GuestUser(id, name, phone, stoi(ptsStr));
}

// BaseRoom
BaseRoom::BaseRoom(int roomNum, double rate, bool occupied)
    : roomNumber(roomNum), baseRate(rate), isOccupied(occupied) {}

int BaseRoom::getRoomNumber() const { return roomNumber; }
double BaseRoom::getBaseRate() const { return baseRate; }
bool BaseRoom::getOccupiedStatus() const { return isOccupied; }
void BaseRoom::setOccupied(bool status) { isOccupied = status; }

void BaseRoom::displayRoom() const {
    cout << left << setw(10) << roomNumber
         << setw(18) << getRoomType()
         << "$" << setw(11) << fixed << setprecision(2) << baseRate
         << (isOccupied ? "OCCUPIED" : "AVAILABLE") << endl;
}

string BaseRoom::serialize() const {
    return to_string(roomNumber) + "," + getRoomType() + "," + to_string(baseRate) + "," + (isOccupied ? "1" : "0");
}

// StandardRoom
StandardRoom::StandardRoom(int roomNum, double rate, bool isDouble, bool occupied)
    : BaseRoom(roomNum, rate, occupied), hasDoubleBeds(isDouble) {}

string StandardRoom::getRoomType() const { return "Standard"; }

double StandardRoom::calculateCost(int nights, double demandMultiplier) const {
    double cost = baseRate * nights * demandMultiplier;
    if (hasDoubleBeds) cost += 15.0 * nights;
    return cost;
}

string StandardRoom::serialize() const {
    return BaseRoom::serialize() + "," + (hasDoubleBeds ? "1" : "0") + ",0";
}

// SuiteRoom
SuiteRoom::SuiteRoom(int roomNum, double rate, bool jacuzzi, bool butler, bool occupied)
    : BaseRoom(roomNum, rate, occupied), hasJacuzzi(jacuzzi), includesButlerService(butler) {}

string SuiteRoom::getRoomType() const { return "Luxury Suite"; }

double SuiteRoom::calculateCost(int nights, double demandMultiplier) const {
    double cost = baseRate * nights * demandMultiplier;
    if (hasJacuzzi) cost += 50.0 * nights;
    if (includesButlerService) cost += 100.0 * nights;
    return cost;
}

string SuiteRoom::serialize() const {
    return BaseRoom::serialize() + "," + (hasJacuzzi ? "1" : "0") + "," + (includesButlerService ? "1" : "0");
}

// Reservation
Reservation::Reservation(string resId, string gId, int rNum, int n, double cost)
    : reservationId(resId), guestId(gId), roomNumber(rNum), nights(n), totalCost(cost) {}

string Reservation::getResId() const { return reservationId; }
string Reservation::getGuestId() const { return guestId; }
int Reservation::getRoomNumber() const { return roomNumber; }
double Reservation::getTotalCost() const { return totalCost; }

void Reservation::display() const {
    cout << left << setw(12) << reservationId << setw(10) << guestId
         << setw(10) << roomNumber << setw(8) << nights
         << "$" << fixed << setprecision(2) << totalCost << endl;
}

string Reservation::serialize() const {
    return reservationId + "," + guestId + "," + to_string(roomNumber) + "," + to_string(nights) + "," + to_string(totalCost);
}

Reservation Reservation::deserialize(const string& line) {
    stringstream ss(line);
    string resId, gId, rNumStr, nStr, costStr;
    getline(ss, resId, ',');
    getline(ss, gId, ',');
    getline(ss, rNumStr, ',');
    getline(ss, nStr, ',');
    getline(ss, costStr, ',');
    return Reservation(resId, gId, stoi(rNumStr), stoi(nStr), stod(costStr));
}

// ReservationEngine
ReservationEngine::ReservationEngine() : currentDemandMultiplier(1.2) {
    loadData();
}

ReservationEngine::~ReservationEngine() {
    saveData();
}

void ReservationEngine::registerGuest(const string& id, const string& name, const string& phone) {
    guests.emplace_back(id, name, phone, 0);
    cout << "\n[SUCCESS] Guest registered successfully.\n";
}

GuestUser* ReservationEngine::findGuest(const string& id) {
    for (auto& g : guests) {
        if (g.getId() == id) return &g;
    }
    return nullptr;
}

shared_ptr<BaseRoom> ReservationEngine::findRoom(int roomNum) {
    for (auto& r : rooms) {
        if (r->getRoomNumber() == roomNum) return r;
    }
    return nullptr;
}

void ReservationEngine::createReservation(const string& guestId, int roomNum, int nights) {
    GuestUser* guest = findGuest(guestId);
    if (!guest) {
        cout << "\n[ERROR] Guest ID not found!\n";
        return;
    }

    shared_ptr<BaseRoom> room = findRoom(roomNum);
    if (!room) {
        cout << "\n[ERROR] Room Number does not exist!\n";
        return;
    }

    if (room->getOccupiedStatus()) {
        cout << "\n[ERROR] Room " << roomNum << " is currently occupied!\n";
        return;
    }

    double totalCost = room->calculateCost(nights, currentDemandMultiplier);

    if (guest->getLoyaltyPoints() >= 100) {
        cout << "\n[PERK] Guest eligible for $20 Loyalty Discount! Redeeming points...\n";
        guest->redeemPoints(100);
        totalCost = max(0.0, totalCost - 20.0);
    }

    string resId = "RES" + to_string(1000 + reservations.size() + 1);
    reservations.emplace_back(resId, guestId, roomNum, nights, totalCost);
    
    room->setOccupied(true);
    int earnedPoints = static_cast<int>(totalCost / 10.0);
    guest->addLoyaltyPoints(earnedPoints);

    cout << "\n============================================\n";
    cout << "         RESERVATION CONFIRMED              \n";
    cout << "============================================\n";
    cout << " Booking ID  : " << resId << "\n";
    cout << " Guest       : " << guest->getName() << "\n";
    cout << " Room        : " << roomNum << " (" << room->getRoomType() << ")\n";
    cout << " Nights      : " << nights << "\n";
    cout << " Dynamic Multiplier: " << currentDemandMultiplier << "x\n";
    cout << " Total Bill  : $" << fixed << setprecision(2) << totalCost << "\n";
    cout << " Points Earned: " << earnedPoints << "\n";
    cout << "============================================\n";
}

void ReservationEngine::cancelReservation(const string& resId) {
    auto it = find_if(reservations.begin(), reservations.end(), [&](const Reservation& r) {
        return r.getResId() == resId;
    });

    if (it != reservations.end()) {
        shared_ptr<BaseRoom> room = findRoom(it->getRoomNumber());
        if (room) room->setOccupied(false);

        reservations.erase(it);
        cout << "\n[SUCCESS] Reservation " << resId << " canceled and room freed.\n";
    } else {
        cout << "\n[ERROR] Reservation ID not found.\n";
    }
}

void ReservationEngine::displayAllRooms() const {
    cout << "\n------------------- ROOM CATALOG -------------------\n";
    cout << left << setw(10) << "Room #" << setw(18) << "Type" << setw(12) << "Base Rate" << "Status\n";
    cout << "----------------------------------------------------\n";
    for (const auto& room : rooms) {
        room->displayRoom();
    }
}

void ReservationEngine::displayAllGuests() const {
    cout << "\n------------------- GUEST DIRECTORY -------------------\n";
    cout << left << setw(10) << "ID" << setw(20) << "Name" << setw(15) << "Phone" << "Loyalty Pts\n";
    cout << "-------------------------------------------------------\n";
    for (const auto& guest : guests) {
        guest.displayDetails();
    }
}

void ReservationEngine::displayAllReservations() const {
    cout << "\n----------------- ACTIVE RESERVATIONS -----------------\n";
    cout << left << setw(12) << "Res ID" << setw(10) << "Guest ID" << setw(10) << "Room #" << setw(8) << "Nights" << "Total Cost\n";
    cout << "-------------------------------------------------------\n";
    for (const auto& res : reservations) {
        res.display();
    }
}

// -----------------------------------------------------------------------
// EXPLICIT FILE HANDLING IMPLEMENTATION
// -----------------------------------------------------------------------
void ReservationEngine::saveData() const {
    // 1. Write Rooms Data to Disk File
    ofstream roomFile(ROOMS_FILE);
    for (const auto& room : rooms) {
        roomFile << room->serialize() << "\n";
    }

    // 2. Write Guest Directory to Disk File
    ofstream guestFile(GUESTS_FILE);
    for (const auto& guest : guests) {
        guestFile << guest.serialize() << "\n";
    }

    // 3. Write Reservation Database to Disk File
    ofstream resFile(RESERVATIONS_FILE);
    for (const auto& res : reservations) {
        resFile << res.serialize() << "\n";
    }
}

void ReservationEngine::loadData() {
    // 1. Read Guests Data from Disk File
    ifstream guestFile(GUESTS_FILE);
    if (guestFile.is_open()) {
        string line;
        while (getline(guestFile, line)) {
            if (!line.empty()) guests.push_back(GuestUser::deserialize(line));
        }
    }

    // 2. Read Rooms Data from Disk File
    ifstream roomFile(ROOMS_FILE);
    if (roomFile.is_open()) {
        string line;
        while (getline(roomFile, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string numStr, type, rateStr, occStr, opt1Str, opt2Str;
            getline(ss, numStr, ',');
            getline(ss, type, ',');
            getline(ss, rateStr, ',');
            getline(ss, occStr, ',');
            getline(ss, opt1Str, ',');
            getline(ss, opt2Str, ',');

            int num = stoi(numStr);
            double rate = stod(rateStr);
            bool occ = (occStr == "1");
            bool opt1 = (opt1Str == "1");
            bool opt2 = (opt2Str == "1");

            if (type == "Standard") {
                rooms.push_back(make_shared<StandardRoom>(num, rate, opt1, occ));
            } else if (type == "Luxury Suite") {
                rooms.push_back(make_shared<SuiteRoom>(num, rate, opt1, opt2, occ));
            }
        }
    } else {
        // Fallback default inventory initialization
        rooms.push_back(make_shared<StandardRoom>(101, 100.0, false));
        rooms.push_back(make_shared<StandardRoom>(102, 120.0, true));
        rooms.push_back(make_shared<SuiteRoom>(201, 300.0, true, true));
        rooms.push_back(make_shared<SuiteRoom>(202, 450.0, true, true));
    }

    // 3. Read Reservations Data from Disk File
    ifstream resFile(RESERVATIONS_FILE);
    if (resFile.is_open()) {
        string line;
        while (getline(resFile, line)) {
            if (!line.empty()) reservations.push_back(Reservation::deserialize(line));
        }
    }
}