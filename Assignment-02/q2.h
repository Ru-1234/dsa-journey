#ifndef Q2_H
#define Q2_H

#include<iostream>
#include<string>
using namespace std;

//data
struct transaction {
    //Each transaction -> attributes:
    string transactionID;
    int timestamp;
    int amount;
    float riskScore;
    string feeExpression;  //infix mathematical-string
    string dependencyTag;  // nested bracket structure
};

struct node {
    transaction data;
    node* next;

    node(transaction d) {data = d; next = NULL;}
};

class Queue {
    private:
        node* front;
        node* rear;

    public:
        Queue(){front = rear = NULL;}
        
        void enqueue(transaction val) {
            node* nn = new node(val);

            //insert at rear
            if(empty()){
                front = rear = nn;
                return;  
            }

            rear->next = nn;
            rear = nn;
        }

        transaction dequeue(){
            if(empty()) {
                cout << "Queue is empty" << endl;
                return {"", -1, -1, -1.0, "", ""};
            }

            // deete from head (front) , fifo
            node* temp = front;
            front = front->next;
            transaction ret = temp->data;
            temp->next = NULL;
            delete temp;
            temp = NULL;

            if(front == NULL) // if queue is empty
                rear = NULL;

            return ret;
        }

        bool empty(){return front == NULL;}
};

class stack {
    public:
        node* top;
    
        //lifo -> head(pop)
        stack(){top =NULL;}

        void push(transaction val){
            node* nn = new node(val);
            if(empty()){
                top = nn; return;
            }

            nn->next = top;
            top = nn;
        }

        transaction pop(){
            if(empty()){
                cout << "Stack is empty!" << endl;
                return {"", -1, -1, -1.0, "", ""};
            }

            node* temp = top;
            transaction ret = temp->data;
            top = top->next;
            temp->next = NULL;
            delete temp;
            temp = NULL;
            return ret;
        }

        bool empty(){return top == NULL;}
};

//Queue using 2 stacks - admission layer ,FIFO via stacks
class QueueUsingStack {
    public:
        stack s1;
        stack s2;

        QueueUsingStack(){}

        void enqueue(transaction val) {
            // content of s1-> s2
            while(!s1.empty()) {
                s2.push(s1.pop());
            }
            // push new val in s1
            s1.push(val);
            // content of s2 - > s1
            while(!s2.empty()){
                s1.push(s2.pop());
            }
        }

        transaction dequeue(){
            if(s1.empty()) {
                cout << "Queue is empty!" << endl;
                return {"", -1, -1, -1.0, "", ""};
            }
            // pop from head
            return s1.pop();
        }

        bool empty(){ return s1.empty(); }
};

class StackUsingQueue {
    public:
        Queue q1, q2;

        void push(transaction val) {
            // content of q1 ->q2
            while(!q1.empty()) {
                q2.enqueue(q1.dequeue());
            }
            // insert on q1
            q1.enqueue(val);
            // content of q2-> q1
            while(!q2.empty()){
                q1.enqueue(q2.dequeue());
            }
        }

        transaction pop() {
            if(q1.empty()){
                cout << "Stack empty! " << endl;
                return {"", -1, -1, -1.0, "", ""};
            }
            return q1.dequeue();
        }

        bool empty(){ return q1.empty(); }
};

// circular queue
class CircularQueue {
    public:
        int front;
        int rear;
        int maxCap;
        int currSize;
        transaction* arr;
        
        CircularQueue(int cap = 50){
            maxCap  = cap;
            currSize = 0;
            front = rear = -1;  // emoty stack
            arr = new transaction[maxCap];
        }

        ~CircularQueue(){
            delete[] arr;
        }

        void enqueue(transaction q) {
            if(full()){
                cout << "Suspicion Queue full!" << endl;
                return;
            }
            if(empty()){
                front= rear =0; // head
            } 
            else {
                rear = (rear+1)%maxCap;
            }
            arr[rear] = q;
            currSize++;
        }

        transaction dequeue(){
            if(empty()){
                cout << "Suspicion Queue empty!" << endl;
                return {"", -1, -1, -1.0, "", ""};
            }

            transaction r=arr[front];
            if(front==rear){    //last element
                front = rear = -1;
            } 
            else {
                front = (front+1)%maxCap;
            }
            currSize--;
            return r;
        }

        bool full() { return currSize == maxCap; }
        bool empty(){ return currSize == 0; }
};

//manual deque using double linked list , push/pop from both ends
struct dequeNode {
    int val;
    int  idx;
    dequeNode* prev;
    dequeNode* next;
    dequeNode(int v, int i){ 
        val=v; 
        idx=i; 
        prev=next=NULL; 
    }
};

class Deque {
    dequeNode* head;
    dequeNode* tail;
    public:
    Deque(){head = tail = NULL;}

    void pushBack(int val, int idx){
        dequeNode* nn = new dequeNode(val, idx);
        if(!head){ 
            head = tail = nn; return; 
    }
        tail->next = nn;
        nn->prev= tail;
        tail = nn;
    }

    void pushFront(int val, int idx){
        dequeNode* nn = new dequeNode(val, idx);
        if(!head){ 
            head = tail = nn; 
            return; 
        }
        nn->next= head;
        head->prev = nn;
        head = nn;
    }

    void popBack(){
        if(!tail) 
            return;

        dequeNode* temp = tail;
        tail = tail->prev;
        if(tail) 
            tail->next = NULL;
        else     
            head = NULL;
        delete temp;
    }

    void popFront(){
        if(!head) 
            return;

        dequeNode* temp = head;
        head = head->next;
        if(head) 
            head->prev = NULL;
        else     
            tail = NULL;
        delete temp;
    }

    int frontVal(){return head? head->val : -1;}
    int backVal() {return tail? tail->val : -1;}
    int  frontIdx(){return head? head->idx : -1;}
    int  backIdx() {return tail? tail->idx : -1;}
    bool empty(){return head== NULL;}
};

// Structural Validation
bool validateDependencyTag(string str);

#endif
