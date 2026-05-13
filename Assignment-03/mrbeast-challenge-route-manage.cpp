#include "q1.h"
#include <ctime>
#include<cstdlib>

void RouteManager::insertZone(string zoneName, int difficultyLevel){
    zone* nn = new zone(zoneName, difficultyLevel);
    if(empty()){
        head = tail = nn;
        return;
    }

    zone* temp = head;
    while(temp!=NULL){
        // same difficulty
        if(temp->difficultyLevel == difficultyLevel){
            if (temp->totalAttempts > nn->totalAttempts) { 
                // temp has more attempts, ne zone first
                nn->next = temp;
                nn->prev = temp->prev;
                if(temp!=head)
                    temp->prev->next = nn;
                else 
                    head = nn;
                temp->prev = nn;
            }
            else{
                nn->prev = temp;
                nn->next = temp->next;
                if(temp==tail)
                    tail = nn;
                else
                    temp->next->prev = nn;
                temp->next = nn;
            }

            cout << "Zone " << nn->zoneName << ", with difficulty level : " <<
            nn->difficultyLevel << " added." << endl;

            return;
        }

        if(temp->difficultyLevel > difficultyLevel){
            nn-> next = temp;
            nn->prev = temp->prev;
            // found at head
            if(head==temp) head = nn;
            else temp->prev->next = nn;
            temp->prev = nn;
            cout << "Zone " << nn->zoneName << ", with difficulty level : " <<
            nn->difficultyLevel << " added." << endl;
            
            return;
        }

        if(temp== tail) {
            tail->next = nn;
            nn->prev = tail;
            tail = nn;

            cout << "Zone " << nn->zoneName << ", with difficulty level : " <<
            nn->difficultyLevel << " added." << endl;
            return;
        }
    temp = temp->next;
    }
}

void RouteManager::addPlayer(string playerName, string zoneName) {
    zone* temp = head;
    zone* container = findZone(zoneName);

    if(container==NULL) {
        cout << "Zone \""<< zoneName << "\" not found!" << endl;
        return;
    }

    player* nn = new player(playerName);
    
    temp = head;
    zone* alreadyExistingZone = NULL;
    bool found = false;

    while(temp!=NULL) {
        if(temp->tail!= NULL){
            player* p = temp->tail->next;  // head of player list
            player* start = p;
            // tail->next = head of player list
            do {
                if(p->playerName==playerName) {
                    found = true;
                    alreadyExistingZone = temp;
                    break;
                }
                p = p->next;
            }
            while(p!=start);
        }    
        if(found) break;
        temp = temp->next;
    }

    if(alreadyExistingZone) {
        cout << "Player named " << playerName << " already exists." << endl;
        // remove player
        player* p = alreadyExistingZone->tail->next;
        player* start = p;
        do {
            if (p->next->playerName==playerName){            
                if(p == p->next){
                    delete p;
                    p = NULL;
                    alreadyExistingZone->tail = p;
                    return;
            
                }
                player* helper = p->next;
                p->next = p->next->next;                
                
                if(helper==alreadyExistingZone->tail) {
                    alreadyExistingZone->tail = p;
                }

                helper->next = NULL;
                delete helper;
                cout << "Player named " << playerName << " removed. " << endl;
                break;
            }

            p = p->next;
        } while(p!=start);
    }

    // inserting
    if(container->tail==NULL) {
        nn->next = nn;
        container->tail = nn;
        cout << "Player named " << playerName <<
        "added in zone \""<< zoneName << "\".";
        return;
    }

    nn->next = container->tail->next;
    container->tail->next = nn;
    container->tail = nn;
    
    cout << "Player named " << playerName <<
    "added in zone \""<< zoneName << "\".";
}

