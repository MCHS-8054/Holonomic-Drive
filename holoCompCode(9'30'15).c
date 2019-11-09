#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port5,           fR,            tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port6,           fL,            tmotorVex393_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port7,           bL,            tmotorVex393_MC29, openLoop, encoderPort, I2C_3)
#pragma config(Motor,  port8,           bR,            tmotorVex393_MC29, openLoop, encoderPort, I2C_4)
#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(15)
#pragma userControlDuration(105)

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!
float frontBackPower=vexRT[Ch2]; //Y-axis power
float leftRightPower=vexRT[Ch1]; //X-axis power
float frontBackPowerMod=vexRT[Ch2]; //Modded Y-axis power to account for degrees turned (changed later)
float leftRightPowerMod=vexRT[Ch1];//Modded X-axis power to account for degrees turned (changed later)
int frontRightMultiplier=-1;//Causes the correct rotation of the named motor
int frontLeftMultiplier=1;//Causes the correct rotation of the named motor
int backRightMultiplier=-1;//Causes the correct rotation of the named motor
int backLeftMultiplier=1;//Causes the correct rotation of the named motor
float degree=0; //degree of robot rotation
float tempDegree=0; //In case you need to make a different degree value
float circle=15400; //Encoder total for a full circle
float jAngle=0; //Default joystick angle
float rSense=1.9; //rotation sensitivity
int turnLR=vexRT[Ch4];//Used for Pythagorean theorem to calculate turn power
int turnBF=vexRT[Ch3];//Used for Pythagorean theorem to calculate turn power
int turnPower=0;//Calculated turn power (later in code)
int turnDir=0;//Calculates the turn direction
float dJAngle=0;//Joystick angle shown on the display
int dTurnPower=0;//Turn power shown on the display
int dTurnDir=0;//Turn direction multiplier shown on the display
//int aThresh=10;// If we needed to slow down rotation, this would be used to find an angle threshold
//to slow down within (10 degrees within our targeted degree)
int rThresh=18;// Rotation threshold so we don't kill the motors with accidental inputs that won't
//do anything
float fullMovePct=1.0;// Multiplier to make movement be full power
float fullRotPct=1.0;// Multiplier to make rotation be full power
float moveWhileRotMovePct=0.75;// Limits motor power so simultaneous movement and rotation are possible
float moveWhileRotRotPct=0.25;// Limits motor power so simultaneous movement and rotation are possible
int mThresh=25;// Movement threshold so we don't kill the motors with accidental inputs that won't
//do anyhting
float desAngle=0;//Desired angle for use in turnToAngle task
int desRotPower=0;//Desired turn speed for use in the turnToAngle task
float turnToAngleSense=1;//Sensitivity for turnToAngle task
bool autoRotate=false;//False when you'renot auto-rotating
bool doneAutoMove=true;//False when not auto-moving
float desMove_cm=0;//Number of centimeters we want to move in move_cm
float dist=0;
int startFR=SensorValue[I2C_1];
int startFL=SensorValue[I2C_2];
int startBL=SensorValue[I2C_3];
int startBR=SensorValue[I2C_4];
float mAngle=0;//Desired movement angle for autonomous movement
/////////////////////////////////////////////////////////////////////////////////////////
//
//                          Pre-Autonomous Functions
//
// You may want to perform some actions before the competition starts. Do them in the
// following function.
//
/////////////////////////////////////////////////////////////////////////////////////////

