#ifndef ROUTE_MANAGER_H
#define ROUTE_MANAGER_H

#include <iostream>
using namespace std;

struct Route {
    int routeId;
    int* stops;      // array of stop IDs
    int stopCount;

    Route() : routeId(-1), stops(nullptr), stopCount(0) {}
    Route(int id, int* stopArray, int count) : routeId(id), stopCount(count) {
        stops = new int[count];
        for (int i = 0; i < count; i++) stops[i] = stopArray[i];
    }

    ~Route() { delete[] stops; }
};

class RouteManager {
private:
    Route* routes;
    int capacity;
    int count;

    void resize() {
        int newCap = capacity * 2;
        Route* temp = new Route[newCap];
        for (int i = 0; i < count; i++)
            temp[i] = routes[i];
        delete[] routes;
        routes = temp;
        capacity = newCap;
    }

public:
    RouteManager(int cap = 10) {
        capacity = cap;
        count = 0;
        routes = new Route[capacity];
    }

    ~RouteManager() { delete[] routes; }

    bool addRoute(int routeId, int* stopList, int stopCount) {
        for (int i = 0; i < count; i++)
            if (routes[i].routeId == routeId) return false; // duplicate

        if (count == capacity) resize();
        routes[count++] = Route(routeId, stopList, stopCount);
        return true;
    }

    bool removeRoute(int routeId) {
        int idx = -1;
        for (int i = 0; i < count; i++)
            if (routes[i].routeId == routeId) { idx = i; break; }
        if (idx == -1) return false;

        for (int i = idx; i < count - 1; i++)
            routes[i] = routes[i + 1];
        count--;
        return true;
    }

    Route* findRoute(int routeId) {
        for (int i = 0; i < count; i++)
            if (routes[i].routeId == routeId) return &routes[i];
        return nullptr;
    }

    void printRoutes() const {
        for (int i = 0; i < count; i++) {
            cout << "Route ID: " << routes[i].routeId << " | Stops: ";
            for (int j = 0; j < routes[i].stopCount; j++)
                cout << routes[i].stops[j] << " ";
            cout << endl;
        }
    }
};

#endif
