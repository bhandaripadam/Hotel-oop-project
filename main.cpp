#include "HotelSystem.h"
#include <limits>

int main() {
    ReservationEngine hotel;
    int choice;

    do {
        cout << "\n============================================\n";
        cout << "      GRAND HORIZON RESERVATION SYSTEM      \n";
        cout << "============================================\n";
        cout << " 1. Display Room Inventory\n";
        cout << " 2. Register New Guest\n";
        cout << " 3. View All Guests\n";
        cout << " 4. Book a Room\n";
        cout << " 5. View Active Reservations\n";
        cout << " 6. Cancel Reservation\n";
        cout << " 7. Save & Exit\n";
        cout << " Select Option [1-7]: ";
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                hotel.displayAllRooms();
                break;
            case 2: {
                string id, name, phone;
                cout << "Enter Guest ID (e.g., G101): "; cin >> id;
                cout << "Enter Full Name: "; cin.ignore(); getline(cin, name);
                cout << "Enter Phone Number: "; cin >> phone;
                hotel.registerGuest(id, name, phone);
                break;
            }
            case 3:
                hotel.displayAllGuests();
                break;
            case 4: {
                string guestId;
                int roomNum, nights;
                cout << "Enter Guest ID: "; cin >> guestId;
                cout << "Enter Room Number: "; cin >> roomNum;
                cout << "Enter Number of Nights: "; cin >> nights;
                hotel.createReservation(guestId, roomNum, nights);
                break;
            }
            case 5:
                hotel.displayAllReservations();
                break;
            case 6: {
                string resId;
                cout << "Enter Reservation ID to Cancel: "; cin >> resId;
                hotel.cancelReservation(resId);
                break;
            }
            case 7:
                cout << "\n[SAVING] Preserving system state to disk...\n";
                cout << "[OFFLINE] System shutdown complete.\n";
                break;
            default:
                cout << "\n[ERROR] Invalid option selected.\n";
        }
    } while (choice != 7);

    return 0;
}