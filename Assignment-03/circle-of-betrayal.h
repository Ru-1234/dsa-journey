#ifndef CIRCLE_OF_BETRAYAL_H
#define CIRCLE_OF_BETRAYAL_H

#include <iostream>
#include <string>
using namespace std;

//alliance member - circular singly linked list
struct allianceMember {
    string memberName;
    int loyaltyLevel;
    allianceMember* next;

    allianceMember(string name, int loyalty=50): memberName(name), loyaltyLevel(loyalty) {
        next = NULL;
    }
};

//contestant - doubly circular linked list
struct contestant {
    string name;
    int trustScore;
    int betrayalCount;
    bool active;

    contestant* next;
    contestant* prev;

    //tail of circular alliance member list (tail->next == head)
    allianceMember* allianceTail;

    contestant(string n, int trust): 
    name(n), trustScore(trust), betrayalCount(0), active(true) {
        next = prev = NULL;
        allianceTail = NULL;
    }
};

class Arena {
private:
    contestant* head;
    int totalContestants;
    int roundNumber;

    contestant* findContestant(string name) {
        if(head==NULL) 
            return NULL;
        contestant* c = head;
        do {
            if(c->name==name && c->active) 
                return c;
            c = c->next;
        } 
        while(c != head);
        return NULL;
    }

    contestant* findAllianceLeader(string memberName) {
        if(head==NULL) 
            return NULL;
        contestant* c=head;
        do {
            if(c->active && c->allianceTail!=NULL) { 
                allianceMember* m = c->allianceTail->next; // head of alliance
                allianceMember* start = m;
                do {
                    if(m->memberName == memberName) 
                        return c;
                    m = m->next;
                } 
                while(m != start);
            }
            c = c->next;
        } while(c != head);
        return NULL;
    }

    int totalLoyalty(allianceMember* allianceTail) {
        if(allianceTail==NULL) 
            return 0;

        int sum = 0;
        allianceMember* m = allianceTail->next;
        allianceMember* start = m;
        do { 
            sum += m->loyaltyLevel; 
            m = m->next; 
        }
        while(m != start);
        return sum;
    }

    float avgLoyalty(allianceMember* allianceTail) {
        if(allianceTail == NULL) 
            return 0;
        float sum = 0; 
        int count = 0;
        allianceMember* m = allianceTail->next; // head
        allianceMember* start = m;
        do { 
            sum += m->loyaltyLevel; 
            count++; 
            m = m->next; 
        } 
        while(m != start);
        return sum/count;
    }

    void deleteAlliance(contestant* leader) {
        if(leader->allianceTail == NULL) 
            return;
       
        allianceMember* head  = leader->allianceTail->next;
        leader->allianceTail->next = NULL;
        allianceMember* cur   = head;
        while(cur != NULL) {
            allianceMember* temp = cur->next;
            delete cur;
            cur = temp;
        }
        leader->allianceTail = NULL;
    }

    int countActive() {
        if(head == NULL) 
            return 0;
        int count = 0;
        contestant* c = head;
        do { 
            if(c->active) 
                count++; 
                c = c->next; 
        }while(c != head);
        return count;
    }

    void collapseAlliance(contestant* leader, contestant* eliminator) {
        cout << "[Alliance Collapse] " << leader->name
             << "'s alliance collapses!" << endl;

        allianceMember* m = leader->allianceTail->next; // head
        allianceMember* start = m;
        do {
            if(m->memberName != leader->name) {
                //each member -> loses 10 loyalty
                m->loyaltyLevel -= 10;
                if(m->loyaltyLevel <0) 
                    m->loyaltyLevel = 0;

                eliminator->trustScore -= m->loyaltyLevel;
                if(eliminator->trustScore<0) 
                    eliminator->trustScore = 0;

                cout << "  [Revenge] " << m->memberName
                     << " reduces " << eliminator->name
                     << "'s trust by " << m->loyaltyLevel
                     << " (loyalty now: " << m->loyaltyLevel << ")" << endl;
            }
            m = m->next;
        } 
        while(m != start);

        deleteAlliance(leader);
        cout << "  " << eliminator->name
             << "\'s trust is now: " << eliminator->trustScore << endl;
    }

