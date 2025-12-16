#include "RouteManager.h"

RouteHashTable::RouteHashTable() {
    for (int i = 0; i < SIZE; i++)
        table[i] = NULL;
}

int RouteHashTable::hashFunction(int key) {
    return key % SIZE;
}

void RouteHashTable::insert(Route* r) {
    int index = hashFunction(r->routeID);
    r->next = table[index];
    table[index] = r;
}

Route* RouteHashTable::search(int routeID) {
    int index = hashFunction(routeID);
    Route* temp = table[index];
    while (temp) {
        if (temp->routeID == routeID)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void RouteHashTable::remove(int routeID) {
    int index = hashFunction(routeID);
    Route* curr = table[index];
    Route* prev = NULL;

    while (curr) {
        if (curr->routeID == routeID) {
            if (prev)
                prev->next = curr->next;
            else
                table[index] = curr->next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

RouteManager::RouteManager(StopManager* sm) {
    head = NULL;
    stopManager = sm;
}

void RouteManager::addRoute(int id, string start, string end, int dist, int time) {
    if (!stopManager->stopExists(start) || !stopManager->stopExists(end)) {
        cout << "[Error] Invalid stop(s).
";
        return;
    }
    if (routeTable.search(id)) {
        cout << "[Error] Route ID already exists.
";
        return;
    }

    Route* newRoute = new Route;
    newRoute->routeID = id;
    newRoute->startStop = start;
    newRoute->endStop = end;
    newRoute->distance = dist;
    newRoute->time = time;
    newRoute->next = head;
    head = newRoute;

    routeTable.insert(newRoute);
    cout << "[OK] Route added.
";
}

void RouteManager::removeRoute(int id) {
    Route* curr = head;
    Route* prev = NULL;

    while (curr) {
        if (curr->routeID == id) {
            if (prev)
                prev->next = curr->next;
            else
                head = curr->next;
            routeTable.remove(id);
            delete curr;
            cout << "[OK] Route removed.
";
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    cout << "[Error] Route not found.
";
}

Route* RouteManager::getAllRoutes() {
    return head;
}

Route* RouteManager::getRouteByID(int id) {
    return routeTable.search(id);
}
