/* Generated by Together */

#ifndef TLOOPFOR_H
#define TLOOPFOR_H
#include <ooptask.h>
class OOPMReturnType;
class TLoopFor : public OOPTask {
public:    

    /**
    * Returns the estimated execution time.
    * returns 0 if the task is instantaneous
    * returns > 0 if estimate is known
    * return < 0 if no estimate is known
    */
	virtual long ExecTime ();

	/**
	* Execute the task, verifying that all needed data acesses are satisfied.
	*/
	virtual OOPMReturnType Execute ();

	/**
	* Returns last created Id.
	*/
	virtual long GetClassID ()
	{
		return TDISTNORM_ID;
	}
  void SetupVersions();
public:
	void SetId_normb(OOPObjectId & Id){fId_normb=Id;}
	void SetId_normr(OOPObjectId & Id){fId_normr=Id;}
	void SetId_resid(OOPObjectId & Id){fId_resid=Id;}
  void SetId_rho_1(OOPObjectId & Id){fId_rho_1=Id;}
	void SetId_rho_2(OOPObjectId & Id){fId_rho_2=Id;}
	void SetId_alpha(OOPObjectId & Id){fId_alpha=Id;}
	void SetId_beta(OOPObjectId & Id){fId_beta=Id;}
	void SetId_omega(OOPObjectId & Id){fId_omega=Id;}
  void SetId_max_iter(OOPObjectId & Id){fId_max_iter=Id;}
	void SetId_tol(OOPObjectId & Id){fId_tol=Id;}

	void SetlId_A(vector <OOPObjectId> & lId){f_lId_A = lId;}
	void SetlId_M(vector <OOPObjectId> & lId){f_lId_M = lId;}
	void SetlId_x(vector <OOPObjectId> & lId){f_lId_x = lId;}
	void SetlId_b(vector <OOPObjectId> & lId){f_lId_b = lId;}
	void SetlId_p(vector <OOPObjectId> & lId){f_lId_p = lId;}
	void SetlId_rtilde(vector <OOPObjectId> & lId){f_lId_rtilde = lId;}
	void SetlId_s(vector <OOPObjectId> & lId){f_lId_s = lId;}
	void SetlId_shat(vector <OOPObjectId> & lId){f_lId_shat = lId;}
	void SetlId_t(vector <OOPObjectId> & lId){f_lId_t = lId;}
	void SetlId_v(vector <OOPObjectId> & lId){f_lId_v = lId;}
	void SetlId_CMatrix(vector <OOPObjectId> & lId){f_lId_CMatrix = lId;}
	void SetlId_phat(vector <OOPObjectId> & lId){f_lId_phat = lId;}
	void SetlId_r(vector <OOPObjectId> & lId){f_lId_r = lId;}
private:  
	OOPObjectId fId_normb;
	OOPObjectId fId_normr;
	OOPObjectId fId_resid;
	OOPObjectId fId_rho_1;
	OOPObjectId fId_rho_2;
	OOPObjectId fId_alpha;
	OOPObjectId fId_beta;
	OOPObjectId fId_omega;
	OOPObjectId fId_max_iter;
	OOPObjectId fId_tol;

	/**
	 * Local Ids
	 */
	vector <OOPObjectId> f_lId_A;
	vector <OOPObjectId> f_lId_M;
	vector <OOPObjectId> f_lId_x;
	vector <OOPObjectId> f_lId_b;
	vector <OOPObjectId> f_lId_p;
	vector <OOPObjectId> f_lId_rtilde;
	vector <OOPObjectId> f_lId_s;
	vector <OOPObjectId> f_lId_shat;
	vector <OOPObjectId> f_lId_t;
	vector <OOPObjectId> f_lId_v;
	vector <OOPObjectId> f_lId_CMatrix;
	vector <OOPObjectId> f_lId_rtilde;
	vector <OOPObjectId> f_lId_r;

  
};
#endif //TLOOPFOR_H