#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>

#include "move.h"
#include "isr.h"


int update_distance(int update){
    current_distance += update;
}

int finished_moving(){
    return done_moving;
}

void speed_up(){
    increment += 100;
}

// int get_distance_moved(){
//     return ;
// }

// uint16_t ticks_to_distance(){
//     return WHEEL_DIAMETER/TICKS_PER_REV_6;
// }

// uint16_t calc_circumference(){
//     return(2* M_PI * WHEEL_DIAMETER);
// }

long ticks_to_distance(long ticks){
    long distance = (ticks / TICKS_PER_REV_6) * (WHEEL_DIAMETER * M_PI / GEAR_RATIO);
    return distance;
}

int distance_to_ticks(int distance){
    return (distance * TICKS_PER_REV_6) / (WHEEL_DIAMETER * M_PI / GEAR_RATIO);
}

int enter_distance(){
    int input_distance;
    printf("Enter a distance: ");
    scanf("%d", &input_distance);
    printf("given distance: %d\n", input_distance);
    entered_distance = input_distance;
    //int ticks = distance_to_ticks(input_distance);
    return 1;
}

int enter_speed_and_distance(int input_distance, long long * increment){
    int input_speed;
    int speed;
    printf("Enter a distance: ");
    scanf("%d", &input_distance);
    int ticks = distance_to_ticks(input_distance);
    printf("Enter a speed: ");
    scanf("%d", &input_speed);
    speed = (input_distance/input_speed >> 3);
    increment = &speed;
    return ticks;
}

int rover_move(){
    done_moving = 0;
    long long initial_position = get_motor_position(0);
    long long cur_target = get_target_position(0) + ((long long)increment<<30);
    set_target_position(0, cur_target);
    ticks_moved_so_far += increment;

    printf("%llu %llu %llu\n", isr_counter, ticks_moved_so_far, initial_position);
    if(ticks_moved_so_far >= distance_to_ticks(entered_distance)){
        //set_motor_speed(0, 0);
        printf("i finished\n");
        done_moving = 1;
    }

    long long difference = get_target_position(0) - ((long long)(get_motor_position(0))<<30); 

    long long speed = ((KP * difference)>>32) -  ( KV * get_motor_velocity(0) );
    set_motor_speed(0, speed);

    ticks_moved_so_far += get_motor_position(0) - initial_position;
    isr_counter++; 

    return 0;
}