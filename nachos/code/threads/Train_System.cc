#include "Train_System.h"

//Station constructor
Station::Station(int t_id, std::string name) {
    Station_id = t_id;
    Station_name = name;
}

// get station id
int Station::get_id() {
    return Station_id;
}

// get the station name
std::string Station::get_name() {
    return Station_name;
}

// Request constructor
Request::Request(int num, int cur_time) {
    request_id = num;
    bool equal = true;
    int s_index = cur_time * 97;
    srand(s_index);
    while(equal == true) {
        srand(s_index);
        departure_station_id = rand()%station_number;
        destination_station_id = rand()%station_number;
        if(departure_station_id < destination_station_id) {
            equal = false;
        }
        s_index++;
    }
    departure_time = rand()%(97 - cur_time) + cur_time;
    seat_type = rand()%2;
    passenger_number = rand()%9 + 1;
    actual_departure_time = 0;
    actual_destination_time = 0;
}

// get the request ID
int Request::get_Request_ID() {
    return request_id;
}

// get the departure station ID
int Request::get_Departure_Station_ID() {
    return departure_station_id;
}

// get the destination station ID
int Request::get_Destination_Station_ID() {
    return destination_station_id;
}

// get passengers number
int Request::get_Passenger_Number() {
    return passenger_number;
}

// get seat type
int Request::get_Seat_Type() {
    return seat_type;
}

// set the actual depart time
void Request::set_actual_depart_t(int t) {
    actual_departure_time = t;
}

// set the actual destination time
void Request::set_actual_destin_t(int t) {
    actual_destination_time = t;
}

// get the actual depart time
int Request::get_actual_depart_t() {
    return actual_departure_time;
}

// get the actual destination time
int Request::get_actual_destin_t() {
    return actual_destination_time;
}

// Ticket constructor
Ticket::Ticket(int f, int dep_si, int des_si, int dep_t, int des_t, int number, int st, bool ava) {
    fares = f;
    departure_station_id = dep_si;
    destination_station_id = des_si;
    departure_time = dep_t;
    destination_time = des_t;
    passengers_number = number;
    seat_type = st;
    available = ava;
}

// get depart time
int Ticket::get_depart_time() {
    return departure_time;
}

// get destination time
int Ticket::get_destination_time() {
    return destination_time;
}

// get passenger number
int Ticket::get_P_Number() {
    return passengers_number;
}

// Train_Schedule_Info constructor
Train_Schedule_Info::Train_Schedule_Info() {
}

// Train_Schedule_info constructor
Train_Schedule_Info::Train_Schedule_Info(int t_id, List<Station*> *input_routes, List<int*>* input_times, List<int*> *input_fares) {
    train_id = t_id;
    routes = new List<Station*>();
    fares = new List<int*>();
    times = new List<int*>();
    end_service_times = new List<int*>();
    service_type = new List<int*>();
    service_passengers_number = new List<int*>();
    business_number = new List<Bitmap*>();
    coach_number = new List<Bitmap*>();
    ListIterator<Station*> routes_iter(input_routes);
    for (; !routes_iter.IsDone(); routes_iter.Next()) {
        routes->Append(routes_iter.Item());
        Bitmap *one_business_number = new Bitmap(business_seats_number);
        business_number->Append(one_business_number);
        Bitmap *one_coach_number = new Bitmap(coach_seats_number);
        coach_number->Append(one_coach_number);
    }
    ListIterator<int*> times_iter(input_times);
    for (; !times_iter.IsDone(); times_iter.Next()) {
        int *new_times = times_iter.Item();
        times->Append(new_times);
    }
    ListIterator<int*> fares_iter(input_fares);
    for (; !fares_iter.IsDone(); fares_iter.Next()) {
        int *new_fares = fares_iter.Item();
        fares->Append(new_fares);
    }
}

