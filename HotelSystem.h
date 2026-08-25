#ifndef HOTEL_SYSTEM_H
#define HOTEL_SYSTEM_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

// 1. Abstract Base Class
class HotelEntity {
protected:
    string id;
    string name;

public:
    HotelEntity(string id, string name);
    virtual ~HotelEntity() = default;

    string getId() const;
    string getName() const;
    virtual void displayDetails() const = 0; // Pure virtual function
};

// 2. Guest User Class
class GuestUser : public HotelEntity {
private:
    string phone;
    int loyaltyPoints;

public:
    GuestUser(string id, string name, string phone, int points = 0);

    string getPhone() const;
    int getLoyaltyPoints() const;
    void addLoyaltyPoints(int points);
    bool redeemPoints(int points);

    void displayDetails() const override;
    string serialize() const;
    static GuestUser deserialize(const string& line);
};

// 3. Abstract Base Room Class
class BaseRoom {
protected:
    int roomNumber;
    double baseRate;
    bool isOccupied;

public:
    BaseRoom(int roomNum, double rate, bool occupied = false);
    virtual ~BaseRoom() = default;

    int getRoomNumber() const;
    double getBaseRate() const;
    bool getOccupiedStatus() const;
    void setOccupied(bool status);

    virtual double calculateCost(int nights, double demandMultiplier = 1.0) const = 0;
    virtual string getRoomType() const = 0;

    virtual void displayRoom() const;
    virtual string serialize() const;
};

// Derived Room Types
class StandardRoom : public BaseRoom {
private:
    bool hasDoubleBeds;

public:
    StandardRoom(int roomNum, double rate, bool isDouble = false, bool occupied = false);
    string getRoomType() const override;
    double calculateCost(int nights, double demandMultiplier = 1.0) const override;
    string serialize() const override;
};

class SuiteRoom : public BaseRoom {
private:
    bool hasJacuzzi;
    bool includesButlerService;

public:
    SuiteRoom(int roomNum, double rate, bool jacuzzi = true, bool butler = true, bool occupied = false);
    string getRoomType() const override;
    double calculateCost(int nights, double demandMultiplier = 1.0) const override;
    string serialize() const override;
};

// 4. Reservation Model
class Reservation {
private:
    string reservationId;
    string guestId;
    int roomNumber;
    int nights;
    double totalCost;

public:
    Reservation(string resId, string gId, int rNum, int n, double cost);

    string getResId() const;
    string getGuestId() const;
    int getRoomNumber() const;
    double getTotalCost() const;

    void display() const;
    string serialize() const;
    static Reservation deserialize(const string& line);
};

// 5. Reservation Engine (Core Logic & File Handling)
class ReservationEngine {
private:
    vector<shared_ptr<BaseRoom>> rooms;
    vector<GuestUser> guests;
    vector<Reservation> reservations;
    double currentDemandMultiplier;

    const string ROOMS_FILE = "rooms.txt";
    const string GUESTS_FILE = "guests.txt";
    const string RESERVATIONS_FILE = "reservations.txt";

public:
    ReservationEngine();
    ~ReservationEngine();

    void addRoom(shared_ptr<BaseRoom> room);
    void registerGuest(const string& id, const string& name, const string& phone);
    GuestUser* findGuest(const string& id);
    shared_ptr<BaseRoom> findRoom(int roomNum);

    void createReservation(const string& guestId, int roomNum, int nights);
    void cancelReservation(const string& resId);

    void displayAllRooms() const;
    void displayAllGuests() const;
    void displayAllReservations() const;

    // Persistent Storage Protocol
    void saveData() const;
    void loadData();
};

#endif // HOTEL_SYSTEM_H