void RouteManager::MiniChallengeRound(string zoneName){
    zone* z = findZone(zoneName);
    if(z==NULL) {cout << "Zone not found!" << endl; return;}
    if(z->isLocked) {cout << "Sorry! zone is currently locked!" << endl; return;}
    if(z->tail==NULL) {cout << "No player found in zone \'" << zoneName << "\'" << endl; return;}

    cout << "           Mini Challenge \'" << zoneName << "\'           " << endl;
    z->totalAttempts++;

    player* p = z->tail->next;
    player*  start = p;

    do {
        if(z->tail==NULL) {break;}
        int energyLoss = 5 + rand()%16 ; // rand %(max-min+1)
        int tokenGain = rand()%4; 

        p->currentEnergy -= energyLoss;

        if(p->currentEnergy<=0){
            p->currentEnergy =0 ;
            p->eliminationStatus = true;
            player* temp = p->next;
            removePlayer(z, p->playerName);
            cout << "Player [" << p->playerName << 
            "] eliminated!" << endl;

            p = temp;
        }
        else {
            p->tokensCollected += tokenGain;
            cout << "Player [" << p->playerName <<
            "] , with Energy [" << p->currentEnergy << "]" <<
            " gained " << tokenGain << " , Total : " << p->tokensCollected << endl;
            p = p->next;
        }
    }
    while(p!=start);

    int active = 0;
    if(z->tail != NULL) { 
        player* q = z->tail->next; 
        player* s = q; 
        do {
            active++; 
            q = q->next;
        } 
        while(q != s);
    }
    if(active <2 && z->prev!=NULL) {
        cout << "[Recovery Mode] Moving back to '" << z->prev->zoneName << "'." << endl;
        movePlayers(z, z->prev);
        return;
    }

    if(totaltokens(z) >= z->difficultyLevel * 5) {
        cout << "[Zone Complete] '" << zoneName << "' cleared!" << endl;
        z->consecutiveFails = 0;
        movePlayers(z, z->next);
    } 
    else {
        z->consecutiveFails++;
        cout << "  Consecutive fails: " << z->consecutiveFails << endl;
        if (z->consecutiveFails >= 3) {
            swapWithPrev(z);
            z->consecutiveFails = 0;
        }
    }
}

void RouteManager::DisplayZone(string zoneName) {
    zone* z = findZone(zoneName);
    cout  << "Zone Name : [" << z->zoneName <<"]" << endl;
    cout << "Difficulty Level : [" << z->difficultyLevel <<"]" << endl;
    cout << "[" << (z->isLocked? "Locked": "Unlocked") << "]" << endl;
    cout << "Totl Attempts : [" << z->totalAttempts <<"]" << endl;
    displayPlayers(z);
}

void RouteManager::DisplayFullChallenge() {
    cout << "Challenge Route" << endl;
    cout << endl;
    zone * h = head;
    while(h!=NULL) {
        DisplayZone(h->zoneName);
        h = h->next;
    }
}

void RouteManager::movePlayers(zone* from, zone* to) {
    if(to==NULL) {
        cout << "No next zone found for " << from->zoneName << endl;
        return;
    }

    if (to->isLocked) {
        int survivors = 0;
        player* p = from->tail->next; 
        player* s = p;

        do { 
            survivors++; 
            p = p->next; 
        } while(p!=s);

        if(survivors>=3) {
            to->isLocked = false;
            cout << "[Unlocked] Zone '" << to->zoneName
                 << "' is now accessible!!" << endl;
        } 
        else {
            cout << "[Locked] Zone '" << to->zoneName
                 << "' needs 3 survivors. (Have: " << survivors << ")" << endl;
            return;
        }
    }
    
    player* p = from->tail->next;
    player* start = p;
    string names[200]; 
    int energies[200]; 
    int tokens[200]; 
    int count = 0;

    do {
        names[count]    = p->playerName;
        energies[count] = p->currentEnergy;
        tokens[count]   = p->tokensCollected;
        count++;
        p = p->next;
    } while (p != start);

    // removing
    player* temp = from->tail->next;
    from->tail->next = NULL;
    while(temp!=NULL){ 
        player* p = temp->next; 
        delete temp; 
        temp = p; 
    }
    from->tail = NULL;

    // inserting into to
    for(int i=0; i<count; i++) {
        player* nn = new player(names[i]);
        nn->currentEnergy  = energies[i];
        nn->tokensCollected = tokens[i];
        if(to->tail==NULL) {
            nn->next = nn;
            to->tail = nn;
        } 
        else {
            nn->next = to->tail->next;
            to->tail->next = nn;
            to->tail = nn;
        }
    }
    cout << "[Players moved from '" << from->zoneName
         << "' to '" << to->zoneName << "']" << endl;
}

