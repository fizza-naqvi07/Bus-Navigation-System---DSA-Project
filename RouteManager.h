
#define ROUTE_MANAGER_H

#include "StopManager.h"

struct Route {
    int routeID;
    string startStop;
    string endStop;
    int distance;
    int time;
    Route* next;
};

class RouteHashTable {
private:
    static const int SIZE = 20;
    Route* table[SIZE];

    int hashFunction(int key);

public:
    RouteHashTable();

    void insert(Route* r);
    Route* search(int routeID);
    void remove(int routeID);
};

class RouteManager {
private:
    Route* head;
    RouteHashTable routeTable;
    StopManager* stopManager;

public:
    RouteManager(StopManager* sm);

    void addRoute(int id, string start, string end, int dist, int time);
    void removeRoute(int id);

    Route* getAllRoutes();      // mem 2 & 4
    Route* getRouteByID(int id);
};

#endif
