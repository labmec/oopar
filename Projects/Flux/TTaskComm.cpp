/* Generated by Together */
#include "oopdatamanager.h"
#include "TTaskComm.h"
#include "TParCompute.h"
#include "fluxdefs.h"
int TTaskComm::Unpack (OOPReceiveStorage * buf)
{
	OOPTask::Unpack (buf);
	return 0;
}
int TTaskComm::Pack (OOPSendStorage * buf)
{
//	OOPSaveable::Pack (buf);
	OOPTask::Pack (buf);
	return 0;
}
OOPMReturnType TTaskComm::Execute ()
{
	int i;
	for (i = 0; i < fDataDepend.NElements (); i++) {
		if (fDataDepend.Dep (i).State () == EWriteAccess) {
			// dat->IncrementVersion();
			// DM->GetObjPtr(i->fDataId)->IncrementVersion();
			cout << "TTaskComm object id ";
			TaskLog << "TTaskComm object id " << fDataDepend.Dep (i).ObjPtr ()->Id ();
			fDataDepend.Dep (i).ObjPtr ()->Id ().Print (cout);
			cout << "TTaskComm::Execute the previous version is "
				<< endl;
			TaskLog << "TTaskComm::Execute the previous version is "
				<< fDataDepend.Dep (i).ObjPtr ()->Version () << endl;
			fDataDepend.Dep (i).ObjPtr ()->Version ().
				Print (cout);
			OOPDataVersion ver =
				fDataDepend.Dep (i).ObjPtr ()->Version ();
			ver.Increment ();
			fDataDepend.Dep (i).ObjPtr ()->SetVersion (ver,
								   this->
								   Id ());
			cout << "TTaskComm::Execute the new version is " <<
				endl;
			TaskLog << "TTaskComm::Execute the new version is " <<
				fDataDepend.Dep (i).ObjPtr ()->Version () << endl;
			fDataDepend.Dep (i).ObjPtr ()->Version ().
				Print (cout);
		}
	}
	TaskFinished ();
	return ESuccess;	// execute the task, verifying that
}
TTaskComm::TTaskComm (int ProcId):OOPTask (ProcId)
{
}
OOPSaveable *TTaskComm::Restore (OOPReceiveStorage * buf) {
	TTaskComm *loc = new TTaskComm(0);
	loc->Unpack(buf);
	return loc;
}
