#include "StopManager.h"

StopManager::StopManager() {
    head = NULL;
}

bool StopManager::stopExists(string name) {
    Stop* temp = head;
    while (temp) {
        if (temp->name == name)
            return true;
        temp = temp->next;
    }
    return false;
}

void StopManager::addStop(string name) {
    if (stopExists(name)) {
        cout << "[Error] Stop already exists.
";
        return;
    }
    Stop* newStop = new Stop;
    newStop->name = name;
    newStop->connectivityCount = 0; // used by Member 3
    newStop->next = head;
    head = newStop;
    cout << "[OK] Stop added.
";
}

void StopManager::removeStop(string name) {
    Stop* curr = head;
    Stop* prev = NULL;

    while (curr) {
        if (curr->name == name) {
            if (prev)
                prev->next = curr->next;
            else
                head = curr->next;
            delete curr;
            cout << "[OK] Stop removed.
";
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    cout << "[Error] Stop not found.
";
}

Stop* StopManager::getAllStops() {
    return head;
}
