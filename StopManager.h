#ifndef STOP_MANAGER_H
#define STOP_MANAGER_H

#include <string>
#include <iostream>
using namespace std;

struct Stop {
    int id;
    string name;

    Stop() : id(-1), name("") {}
    Stop(int _id, string _name) : id(_id), name(_name) {}
};

class StopManager {
private:
    Stop* stops;
    int capacity;
    int count;

    void resize() {
        int newCap = capacity * 2;
        Stop* temp = new Stop[newCap];
        for (int i = 0; i < count; i++)
            temp[i] = stops[i];
        delete[] stops;
        stops = temp;
        capacity = newCap;
    }

public:
    StopManager(int cap = 10) {
        capacity = cap;
        count = 0;
        stops = new Stop[capacity];
    }

    ~StopManager() { delete[] stops; }

    bool addStop(int id, const string& name) {
        for (int i = 0; i < count; i++)
            if (stops[i].id == id) return false; // duplicate

        if (count == capacity) resize();
        stops[count++] = Stop(id, name);
        return true;
    }

    bool removeStop(int id) {
        int idx = -1;
        for (int i = 0; i < count; i++)
            if (stops[i].id == id) { idx = i; break; }
        if (idx == -1) return false;

        for (int i = idx; i < count - 1; i++)
            stops[i] = stops[i + 1];
        count--;
        return true;
    }

    Stop* findStop(int id) {
        for (int i = 0; i < count; i++)
            if (stops[i].id == id) return &stops[i];
        return nullptr;
    }

    void printStops() const {
        for (int i = 0; i < count; i++)
            cout << "Stop ID: " << stops[i].id << " | Name: " << stops[i].name << endl;
    }
};

#endif
