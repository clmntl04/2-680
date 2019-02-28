/************************************************************/
/*    NAME: Clement Li                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "PrimeFactor.h"
#include <sstream>
#include <list>
#include <cstdint>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
using namespace std;

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{
  //initialize variables
  numReceived = "";
  receivedIndex = 1; //counter for index of received numbers
  calculatedIndex = 1; //counter for index of calculated numbers
  max_steps = 100000; //max iteration steps at a time, before moving to next number, and returning later
}

//---------------------------------------------------------
// Destructor

PrimeFactor::~PrimeFactor()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

   if(key == "FOO") 
       cout << "great!";

   else if(key == "NUM_VALUE") { 
      //receive string of 64bit unsigned int, convert to uint64, push to list
        numReceived = msg.GetString();
        numList.push_front(strtoul(numReceived.c_str(), NULL, 0));

    }

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactor::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PrimeFactor::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!

    //handle new numbers
    while (!numList.empty()) { //while new numbers are received
      uint64_t currentNum;
      currentNum = numList.back(); //get new number
      numList.pop_back(); //delete from list

      //create PrimeEntry instance
      PrimeEntry pe;
      pe.setOriginalVal(currentNum);
      pe.setReceivedIndex(receivedIndex);
      receivedIndex++;
      workList.push_front(pe); //pop new number onto front of work list
    }

    //handle incompleted entries, work on each entry up to max_steps
    list<PrimeEntry>::iterator p;
    for(p=workList.begin(); p!=workList.end(); p++) {
      PrimeEntry &pe = *p;
      bool isdone = pe.factor(max_steps); // apply factor operation up to max_steps
      if (isdone) { //if factorization complete
        pe.setDone(true);
        pe.setCalculatedIndex(calculatedIndex);
        calculatedIndex++;
        Notify("NUM_RESULT",pe.getReport());
        workList.erase(p);
        
      }
    }
    // publish size of the worklist
    Notify("WL_SIZE",workList.size());

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactor::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "foo") {
      handled = true;
    }
    else if(param == "bar") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void PrimeFactor::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
  Register("NUM_VALUE",0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool PrimeFactor::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "File:                                        \n";
  m_msgs << "============================================ \n";

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}


void PrimeEntry::setOriginalVal(unsigned long int v) 
{
  //initialize values, received time
  m_orig = v;
  m_current = v;
  m_start_time = MOOSTime();
};


bool PrimeEntry::factor(unsigned long int max_steps) {
  
  unsigned long int count = 0;
  unsigned long int i=m_start_index; //continue where left off

  while (count<max_steps) { // stop after max_steps
    
    if (m_current % i == 0) { //check if divisible by i
      m_current = m_current/i;
      if (checkPrime(i)) {      //if divisible, check if divisor is prime
        m_factors.push_back(i); //if divisor is prime, put on vector of prime factors
      }

      i = 2;                    //restart i to check from 2 again
    }
    else {                    //if not divisible by i, increment i
      i++;
    }

    // check if finished factoring
    if ((i >= sqrt(m_current)) || (m_current == 1)) {
      m_factors.push_back(m_current);       //put on vector of prime factors
      m_solve_time = MOOSTime() - m_start_time;   //update calc time required
      cout << "done!" << endl;              //output "done!"" to command line
      return true;
    }

    count++; //increment number of iterations run so far on this entry this time around
  }

  m_start_index = m_start_index + max_steps;  //once max steps reached, save current value to pick up next time
  return false;
 }

 bool PrimeEntry::checkPrime(unsigned long int val) {
  //check if factor is prime by seeing if divisible

  int i = 2;
  unsigned long int stopval = sqrt(val);    //check up to square root
  
  if (val == 1)
    return false;

  while ((val > 1) && (i <= stopval)) {  
    if (val % i == 0) {             //if divisible factor found, 
      return false;
    }
    if ((i >= stopval) || (val == 1)) {   //if no factor found
      return true;
    }
    i++;
  }
  return true;
 
 }

string PrimeEntry::getReport() {
  //format report out
  stringstream ss;
  string output; 

  ss << "orig=" << m_orig << ",received=" << m_received_index << ",calculated" << m_calculated_index;
  ss << ",solve_time=" << m_solve_time << ",primes=";

  //go through list of factors
  vector<uint64_t>::iterator p;
  for (p=m_factors.begin(); p!=m_factors.end(); p++) {
      uint64_t &num = *p;
      ss <<  num << ':';
  }

  ss << ",username=clementl";

  output = ss.str();
  return output;
 }