    void eliminateContestant(contestant* c, contestant* eliminator) {
        cout << "[Eliminated] " << c->name << " removed from the circle." << endl;

        //if victim is a leader
        if(c->allianceTail != NULL)
            collapseAlliance(c, eliminator);

        c->active = false;

        c->prev->next = c->next;
        c->next->prev = c->prev;
        if(head == c) head = c->next;

        totalContestants--;
    }

    // trust < 15 -> mov 2 pos fwd
    void checkMigration(contestant* c) {
        if(c==NULL || !c->active || c->trustScore>= 15 || countActive()<3) 
            return;

        cout << "[Migration] " << c->name
             << " (Trust: " << c->trustScore
             << ") moves 2 positions forward." << endl;

        c->prev->next = c->next;
        c->next->prev = c->prev;
        if(head == c) head = c->next;

        // move 2 steps fwd from where c was
        contestant* insertAfter = c->next; // 1st step
        insertAfter = insertAfter->next;   // 2nd step

        contestant* insertBefore = insertAfter->next;
        c->prev = insertAfter;
        c->next = insertBefore;
        insertAfter->next  = c;
        insertBefore->prev = c;

        cout << "  " << c->name << " is now between '" << c->prev->name << "' and '" << c->next->name << "'." << endl;

        checkAllianceMerge(c);
    }

    void checkAllianceMerge(contestant* c) {
        if(head == NULL) 
            return;

        contestant* neighbors[2] = {c->prev, c->next };
        for(int i = 0; i <2; i++) {
            contestant* nb = neighbors[i];
            if(nb==NULL || !nb->active) 
                continue;
            if(c->allianceTail!= NULL && nb->allianceTail!=NULL) {
                cout << "[Alliance Merge] " << c->name << " and " << nb->name
                     << " are neighbors - merging alliances!" << endl;

                contestant* winner = (totalLoyalty(c->allianceTail)>=totalLoyalty(nb->allianceTail)) ? c : nb;
                contestant* loser = (winner == c) ? nb : c;

                allianceMember* m = loser->allianceTail->next;
                allianceMember* start = m;
                
                do {
                    allianceMember* newM = new allianceMember(m->memberName, m->loyaltyLevel);
                    if(winner->allianceTail == NULL) {
                        newM->next = newM;
                        winner->allianceTail = newM;
                    } 
                    else {
                        newM->next = winner->allianceTail->next;
                        winner->allianceTail->next = newM;
                        winner->allianceTail= newM;
                    }
                    m = m->next;
                } while(m != start);

                deleteAlliance(loser);
                cout << "  " << winner->name << "'s alliance absorbed " << loser->name << "'s alliance." << endl;
                return;
            }
        }
    }

    void displayAllianceMembers(contestant* leader) {
        if(leader->allianceTail == NULL) {
            cout << "  (no alliance)" << endl; 
            return;
        }
        allianceMember* m = leader->allianceTail->next;
        allianceMember* start = m;

        do {
            cout << m->memberName << " | Loyalty: " << m->loyaltyLevel << endl;
            m = m->next;
        } 
        while(m != start);
        cout << "Avg Loyalty: " << avgLoyalty(leader->allianceTail) << endl;
    }

public:
    Arena() : head(NULL), totalContestants(0), roundNumber(0) {}

    bool empty() { return head == NULL; }

    void addContestant(string name, int trustScore);
    void createAlliance(string leaderName);
    void addAllianceMember(string leaderName, string memberName, int loyalty);
    void betrayalRound();
    void checkLoyaltyDecay();
    void displayCircle();
    void displayAlliance(string leaderName);
};

#endif
