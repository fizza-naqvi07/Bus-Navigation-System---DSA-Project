long long opsDijkstra = 0;
long long opsDFS = 0;
long long opsMerge = 0;
long long opsQuick = 0;

#include <iostream>
using namespace std;

struct Route
{
    int id, src, dest;
    double dist, time;
    int popularity;
    Route *next;
};

struct Stop
{
    int id;
    char name[50];
    Route *head;
};

struct StackNode
{
    int id, type;
    StackNode *next;
};

class Stack
{
    StackNode *top;

public:
    Stack() : top(0) {}
    void push(int id, int t) { top = new StackNode{id, t, top}; }
    bool pop(int &id, int &t)
    {
        if (!top)
            return false;
        id = top->id;
        t = top->type;
        StackNode *x = top;
        top = top->next;
        delete x;
        return true;
    }
};

struct QNode
{
    int s, d;
    QNode *next;
};

class Queue
{
    QNode *f, *r;

public:
    Queue() : f(0), r(0) {}
    void enqueue(int s, int d)
    {
        QNode *n = new QNode{s, d, 0};
        if (!r)
            f = r = n;
        else
            r->next = n, r = n;
    }
    bool dequeue(int &s, int &d)
    {
        if (!f)
            return false;
        s = f->s;
        d = f->d;
        QNode *t = f;
        f = f->next;
        if (!f)
            r = 0;
        delete t;
        return true;
    }
};

struct HeapNode
{
    int v;
    double w;
};

class MinHeap
{
    HeapNode *a;
    int sz, cap;

public:
    long long ops;
    MinHeap(int c) : sz(0), cap(c), ops(0) { a = new HeapNode[c]; }
    ~MinHeap() { delete[] a; }
    bool empty() { return sz == 0; }
    void push(int v, double w)
    {
        ops++;
        int i = sz++;
        a[i] = {v, w};
        while (i && a[(i - 1) / 2].w > a[i].w)
        {
            ops++;
            HeapNode t = a[i];
            a[i] = a[(i - 1) / 2];
            a[(i - 1) / 2] = t;
            i = (i - 1) / 2;
        }
    }
    HeapNode pop()
    {
        ops++;
        HeapNode r = a[0];
        a[0] = a[--sz];
        int i = 0;
        while (i * 2 + 1 < sz)
        {
            int j = i * 2 + 1;
            if (j + 1 < sz && a[j + 1].w < a[j].w)
                j++;
            if (a[i].w <= a[j].w)
                break;
            HeapNode t = a[i];
            a[i] = a[j];
            a[j] = t;
            i = j;
        }
        return r;
    }
};

class BusSystem
{
    Stop *stops;
    int cap;
    Stack undoS, redoS;
    Queue req;
    long long opsLinear, opsBFS, opsDij, opsMerge, opsQuick;

