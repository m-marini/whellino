#ifndef Wheelly_h
#define Wheelly_h

#include "Arduino.h"

#include "Contacts.h"
#include "mpu6050mm.h"
#include "MotionCtrl.h"
#include "Display.h"
#include "ProxySensor.h"
#include "CommandInterpreter.h"

/*
   Wheelly controller.
   Handles all the behavior of wheelly robot:
   - commands from wifi modules
   - scanning of proxy sensors
   - contact sensors
   - mpu sensors
   - motor sensors
   - drives the motors
   - operational timing processes
*/
class Wheelly {
  public:
    /*
       Creates wheelly controller
    */
    Wheelly();

    /*
       Initializes wheelly controller
       Return true if successfully initialized
    */
    boolean begin(void);

    /*
       Sets on reply call back
    */
    void onReply(void(* callback)(void*, const char* cmd), void* context = NULL) {
      _onReply = callback;
      _context = context;
    }

    /*
       Pools wheelly controller
    */
    void polling(const unsigned long t0 = millis());

    /**
       Execute a command
       Returns true if command ok
       @param t0 the current time
       @param command the command
    */
    const boolean execute(const unsigned long t0, const char* command) {
      return _commandInterpreter.execute(t0, command);
    }

    /**
       Scans proximity to the direction

       @param angle the angle (DEG)
       @param t0 the scanning instant
    */
    void scan(const int angle, const unsigned long t0 = millis());


    /*
       Moves the robot to the direction at speed

       @param direction the direction (DEG)
       @param speed the speed (pps)
    */
    void move(const int direction, const int speed);

    /*
       Moves the robot to the direction at speed
    */
    void halt(void) {
      _motionCtrl.halt();
    }

    /*
       Resets wheelly
    */
    void reset(void);

    /*
       Displays the message
       @param message the message
    */
    void display(const char *message) {
      _display.showWiFiInfo(message);
    }

    /**
       Sets on line status
       @param onLine true if wifi module online
    */
    void onLine(const boolean onLine) {
      _onLine = onLine;
    }

    /**
       Set connected state
       @param state true if connected
    */
    void connected(const boolean state) {
      _display.connected(state);
    }

    /**
       Queries and sends the status
    */
    void queryStatus(void);

    /*
       Configures motion controller
       @param params the motion controller parameters
    */
    void configMotionController(const int* params) {
      _motionCtrl.configController(params);
    }

    /*
       Configures motor sensors
       @param tau the decay value of motor sensors
    */
    void configMotorSensors(const int tau) {
      _motionCtrl.tau(tau);
    }

    /*
       Configures feedback motor controller
       @param p the feedback motor controller parameters
       @param left true for left motor controller
    */
    void configFeedbackMotorController(const long *p, const boolean left) {
      (left ? _motionCtrl.leftMotor() : _motionCtrl.rightMotor()).muConfig(p);
    }

    /*
       Configures tcs motor controller
       @param p the tcs motor controller parameters
       @param left true for left motor controller
    */
    void configTcsMotorController(const int *p, const boolean left) {
      (left ? _motionCtrl.leftMotor() : _motionCtrl.rightMotor()).tcsConfig(p);
    }

    /**
       Configures intervals [send interval, scan interval]
       @param p the right motor controller parameters
    */
    void configIntervals(const int *p);

    /*
       Sets activity
    */
    void activity(const unsigned long t0 = millis()) {
      _display.activity(t0);
    }

    /*
       Sends the data reply
       @param data the data reply
    */
    void sendReply(const char* data);

    /**
       Returns the motion controller
    */
    MotionCtrlClass& motionCtrl(void) {
      return _motionCtrl;
    }

  private:
    DisplayClass _display;

    MotionCtrlClass _motionCtrl;

    MPU6050Class _mpu;
    boolean _onLine;
    int _yaw;
    unsigned long _mpuTimeout;
    uint8_t _mpuError;

    ContactSensors _contactSensors;

    unsigned long _sendInterval;
    Timer _sendTimer;

    Timer _ledTimer;
    boolean _ledActive;

    Timer _statsTimer;
    unsigned long _counter;
    unsigned long _statsTime;

    ProxySensor _proxySensor;
    unsigned long _echoTime;
    unsigned long _echoDelay;
    int _echoDirection;
    int _echoYaw;
    float _echoXPulses;
    float _echoYPulses;

    unsigned long _supplyTimeout;
    unsigned long _supplyTime;
    int _supplyVoltage;

    CommandInterpreter _commandInterpreter;

    void (*_onReply)(void*, const char*);
    void* _context;

    const boolean canMoveForward(void) const;
    const boolean canMoveBackward(void) const;
    const int forwardBlockDistanceTime() const;

    void handleProxyData(void);
    void handleStats(void);
    void handleLed(const unsigned long n);
    void handleMpuData(void);
    void handleChangedContacts(void);

    void queryConfig(void);

    void sendStatus(void);
    void sendProxy(void);
    void sendContacts(void);
    void sendSupply(void);
    void sendMotion(void);
    void sampleSupply(void);

};

#endif
