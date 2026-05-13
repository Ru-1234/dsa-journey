#include "Q2.h"
int globalBalance = 0;

//Structural Validation
bool validateDependencyTag(string str){
    stack s;
    transaction d;

    for(int i=0; i<str.length(); i++){
        // opening bracket-> push
        if(str[i]=='(' || str[i]=='{' || str[i]=='['){
            d.dependencyTag = str[i];
            s.push(d);
        }
        else if(str[i]==')' || str[i]=='}' || str[i]==']'){
            if(s.empty()) 
                return false;
            transaction top = s.pop();
            char open = top.dependencyTag[0];

            if ((str[i]==')' && open!='(') || (str[i]=='}' && open!='{') || (str[i]==']' && open!='[')){
                return false;
            }
        }
    }

    return s.empty();
}

// if riskScore>R -> mov to circular queue (suspicion)
// riskScore< R -> preserving original relative order.
void suspicionCheck(QueueUsingStack &admission, CircularQueue &suspicion, float R){

    QueueUsingStack temp;

    while(!admission.empty()){
        transaction t = admission.dequeue(); // fifo head

        if(t.riskScore>R){
            suspicion.enqueue(t);
            cout << "Held for suspicion: " << t.transactionID << " (risk=" << t.riskScore << ")" << endl;
        } 
        else {
            temp.enqueue(t);
        }
    }

    // restore admission
    while(!temp.empty())
        admission.enqueue(temp.dequeue());
}

int precedence(char op){
    if(op=='+' || op=='-') 
        return 1;
    if(op=='*' || op=='/') 
        return 2;
    return 0;
}

// Deterministic Fee Evaluation
string infixToPostfix(string exp){
    stack s;
    transaction temp;
    string postfix = "";

    // if digit: add to postfix
    // else if sign +, -, /, * -> push to stack
    for(int i=0; i<exp.length(); i++){
        if(isdigit(exp[i])){
            postfix += exp[i];
        }
        else if(exp[i]=='('){
            temp.feeExpression = exp[i];
            s.push(temp);
        }
        else if(exp[i]==')'){
            while(!s.empty() && s.top->data.feeExpression[0]!='('){
                postfix += s.pop().feeExpression;
            }
            s.pop(); // pop at the '('
        }
        else{
            while(!s.empty() && precedence(s.top->data.feeExpression[0])>=precedence(exp[i])){
                postfix += s.pop().feeExpression;
            }
            temp.feeExpression = exp[i];
            s.push(temp);
        }
    }

    while(!s.empty()){
        postfix += s.pop().feeExpression;
    }

    return postfix;
}

int evaluatePostfix(string exp){
    stack s;
    transaction temp; 

    for(int i=0; i<exp.length(); i++){

        if(isdigit(exp[i])){
            temp.amount=exp[i]-'0'; // ascii to digit
            s.push(temp);
        }
        else{
            int b=s.pop().amount;
            int a=s.pop().amount;
            int result = 0;

            if(exp[i]=='+') 
                result = a + b;
            if(exp[i]=='-') 
                result = a - b;
            if(exp[i]=='*') 
                result = a * b;

            if(exp[i]=='/'){
                if(b == 0){
                    cout << "Divide by zero" << endl;
                    return 0;
                }
                result = a/b;
            }

            temp.amount = result;
            s.push(temp);
        }
    }

    return s.pop().amount;
}

// Execution and State Mutation
void executeTransaction(transaction t, StackUsingQueue &executionStack){
    if(!validateDependencyTag(t.dependencyTag)){
        cout << "Invalid dependencyTag - transaction rejected.." << endl;
        return;
    }

    string post = infixToPostfix(t.feeExpression);
    int fee =evaluatePostfix(post);

    globalBalance = globalBalance + t.amount - fee;
    cout << "Executed: " << t.transactionID << endl;
    cout << "Amount: " << t.amount << endl;
    cout <<"Fee: " << fee << endl;
    cout<< "Balance: " << globalBalance << endl;

    executionStack.push(t);
}

void rollback(StackUsingQueue &executionStack, QueueUsingStack &admission){
    if(globalBalance>=0){
        cout << "Balance is non-negative, no rollback needed" << endl;
        return;
    }

    cout << "Rolling back.... "<< endl;

    while(globalBalance<0){

        if(executionStack.empty()) break;

        transaction t = executionStack.pop();

        //reverse
        string post = infixToPostfix(t.feeExpression);
        int fee    = evaluatePostfix(post);
        globalBalance = globalBalance - t.amount + fee;

        cout << "Rolled back: " << t.transactionID << endl;
        cout << "Balance now: "<< globalBalance <<endl;

        admission.enqueue(t);
    }
}

void priorityEscalation(QueueUsingStack &mainQ, int X){
    QueueUsingStack high;
    QueueUsingStack low;

    while(!mainQ.empty()){
        transaction t = mainQ.dequeue();

        if(t.amount>=X)
            high.enqueue(t);
        else
            low.enqueue(t);
    }

    // high  -> mainQ
    while(!high.empty())
        mainQ.enqueue(high.dequeue());
    // low    
    while(!low.empty())
        mainQ.enqueue(low.dequeue());

    cout << "Escalation done - high-amount transactions moved to front" << endl;
}