    Route *merge(Route *a, Route *b, int c)
    {
        opsMerge++;
        if (!a)
            return b;
        if (!b)
            return a;
        opsMerge++;
        bool ok = (c == 1 ? a->id <= b->id : (c == 2 ? a->dist <= b->dist : a->popularity >= b->popularity));
        if (ok)
        {
            a->next = merge(a->next, b, c);
            return a;
        }
        b->next = merge(a, b->next, c);
        return b;
    }
    void split(Route *s, Route **f, Route **b)
    {
        Route *fast = s->next, *slow = s;
        while (fast)
        {
            fast = fast->next;
            if (fast)
            {
                slow = slow->next;
                fast = fast->next;
            }
        }
        *f = s;
        *b = slow->next;
        slow->next = 0;
    }
    Route *mergeSort(Route *h, int c)
    {
        if (!h || !h->next)
            return h;
        Route *a, *b;
        split(h, &a, &b);
        return merge(mergeSort(a, c), mergeSort(b, c), c);
    }
    Route *quickSort(Route *h, int c)
    {
        opsQuick++;
        if (!h || !h->next)
            return h;
        Route *p = h, *l = 0, *r = 0;
        while (h->next)
        {
            Route *x = h->next;
            h->next = x->next;
            opsQuick++;
            bool ok = (c == 2 ? x->dist < p->dist : x->popularity > p->popularity);
            if (ok)
            {
                x->next = l;
                l = x;
            }
            else
            {
                x->next = r;
                r = x;
            }
        }
        l = quickSort(l, c);
        r = quickSort(r, c);
        Route *t = l;
        if (!t)
            return p;
        while (t->next)
            t = t->next;
        t->next = p;
        p->next = r;
        return l;
    }
    void dfsPath(int u, int d, bool *vis, int *path, int dep)
    {
        opsDFS++;
        vis[u] = 1;
        path[dep] = u;
        if (u == d)
        {
            for (int i = 0; i <= dep; i++)
            {
                cout << stops[path[i]].name << (i == dep ? "" : " -> ");
                cout << path[i];
             if (i < dep) cout << " -> ";

            }
            cout << endl;
        }
        else if (dep < 5)
        {
            for (Route *e = stops[u].head; e; e = e->next)
                if (!vis[e->dest])
                    dfsPath(e->dest, d, vis, path, dep + 1);
        }
        vis[u] = 0;
    }
    void bfs(int s)
    {
        bool *vis = new bool[cap];
        for (int i = 0; i < cap; i++)
            vis[i] = 0;
        Queue q;
        q.enqueue(s, s);
        vis[s] = 1;
        while (q.dequeue(s, s))
        {
            opsBFS++;
            for (Route *e = stops[s].head; e; e = e->next)
            {
                if (!vis[e->dest])
                {
                    vis[e->dest] = 1;
                    q.enqueue(e->dest, e->dest);
                }
            }
        }
        delete[] vis;
    }

public:
    BusSystem(int c) : cap(c)
    {
        stops = new Stop[cap];
        for (int i = 0; i < cap; i++)
        {
            stops[i].id = i;
            stops[i].head = 0;
        }
        opsLinear = opsBFS = opsDij = opsMerge = opsQuick = 0;
    }
    void addStop(int id, const char *n)
    {
        for (int i = 0; n[i] && i < 49; i++)
            stops[id].name[i] = n[i];
        stops[id].name[49] = 0;
    }
    void addRoute(int id, int u, int v, double d, double t, bool log = 1)
    {
        Route *r = new Route{id, u, v, d, t, 0, stops[u].head};
        stops[u].head = r;
        if (log)
            undoS.push(id, 1);
    }
    void removeRoute(int id, bool log = 1)
    {
        for (int i = 0; i < cap; i++)
        {
            Route *c = stops[i].head, *p = 0;
            while (c)
            {
                if (c->id == id)
                {
                    if (p)
                        p->next = c->next;
                    else
                        stops[i].head = c->next;
                    if (log)
                        undoS.push(id, 2);
                    delete c;
                    return;
                }
                p = c;
                c = c->next;
            }
        }
    }
    void undo()
    {
        int id, t;
        if (!undoS.pop(id, t))
            return;
        if (t == 1)
            removeRoute(id, 0);
        else
            redoS.push(id, t);
    }
    void redo()
    {
        int id, t;
        if (!redoS.pop(id, t))
            return;
        if (t == 2)
            addRoute(id, id % cap, (id + 7) % cap, 5, 10, 0);
    }
    void linearShortest(int s, int d)
    {
        opsLinear = 0;
        int cur = s;
        double cost = 0;
        bool vis[200] = {0};
        while (cur != d)
        {
            vis[cur] = 1;
            Route *best = 0;
            for (Route *e = stops[cur].head; e; e = e->next)
            {
                opsLinear++;
                if (!vis[e->dest] && (!best || e->dist < best->dist))
                    best = e;
            }
            if (!best)
                break;
            best->popularity++;
            cost += best->dist;
            cur = best->dest;
        }
        cout << "Cost: " << cost << endl;
    }
    void dijkstra(int s, int d)
    {
        opsDij = 0;
        double *dist = new double[cap];
        int *par = new int[cap];
        for (int i = 0; i < cap; i++)
        {
            dist[i] = 1e9;
            par[i] = -1;
        }
        MinHeap pq(cap * 5);
        dist[s] = 0;
        pq.push(s, 0);
        while (!pq.empty())
        {
            opsDijkstra++;
            HeapNode h = pq.pop();
            opsDij++;
            if (h.w > dist[h.v])
                continue;
            for (Route *e = stops[h.v].head; e; e = e->next)
            {
                opsDijkstra++;
                opsDij++;
                if (dist[h.v] + e->dist < dist[e->dest])
                {
                    dist[e->dest] = dist[h.v] + e->dist;
                    par[e->dest] = h.v;
                    e->popularity++;
                    pq.push(e->dest, dist[e->dest]);
                }
            }
        }
        cout << "Cost: " << dist[d] << endl;
        delete[] dist;
        delete[] par;
    }
    void discover(int s, int d)
    {
        bool *vis = new bool[cap];
        int *path = new int[cap];
        for (int i = 0; i < cap; i++)
            vis[i] = 0;
        dfsPath(s, d, vis, path, 0);
        delete[] vis;
        delete[] path;
    }
    void reachability(int s)
    {
        opsBFS = 0;
        bfs(s);
        cout << "Reachable ops: " << opsBFS << endl;
    }
    void rankRoutes(int crit, bool useQuick)
    {
        Route *list = 0;

        for (int i = 0; i < cap; i++)
        {
            Route *e = stops[i].head;
            while (e)
            {
                Route *temp = new Route;
                temp->id = e->id;
                temp->src = e->src;
                temp->dest = e->dest;
                temp->dist = e->dist;
                temp->time = e->time;
                temp->popularity = e->popularity;
                temp->next = list;
                list = temp;
                e = e->next;
            }
        }

        opsMerge = 0;
        opsQuick = 0;

        if (useQuick)
            list = quickSort(list, crit);
        else
            list = mergeSort(list, crit);

        cout << "\nTop Ranked Routes:\n";
        cout << "Rank\tRouteID\tValue\n";

        int rank = 1;
        Route *curr = list;
        while (curr && rank <= 10)
        {
            cout << rank << "\t" << curr->id << "\t";
            if (crit == 2)
                cout << curr->dist << endl;
            else
                cout << curr->popularity << endl;

            curr = curr->next;
            rank++;
        }

        while (list)
        {
            Route *t = list;
            list = list->next;
            delete t;
        }
    }

