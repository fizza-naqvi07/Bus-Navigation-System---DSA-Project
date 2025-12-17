#ifndef BUSNAV_H
#define BUSNAV_H

#include <iostream>

using namespace std;

struct Route {
    int id;
    int dest;
    double dist;
    double time;
    Route* next;
};

struct Stop {
    int id;
    char name[50];
    Route* head;
};

class Stack {
    struct Node { int val; Node* next; };
    Node* top;
public:
    Stack() : top(nullptr) {}
    void push(int v) { top = new Node{v, top}; }
    int pop() {
        if (!top) return -1;
        int v = top->val;
        Node* t = top;
        top = top->next;
        delete t;
        return v;
    }
};

class Queue {
    struct Node { int s, d; Node* next; };
    Node *f, *r;
public:
    Queue() : f(nullptr), r(nullptr) {}
    void enqueue(int s, int d) {
        Node* n = new Node{s, d, nullptr};
        if (!r) { f = r = n; return; }
        r->next = n; r = n;
    }
    bool dequeue(int& s, int& d) {
        if (!f) return false;
        s = f->s; d = f->d;
        Node* t = f; f = f->next;
        if (!f) r = nullptr;
        delete t;
        return true;
    }
};

class BusSystem {
    Stop* stops;
    int capacity;
    int currentCount;
    Stack undo;
    Queue requests;

    void resize() {
        int newCap = capacity * 2;
        Stop* newStops = new Stop[newCap];
        for (int i = 0; i < currentCount; i++) newStops[i] = stops[i];
        for (int i = currentCount; i < newCap; i++) {
            newStops[i].id = -1;
            newStops[i].head = nullptr;
        }
        delete[] stops;
        stops = newStops;
        capacity = newCap;
    }

public:
    BusSystem(int cap) : capacity(cap), currentCount(0) {
        stops = new Stop[capacity];
        for (int i = 0; i < capacity; i++) {
            stops[i].id = -1;
            stops[i].head = nullptr;
        }
    }

    void addStop(int id, const char* name) {
        if (id >= capacity) resize();
        stops[id].id = id;
        int i = 0;
        while (name[i] && i < 49) { stops[id].name[i] = name[i]; i++; }
        stops[id].name[i] = '\0';
        currentCount++;
    }

    void addRoute(int rid, int u, int v, double d, double t, bool log = true) {
        stops[u].head = new Route{rid, v, d, t, stops[u].head};
        if (log) undo.push(rid);
    }

    void removeRoute(int rid) {
        for (int i = 0; i < capacity; i++) {
            Route *curr = stops[i].head, *prev = nullptr;
            while (curr) {
                if (curr->id == rid) {
                    if (!prev) stops[i].head = curr->next;
                    else prev->next = curr->next;
                    delete curr;
                    return;
                }
                prev = curr; curr = curr->next;
            }
        }
    }

    void dijkstra(int s, int d, bool mode) {
        double* dist = new double[capacity];
        int* par = new int[capacity];
        bool* vis = new bool[capacity];
        for (int i = 0; i < capacity; i++) { dist[i] = 1e9; par[i] = -1; vis[i] = false; }
        dist[s] = 0;
        for (int i = 0; i < capacity; i++) {
            int u = -1;
            for (int j = 0; j < capacity; j++)
                if (!vis[j] && (u == -1 || dist[j] < dist[u])) u = j;
            if (u == -1 || dist[u] == 1e9) break;
            vis[u] = true;
            for (Route* e = stops[u].head; e; e = e->next) {
                double w = mode ? e->dist : e->time;
                if (dist[u] + w < dist[e->dest]) {
                    dist[e->dest] = dist[u] + w;
                    par[e->dest] = u;
                }
            }
        }
        if (dist[d] == 1e9) cout << "\n[System Notification] No viable path found between the selected terminals.\n";
        else {
            cout << "\n>>> OPTIMIZED ROUTE CALCULATION COMPLETE <<<\n";
            cout << "Navigation Path: "; printPath(par, d);
            cout << "\nCumulative " << (mode ? "Distance: " : "Travel Time: ") << dist[d] << (mode ? " km" : " minutes") << endl;
        }
        delete[] dist; delete[] par; delete[] vis;
    }

    void printPath(int* p, int j) {
        if (p[j] == -1) { cout << stops[j].name; return; }
        printPath(p, p[j]); cout << " -> " << stops[j].name;
    }

    void displayTable() {
        cout << "\n============================================================\n";
        cout << "               CENTRAL ROUTE DATABASE (TABLE VIEW)          \n";
        cout << "============================================================\n";
        cout << "Route ID\tOrigin\t\tDestination\tDist\tTime\n";
        cout << "------------------------------------------------------------\n";
        for (int i = 0; i < capacity; i++) {
            Route* curr = stops[i].head;
            while (curr) {
                cout << curr->id << "\t\t" << stops[i].name << "\t\t" << "Stp" << curr->dest << "\t\t" << curr->dist << "\t" << curr->time << endl;
                curr = curr->next;
            }
        }
    }

    void queueReq(int s, int d) { requests.enqueue(s, d); }
    void process() {
        int s, d;
        if (requests.dequeue(s, d)) {
            cout << "\nInitiating processing for request: Terminal " << s << " to Terminal " << d << endl;
            dijkstra(s, d, true);
        } else cout << "\n[System Status] Request Queue is currently empty. No pending tasks.\n";
    }

    void performUndo() {
        int id = undo.pop();
        if (id != -1) { removeRoute(id); cout << "\n[Undo Success] Route Entry " << id << " has been purged from system memory.\n"; }
        else cout << "\n[System Warning] No recent transactions available to undo.\n";
    }
};

#endif