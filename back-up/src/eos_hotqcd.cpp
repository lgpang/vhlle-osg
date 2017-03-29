#include <iostream>
#include <fstream>
#include "eos.h"
#include "eos_hotqcd.h"
#include "fast.h"

using namespace std ;

Eos_hotqcd::Eos_hotqcd()
{
  double e, p, s, t;
  etab.clear();
  ptab.clear();
  stab.clear();
  ttab.clear();
  ifstream filein("eos/hotqcd.dat");
    while(!filein.eof())
      {
	filein >> e >> p >> s >> t;
	etab.push_back(e);
	ptab.push_back(p);
	stab.push_back(s);
	ttab.push_back(t);
      }
	loge_min = log(etab[0]);
	dloge = log(etab[1]) - log(etab[0]);
	cout << "log(e) min = " << loge_min << ", dlog(e) = " << dloge << endl;
    cout << "hotqcd eos read done" << endl;
}

Eos_hotqcd::~Eos_hotqcd()
{
}


void Eos_hotqcd::eos(double e, double nb, double nq, double ns,
		double &T, double &mub, double &muq, double &mus, double &p)
{
  mub = 0.0;
  muq = 0.0;
  mus = 0.0;
	int index_e = 0;
	if(e>0) index_e = int((log_fast_ankerl(e) - loge_min)/dloge);
  if(index_e < 0)
    {
      T = 0;
      p = 0;
      return;
    }
  if(index_e > etab.size()-2)
    {
      T = ttab[etab.size()-2];
      p = ptab[etab.size()-2];
      return;
    }
  
  double de = (e - etab[index_e])/(etab[index_e+1] - etab[index_e]);

  T = ttab[index_e] * (1. - de) + de * ttab[index_e + 1];
  p = ptab[index_e] * (1. - de) + de * ptab[index_e + 1];
	return;
}


double Eos_hotqcd::p(double e, double nb, double nq, double ns)
{ 
  int index_e = 0;
	if(e>0) index_e = int((log_fast_ankerl(e) - loge_min)/dloge);
  if(index_e < 0)
    {
      return 0.0;
    }
  if(index_e > etab.size()-2)
    {
      return ptab[etab.size()-2];
    }

   double de = (e - etab[index_e])/(etab[index_e+1] - etab[index_e]);

   return ptab[index_e] * (1. - de) + de * ptab[index_e + 1];
}

void Eos_hotqcd::gete(double s, double& e, double nb)
{
  if(s < stab[0])
    {
      e = 0.0;
      return;
    }
  if(s > stab[stab.size()-1])
    {
      e = etab[stab.size()-1];
      return;
    }
  e = 0.0;
  int index_s = 0;
  double ds = 0;
  for ( int is = 0; is < stab.size() - 2; is ++)
    {
      if ( stab[is] <= s && s < stab[is + 1] )
        {
          index_s = is;
          ds = (s - stab[index_s])/(stab[index_s+1] - stab[index_s]);
          break;
        }
    }
  e = etab[index_s] * (1. - ds) + ds * etab[index_s + 1];
}

double Eos_hotqcd::cs2(double e)
{
  double cs2 =  ( p(e*1.02, 0.0, 0.0, 0.0) - p(e*0.98, 0.0, 0.0, 0.0) ) / (0.04*e);
  return cs2;
}



