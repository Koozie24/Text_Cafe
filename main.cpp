#include <iostream>
#include <string>
#include <thread>
#include <time.h>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <variant>

std::string input_command;
int customer_count = 1;
time_t current_time;
int elapsed = 0;
int total_earnings = 0;

std::string possible_orders[4] = {
    "cheese pizza",
    "pepperoni pizza",
    "hamburger",
    "cheeseburger"
};

struct Ingredients{
    std::string order_name;
    std::string ingredient_one;
    std::string ingredient_two;
    std::string ingredient_three;
};

//initialize foods
Ingredients hamburger = {.order_name = "hamburger", .ingredient_one = "bun", .ingredient_two = "patty"};
Ingredients cheeseburger = {.order_name = "cheeseburger", .ingredient_one = "bun", .ingredient_two = "patty", .ingredient_three = "cheese"};
Ingredients cheese_pizza = {.order_name = "cheese pizza", .ingredient_one = "dough", .ingredient_two = "cheese"};
Ingredients pepperoni_pizza = {.order_name ="pepperoni pizza", .ingredient_one = "dough", .ingredient_two = "cheese", .ingredient_three = "pepperoni"};

class Customer{
    public:
        std::string customer_name = pick_name();
        std::string order = pick_order();
        time_t cust_arrives = time(0);
        std::vector<std::string> added_ingredients;
        int max_payout = max_order_payout();
        int served = 0;
        int arrival_order = customer_count - 1;

        //constructor
        Customer(std::string name, std::string order)
        : customer_name(name), order(order), cust_arrives(time(0)){}

        /*pick_order() is a method of customer that randomly selects a customers order upon initialization.
        Return value is a string containing the customers order.*/
        static std::string pick_order(){
            int pick_a_num = rand() % 4;

            std::string customer_order = possible_orders[pick_a_num];
            
            return customer_order;
        }

        static int max_order_payout(){
            //random int 4-34
            int random_payout =  rand() % 30 + 5;

            return random_payout;
        }

        /*pick_name() is a method of customer that randomly selects a customers name upon initialization.
        Return value is a string containing the customers name.*/
        static std::string pick_name(){
            std::string count_to_str = std::to_string(customer_count);
            std::string customer_name = "Hungry " + count_to_str;

            //customer_count++;
            return customer_name;
        }

        // Overload operator<< to print Customer objects
        friend std::ostream& operator<<(std::ostream& os, const Customer& customer) {
        os << "Customer Name: " << customer.customer_name
           << ", Order: " << customer.order
           << ", Arrival Time: " << customer.cust_arrives;
        return os;
    }
};

std::vector<Customer> customer_vec;

//function runs in detached thread to continuosly read input through getline
void take_input(){
    for(;;){
        //skip leading ws
        std::getline(std::cin >> std::ws, input_command);
    }
}

//function takes string argument of order. prints the required ingredient list based on current customer selected. 
void show_ingredients(std::string current_customer_order){

    if (current_customer_order == "hamburger"){
        std::cout << hamburger.ingredient_one << " and " << hamburger.ingredient_two << "                  |" << std::endl;
    }
    else if (current_customer_order == "cheeseburger"){
        std::cout << cheeseburger.ingredient_one << " and " << cheeseburger.ingredient_two << " and " << cheeseburger.ingredient_three << "       |" << std::endl;
    }
    else if (current_customer_order == "cheese pizza"){
        std::cout << cheese_pizza.ingredient_one << " and " << cheese_pizza.ingredient_two << "               |" << std::endl;
    }
    else if(current_customer_order == "pepperoni pizza"){
        std::cout << pepperoni_pizza.ingredient_one << " and " << pepperoni_pizza.ingredient_two << " and " << pepperoni_pizza.ingredient_three << " |" << std::endl;
    }
}

