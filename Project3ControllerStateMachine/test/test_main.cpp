
#include <util/delay.h>
#include "unity.h"
#include "controller.h"

void test_normal_flow(void)
{ //PI_controller pi(kp, ki, MAX_RPM, MAX_PWM, DELTA_T);
    PI_controller pi(0, 0, 15000, 255, 5);
    pi.update(0, 0);
    pi.update(0, 0);
    pi.update(0, 0);
    pi.update(0, 0);
    TEST_ASSERT_EQUAL(0, pi.update(0,0));
    PI_controller pi2(100, 0, 15000, 255, 5);
    pi2.update(0, 0);
    pi2.update(0, 0);
    pi2.update(0, 0);
    pi2.update(0, 0);
    TEST_ASSERT_EQUAL(0, pi2.update(0,0));
    PI_controller pi3(100, 100, 15000, 255, 5);
    pi3.update(0, 0);
    pi3.update(0, 0);
    pi3.update(0, 0);
    pi3.update(0, 0);
    TEST_ASSERT_EQUAL(0, pi3.update(0,0));
    PI_controller pi4(0, 100, 15000, 255, 5);
    pi4.update(0, 0);
    pi4.update(0, 0);
    pi4.update(0, 0);
    pi4.update(0, 0);
    TEST_ASSERT_EQUAL(0, pi4.update(0,0));

}

void test_p()
{ //PI_controller pi(kp, ki, MAX_RPM, MAX_PWM, DELTA_T);
    PI_controller pi(1, 0, 15000, 255, 5);
    TEST_ASSERT_EQUAL(255, pi.update(15000,0));

    PI_controller pi2(10, 0, 15000, 255, 5);
    TEST_ASSERT_EQUAL((int)10*255/15000, pi2.update(1,0));

    PI_controller pi3(10, 100, 15000, 255, 5);
    TEST_ASSERT_EQUAL((int)-10*255/15000, pi3.update(0,1));

    PI_controller pi4(200, 0, 15000, 255, 5);
    TEST_ASSERT_EQUAL(255, pi4.update(15000,0));

    PI_controller pi5(200, 0, 15000, 255, 5);
    TEST_ASSERT_EQUAL(-255, pi4.update(0,15000));
}

void test_i()
{ //PI_controller pi(kp, ki, MAX_RPM, MAX_PWM, DELTA_T);
    PI_controller pi(0, 1000, 100, 100, 5); 
    for(int i=0; i<9;i++) {
        pi.update(1,0);
    }
    TEST_ASSERT_INT_WITHIN(1, 50, pi.update(1,0)); // within because of floating point errors


}
void test_windup() {
    // input some really high numbers with very high ki
    // then input lower number and make sure output is <255
    PI_controller pi(1, 1000, 100, 100, 5); 
    for(int i=0; i<255;i++) {
        pi.update(1,0);
    }
    pi.update(0,1);
    TEST_ASSERT_LESS_THAN(100, pi.update(0,1)); // needs to overcome delta_t
}

void test_test() {
    TEST_ASSERT_EQUAL(1,1);
}

int main()
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    //_delay_ms(2000); 

    UNITY_BEGIN(); // IMPORTANT LINE!a
    RUN_TEST(test_test);
    RUN_TEST(test_normal_flow);
    RUN_TEST(test_p);
    RUN_TEST(test_i);
    RUN_TEST(test_windup);
    // Add more unit tests here

    UNITY_END(); // stop unit testing
}


