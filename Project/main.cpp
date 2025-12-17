#include "BusNav.h"

void clearScreen() {
    for(int i = 0; i < 40; i++) cout << endl;
}

void loadDataset(BusSystem& s) {
    for (int i = 0; i < 100; i++) {
        char name[10] = {'S', 't', 'p', (char)(i / 10 + '0'), (char)(i % 10 + '0'), '\0'};
        s.addStop(i, name);
    }
    for (int i = 0; i < 500; i++) {
        s.addRoute(1000 + i, i % 100, (i + 7) % 100, 4.0 + (i % 8), 12.0 + (i % 12), false);
    }
}

bool login() {
    char user[20], pass[20];
    cout << "\n--- ADMINISTRATIVE ACCESS CONTROL ---\n";
    cout << "Employee Username: "; cin >> user;
    cout << "Security Password: "; cin >> pass;
    if (user[0]=='a' && user[1]=='d' && pass[0]=='a' && pass[1]=='d') return true;
    return false;
}

int main() {
    BusSystem sys(128);
    loadDataset(sys);
    int role;

    while (true) {
        cout << "\n============================================\n";
        cout << "      BUSNAV: ENTERPRISE TRANSIT SYSTEM     \n";
        cout << "============================================\n";
        cout << "Select Access Level:\n";
        cout << "1. Commuter Portal (Public Access)\n";
        cout << "2. System Administrator (Secure Access)\n";
        cout << "3. Terminate Application\n";
        cout << "Response >> "; cin >> role;

        if (role == 3) break;
        clearScreen();

        if (role == 1) {
            int ch;
            while (true) {
                cout << "\n--- COMMUTER NAVIGATION INTERFACE ---\n";
                cout << "1. Calculate Shortest Path (Distance-Optimized)\n";
                cout << "2. Calculate Fastest Path (Time-Optimized)\n";
                cout << "3. Log Formal Route Connection Request\n";
                cout << "4. Sign Out to Main Menu\n";
                cout << "Choice >> "; cin >> ch;
                if (ch == 4) break;
                int u, v;
                if (ch == 1 || ch == 2) {
                    cout << "Origin Terminal ID: "; cin >> u; 
                    cout << "Destination Terminal ID: "; cin >> v;
                    sys.dijkstra(u, v, (ch == 1));
                } else if (ch == 3) {
                    cout << "Requested Origin ID: "; cin >> u; 
                    cout << "Requested Destination ID: "; cin >> v;
                    sys.queueReq(u, v); 
                    cout << "[Notification] Your request has been logged in the processing queue.\n";
                }
            }
        } 
        else if (role == 2) {
            if (login()) {
                int ch;
                while (true) {
                    cout << "\n--- ADMINISTRATIVE DASHBOARD ---\n";
                    cout << "1. Process Commuter Route Requests (FIFO)\n";
                    cout << "2. Append Manual Route Entry to Network\n";
                    cout << "3. Generate Global Connectivity Report (Table)\n";
                    cout << "4. Revert Last Transaction (Stack-Based Undo)\n";
                    cout << "5. System Log-Off\n";
                    cout << "Choice >> "; cin >> ch;
                    if (ch == 5) break;
                    switch (ch) {
                        case 1: sys.process(); break;
                        case 2: {
                            int id, u, v; double d, t;
                            cout << "Enter Unique Route ID: "; cin >> id;
                            cout << "Origin ID: "; cin >> u;
                            cout << "Destination ID: "; cin >> v;
                            cout << "Distance (km): "; cin >> d;
                            cout << "Est. Time (min): "; cin >> t;
                            sys.addRoute(id, u, v, d, t); 
                            cout << "[Update] Network registry updated with new route entry.\n";
                            break;
                        }
                        case 3: sys.displayTable(); break;
                        case 4: sys.performUndo(); break;
                    }
                }
            } else {
                cout << "\n[Access Denied] Invalid Credentials Provided.\n";
            }
        }
    }
    return 0;
}