    void loadData(BusSystem &sys)
    {
        int totalStops = 100;
        int totalRoutes = 500;

        for (int i = 0; i < totalStops; i++)
        {
            char name[20];
            name[0] = 'S';
            name[1] = 't';
            name[2] = 'o';
            name[3] = 'p';
            name[4] = '_';
            name[5] = (char)('0' + (i / 10));
            name[6] = (char)('0' + (i % 10));
            name[7] = '\0';
            sys.addStop(i, name);
        }

        int rid = 1000;
        for (int i = 0; i < totalRoutes; i++)
        {
            int src = i % totalStops;
            int dest = (i * 7 + 13) % totalStops;
            if (src == dest)
                dest = (dest + 1) % totalStops;

            double dist = 2.0 + (i % 15) * 1.3;
            double time = 5.0 + (i % 20) * 2.1;

            sys.addRoute(rid++, src, dest, dist, time);
        }

        cout << "\n==============================================\n";
        cout << "DATASET LOADED SUCCESSFULLY\n";
        cout << "Stops   : " << totalStops << endl;
        cout << "Routes  : " << totalRoutes << endl;
        cout << "==============================================\n";
    }

    void runtimeReport()
    {
        cout << "\n===============================================\n";
        cout << "        ALGORITHM RUNTIME ANALYSIS TABLE\n";
        cout << "===============================================\n";
        cout << "Algorithm\t\tOperations\tComplexity\n";
        cout << "-----------------------------------------------\n";
        cout << "Dijkstra\t\t" << opsDijkstra << "\t\tO(E log V)\n";
        cout << "DFS Path Search\t\t" << opsDFS << "\t\tO(V + E)\n";
        cout << "Merge Sort\t\t" << opsMerge << "\t\tO(N log N)\n";
        cout << "Quick Sort\t\t" << opsQuick << "\t\tO(N log N)\n";
        cout << "===============================================\n";
    }

    void resetRuntime() {
    opsDijkstra = 0;
    opsDFS = 0;
    opsMerge = 0;
    opsQuick = 0;
    }

    void enqueueReq(int s, int d) { req.enqueue(s, d); }
    void processReq()
    {
        int s, d;
        if (req.dequeue(s, d))
            dijkstra(s, d);
    }
};

void load(BusSystem &b)
{
    for (int i = 0; i < 100; i++)
    {
        char n[6] = {'S', 't', 'p', (char)('0' + i / 10), (char)('0' + i % 10), 0};
        b.addStop(i, n);
    }
    for (int i = 0; i < 500; i++)
        b.addRoute(1000 + i, i % 100, (i + 17) % 100, 5 + i % 7, 10 + i % 13, 0);
}

void loadData(class BusSystem& s);