// find whether the seats is available for the request
bool Train_Schedule_Info::find_Available(Request rq, int cur_time) {
    ListIterator<Station*> routes_iter(routes);
    ListIterator<Bitmap*> business_seats_iter(business_number);
    ListIterator<Bitmap*> coach_seats_iter(coach_number);
    bool find_departure_station = false;
    bool find_destination_station = false;
    bool find_seat_number_start_available = false;
    bool find_seat_number_end_available = true;
    int index = 0, start_index = -1;
    for (; !routes_iter.IsDone(); routes_iter.Next(), business_seats_iter.Next(), coach_seats_iter.Next()) {
        if(routes_iter.Item()->get_id() == rq.get_Departure_Station_ID()) {
            if(rq.get_Seat_Type() == 0) {
                if(business_seats_iter.Item()->NumClear() >= rq.get_Passenger_Number()) {
                    find_seat_number_start_available = true;
                }
            }
            else if(rq.get_Seat_Type() == 1) {
                if(coach_seats_iter.Item()->NumClear() >= rq.get_Passenger_Number()) {
                    find_seat_number_start_available = true;
                }
            }
            find_departure_station = true;
            start_index = index;
        }
        if(find_seat_number_start_available == true) {
            if(rq.get_Seat_Type() == 0) {
                if(business_seats_iter.Item()->NumClear() < rq.get_Passenger_Number()) {
                    find_seat_number_end_available = false;
                }
            }
            else if(rq.get_Seat_Type() == 1) {
                if(coach_seats_iter.Item()->NumClear() < rq.get_Passenger_Number()) {
                    find_seat_number_end_available = false;
                }
            }
        }
        if(find_departure_station && routes_iter.Item()->get_id() == rq.get_Destination_Station_ID()) {
            find_destination_station = true;
        }
        ++index;
    }
    if(start_index == -1 || get_Departure_Time(start_index) == -1) {
        return false;
    }
    if(find_departure_station == false || find_destination_station == false || find_seat_number_start_available == false || find_seat_number_end_available == false) {
        return false;
    }
    return true;
}

// handle changes by the request
Ticket* Train_Schedule_Info::handle_changes(Request rq) {
    ListIterator<Station*> routes_iter(routes);
    bool find_departure_station = false;
    int start_index = 0, end_index = 0, count = 0;
    int start_time = 0, end_time = 0;
    for (; !routes_iter.IsDone(); routes_iter.Next()) {
        if(routes_iter.Item()->get_id() == rq.get_Departure_Station_ID()) {
            start_index = count;
            start_time = get_Departure_Time(start_index);
            find_departure_station = true;
        }
        if(find_departure_station && routes_iter.Item()->get_id() == rq.get_Destination_Station_ID()) {
            end_index = count;
            int fare = get_Price(start_index, end_index);
            end_time = get_Destination_Time(end_index);
            record_Decrease_Seat_Number(start_index, end_index, end_time, rq.get_Seat_Type(), rq.get_Passenger_Number());
            return new Ticket(fare, rq.get_Departure_Station_ID(), rq.get_Destination_Station_ID(), start_time, end_time, rq.get_Passenger_Number(), rq.get_Seat_Type(), true);
        }
        count++;
    }
    return new Ticket(0, 0, 0, 0, 0, 0, 0, false);
}

// get the price of from start_index to end_index
int Train_Schedule_Info::get_Price(int start_index, int end_index) {
    ListIterator<int*> fares_iter(fares);
    int start_price = 0;
    int end_price = 0;
    int count = 0;
    for (; !fares_iter.IsDone(); fares_iter.Next()) {
        if(count == start_index) {
            start_price = *fares_iter.Item();
        }
        else if(count == end_index) {
            end_price = *fares_iter.Item();
        }
    }
    return (end_price - start_price);
}

// get the departure time of the satrt_index
int Train_Schedule_Info::get_Departure_Time(int start_index) {
    ListIterator<int*> times_iter(times);
    int count = 0;
    for (; !times_iter.IsDone(); times_iter.Next()) {
        if(count == (start_index * 2 + 1)) {
            return *times_iter.Item();
        }
        count++;
    }
    return -1;
}

// get the destination time of the end_index
int Train_Schedule_Info::get_Destination_Time(int end_index) {
    ListIterator<int*> times_iter(times);
    int count = 0;
    for (; !times_iter.IsDone(); times_iter.Next()) {
        if(count == (end_index * 2)) {
            return *times_iter.Item();
        }
        count++;
    }
    return 0;
}

