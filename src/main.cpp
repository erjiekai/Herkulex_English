/*! \mainpage Herkulex_English
 *
 * Read README.md for installation instructions, errors and dependencies
 * 
 */

#include "herkulex_driver.hpp"
#include "KeyboardFunctions.hpp"

/** @brief Blink all the specified motors
*
* Please change the motor pID to your corresponding pID. This test is using 3 motors, with pID = 1, 2, 3
*
* Alternate blinking between red, green and blue
*
* @return returns nothing
*/
void testBlink()
{
	HerkulexDriver hlx("USB Serial Port");
	KeyboardFunctions kb;

	printf("Blinking.");
	while (1)
	{
		printf(".");
		hlx.setLEDColour(1, kRed);
		hlx.setLEDColour(2, kGreen);
		hlx.setLEDColour(3, kBlue);
		Sleep(1000);
		if (kb.getNonBlockingTriggers() == KB_ESCAPE) //Press escape to quit
			break;

		printf(".");
		hlx.setLEDColour(1, kBlue);
		hlx.setLEDColour(2, kRed);
		hlx.setLEDColour(3, kGreen);
		Sleep(1000);
		if (kb.getNonBlockingTriggers() == KB_ESCAPE) //Press escape to quit
			break;

		printf(".");
		hlx.setLEDColour(1, kGreen);
		hlx.setLEDColour(2, kBlue);
		hlx.setLEDColour(3, kRed);
		Sleep(1000);
		if (kb.getNonBlockingTriggers() == KB_ESCAPE) //Press escape to quit
			break;
	}
}

/** @brief Read the angles from all the specified motors
*
* Please change the motor pID to your corresponding pID. This test is using 3 motors, with pID = 1, 2, 3
*
* Read either calibrated angle or absolute angle
*
* @return returns nothing
*/
void testRead()
{
	HerkulexDriver hlx("USB Serial Port");
	KeyboardFunctions kb;
	hlx.setAcknowledgePolicy(1, 1);
	hlx.setAcknowledgePolicy(2, 1);
	hlx.setAcknowledgePolicy(3, 1);
	hlx.setTorqueControl(1, 0);
	hlx.setTorqueControl(2, 0);
	hlx.setTorqueControl(3, 0);
	while (1)
	{
		//////// Read calibrated angle between 0 to 1023 ////////////
		//unsigned short angle1 = hlx.getCalibratedAngle(1);
		//unsigned short angle2 = hlx.getCalibratedAngle(2);
		//unsigned short angle3 = hlx.getCalibratedAngle(3);
		//printf("Angle 1=%hd\t2=%hd\t3=%hd\n", angle1, angle2, angle3);

		//////// Read absolute angle between 0deg to 360 deg ////////////
		float angle1 = hlx.getAbsoluteAngle(1);
		float angle2 = hlx.getAbsoluteAngle(2);
		float angle3 = hlx.getAbsoluteAngle(3);
		printf("Angle 1=%f\t2=%f\t3=%f\n", angle1, angle2, angle3);

		hlx.getError(1);
		hlx.getError(2);
		hlx.getError(3);

		if (kb.getNonBlockingTriggers() == KB_ESCAPE) //Press escape to quit
			break;
	}
}

/** @brief Set the motors to move through a series of positions
*
* Please change the motor pID to your corresponding pID. This test is using 3 motors, with pID = 1, 2, 3
*
* You have to observe the motor angle limit, else error will appear
*
* @return returns nothing
*/
void testMove()
{
	HerkulexDriver hlx("USB Serial Port");
	KeyboardFunctions kb;
	hlx.setAcknowledgePolicy(1, 1); // acknowledge when Read_only
	hlx.setAcknowledgePolicy(2, 1); // acknowledge when Read_only
	hlx.setAcknowledgePolicy(3, 1); // acknowledge when Read_only
	hlx.setTorqueControl(1, 2); // Torque ON
	hlx.setTorqueControl(2, 2); // Torque ON
	hlx.setTorqueControl(3, 2); // Torque ON

	S_JOG_TAG* sjog = new S_JOG_TAG[3];
	while (1)
	{
		sjog[0].set(1, 100, 60, kRed, 0); //pID, angle, operating time, operating mode
		sjog[1].set(2, 100, 60, kRed, 0);
		sjog[2].set(3, 100, 60, kRed, 0);
		hlx.runMotor(sjog, 1);
		hlx.runMotor(sjog + 1, 1);
		hlx.runMotor(sjog + 2, 1);
		Sleep(1000);
		if (kb.getNonBlockingTriggers() == KB_ESCAPE) //Press escape to quit
			break;

		sjog[0].set(1, 500, 60, kGreen, 0);
		sjog[1].set(2, 500, 60, kGreen, 0);
		sjog[2].set(3, 500, 60, kGreen, 0);
		hlx.runMotor(sjog, 1);
		hlx.runMotor(sjog + 1, 1);
		hlx.runMotor(sjog + 2, 1);
		Sleep(1000);
		if (kb.getNonBlockingTriggers() == KB_ESCAPE) //Press escape to quit
			break;

		sjog[0].set(1, 1000, 60, kBlue, 0);
		sjog[1].set(2, 1000, 60, kBlue, 0);
		sjog[2].set(3, 1000, 60, kBlue, 0);
		hlx.runMotor(sjog, 1);
		hlx.runMotor(sjog + 1, 1);
		hlx.runMotor(sjog + 2, 1);
		Sleep(1000);
		if (kb.getNonBlockingTriggers() == KB_ESCAPE) //Press escape to quit
			break;

		sjog[0].set(1, 500, 60, kGreen, 0);
		sjog[1].set(2, 500, 60, kGreen, 0);
		sjog[2].set(3, 500, 60, kGreen, 0);
		hlx.runMotor(sjog, 1);
		hlx.runMotor(sjog + 1, 1);
		hlx.runMotor(sjog + 2, 1);
		Sleep(1000);
		if (kb.getNonBlockingTriggers() == KB_ESCAPE) //Press escape to quit
			break;

		hlx.getError(1);
		hlx.getError(2);
		hlx.getError(3);

		if (kb.getNonBlockingTriggers() == KB_ESCAPE) //Press escape to quit
			break;
	}
	delete[] sjog;
}

// main used for testing
void main()
{
	testBlink();
	printf("Press enter to go to next test\n");
	getchar();
	testRead();
	printf("Press enter to go to next test\n");
	getchar();
	testMove();
}
