#ifndef Train_System_h
#define Train_System_h
#define business_seats_number 20
#define coach_seats_number 40
#define station_number 20
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "list.h"
#include "bitmap.h"

class Station {
public:
    Station(int t_id, std::string name);
    int get_id();
    std::string get_name();
private:
    int Station_id;
    std::string Station_name;
};

class Request {
public:
    Request(int num, int cur_time);
    int get_Request_ID();
    int get_Departure_Station_ID();
    int get_Destination_Station_ID();
    int get_Passenger_Number();
    int get_Seat_Type();
    void set_actual_depart_t(int t);
    void set_actual_destin_t(int t);
    int get_actual_depart_t();
    int get_actual_destin_t();
private:
    int request_id;
    int departure_station_id;
    int destination_station_id;
    int departure_time;
    int seat_type;
    int passenger_number;
    int actual_departure_time;
    int actual_destination_time;
};

class Ticket {
public:
    Ticket(int f, int dep_si, int des_si, int dep_t, int des_t, int number, int st, bool ava);
    int get_depart_time();
    int get_destination_time();
    int get_P_Number();
private:
    int fares;
    int departure_station_id;
    int destination_station_id;
    int departure_time;
    int destination_time;
    int passengers_number;
    int seat_type;
    bool available;
};

class Train_Schedule_Info {
public:
    Train_Schedule_Info();
    Train_Schedule_Info(int t_id, List<Station*> *input_routes, List<int*>* input_times, List<int*> *input_fares);
    bool find_Available(Request rq, int cur_time);
    Ticket* handle_changes(Request rq);
    int get_Price(int start_index, int end_index);
    int get_Departure_Time(int start_index);
    int get_Destination_Time(int end_index);
    void occupy_business_seats(int start_index, int end_index, int passengers_num);
    void occupy_coach_seats(int start_index, int end_index, int passengers_num);
    void record_Decrease_Seat_Number(int start_index, int end_index, int et, int st, int pn);
    void check_Service_Complete(int cur_time);
    void passenger_get_in();
    List<int*>* get_times();
    void passenger_get_out(int cur_time, int passengers_seats, int numbers);
    int get_train_id();
private:
    int train_id;
    List<Station*> *routes;
    List<int*> *fares;
    List<int*> *times;
    List<Bitmap*> *business_number;
    List<Bitmap*> *coach_number;
    List<int*> *end_service_times;
    List<int*> *service_type;
    List<int*> *service_passengers_number;
};

class Handle_Request {
public:
    Handle_Request();
    void get_file_schedule(int* t_id, List<Station*> *file_routes, List<int*>* file_times, List<int*>* file_fares, std::string &file_content);
    List<int*> * getContent(std::string &str);
    Station* get_order_station(int index);
    Train_Schedule_Info* Handle_One_Request(Request* rqs, int cur_time);
    void check_Service_Complete(int cur_time);
    List<Train_Schedule_Info*>* get_Train_Schedule_Info();
    void print_per(int cur_time);
    void print();
private:
    List<Station*>* stations;
    List<List<Ticket*>*>* trains_tickets;
    List<Train_Schedule_Info*>* schedules_info;
    List<Request*>* granted_requests;
    List<Request*>* refused_requests;
    List<int*>* section_statistics;
    int request_per_time;
    int refuesed_per_time;
    int total_request;
    int total_granted_request;
    int total_served_itinerary;
    int total_passengers;
};
#endif /* Train_System_h */

