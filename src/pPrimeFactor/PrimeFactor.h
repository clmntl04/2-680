/************************************************************/
/*    NAME: Clement Li                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef PrimeFactor_HEADER
#define PrimeFactor_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <list>
#include <cstdint>
#include <cstdlib>
#include <vector>



class PrimeEntry
	{
public:

   PrimeEntry()		{  m_start_index = 2; m_done = false;};
  ~PrimeEntry() 	{};

   void setOriginalVal(unsigned long int v);
   void setReceivedIndex(unsigned int v)    {m_received_index=v;};
   void setCalculatedIndex(unsigned int v)  {m_calculated_index=v;};
   void setDone(bool v)                     {m_done=v;};

   bool   done() {return(m_done);};

   bool   factor(unsigned long int max_steps);
   bool   checkPrime(unsigned long int val);

   std::string  getReport();

 protected:   
   uint64_t      m_start_index;
   uint64_t      m_orig;
   uint64_t      m_current;
   bool          m_done;
   unsigned int  m_received_index;
   unsigned int  m_calculated_index;
   double 		 m_start_time;
   double	 	 m_solve_time;


   std::vector<uint64_t> m_factors;

 };



class PrimeFactor : public AppCastingMOOSApp
{
 public:
   PrimeFactor();
   ~PrimeFactor();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables

 private: // State variables
 	std::string numReceived;
 	std::list<uint64_t> numList;
 	std::list<PrimeEntry> workList;
 	uint64_t receivedIndex;
 	uint64_t calculatedIndex;
 	unsigned long int max_steps;
 	std::vector<uint64_t> del_list;
};

#endif 