// occupy the business seats forom the start_index to the end_index
void Train_Schedule_Info::occupy_business_seats(int start_index, int end_index, int passengers_num) {
    ListIterator<Bitmap*> business_iter(business_number);
    int count1 = 0;
    for(; !business_iter.IsDone(); business_iter.Next()) {
        if(start_index == count1) {
            int i = 0, count = 0;
            while(i < passengers_num && count < business_seats_number) {
                if(!business_iter.Item()->Test(count)) {
                    business_iter.Item()->Mark(count);
                    i++;
                }
                count++;
            }
        }
        if(end_index == count1) {
            int i = 0, count = 0;
            while(i < passengers_num && count < business_seats_number) {
                if(!business_iter.Item()->Test(count)) {
                    business_iter.Item()->Mark(count);
                    i++;
                }
                count++;
            }
        }
        count1++;
    }
}

// occupy the coach seats from start_index to end_index
void Train_Schedule_Info::occupy_coach_seats(int start_index, int end_index, int passengers_num) {
    ListIterator<Bitmap*> coach_iter(coach_number);
    int count1 = 0;
    for(; !coach_iter.IsDone(); coach_iter.Next()) {
        if(start_index == count1) {
            int i = 0, count = 0;
            while(i < passengers_num && count < coach_seats_number) {
                if(!coach_iter.Item()->Test(count)) {
                    coach_iter.Item()->Mark(count);
                    i++;
                }
                count++;
            }
        }
        if(end_index == count1) {
            int i = 0, count = 0;
            while(i < passengers_num && count < coach_seats_number) {
                if(!coach_iter.Item()->Test(count)) {
                    coach_iter.Item()->Mark(count);
                    i++;
                }
                count++;
            }
        }
        count1++;
    }
}

// record the seat and decrease seats number
void Train_Schedule_Info::record_Decrease_Seat_Number(int start_index, int end_index, int et, int st, int pn) {
    int *temp1 = new int(et);
    int *temp2 = new int(st);
    int *temp3 = new int(pn);
    end_service_times->Append(temp1);
    service_type->Append(temp2);
    service_passengers_number->Append(temp3);
    if(st == 0) {
        occupy_business_seats(start_index, end_index, pn);
    }
    else if(st == 1) {
        occupy_coach_seats(start_index, end_index, pn);
    }
}

// check whether the service is completed
void Train_Schedule_Info::check_Service_Complete(int cur_time) {
    int len = end_service_times->NumInList();
    int i = 0;
    while(i < len) {
        int* a = end_service_times->RemoveFront();
        int* b = service_type->RemoveFront();
        int* c = service_passengers_number->RemoveFront();
        if(*a != cur_time) {
            end_service_times->Append(a);
            service_type->Append(b);
            service_passengers_number->Append(c);
        }
        i++;
    }
}

// let the passengers in the train
void Train_Schedule_Info::passenger_get_in() {
}

// get the times of the train schedule information
List<int*>* Train_Schedule_Info::get_times() {
    return times;
}

// let the passenger get off the train when the passengers arrive the destination station
void Train_Schedule_Info::passenger_get_out(int cur_time, int passengers_seats, int numbers) {
    int len = end_service_times->NumInList();
    int i = 0;
    while(i < len) {
        int* a = end_service_times->RemoveFront();
        int* b = service_type->RemoveFront();
        int* c = service_passengers_number->RemoveFront();
        if(*a == cur_time && *b == passengers_seats && *c == numbers) {
            continue;
        }
        end_service_times->Append(a);
        service_type->Append(b);
        service_passengers_number->Append(c);
        i++;
    }
}

// get the train id
int Train_Schedule_Info::get_train_id() {
    return train_id;
}

// Handle_Request constructor
Handle_Request::Handle_Request() {
    granted_requests = new List<Request*>();
    refused_requests = new List<Request*>();
    std::ifstream infile("../threads/trains.txt");
    std::string temp = "";
    int count = 0;
    stations = new List<Station*>();
    trains_tickets = new List<List<Ticket*>*>();
    section_statistics = new List<int*>();
    while(getline(infile, temp) && count < station_number) {
        Station* one_station = new Station(count, temp);
        stations->Append(one_station);
        count++;
    }
    infile.close();
    schedules_info = new List<Train_Schedule_Info*>();
    std::ifstream infile1("../threads/schedule.txt");
    temp = "";
    while(getline(infile1, temp)) {
        int* t_id = new int(0);
        List<Station*>* file_routes = new List<Station*>();
        List<int*>* file_times = new List<int*>();
        List<int*>* file_fares = new List<int*>();
        get_file_schedule(t_id, file_routes, file_times, file_fares, temp);
        Train_Schedule_Info* one_train_sechdule = new Train_Schedule_Info(*t_id, file_routes, file_times, file_fares);
        schedules_info->Append(one_train_sechdule);
        List<Ticket*>* new_tickets_list = new List<Ticket*>();
        trains_tickets->Append(new_tickets_list);
    }
    infile1.close();
    total_request = 0;
    total_granted_request = 0;
    total_served_itinerary = 0;
    total_passengers = 0;
}

