#include "array.h"

int Array::sumA = 0;
unsigned int Array::t = 0;
Array::Array(int n) {
    unsigned int i;

    p = new int[n];
    N = n;
    for(i = 0; i < N; i++)
        p[i] = 0;
}

bool Array::Insert(Request r) {
    unsigned int i, end;

    end = t + r.start + r.duration;
    //进行接纳控制
    i = t + r.start;
    if(r.duration > N)
        return false;
    while(i < end && i < N && p[i] + r.value <= MAX){
        i++;
    }
    if(p[i] + r.value > MAX){
        return false;
    }else if(i >= N){
        while(i < end && p[i - N] + r.value <= MAX){
            i++;
        }
        if(p[i - N] + r.value > MAX){
            return false;
        }
    }

    sumA++;
    i = t + r.start;
    while(i < end && i < N){
        p[i] += r.value;
        i++;
    }
    if(i >= N){
        while(i < end){
            p[i - N] += r.value;
            i++;
        }
    }
    return true;
}

void Array::Clear() {
    p[t] = 0;
    t = (t + 1) % N;
}

void Array::Display() {
    unsigned int i;

    for(i = t; i < N; i++)
        cout << p[i] << ' ';
    for(i = 0; i < t; i++)
        cout << p[i] << ' ';
    cout << endl;
}
