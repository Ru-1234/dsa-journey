#include "q2.h"

void Arena::addContestant(string name, int trustScore) {
    contestant* nn = new contestant(name, trustScore);

    //empty circle
    if(head == NULL) {
        head = nn;
        nn->next = nn;
        nn->prev = nn;
        totalContestants++;
        cout << "[Added] " << name << " (Trust: " << trustScore
             << ") - first contestant" << endl;
        return;
    }

    // only one contestant
    if(head->next == head) {
        head->next = nn;
        head->prev = nn;
        nn->next = head;
        nn->prev = head;
        totalContestants++;
        cout << "[Added] " << name << " (Trust: " << trustScore << ")" << endl;
        checkAllianceMerge(nn);
        return;
    }

    // search for valid gap: both neighbors differ by <= 20
    contestant* cur = head;
    do {
        contestant* nxt = cur->next;
        if(abs(cur->trustScore - trustScore) <= 20 && abs(nxt->trustScore - trustScore) <= 20) {
            nn->next = nxt;
            nn->prev = cur;
            cur->next = nn;
            nxt->prev = nn;
            totalContestants++;
            cout << "[Added] " << name << " (Trust: " << trustScore
                 << ") between '" << cur->name
                 << "' and '" << nxt->name << "'" << endl;
            checkAllianceMerge(nn);
            return;
        }
        cur = cur->next;
    } while(cur != head);

    //no valid gap - ins before head
    contestant* tail = head->prev;
    nn->next = head;
    nn->prev = tail;
    tail->next = nn;
    head->prev = nn;
    head = nn; // new node -> start
    totalContestants++;
    cout << "[Added] " << name << " (Trust: " << trustScore
         << ") - no valid gap, inserted before start" << endl;
    checkAllianceMerge(nn);
}

void Arena::createAlliance(string leaderName) {
    contestant* leader = findContestant(leaderName);
    if(leader==NULL) {
        cout << "Contestant '" << leaderName << "' not found" << endl; return;
    }
    if(leader->allianceTail != NULL) {
        cout << "'" << leaderName << "' already leads an alliance" << endl; return;
    }
    if(findAllianceLeader(leaderName) != NULL) {
        cout << "'" << leaderName << "' is already in an alliance" << endl; return;
    }

    // single member circular list
    allianceMember* m = new allianceMember(leaderName);
    m->next = m;
    leader->allianceTail = m;

    cout << "[Alliance Created] Leader: " << leaderName << endl;
}

void Arena::addAllianceMember(string leaderName, string memberName, int loyalty) {
    contestant* leader = findContestant(leaderName);
    if(leader == NULL || leader->allianceTail == NULL) {
        cout << "Alliance not found for '" << leaderName << endl; return;
    }
    if(findContestant(memberName) ==NULL) {
        cout << "Contestant '" << memberName << "' not in circle" << endl; return;
    }
    if(findAllianceLeader(memberName)!= NULL) {
        cout << "'" << memberName << "' is already in an alliance" << endl; return;
    }

    allianceMember* newM = new allianceMember(memberName, loyalty);
    // insert after tail(end of circle)
    newM->next = leader->allianceTail->next; // newM -> old head
    leader->allianceTail->next= newM;                       // old tail -> newM
    leader->allianceTail = newM;                       // advance tail

    cout << "[Alliance] " << memberName << " (Loyalty: " << loyalty
         << ") joined " << leaderName << "'s alliance" << endl;
}

void Arena::betrayalRound() {
    if(countActive() < 2) {
        cout << "Need at least 2 active contestants" << endl; 
        return;
    }

    roundNumber++;
    cout << "\nROUND " << roundNumber << endl;

    // finding contestant with lowest trust-score
    contestant* betrayer = NULL;
    contestant* cur = head;
    do {
        if(cur->active) {
            if(betrayer == NULL || cur->trustScore < betrayer->trustScore)
                betrayer = cur;
        }
        cur = cur->next;
    } while(cur != head);

    betrayer->betrayalCount++;

    bool reverse = (roundNumber % 5 == 0);
    contestant* victim = reverse? betrayer->prev : betrayer->next;

    cout << "[Betrayal] " << betrayer->name
         << " (Trust: " << betrayer->trustScore << ") -> "
         << victim->name << (reverse ? " [REVERSE ROUND]" : "") << endl;

    eliminateContestant(victim, betrayer);

    checkMigration(betrayer);
    if(betrayer->active) 
        checkAllianceMerge(betrayer);
}

void Arena::checkLoyaltyDecay() {
    if(head==NULL) 
        return;
    contestant* cur = head;
    do {
        if(cur->active && cur->allianceTail!=NULL) {
            if(avgLoyalty(cur->allianceTail) < 30) {
                cout << "[Loyalty Decay] " << cur->name << "'s alliance collapses (avg loyalty < 30)!" << endl;
                deleteAlliance(cur);
            }
        }
        cur = cur->next;
    } 
    while(cur != head);
}

void Arena::displayCircle() {
    if(head == NULL) { 
        cout << "Arena is empty" << endl; 
        return; 
    }
    cout << "\nCONTESTANT CIRCLE" << endl;
    contestant* cur = head;
    do {
        cout << "  " << cur->name << " | Trust: "     << cur->trustScore
             << " | Betrayals: " << cur->betrayalCount
             << (cur->allianceTail != NULL ? " [ALLIANCE LEADER]" : "")
             << endl;
        cur = cur->next;
    } 
    while(cur != head);
    cout << "  Round: "  << roundNumber << " | Active: " << countActive() << endl;
}

void Arena::displayAlliance(string leaderName) {
    contestant* leader = findContestant(leaderName);
    if(leader == NULL) { 
        cout << "Contestant not found" << endl; 
        return; 
    }
    if(leader->allianceTail == NULL) {
        cout << leaderName << " has no alliance" << endl; 
        return;
    }
    cout << "Alliance of " << leaderName << endl;
    displayAllianceMembers(leader);
}

int main() {
    Arena A;
    string name, name2;
    int trust, loyalty;
    int choice;

    do {
        cout << "\n-----------------------------------------------------" << endl;
        cout << "|           The Circle of Betrayal                  |" << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << "1. Add Contestant"       << endl;
        cout << "2. Create Alliance"      << endl;
        cout << "3. Add Alliance Member"  << endl;
        cout << "4. Run Betrayal Round"   << endl;
        cout << "5. Check Loyalty Decay"  << endl;
        cout << "6. Display Circle"       << endl;
        cout << "7. Display Alliance"     << endl;
        cout << "8. Exit"                 << endl;
        cout << "Enter Choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "Contestant Name: ";
                getline(cin, name);
                cout << "Trust Score: ";
                cin >> trust;
                cin.ignore();
                A.addContestant(name, trust);
                break;

            case 2:
                cout << "Leader Name: ";
                getline(cin, name);
                A.createAlliance(name);
                break;

            case 3:
                cout << "Leader Name: ";
                getline(cin, name);
                cout << "Member Name: ";
                getline(cin, name2);
                cout << "Loyalty: ";
                cin >> loyalty;
                cin.ignore();
                A.addAllianceMember(name, name2, loyalty);
                break;

            case 4:
                A.betrayalRound();
                break;

            case 5:
                A.checkLoyaltyDecay();
                break;

            case 6:
                A.displayCircle();
                break;

            case 7:
                cout << "Leader Name: ";
                getline(cin, name);
                A.displayAlliance(name);
                break;

            case 8:
                cout << "Exiting" << endl;
                break;

            default:
                cout << "Invalid choice" << endl;
        }
    } while(choice != 8);

    return 0;
}