void RouteManager::removeZone(string zoneName) {
    zone* z = findZone(zoneName);
    if (!z)     { cout << "Zone not found." << endl; return; }
    if (z->tail){ cout << "Cannot remove: players still inside." << endl; return; }
    if (z->difficultyLevel >= avgDifficulty()) {
        cout << "Cannot remove: difficulty (" << z->difficultyLevel
             << ") must be below average (" << avgDifficulty() << ")." << endl;
        return;
    }
    if (z->prev) z->prev->next = z->next; else head = z->next;
    if (z->next) z->next->prev = z->prev; else tail = z->prev;
    cout << "Zone \"" << zoneName << "\" removed." << endl;
    delete z;
}

int main(){
    srand(time(0));
    // zone
    string zoneName;
    int diffLevel;
    int totalAttempts;

    // player
    string playerName;
    int currentEnergy;
    int tokensCollected;

    RouteManager R;
    int choice;
    do {
        cout << "-----------------------------------------------------" << endl;
        cout << "|           MrBeast Challenge Manager               |" << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << endl;

        cout << "1. Discover New Zone" << endl;
        cout << "2. Add Player to Current Zone" << endl;
        cout << "3. Play Mini-Challenge Rounf" << endl;
        cout << "4. Move Players to Next Zone" << endl;
        cout << "5. Display Current Zone" << endl;
        cout << "6. Display Full Challenge Route" << endl;
        cout << "7. Remove Zone" << endl;
        cout << "8. Exit" << endl;

        cout << endl;
        cout << "Enter Choice: " ;
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 1 :
                cout << "Enter Zone Name: " << endl;
                getline(cin, zoneName);
                cout << "Enter difficulty Level: " << endl;
                cin >> diffLevel;
                if(diffLevel>=1 && diffLevel<=10) {
                    R.insertZone(zoneName, diffLevel);
                }
                else 
                    cout << "Difficulty Level must be between 1 and 10" << endl;
                break;
            case 2:
                cout << "Enter Player Name: " << endl;
                getline(cin, playerName);
                cout << "Enter the zone: " << endl;
                getline(cin, zoneName);
    
                R.addPlayer(playerName, zoneName);
                break;
            case 3:
                cout << "Enter Zone Name: ";
                getline(cin, zoneName);
                R.MiniChallengeRound(zoneName);
                break;
            case 4:{
                cout << "Enter Zone Name: ";
                getline(cin, zoneName);
                zone* from = R.findZone(zoneName);
                if(from!=NULL && from->next!=NULL)
                    R.movePlayers(from, from->next);
                else if(from!=NULL)
                    cout << "No next zone." << endl;
                break;}
            case 5:
                cout << "Enter Zone Name: ";
                getline(cin, zoneName);
                R.DisplayZone(zoneName);
                break;
            case 6:
                R.DisplayFullChallenge();
                break;
            case 7:
                cout << "Enter Zone Name to remove: ";
                getline(cin, zoneName);
                R.removeZone(zoneName);
                break;
            case 8:
                cout << "Exiting." << endl;
                break;
            default:
                cout << "Invalid choice." << endl;
        }
    }
    while(choice!=8);
    return 0;
}
