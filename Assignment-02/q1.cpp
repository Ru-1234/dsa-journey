#include "Q1.h"

// register a new case - insert at tail..
void ActiveResponseQueue::insert(info val) {
    Incident* nn = new Incident(val);
    if(empty()){
        top = tail = nn;
        return;
    }

    tail->next = nn;
    tail = nn;
}

// dequeue - from front (top)
void ActiveResponseQueue::dispatch(){
    if(empty()) {
        cout << "Queue is empty! " << endl;
        return;
    }

    Incident* temp = top;
    top = top->next;
    temp->next = NULL;
    delete temp;
    temp = NULL;

    // if queue fot empty
    if(top == NULL) 
        tail = NULL;
}

void ActiveResponseQueue::reverseKGroups(int k){
    // if k<=1 or empty then:
    if(k<=1 || empty()) return;
    
    ActiveResponseQueue tempQ;
    
    while(!empty()){
        stack s;
        int count = 0;
        
        // peek ahead to see if full K group exists
        Incident* ptr = top;
        int check = 0;
        while(ptr!=NULL && check<k){
            ptr = ptr->next;
            check++;
        }
        
        // in cas of incomplete last group
        if(check<k){
            while(!empty()){
                tempQ.insert(top->data);
                dispatch();
            }
            break;
        }
        
        //push k elements to stack - order reverse
        while(!empty() && count<k){
            s.push(top->data);
            dispatch();
            count++;
        }
        
        // pop from stack into temQ
        while(!s.empty()){
            tempQ.insert(s.pop());
        }
    }
    
    // copy from tempQ into the queue
    while(!tempQ.empty()){
        insert(tempQ.top->data);
        tempQ.dispatch();
    }
}

bool ActiveResponseQueue::symmetry() {
    if(empty()) return true;
    
    stack s;

    // push all severity levels onto stack
    Incident* temp = top;
    while(temp != NULL){
        s.push(temp->data);
        temp = temp->next;
    }

    // compare
    temp = top;
    bool flag = true;
    while(temp != NULL){
        if(!(s.pop() == temp->data)){
            flag = false;
            break;
        }
        temp = temp->next;
    }

    return flag;
}

void ActiveResponseQueue::display(){
    if(empty()){
        cout << "Queue is empty!" << endl;
        return;
    }
    Incident* temp = top;
    while(temp != NULL){
        cout << "CaseID: " << temp->data.caseID
             << " Name: " << temp->data.callerName
             << " Severity: " << temp->data.severityLevel
             << " Resolved: " << temp->data.isResolved << endl;
        temp = temp->next;
    }
}

void ActiveResponseQueue::riskbasedProtocol(int X, ActiveResponseQueue& JuniorQueue, ActiveResponseQueue& SpecialQueue){
    while(!empty()){
        info data = top->data;
        
        // <X -> juniorUnitsQueue
        // else -> SpecialForcesQueue
        if(data.severityLevel < X)
            JuniorQueue.insert(data);
        else
            SpecialQueue.insert(data);
        dispatch(); //dequeue
    }
}

int main() {
    ActiveResponseQueue mainQ;
    ActiveResponseQueue juniorQ;
    ActiveResponseQueue specialQ;

    int choice;
    cout << endl;
    do{
        cout << "1. Register New Emergency Case" << endl;
        cout << "2. Dispatch Case" << endl;
        cout << "3. Reverse K Groups" << endl;
        cout << "4. Symmetry Audit" << endl;
        cout << "5. Risk Based Segregation" << endl;
        cout << "6. Display Active Queue" << endl;
        cout << "0. Exit" << endl;
        cout << "\tEnter choice: ";
        cin >> choice;

        if(choice == 1){
            info data;
            cout << "Enter Case ID: ";
            cin >> data.caseID;
            cout << "Enter Caller Name: ";
            cin >> data.callerName;
            cout << "Enter Severity Level: ";
            cin >> data.severityLevel;
            data.isResolved = false;
            mainQ.insert(data);
        }
        else if(choice == 2){
            mainQ.dispatch();
        }
        else if(choice == 3){
            int k;
            cout << "Enter K: ";
            cin >> k;
            mainQ.reverseKGroups(k);
            cout << "Queue after reversing K groups:" << endl;
            mainQ.display();
        }
        else if(choice == 4){
            if(mainQ.symmetry())
                cout << "Severity pattern is symmetric.." << endl;
            else
                cout << "Severity pattern is not symmetric.." << endl;
        }
        else if(choice == 5){
            int x;
            cout << "Enter threshold severity X: ";
            cin >> x;
            mainQ.riskbasedProtocol(x, juniorQ, specialQ);
            cout << "Junior queue: " << endl;
            juniorQ.display();
            cout << "Special queue: " << endl;
            specialQ.display();
        }
        else if(choice == 6){
            mainQ.display();
        }

    }
    while(choice!=0);
    return 0;
}