//function takes two string arguments and prints current screen
void print_screen(std::string current_customer, std::string current_customer_order){
    std::cout << "------------------------------------------------------------------" << std::endl;
    std::cout << "|  Current customers in cafe:   | Earnings: $" << total_earnings << "                   |" << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;

    for(int i = 0; i < customer_vec.size(); i++){
        //check if already served
        if(customer_vec[i].served == 0){
            int size_of_print = 25 - (customer_vec[i].customer_name.size() + customer_vec[i].order.size());
            std::string str_sep(size_of_print, ' ');
            std::cout << "|    " << customer_vec[i].customer_name << ": "
            << customer_vec[i].order << str_sep << "| ";

            show_ingredients(customer_vec[i].order);
            std::cout << std::endl;
        }
    }
    std::cout << "------------------------------------------------------------------" << std::endl;

    if(!current_customer.empty()){
        std::cout << "Current User Selected: " << current_customer << std::endl;
        //show_ingredients(current_customer_order);
    }
}

//function clears current terminal screen
void clear_screen(){
    std::cout << "\033[2J\033[1;1H";
}

//function takes integer time argument and two string arguments to instantiate new customer object. 
void new_customer(int total_time, std::string current_cust_selected, std::string current_customer_order){
    if(total_time % 10 == 0){
        std::cout << "Time is: " << total_time << " looks like we need a new customer" << std::endl;
        std::string new_cust_name = Customer::pick_name();
        std::string new_cust_order = Customer::pick_order();

        Customer new_customer(new_cust_name, new_cust_order);
        customer_vec.push_back(new_customer);

        clear_screen();
        print_screen(current_cust_selected, current_customer_order);

        customer_count++;
    }

}

//searches customer vector by reference of customer for match to search name. if match found returns reference to customer object match or null pointer
Customer* find_cust_obj_by_string_name(std::string &search_name){
    for(auto &customer : customer_vec){
        if(customer.customer_name == search_name){
            return &customer;
        }
    }
    return nullptr;
}

//function takes pointer to customer object and string. adds ingredient to customer objects' added ingredients vector
void add_ingredient_to_order_vect(Customer* customer, std::string ingredient){
    customer->added_ingredients.push_back(ingredient);
}

//function takes 4 required and 2 option string arguments. initializes two vectors, iterates through nested loops to compare and check they are the same. returns 0 or 1 if incorrect or correct, respectively. 
int check_ingredients_correct(std::string first_added, std::string second_added, std::string compare_one, std::string compare_two, std::string third_added = "", std::string compare_three = ""){
    std::vector<std::string> added_vec {first_added, second_added, third_added};
    std::vector<std::string> referencing_vec {compare_one, compare_two, compare_three};
    int ret_val;
    int matches = 0;

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(added_vec[i] == referencing_vec[j]){
                //std::cout << "ADDED vec i: " << added_vec[i] << " ref vec j: " << referencing_vec[j] << std::endl;
                matches++;
                break;
            }
        }
    }

    if(matches == 3){
        ret_val = 1;
        std::cout << "You order was delivered to the customer and was correct. You get a payout!" << std::endl;
    }
    else{
        std::cout << "Sorry your order was incorrect, thats coming out of your paycheck!" << std::endl;
        ret_val = 0;
    }

    return ret_val;
}

//Function takes pointer to customer object and string of input order. Verifies ingredients added and returns either 0 or 1, if incorrect or correct, respectively. 
int verify_added_ingredients(Customer* customer, std::string current_customer_order){
    int ingredient_check = 0;
    std::string first_added, second_added, third_added, compare_one, compare_two, compare_three;

    int number_of_ingredients = customer->added_ingredients.size();

    //initial check minimum number of ingredients added
    if(number_of_ingredients >= 2){
        first_added = customer->added_ingredients[0];
        second_added = customer->added_ingredients[1];

        if(current_customer_order == "hamburger" || current_customer_order == "cheese pizza"){
            if(current_customer_order == "hamburger"){
                compare_one = hamburger.ingredient_one;
                compare_two = hamburger.ingredient_two;
            }
            else{
                compare_one = cheese_pizza.ingredient_one;
                compare_two = cheese_pizza.ingredient_two;
                std::cout << ingredient_check << std::endl;
            }   

            ingredient_check = check_ingredients_correct(first_added, second_added, compare_one, compare_two);

        }

        else if ((current_customer_order == "cheeseburger" || current_customer_order == "pepperoni pizza") && number_of_ingredients == 3){
            third_added = customer->added_ingredients[2];

            if(current_customer_order == "cheeseburger"){
                compare_one = cheeseburger.ingredient_one;
                compare_two = cheeseburger.ingredient_two;
                compare_three = cheeseburger.ingredient_three;
            }
            else{
                compare_one = pepperoni_pizza.ingredient_one;
                compare_two = pepperoni_pizza.ingredient_two;
                compare_three = pepperoni_pizza.ingredient_three;
            }

            ingredient_check = check_ingredients_correct(first_added, second_added, compare_one, compare_two, third_added, compare_three);
        }
    }

    return ingredient_check;
}

