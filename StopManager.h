
#define STOP_MANAGER_H

#include <iostream>
#include <string>
using namespace std;

struct Stop {
    string name;
    int connectivityCount;    // Used by Member 3
    Stop* next;
};

class StopManager {
private:
    Stop* head;

public:
    StopManager();

    bool stopExists(string name);
    void addStop(string name);
    void removeStop(string name);

    Stop* getAllStops();  // Used by Members 2, 3, 4
};

#endif