// get the file from schedule.txt and trains.txt
void Handle_Request::get_file_schedule(int* t_id, List<Station*> *file_routes, List<int*>* file_times, List<int*>* file_fares, std::string &file_content) {
    int len = (int)file_content.length();
    int i = 0, count = 0;
    while(i < len) {
        std::string str = "";
        while(i < len && file_content[i] != '#') {
            str.push_back(file_content[i]);
            i++;
        }
        if(count == 0) {
            int index = atoi(str.c_str());
            *t_id = index;
        }
        else if(count == 1) {
            List<int*>* temps = getContent(str);
            ListIterator<int*> temps_iter(temps);
            for (; !temps_iter.IsDone(); temps_iter.Next()) {
                int temp_num = *temps_iter.Item();
                file_routes->Append(get_order_station(temp_num));
            }
        }
        else if(count == 2) {
            List<int*>* temps = getContent(str);
            ListIterator<int*> temps_iter(temps);
            for (; !temps_iter.IsDone(); temps_iter.Next()) {
                int temp_num = *temps_iter.Item();
                int* each_num = new int(temp_num);
                file_times->Append(each_num);
            }
        }
        else if(count == 3) {
            List<int*>* temps = getContent(str);
            ListIterator<int*> temps_iter(temps);
            for (; !temps_iter.IsDone(); temps_iter.Next()) {
                int temp_num = *temps_iter.Item();
                int* each_num = new int(temp_num);
                file_fares->Append(each_num);
            }
        }
        i++;
        count++;
    }
}

// get content in the string
List<int*> * Handle_Request::getContent(std::string &str) {
    int len = (int)str.length();
    int i = 0;
    List<int*>* ans = new List<int*>();
    while(i < len) {
        std::string number = "";
        while(i < len && str[i] != ' ') {
            number.push_back(str[i]);
            i++;
        }
        int num = atoi(number.c_str());
        int* each_num = new int(num);
        ans->Append(each_num);
        i++;
    }
    return ans;
}

// get the station in the index
Station* Handle_Request::get_order_station(int index) {
    int len = stations->NumInList();
    Station* ans = new Station(0, "");
    for(int i = 0; i < len; i++) {
        Station* temp = stations->RemoveFront();
        if(i == index) {
            ans = temp;
        }
        stations->Append(temp);
    }
    return ans;
}

// handle one request and do the corresponding record
Train_Schedule_Info* Handle_Request::Handle_One_Request(Request* rqs, int cur_time) {
    List<Train_Schedule_Info*>* available_trains = new List<Train_Schedule_Info*>();
    ListIterator<Train_Schedule_Info*> tsi_iter(schedules_info);
    Train_Schedule_Info* train_for_rq = new Train_Schedule_Info();
    int count = 0;
    Request* rq = rqs;
    Request new_rq = *rq;
    total_request++;
    for (; !tsi_iter.IsDone(); tsi_iter.Next()) {
        if(tsi_iter.Item()->find_Available(new_rq, cur_time) == true) {
            available_trains->Append(tsi_iter.Item());
        }
        count++;
    }
    srand(count);
    if(available_trains->NumInList() == 0) {
        refused_requests->Append(rq);
        refuesed_per_time++;
        return train_for_rq;
    }
    int index = rand()%(available_trains->NumInList());
    count = 0;
    ListIterator<Train_Schedule_Info*> at_iter(available_trains);
    for (; !at_iter.IsDone(); at_iter.Next()) {
        if(index == count) {
            Ticket* p_ticket = at_iter.Item()->handle_changes(*rq);
            rq->set_actual_depart_t(p_ticket->get_depart_time());
            rq->set_actual_destin_t(p_ticket->get_destination_time());
            train_for_rq = at_iter.Item();
            granted_requests->Append(rq);
            total_granted_request++;
            total_served_itinerary++;
            total_passengers += rq->get_Passenger_Number();
            int count1 = 0;
            ListIterator<List<Ticket*>*> trains_tickets_iter(trains_tickets);
            for(; !trains_tickets_iter.IsDone(); trains_tickets_iter.Next()) {
                if(count1 == at_iter.Item()->get_train_id()) {
                    trains_tickets_iter.Item()->Append(p_ticket);
                }
                count1++;
            }
        }
        count++;
    }
    request_per_time++;
    return train_for_rq;
}

