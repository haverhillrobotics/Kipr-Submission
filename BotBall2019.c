#include <kipr/botball.h>

//Check USB is plugged in for camera and roomba
//Check the battery for Roomba and that Wallaby's battery is plugged in
//Direction variable reference
int f = 1;
int b = 0;

//Arm servo number
int arm_servo = 1;
int arm_motor = 0;
//Start servo 1 at 2047
int servo_arm_start = 2047;
//Arm is pointing straight up
int servo_arm_up = 1178;
int arm_motor_velocity = 750;
int arm_extend_time = 9400;
int power = 100;
int clock_speed = 50;
int botguy = 2;
int blue_cube = 0;
int red_cube = 1;
int yellow = 3;
//11.2 seconds to go the distance displayed by camera
int cam_scr_dis_time = 8;
//
int ninety_degrees = 3250;

//========================
//MOVING ARM
void extendArm(){
    set_servo_position(arm_servo, servo_arm_up);
    enable_servo(arm_servo);
    printf("directed up...\n");
    mav(arm_motor, arm_motor_velocity);
    msleep(arm_extend_time);
    ao();
    printf("fully extended...\n");
}
void downArm(){
    mav(arm_motor, -arm_motor_velocity);
    msleep(arm_extend_time);
    ao();
    set_servo_position(arm_servo, servo_arm_start);
    enable_servo(arm_servo);
    disable_servo(arm_servo);
}
void startArm(){
    set_servo_position(arm_servo, servo_arm_start);
    enable_servo(arm_servo);
    disable_servo(arm_servo);
}
//========================

//========================
// Cool Stuff
void spiral()
{
    int num = 0;
    while(num<=500)
    {
        create_power_led (50,255);
        create_drive_direct(num,500);
        num+=50;
        msleep(1000);
        create_stop();
    }
    create_power_led (255,255);
}
//========================

// MOVEMENTS
void forward(int time)
{
    create_drive_direct(power,power);
    msleep(time);
    create_stop();
}
void backwards(int time)
{
    create_drive_direct(-power,-power);
    msleep(time);
    create_stop();
}
void turnRight()
{
    create_drive_direct(power,0);
    msleep(1000);
    create_stop();
}
void turnLeft()
{
    create_drive_direct(0,power);
    msleep(1000);
    create_stop();
}
void roundL()
{
    create_drive_direct(200,50);
    msleep(500);
    create_stop();
}
void roundR()
{
    create_drive_direct(50,200);
    msleep(500);
    create_stop();
}
void clockwise(int time)
{
    create_drive_direct(clock_speed,-clock_speed);
    msleep(time);
    create_stop();
}
void counter_clockwise(int time)
{
    create_drive_direct(-clock_speed,clock_speed);
    msleep(time);
    create_stop();
}
// l for Left, r for Right
void turnAround(char dir)
{
    if(dir == 'l')
    {
        counter_clockwise(1900);
    }
    if(dir == 'r')
    {
        clockwise(1900);
    }
}


//========================

//========================
// Initiation Ceremony
void start()
{
    printf("Initiation ceremony has begun...\n");
    //Reset camera to prepare for competition
    create_connect();
    //camera_open_black();
    //camera_open_at_res(LOW_RES);
    printf("Create has been connected...\n");
    //Get arm servo into starting position
    startArm();
    printf("Ready to go...\n");
    
    printf("Waiting for Lights! Camera! Action!");
    wait_for_light(0);
    wait_for_shutdown_in(1180);
}
//========================

//========================
// Closing Ceremony
void end()
{
    create_disconnect();
    //camera_close();
    printf("Adios amigos...\n");
}
//========================

//========================
// Sense and Do
void sense_surroundings()
{
    //int x = 0;
    while(1)
    {
        msleep(100);
        if(get_create_lbump()==1||get_create_rbump()==1)
        {
            backwards(500);
            roundR();
        }
        if(get_create_rfcliff()==1||get_create_lfcliff()==1)
        {
            printf("Cliff, I'm falling...\n");
            backwards(250);
            turnAround('r');
            
        }
        msleep(100);
        forward(100);
    }
}

//move back and forth until Botguy is within an acceptable range
void center_it(int channel){
    while((get_object_center_x(channel, 0) >83) || (get_object_center_x(channel, 0) < 77))
    {
        printf("Botguy!!! Where are you?\n");
        camera_update();
       	msleep(100);
        if(get_object_center_x(channel,0)>83)
        {
            clockwise(75);
            printf("Right...\n");
        }
        else if(get_object_center_x(channel,0)<77)
        {
            counter_clockwise(75);
            printf("Left...\n");
        }
    }
    if((get_object_center_x(channel, 0) > 83) || (get_object_center_x(channel, 0) < 77))
    {
        center_it(channel);
    }
}
//Calculate and execute time needed to reach said object
void catch_it(int channel)
{
    int y_position = get_object_center_y(channel,0);
    printf("y position: %d\n",y_position);
    double time_go = 120 - y_position;
    printf("Distance to bottom is: %lf\n", time_go);
    time_go = time_go/120;
    printf("Percentage of height to go: %lf\n", time_go);
    time_go = time_go *cam_scr_dis_time;
    printf("Amount of time in seconds to go forward: %lf\n",time_go);
    time_go = time_go * 1000;
    printf("Time to go forward in milliseconds: %lf\n",time_go);
    //double time_go = (((120-y_position) / 120) * 11.2)*1000;
    camera_update();
    if(y_position < 120)
    {
        forward(time_go);
        printf("time_go is %lf\n", time_go);
        msleep(500);
    }
    
}
//turns clockwise until camera identifies at least one object in the channel
void sense_find_it(int channel)
{
    camera_update();
    while(get_object_count(channel)==0)
    {
        camera_update();
        clockwise(100);
    }
    // once object has been identified, will readjust until biggest object is centered in view
    center_it(channel);
    // calculate the time needed to move forward and reach object
    catch_it(channel);
    
   printf("Found you!!! You are at (%d, %d)\n", get_object_center_x(channel,0), get_object_center_y(channel,0));
}
//Go forward until front sensors are enacted
void moveTouch(int direction){
	while(get_create_lbump() !=1 || get_create_rbump() != 1){
        if(direction == 0){
        	backwards(500);
        }
        else{
            forward(500);
        }
        printf("move one time...\n");
    }
}
//========================

//========================
//MAIN MAIN MAIN MAIN MAIN
int main()
{
    start();
    extendArm();
    
    // Go forward so lines up with main tower
    backwards(500);
    
    // Turn to face building
    counter_clockwise(ninety_degrees);
    
    //Forward to building
    forward(5800);
    
    // Turn to line up pole with critter on top tower
    counter_clockwise(1625);
    
    // Put arm at an angle for best approach
    set_servo_position(arm_servo, 900);
    
    // Curve around to get pole in behind botguy
    roundL();
    roundL();
    
    end();
    return 0;
}