void pre_auton()
{
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
	// Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;

	// All activities that occur before the competition starts
	// Example: clearing encoders, setting servo positions, ...
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Autonomous Task
//
// This task is used to control your robot during the autonomous phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task display()
{
	const short leftButton = 1;//Button input values
	const short centerButton = 2;//Button input values
	const short rightButton = 4;//Button input values
	const short joystickInOut=0;
	const short degreePage=1;
	const short turnPage=2;
	const short distPage=3;

	int pageIndex=distPage;//Shows us a cetain screen for testing


	clearLCDLine(0);
	clearLCDLine(1);//Clears the LCD to start the program

	bLCDBacklight = true;
	string str;//A misc. string


	while(true)
	{
		clearLCDLine(0);
		clearLCDLine(1);
		if(nLCDButtons==leftButton){
			while(nLCDButtons==leftButton)
			{
			}
			waitUntil(nLCDButtons==0);
			pageIndex-=1;
		}
		else if(nLCDButtons==centerButton){
			while(nLCDButtons==centerButton)
			{
			}
			waitUntil(nLCDButtons==0);
			pageIndex+=1;
		}
		if(pageIndex>3){
			pageIndex=0;
		}
		if(pageIndex<0){
			pageIndex=3;
		}
		if(pageIndex==joystickInOut)
		{

			displayLCDPos(0,0);                   // Set the cursor to line 0, position 0 (top line, far left)
			sprintf(str, "XIn:%d", leftRightPower);
			displayNextLCDString(str);
			displayLCDPos(0,8);                   // Set the cursor to line 0, position 8 (top line, middle)
			sprintf(str, "XOut:%d", leftRightPowerMod);
			displayNextLCDString(str);
			//The top line is used to show us the inputs from the joystick
			displayLCDPos(1,0);                   // Set the cursor to line 1, position 0 (bottom line, far left)
			sprintf(str, "YIn:%d", frontBackPower);
			displayNextLCDString(str);
			displayLCDPos(1,8);                   // Set the cursor to line 1, position 8 (bottom line, middle)
			sprintf(str, "YOut:%d",frontBackPowerMod);
			displayNextLCDString(str);
			//The bottom line is used to show modded outputs form the joystick
		}
		else if(pageIndex==degreePage)
		{
			displayLCDPos(0,0);
			sprintf(str, "Deg:%3.3f", degree);//Shows the degree for purposes of use within later math
			displayNextLCDString(str);
			displayLCDPos(1,0);
			sprintf(str, "TDeg:%3.3f", tempDegree);//Shows the altered degree to make the degree be between
			//0 and 359 (not negative and not greater than 359)
			displayNextLCDString(str);

		}
		else if(pageIndex==turnPage)
		{
			displayLCDPos(0,0);
			sprintf(str, "TP:%3.3f", dTurnPower);//Displays turn power
			displayNextLCDString(str);
			displayLCDPos(0,9);
			sprintf(str,"TD:%3.3f",dTurnDir);//Displays the multiplier for the shortest turn direction
			//
			displayNextLCDString(str);
			displayLCDPos(1,0);
			sprintf(str, "JAng:%3.3f", dJAngle);//Displays the angle which the joystick is poitning towards
			displayNextLCDString(str);
		}
		else if(pageIndex==distPage){
			int fRDist=0;
			int bRDist=0;
			int fLDist=0;
			int bLDist=0;
			float distMultiplier=(1-(abs(45 -((mAngle-tempDegree)%90))/45.0))+1;

			fRDist=abs(startFR-SensorValue[I2C_1]);
			bLDist=abs(startBL-SensorValue[I2C_3]);
			fLDist=abs(startFL-SensorValue[I2C_2]);
			bRDist=abs(startBR-SensorValue[I2C_4]);
			dist=distMultiplier*(fRDist+bLDist+fLDist+bRDist);
			//doneAutoMove=false;
			displayLCDPos(0,0);
			sprintf(str, "D:%3.3f", dist);//Displays turn power
			displayNextLCDString(str);
			/*displayLCDPos(0,9);
			sprintf(str,"TD:%3.3f",dTurnDir);//Displays the multiplier for the shortest turn direction
			//
			displayNextLCDString(str);
			displayLCDPos(1,0);
			sprintf(str, "JAng:%3.3f", dJAngle);//Displays the angle which the joystick is poitning towards
			displayNextLCDString(str);*/
		}
		wait1Msec(50);
	}
}
task readJoystick(){
	while(true){
		frontBackPower=vexRT[Ch2];//See previous reference at the top of the page
		leftRightPower=vexRT[Ch1];//See previous reference at the top of the page
		turnLR=vexRT[Ch4];//See previous reference at the top of the page
		turnBF=vexRT[Ch3];//See previous reference at the top of the page

		jAngle=radiansToDegrees(atan2(turnLR,turnBF));//Calculates the angle of the joystick by taking the
		//inverse tangent of the Pythagorean triangle that's formed from the inputs of Ch3 and Ch4
		turnPower=sqrt((turnLR*turnLR)+(turnBF*turnBF));//Turn power is the hypotenuse of the triangle formed by
		//the  inputs of Ch3 and Ch4
		if(turnPower<rThresh)
		{
			turnPower=0;//Prevents killing the motors with small, unintentional inputs that won't do anything
		}

	}
}

task holoDrive(){
	while(true){

		degree=(SensorValue[I2C_1]+SensorValue[I2C_2]+SensorValue[I2C_3]+SensorValue[I2C_4])/(circle/360);
		//Calculates the degrees turned by measuring total encoder counts and then converting to degrees
		//If it's less than the full circle degrees, it will act as a percent multiplier to 360 degrees

		tempDegree=degree%360;//Makes degrees<360 always, using modulus (not percent)
		if(tempDegree<0){
			tempDegree+=360;//Makes degrees positive always
		}
		int angleDif=jAngle-tempDegree;//Calculates how many degrees we need to turn
		turnDir=sgn(angleDif);//Powers motors the direction we need them;lets us turn using the shortest path
		if(abs(angleDif)>180){
			turnDir=-turnDir;//Can override turnDir in circumstances where turnDir doesn't use the fastest path
		}
		int turnPowerLim=abs(rSense*angleDif);
		if(turnPower>turnPowerLim)
		{
			turnPower=turnPowerLim;//Limits turn power so we don't turn too fast
		}

		leftRightPowerMod=((leftRightPower)*(cosDegrees(tempDegree)))+((frontBackPower)*(-sinDegrees(tempDegree)));
		frontBackPowerMod=((leftRightPower)*(sinDegrees(tempDegree)))+((frontBackPower)*(cosDegrees(tempDegree)));
		//The mod code essentially rotates the joystick axis based on the degree
		//Formula:
		//This allows us to move move forward no matter how we're rotated

		if(vexRT[Btn7U]==1){//Acts as a reset button in the event that you're turned too much/inaccurate somehow
			degree=0;
			tempDegree=0;
			SensorValue[I2C_1]=0;
			SensorValue[I2C_2]=0;
			SensorValue[I2C_3]=0;
			SensorValue[I2C_4]=0;
		}

		dJAngle=jAngle;
		dTurnPower=turnPower;
		dTurnDir=turnDir;
		//Used for for display variables

		float movePct=fullMovePct;//Used to give full move power when you're only moving
		float rotPct=0.0;//0% rotation power when you're moving, because it would result in less move power
		if((abs(frontBackPower)>mThresh||abs(leftRightPower)>mThresh)&&turnPower>rThresh)//If you're pressing
			//the move joystick and the rotate joystick (over the threshold) at the same time
		{
			movePct=moveWhileRotMovePct;//Lowers move power to make room for rotation power
			rotPct=moveWhileRotRotPct;//Adds in rotation power
		}

		else if(turnPower>rThresh)//If you're only rotating
		{
			movePct=0.0;//Cancels all normal movement power
			rotPct=fullRotPct;//Rotation power is now at max
		}


		motor[fR]=((turnPower*turnDir)*rotPct)+(((frontBackPowerMod*frontRightMultiplier)+(leftRightPowerMod*frontRightMultiplier*-1))/2)*movePct;
		motor[bR]=((turnPower*turnDir)*rotPct)+(((frontBackPowerMod*backRightMultiplier)+(leftRightPowerMod*backRightMultiplier))/2)*movePct;
		motor[fL]=((turnPower*turnDir)*rotPct)+(((frontBackPowerMod*frontLeftMultiplier)+(leftRightPowerMod*frontLeftMultiplier))/2)*movePct;
		motor[bL]=((turnPower*turnDir)*rotPct)+(((frontBackPowerMod*backLeftMultiplier)+(leftRightPowerMod*backLeftMultiplier*-1))/2)*movePct;
		//This is really complicated code written by Mr. Braskin
		//It allows you to go in any direction without any trouble from motors cancelling, etc.

	}
}
task turnToAngle(){
	jAngle=desAngle;
	turnPower=desRotPower;
	while(abs(desAngle-tempDegree)>turnToAngleSense){
		/*		turnLR=vexRT[Ch4];//See previous reference at the top of the page
		turnBF=vexRT[Ch3];//See previous reference at the top of the page
		desSin=radiansToDegrees(sin(desAngle));
		desCos=radiansToDegrees(cos(desAngle));
		tanDesAngle=(desSin/desCos);
		desTurnSpeed=sqrt((desSin*desSin)+(desCos*desCos));*/
	}
	turnPower=0;
}
void resetDistMeasure(){
	startFR=SensorValue[I2C_1];
	startFL=SensorValue[I2C_2];
	startBL=SensorValue[I2C_3];
	startBR=SensorValue[I2C_4];
}

task move_cm(){

}

task autonomous()
{

	startTask(display);
	startTask(holoDrive);

	//Start your tasks so your robot is actually operational
	/*	frontBackPower=vexRT[Ch2];//See previous reference at the top of the page
	leftRightPower=vexRT[Ch1];//See previous reference at the top of the page
	turnLR=vexRT[Ch4];//See previous reference at the top of the page
	turnBF=vexRT[Ch3];//See previous reference at the top of the page*/
	//	frontBackPower=127;
	//	leftRightPower=0;
	mAngle=radiansToDegrees(atan2(0,127));
	wait10Msec(100);
	/*	frontBackPower=0;
	desAngle=180;
	desRotPower=50;
	startTask(turnToAngle);
	wait10Msec(50);
	frontBackPower=96;
	wait10Msec(100);
	frontBackPower=0;*/

}


/////////////////////////////////////////////////////////////////////////////////////////
//
//                                 User Control Task
//
// This task is used to control your robot during the user control phase of a VEX Competition.
// You must modify the code to add your own robot specific commands here.
//
/////////////////////////////////////////////////////////////////////////////////////////

task usercontrol()
{
	startTask(readJoystick);
	startTask(display);
	startTask(holoDrive);
	//Start your tasks so your robot is actually operational
	while (true)
	{
		if(vexRT[Btn5D]==1){

		}
	}
}