// check whether the service is completed
void Handle_Request::check_Service_Complete(int cur_time) {
    ListIterator<Train_Schedule_Info*> tsi_iter(schedules_info);
    for(; !tsi_iter.IsDone(); tsi_iter.Next()) {
        tsi_iter.Item()->check_Service_Complete(cur_time);
    }
}

// get the train schedule information
List<Train_Schedule_Info*>* Handle_Request::get_Train_Schedule_Info() {
    return schedules_info;
}

// print statistics per simulated time
void Handle_Request::print_per(int cur_time) {
    int hour = 6 + (cur_time*10)/60;
    int minutes = (cur_time*10)%60;
    std::cout << "For simulated time " << hour << ":" << minutes;
    if(minutes == 0) {
        std::cout << "0";
    }
    std::cout << ", Granted requests number is: " << request_per_time << std::endl;
    std::cout << "Refuesd requests number is: " << refuesed_per_time << std::endl;
    request_per_time = 0;
    refuesed_per_time = 0;
    int index = 0;
    int total_p_statistics = 0;
    ListIterator<List<Ticket*>*> t_t_iter(trains_tickets);
    for(; !t_t_iter.IsDone(); t_t_iter.Next()) {
        ListIterator<Ticket*> t_l_iter(t_t_iter.Item());
        int b_itinerary_num = 0;
        int o_itinerary_num = 0;
        int boarding_pass_num = 0;
        int get_off_num = 0;
        for(; !t_l_iter.IsDone(); t_l_iter.Next()) {
            if(t_l_iter.Item()->get_depart_time() == cur_time) {
                b_itinerary_num++;
                boarding_pass_num += t_l_iter.Item()->get_P_Number();
            }
            if(t_l_iter.Item()->get_destination_time() == cur_time) {
                o_itinerary_num++;
                get_off_num += t_l_iter.Item()->get_P_Number();
            }
        }
        total_p_statistics += boarding_pass_num;
        std::cout << "For train" << index << " ," << b_itinerary_num
        << " of itinerary, " << boarding_pass_num
        << " of passengers for boarding at a station of running trains at the time"
        << std::endl;
        std::cout << "For train" << index << " ," << o_itinerary_num
        << " of itinerary, " << get_off_num
        << " of passengers for getting off a station of running trains at the time"
        << std::endl;
        index++;
    }
    int* new_section_st = new int(total_p_statistics);
    section_statistics->Append(new_section_st);
    std::cout << "============================================" << std::endl;
    std::cout << std::endl;
}

// print the overall statistics at the end of the simulation
void Handle_Request::print() {
    ListIterator<Request*> gr_iter(granted_requests);
    ListIterator<Request*> rr_iter(refused_requests);
    std::cout << "Total number of requests is: " << total_request << std::endl;
    std::cout << "Total number of granted requests is: " << total_granted_request << std::endl;
    std::cout << "Total number of served itinerary is: "
    << total_served_itinerary << std::endl;
    std::cout << "Total number of passenger is: " << total_passengers << std::endl;
    ListIterator<int*> sst_iter(section_statistics);
    int busiest_section = 0;
    int busies_time = 0;
    int index = 0;
    for(; !sst_iter.IsDone(); sst_iter.Next()) {
        if(*sst_iter.Item() > busiest_section) {
            busiest_section = *sst_iter.Item();
            busies_time = index;
        }
        index++;
    }
    int hour = 6 + (busies_time*10)/60;
    int minutes = (busies_time*10)%60;
    std::cout << "Busiest section is: " << hour << ":" << minutes;
    if(minutes == 0) {
        std::cout << "0";
    }
    std::cout << std::endl;
}
