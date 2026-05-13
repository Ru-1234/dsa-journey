#ifndef Q1_H
#define Q1_H

#include <iostream>
#include <string>
using namespace std;

struct player {
    string playerName;
    int currentEnergy;
    int tokensCollected;
    bool eliminationStatus;
    player* next;

    player(string name) : playerName(name) 
    {
        currentEnergy = 100;
        tokensCollected = 0;
        eliminationStatus = false;
        next = NULL;
    }
};
struct zone {
    string zoneName;
    int difficultyLevel;
    bool isLocked;
    int totalAttempts;
    int consecutiveFails;

    // ref to next and prev zones
    zone* next;
    zone* prev;
    // ref to circular group of players inside the zone
    player* tail;

    zone(string name, int level) :
    zoneName(name), difficultyLevel(level), isLocked(difficultyLevel>=8) , totalAttempts(0), consecutiveFails(0) {
        next = prev = NULL;
        tail = NULL;
    }
};

class RouteManager {
    private:
        zone* head;
        zone* tail;
        zone* currentZone;

        float avgDifficulty() {
            if(head==NULL) return 0;
            double sum = 0;
            int cnt = 0;

            zone* z = head;
            while(z!=NULL) { 
                sum += z->difficultyLevel; 
                cnt++; 
                z = z->next; 
            }
            return sum / cnt;
        }

        void removePlayer(zone *z, string playerName) {
            player* p = z->tail->next; // head
            player* start = p;

            do {
                if(p->next->playerName==playerName) {
                    // only one node
                    if(p->next->next == p->next){
                        delete p;
                        p = NULL;
                        z->tail = p;
                        return;
                    }

                    player* temp = p->next; // to be deleted
                    p->next = p->next->next;

                    if(temp==z->tail) {
                        z->tail = p;
                    }

                    temp->next  = NULL;
                    delete temp;

                    cout << "Player [" << playerName << "]" << "removed!" 
                    <<  "from Zone [" << z->zoneName << "]" << endl;
                    return;
                }
                p = p->next;
            } while(p!=start);
        }

    int totaltokens(zone *z) {
        if(z==NULL || z->tail==NULL) {return 0;}
        player* p = z->tail->next;
        player* start = p;
        int sum = 0;

        do{
            sum += p->tokensCollected;
            p = p->next;
        } while(p!=start);

        return sum;
    }

    void displayPlayers(zone* z){
        if(z->tail==NULL) {
            cout << "This zone has no player!" << endl;
            return;
        }
        player* p = z->tail->next;
        player*start = p;
        do {
            cout << p->playerName << " -> ";
            p= p->next;
        }
        while(p!=start);
        cout << z->tail->playerName << endl;


        cout << "Detailed info of players" << endl;
        p = z->tail->next;
        do {
            cout << p->playerName << " | " << p->currentEnergy <<
            " energy | " << p->tokensCollected << " tokens | " 
            << (p->eliminationStatus? "Eliminated" : " Not Eliminated") << endl;
            p = p->next;
        }
        while(p!=start);
    }

    void swapWithPrev(zone* z) {
        if(z==NULL || z->prev==NULL) return;
        zone* A = z->prev;
        zone* B = z;
        zone* beforeA = A->prev;
        zone* afterB  = B->next;
        if(beforeA!=NULL) // A not head
            beforeA->next = B; 
        else 
            head = B;

        B->prev = beforeA; 
        B->next = A;
        A->prev = B;       
        A->next = afterB;

        if(afterB!=NULL)    // B isnot tai;
            afterB->prev = A; 
        else 
            tail = A;
        cout << "[Route Reorganized] '" << B->zoneName
             << "' swapped with '" << A->zoneName << "'." << endl;
    }

    public:
        RouteManager(){
            head = tail = currentZone = NULL;
        }

        bool empty(){return head == NULL;}

        zone* findZone(string zoneName) {
            zone* z = head;
            while(z!=NULL) {
                if(z->zoneName == zoneName) {
                    return z;
                }
                z = z->next;
            }

            return NULL;
        }

        // new zone -> discover -> insert acc to diff level
        void insertZone(string zoneName, int difficultyLevel);
        void addPlayer(string playerName, string zoneName);
        void MiniChallengeRound(string zoneName);
        void DisplayZone(string zoneName);
        void DisplayFullChallenge();
        void movePlayers(zone* from, zone* to);
        void removeZone(string zoneName);
};
#endif
