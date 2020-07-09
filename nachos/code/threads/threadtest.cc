#include "kernel.h"
#include "main.h"
#include "thread.h"
#include <iostream>
#include "Train_System.h"
#include "bitmap.h"
#include <map>
Handle_Request hr;
List<Train_Schedule_Info*>* train_schedules;
List<Thread*>* threads;
std::map<Train_Schedule_Info*, List<Request*>*> trains_requests;
std::map<Request*, Thread*> request_reservation;
int request_index = 0;
Train_Schedule_Info* cur_train = new Train_Schedule_Info();
Thread* admin_thread = new Thread("temp one");
int cur_time = 0;
IntStatus oldLevel;

void train(int i) {
    oldLevel = kernel->interrupt->SetLevel(IntOff);
    kernel->currentThread->Sleep(false);
    (void) kernel->interrupt->SetLevel(oldLevel);
    while(1) {
        if(trains_requests.find(cur_train) == trains_requests.end()) {
            trains_requests[cur_train] = new List<Request*>();
        }
        List<Request*>* temp_train_request = trains_requests[cur_train];
        ListIterator<Request*> temp_train_request_iter(temp_train_request);
        for(; !temp_train_request_iter.IsDone(); temp_train_request_iter.Next()) {
            if(temp_train_request_iter.Item()->get_actual_depart_t() == cur_time) {
                oldLevel = kernel->interrupt->SetLevel(IntOff);
                kernel->scheduler->ReadyToRun(request_reservation[temp_train_request_iter.Item()]);
                (void) kernel->interrupt->SetLevel(oldLevel);
                kernel->currentThread->Yield();
            }
        }
        oldLevel = kernel->interrupt->SetLevel(IntOff);
        kernel->scheduler->ReadyToRun(admin_thread);
        (void) kernel->interrupt->SetLevel(oldLevel);
        oldLevel = kernel->interrupt->SetLevel(IntOff);
        kernel->currentThread->Sleep(false);
        (void) kernel->interrupt->SetLevel(oldLevel);
        if(trains_requests.find(cur_train) == trains_requests.end()) {
            trains_requests[cur_train] = new List<Request*>();
        }
        temp_train_request = trains_requests[cur_train];
        ListIterator<Request*> temp_train_request_iter1(temp_train_request);
        for(; !temp_train_request_iter1.IsDone(); temp_train_request_iter1.Next()) {
            if(temp_train_request_iter1.Item()->get_actual_destin_t()  == cur_time) {
                oldLevel = kernel->interrupt->SetLevel(IntOff);
                kernel->scheduler->ReadyToRun(request_reservation[temp_train_request_iter1.Item()]);
                (void) kernel->interrupt->SetLevel(oldLevel);
                kernel->currentThread->Yield();
            }
        }
        oldLevel = kernel->interrupt->SetLevel(IntOff);
        kernel->scheduler->ReadyToRun(admin_thread);
        (void) kernel->interrupt->SetLevel(oldLevel);
        oldLevel = kernel->interrupt->SetLevel(IntOff);
        kernel->currentThread->Sleep(false);
        (void) kernel->interrupt->SetLevel(oldLevel);
    }
    
}
void reservation(int i) {
    Request *rq = new Request(request_index++, i);
    Train_Schedule_Info * temp_train = hr.Handle_One_Request(rq, i);
    request_reservation[rq] = kernel->currentThread;
    if(trains_requests.find(temp_train) == trains_requests.end()) {
        trains_requests[temp_train] = new List<Request*>();
    }
    trains_requests[temp_train]->Append(rq);
    oldLevel = kernel->interrupt->SetLevel(IntOff);
    kernel->currentThread->Sleep(false);
    (void) kernel->interrupt->SetLevel(oldLevel);
    temp_train->passenger_get_in();
    oldLevel = kernel->interrupt->SetLevel(IntOff);
    kernel->currentThread->Sleep(false);
    (void) kernel->interrupt->SetLevel(oldLevel);
    temp_train->passenger_get_out(i, rq->get_Seat_Type(), rq->get_Passenger_Number());
    kernel->currentThread->Finish();
}
void admin(int m) {
    admin_thread = kernel->currentThread;
    train_schedules = hr.get_Train_Schedule_Info();
    threads = new List<Thread*>();
    ListIterator<Train_Schedule_Info*> ts_iter(train_schedules);
    for(; !ts_iter.IsDone(); ts_iter.Next()) {
        Thread *t = new Thread("train thread");
        t->Fork((VoidFunctionPtr) train, (void *) 0);
        kernel->currentThread->Yield();
        threads->Append(t);
    }
    for(; cur_time < 96; cur_time++) {
        for(int j = 0; j < 5; j++) {
            Thread *t = new Thread("reservation thread");
            t->Fork((VoidFunctionPtr) reservation, (void *) 0);
            kernel->currentThread->Yield();
        }
        ListIterator<Train_Schedule_Info*> train_schedules_iter(train_schedules);
        ListIterator<Thread*> threads_iter(threads);
        for(; !train_schedules_iter.IsDone(); train_schedules_iter.Next(), threads_iter.Next()) {
            List<int*>* temp_times = train_schedules_iter.Item()->get_times();
            ListIterator<int*> times_iter(temp_times);
            for(; times_iter.IsDone(); times_iter.Next()) {
                if(*times_iter.Item() == cur_time) {
                    oldLevel = kernel->interrupt->SetLevel(IntOff);
                    kernel->scheduler->ReadyToRun(threads_iter.Item());
                    (void) kernel->interrupt->SetLevel(oldLevel);
                    cur_train = train_schedules_iter.Item();
                    oldLevel = kernel->interrupt->SetLevel(IntOff);
                    kernel->currentThread->Sleep(false);
                    (void) kernel->interrupt->SetLevel(oldLevel);
                }
            }
        }
        hr.print_per(cur_time);
    }
    hr.print();
}
void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
        printf("*** thread %d looped %d times\n", which, num);
        kernel->currentThread->Yield();
    }
}

void
ThreadTest()
{
    train_schedules = new List<Train_Schedule_Info*>();
    threads = new List<Thread*>();
    Thread *t = new Thread("forked thread");
    t->Fork((VoidFunctionPtr) admin, (void *) 1);
}