int main(){
    srand(time(NULL));
    std::string current_customer, current_customer_order;

    int check_submit_correct;
    time_t start_time = time(0);
    new_customer(elapsed, current_customer, current_customer_order);
    //detached thread handling getline
    std::thread command_thread(take_input);
    command_thread.detach();
    
    for(;;){
        current_time = time(0);
        sleep(1);

        if(!input_command.empty()){
            
            auto my_cust_obj = find_cust_obj_by_string_name(current_customer);

            if(input_command == "quit"){
                break;
            }
            else if(input_command == "deselect"){
                current_customer.clear();
                current_customer_order.clear();

                clear_screen();
                print_screen(current_customer, current_customer_order);
            }
            else if((current_customer_order == "pepperoni pizza" || current_customer_order == "cheese pizza") && (input_command == "dough" || input_command == "cheese") && my_cust_obj){
                add_ingredient_to_order_vect(my_cust_obj, input_command);
                std::cout << "Added ingredient: " << input_command << " for " << current_customer << "'s order. " << std::endl;
            }
            else if(current_customer_order == "pepperoni pizza" && input_command == "pepperoni" && my_cust_obj){
                add_ingredient_to_order_vect(my_cust_obj, input_command);
                std::cout << "Added ingredient: " << input_command << " for " << current_customer << "'s order. " << std::endl;
            }
            else if((current_customer_order == "cheeseburger" || current_customer_order == "hamburger") && (input_command == "bun" || input_command == "patty") && my_cust_obj){
                add_ingredient_to_order_vect(my_cust_obj, input_command);
                std::cout << "Added ingredient: " << input_command << " for " << current_customer << "'s order. " << std::endl;
            }
            else if(current_customer_order == "cheeseburger" && input_command == "cheese" && my_cust_obj){
                add_ingredient_to_order_vect(my_cust_obj, input_command);
                std::cout << "Added ingredient: " << input_command << " for " << current_customer << "'s order. " << std::endl;
            }
            else if (!current_customer.empty() && input_command == "serve" && my_cust_obj){
                check_submit_correct = verify_added_ingredients(my_cust_obj, current_customer_order);
                
                //update earnings
                if(check_submit_correct == 1){
                    total_earnings =  total_earnings + my_cust_obj->max_payout;
                }
                else if (check_submit_correct == 0){
                    std::cout << my_cust_obj->max_payout << std::endl;
                    total_earnings = total_earnings - my_cust_obj->max_payout;
                }
                
                //update served status
                my_cust_obj->served = 1;
                int vec_index = my_cust_obj->arrival_order;
                customer_vec[vec_index].served = 1;

                current_customer.clear();
                current_customer_order.clear();

                clear_screen();
                print_screen(current_customer, current_customer_order);
            }
            else{
                for(int i = 0; i < customer_vec.size(); i++){
                    if(customer_vec[i].customer_name == input_command){
                        current_customer.clear();
                        current_customer = input_command;
                        current_customer_order = customer_vec[i].order;

                        std::cout << "Current User Selected: " << current_customer << std::endl;

                        clear_screen();
                        print_screen(current_customer, current_customer_order);
                    }
                }

                input_command.clear();
            }
        }

        elapsed = current_time - start_time + 1;

        input_command.clear();

        if(elapsed > 59){
            clear_screen();
            std::cout << "One minute has passed. Game over. You ended the game with: $" << total_earnings << std::endl;
            break;
        }
        
        new_customer(elapsed, current_customer, current_customer_order);
    }

    return 0;
}