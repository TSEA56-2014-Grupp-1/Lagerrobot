#ifndef ARM_H_
#define ARM_H_

#define JOINT_1_MINANGLE 1;
#define JOINT_2_MINANGLE 1;
#define JOINT_3_MINANGLE 1;
#define JOINT_4_MINANGLE 1;
#define JOINT_5_MINANGLE 1;
#define JOINT_6_MINANGLE 1;
#define JOINT_1_MAXANGLE 1023;
#define JOINT_2_MAXANGLE 1023;
#define JOINT_3_MAXANGLE 1023;
#define JOINT_4_MAXANGLE 1023;
#define JOINT_5_MAXANGLE 1023;
#define JOINT_6_MAXANGLE 511;


// --- Global variables ---
uint8_t parameter[128];

void move_servo(uint8_t id, int goal_angle);

#endif /* ARM_H_ */
