#ifndef Q1_H
#define Q1_H

#include<iostream>
#include<string>
using namespace std;

struct info {
    int caseID;
    string callerName;
    int severityLevel;  //1(Low) to 10 (Critical)
    bool isResolved;

    bool operator== (info i2){
        //return this->caseID==i2.caseID && this->callerName==i2.callerName && 
        //        this->severityLevel==i2.severityLevel && this->isResolved==i2.isResolved;
        return this->severityLevel==i2.severityLevel;
    }
};

struct Incident {
    info data;
    Incident* next;
    Incident(info d) {data = d; next = NULL;}
};

class ActiveResponseQueue {
    private:
        Incident* top;
        Incident* tail;
        //fifo -> inserting at tail , deleting from top (head)

    public:
        ActiveResponseQueue() {
            top = tail = NULL;
        }
        //enqueue - register a ew emergency case.
        void insert(info val);
        //dequeue - dispatch & close resolved case.
        void dispatch();
        // crisis escalation protocol - reverse groups of K incidents
        void reverseKGroups(int k);
        // Symmetry Audit - checks if severity levels are palindrome or not
        bool symmetry();
        //main queue -> juniorQueue(<x) & SpecialQueue.
        void riskbasedProtocol(int x, ActiveResponseQueue &JuniorQueue, ActiveResponseQueue& SpecialQueue);

        bool empty(){return top == NULL;}
        void display();
        Incident* getTop(){ return top; }
};

class stack {
    Incident* top;
    public:
    //lifo -> head(pop), top
    stack(){top=NULL;}

    void push(info val) {
        Incident* nn = new Incident(val);
        if(empty()){
            top = nn;
            return;
        }

        nn->next = top;
        top = nn;
    }

    info pop(){
        if(empty()){
            cout << "Stack is empty!" << endl;
            return {-1, "", -1, false};
        }

        Incident* temp = top;
        info ret = temp->data;
        top = top->next;
        temp->next = NULL;
        delete temp;
        temp = NULL;
        return ret;
    }

    bool empty(){return top==NULL;}
    
};

#endif