int main()
{
    BusSystem sys(150);
    load(sys);

    int role;
    while (true)
    {
        cout << "\n=============================================\n";
        cout << "        BUSNAV - INTELLIGENT BUS SYSTEM       \n";
        cout << "=============================================\n";
        cout << "Select Access Mode:\n";
        cout << "1. Passenger Navigation Portal\n";
        cout << "2. System Administration Panel\n";
        cout << "3. Exit Application\n";
        cout << "Enter Choice: ";
        cin >> role;

        if (role == 3)
        {
            cout << "\nSystem shutting down. Safe travels.\n";
            break;
        }

        if (role == 1)
        {
            int ch, u, v;
            while (true)
            {
                cout << "\n---------------------------------------------\n";
                cout << "        PASSENGER NAVIGATION MENU             \n";
                cout << "---------------------------------------------\n";
                cout << "1. Find Shortest Route (Simple Method)\n";
                cout << "2. Find Reachable Stops (Connectivity Check)\n";
                cout << "3. Find Optimal Route (Dijkstra Algorithm)\n";
                cout << "4. Explore All Possible Routes (DFS)\n";
                cout << "5. Submit Route Request for Processing\n";
                cout << "6. Return to Main Menu\n";
                cout << "Enter Option: ";
                cin >> ch;

                if (ch == 6)
                    break;

                if (ch >= 1 && ch <= 4)
                {
                    cout << "Enter Source Stop ID: ";
                    cin >> u;
                    cout << "Enter Destination Stop ID: ";
                    cin >> v;
                }

                if (ch == 1)
                {
                    cout << "\n[Processing Simple Shortest Route]\n";
                    sys.linearShortest(u, v);
                }
                else if (ch == 2)
                {
                    cout << "\n[Checking Network Reachability]\n";
                    cout << "Enter Source Stop ID: ";
                    cin >> u;
                    sys.reachability(u);
                }
                else if (ch == 3)
                {
                    cout << "\n[Running Dijkstra’s Optimized Routing]\n";
                    sys.dijkstra(u, v);
                }
                else if (ch == 4)
                {
                    cout << "\n[Discovering All Valid Routes]\n";
                    sys.discover(u, v);
                }
                else if (ch == 5)
                {
                    cout << "Enter Source Stop ID: ";
                    cin >> u;
                    cout << "Enter Destination Stop ID: ";
                    cin >> v;
                    sys.enqueueReq(u, v);
                    cout << "Request successfully queued for admin processing.\n";
                }
                else
                {
                    cout << "Invalid option selected.\n";
                }
            }
        }

        else if (role == 2)
        {
            string pin;
            cout << "\nEnter Administrator PIN: ";
            cin >> pin;
            if (pin != "admin123")
            {
                cout << "Authentication failed. Access denied.\n";
                continue;
            }

            int ch, id, u, v, m;
            while (true)
            {
                cout << "\n=============================================\n";
                cout << "           ADMINISTRATIVE CONTROL PANEL       \n";
                cout << "=============================================\n";
                cout << "1. Add New Route to Network\n";
                cout << "2. Remove Existing Route\n";
                cout << "3. Undo Last Operation\n";
                cout << "4. Redo Last Reverted Operation\n";
                cout << "5. View Complete Route Dataset\n";
                cout << "6. Rank Routes (Sorting & Analysis)\n";
                cout << "7. View Runtime & Algorithm Statistics\n";
                cout << "8. Process Passenger Route Requests\n";
                cout << "9. Logout to Main Menu\n";
                cout << "Enter Option: ";
                cin >> ch;

                if (ch == 9)
                {
                    cout << "Logging out from admin panel.\n";
                    break;
                }

                if (ch == 1)
                {
                    cout << "Enter Route ID: ";
                    cin >> id;
                    cout << "Enter Source Stop ID: ";
                    cin >> u;
                    cout << "Enter Destination Stop ID: ";
                    cin >> v;
                    sys.addRoute(id, u, v, 5, 10);
                    cout << "Route successfully added to network.\n";
                }
                else if (ch == 2)
                {
                    cout << "Enter Route ID to Remove: ";
                    cin >> id;
                    sys.removeRoute(id);
                    cout << "Route removed from network.\n";
                }
                else if (ch == 3)
                {
                    sys.undo();
                    cout << "Undo operation executed.\n";
                }
                else if (ch == 4)
                {
                    sys.redo();
                    cout << "Redo operation executed.\n";
                }
                else if (ch == 5)
                {
                    cout << "\nDisplaying route dataset (press Enter to scroll)...\n";
                    BusSystem sys(150);
                    loadData(sys);

                }
                else if (ch == 6)
                {
                    cout << "Select Ranking Mode:\n";
                    cout << "2. Sort by Distance\n";
                    cout << "3. Sort by Popularity\n";
                    cout << "Enter Choice: ";
                    cin >> m;
                    sys.rankRoutes(m, m == 3);
                }
                else if (ch == 7)
                {
                    cout << "\nAlgorithm Runtime Summary:\n";
                    sys.runtimeReport();
                }
                else if (ch == 8)
                {
                    cout << "Processing queued passenger requests...\n";
                    sys.processReq();
                }
                else
                {
                    cout << "Invalid administrative option.\n";
                }
            }
        }
    }
    return 0;
}
