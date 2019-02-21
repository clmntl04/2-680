/************************************************************/
/*    NAME: Clement Li                                              */
/*    ORGN: MIT                                             */
/*    FILE: Odometry.cpp                                        */
/*    DATE: 2-14-19
/*                                                            */
/*    Calculates total distance travelled (ODOMETRY_DIST)            */
/*    Registers for: NAV_X, NAV_Y                          */
/*    Publishes: ODOMETRY_DIST                                      */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "Odometry.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Odometry::Odometry()
{
  m_first_reading = true;
  m_current_x = 0;
  m_current_y = 0;
  m_previous_x = 0;
  m_previous_y = 0;
  m_total_distance = 0;
}

//---------------------------------------------------------
// Destructor

Odometry::~Odometry()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Odometry::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    string key = msg.GetKey();

    //update NAV_X
    if (key == "NAV_X") {
      m_current_x = msg.GetDouble();

    }

    //update NAV_Y
    if (key == "NAV_Y") {
      m_current_y = msg.GetDouble();
    }

   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Odometry::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Odometry::Iterate()
{
  AppCastingMOOSApp::Iterate();

  double subdist; //distance travelled since last calculation

  //if first measurement, set odometer to zero, calibrate location
  if (m_first_reading) {
    m_total_distance = 0;
    m_first_reading = false;

  }
  else {

    //calculate distance travelled since last point (calculated)
    subdist = sqrt((m_current_x - m_previous_x)*(m_current_x - m_previous_x) +
    (m_current_y - m_previous_y)*(m_current_y - m_previous_y));
    m_total_distance = m_total_distance + subdist; 
  }

  //update old position to current position
  m_previous_x = m_current_x;
  m_previous_y = m_current_y;

  //post total distance to ODOMETRY_DIST
  Notify("ODOMETRY_DIST",m_total_distance);

  AppCastingMOOSApp::PostReport();

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Odometry::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }
    }
  }
  
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void Odometry::RegisterVariables()
{

  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
  Register("NAV_X",0);
  Register("NAV_Y",0);
}

//-------------
// Build Report for AppCasting
bool Odometry::buildReport() 
{
  //report total distance
  m_msgs << "Total distance travelled: " << m_total_distance << endl;
  return(true);
}