// max amount, min amoun and max diff
void slidingFraudAnalytics(StackUsingQueue &executionStack, int K){
    if(executionStack.empty()){
        cout << "Execution stack is empty" << endl;
        return;
    }

    int total =0;
    QueueUsingStack tempQ;
    while(!executionStack.empty()){
        tempQ.enqueue(executionStack.pop());
        total++;
    }

    cout << endl;
    cout << "Sliding Fraud Analytics (window=" << K << ") "<< endl;

    Deque maxDq;  // decreasing -> front: max of window
    Deque minDq;  // increasing -> front: min of window

    // first window
    for(int i = 0; i < K; i++){
        transaction cur = tempQ.dequeue();
        int curAmt = cur.amount;

        while(!maxDq.empty() && maxDq.backVal() <= curAmt)
            maxDq.popBack();
        maxDq.pushBack(curAmt, i);

        while(!minDq.empty() && minDq.backVal() >= curAmt)
            minDq.popBack();
        minDq.pushBack(curAmt, i);
    }

    cout<<"Window [0-" << K-1<< "]" << " Max=" << maxDq.frontVal()
         <<" Min=" <<minDq.frontVal() <<" Diff=" << (maxDq.frontVal() - minDq.frontVal()) 
         <<endl;

    for(int i=K; i<total; i++){
        transaction cur = tempQ.dequeue();
        int curAmt = cur.amount;

        //not part of curr window
        while(!maxDq.empty() && maxDq.frontIdx() <= i - K)
            maxDq.popFront();
        while(!minDq.empty() && minDq.frontIdx() <= i - K)
            minDq.popFront();

        //remov smaller vals - for max deque)
        while(!maxDq.empty() && maxDq.backVal() <= curAmt)
            maxDq.popBack();
        maxDq.pushBack(curAmt, i);

        //remov larger vals - for min deque)
        while(!minDq.empty() && minDq.backVal() >= curAmt)
            minDq.popBack();
        minDq.pushBack(curAmt, i);

        cout<< "Window [" << (i-K+1)<< "-" << i<< "]"
            << " Max=" << maxDq.frontVal()
            <<" Min=" << minDq.frontVal()
            <<" Diff=" << (maxDq.frontVal() - minDq.frontVal()) << endl;
    }
}

// if pending transactions>C, reverse oldest half
// complexity: O(N)
void loadShedding(QueueUsingStack &admission, int C){
    int count = 0;
    QueueUsingStack temp;
    
    // count no. of nodes, 

    while(!admission.empty()){
        temp.enqueue(admission.dequeue());
        count++;
    }
    while(!temp.empty()) 
        admission.enqueue(temp.dequeue());

    if(count <= C){
        cout << "Load within capacity (" << count << "/" << C << "), no shedding needed" << endl;
        return;
    }

    int half = count/2;
    cout << "Load exceeded! Reversing oldest " << half << " transactions..." << endl;

    stack reverser;
    QueueUsingStack rest;

    for(int i=0; i<count; i++){
        transaction t = admission.dequeue();
        if(i<half)
            reverser.push(t); 
        else
            rest.enqueue(t);
    }

    while(!reverser.empty())
        admission.enqueue(reverser.pop());

    while(!rest.empty())
        admission.enqueue(rest.dequeue());

    cout << "Load shedding complete" << endl;
}

int main(){
    QueueUsingStack admission;
    StackUsingQueue execution;
    CircularQueue suspicion(100);

    int choice;

    do{
        cout << endl;
        cout << "1- Add transaction - Admission Layer" << endl;
        cout << "2- Execute next" << endl;
        cout << "3- Rollback - Conditional Reverse Compensation"<< endl;
        cout << "4- Escalate Priority - Priority escalation" << endl;
        cout << "5- Check balance - State" << endl;
        cout << "6- Suspicion check -Deferred Suspicion Hold"<< endl;
        cout << "7- Sliding Fraud Analytics" << endl;
        cout << "8- Load shedding" << endl;
        cout << "0- Exit" << endl;
        cout << "Enter: ";
        cin >> choice;

        if(choice == 1){
            transaction t;
            cout << "ID: ";             
            cin >> t.transactionID;
            cout << "Time: ";           
            cin >> t.timestamp;
            cout << "Amount: ";         
            cin >> t.amount;
            cout << "Risk: ";           
            cin >> t.riskScore;
            cout << "Fee Infix: ";      
            cin >> t.feeExpression;
            cout << "DependencyTag: ";  
            cin >> t.dependencyTag;

            admission.enqueue(t);
            cout << "Transaction added" << endl;
        }
        else if(choice==2){
            if(!admission.empty()){
                transaction t = admission.dequeue();
                executeTransaction(t, execution);
            } 
            else {
                cout << "Admission queue is empty" << endl;
            }
        }
        else if(choice == 3){
            rollback(execution, admission);
        }
        else if(choice == 4){
            int x;
            cout << "Enter X: ";
            cin >> x;
            priorityEscalation(admission, x);
        }
        else if(choice == 5){
            cout << "Balance: " << globalBalance << endl;
        }
        else if(choice == 6){
            float r;
            cout << "Enter risk threshold R: ";
            cin >> r;
            suspicionCheck(admission, suspicion, r);
        }
        else if(choice == 7){
            int k;
            cout << "Enter window size K: ";
            cin >> k;
            slidingFraudAnalytics(execution, k);
        }
        else if(choice == 8){
            int c;
            cout << "Enter capacity C: ";
            cin >> c;
            loadShedding(admission, c);
        }

    } while(choice != 0);


    return 0;
}
