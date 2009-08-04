// -*- c++ -*-
#ifndef TTASKH
#define TTASKH
#include <deque>
#include "ooppardefs.h"
#include "oopdataversion.h"
#include "oopobjectid.h"
#include "oopaccesstaglist.h"
#include "pzsave.h"
#include "tpzautopointer.h"
class OOPStorageBuffer;
class OOPAccessTagList;
class OOPTaskManager;
//using namespace std;
//class   OOPMetaData;
/**
 * Implements a abstract Task on the environment.
 * Any task which are going to parallelized on OOPar must be derived from TTask
 * Since tasks are transmitted through the network TTask is also derived from TSaveable
 * and implements Pack() and Unpack() methods.
 */
class   OOPTask : public TPZSaveable
{


public:

  void SubmitDependencyList();
  void IncrementWriteDependentData();

  /**
   * Release access to the pointers, submit write dependent data to the DataManager and
   * notify the DM that the access count of the objects has changed
   */
  void ReleaseDepObjPtr();

  void ClearDependentData();

  bool CanExecute()
  {
    return fDependRequest.CanExecute();
  }
  void GrantAccess(const OOPAccessTag & granted)
  {
    fDependRequest.GrantAccess(granted);
  }

  /**
   * Sets the label property
   */
  void SetLabel(const char * label){
    fLabel = label;
  }
  void SetLabel(string label){
    fLabel = label;
  }
  string GetLabel(){
    return fLabel;
  }

  /**
   * Prints log information on a log file
   * @since 16/08/2003
   * @author Gustavo C Longhin
   */
  void PrintLog(std::ostream & out, char * message = "empty log message");
  void Print (std::ostream & out);
  void PrintDependency(std::ostream & out){
    fDependRequest.Print(out);
  }
  /**
   * This method is called when a task terminates its execution
   * Update information such as data access, ownership etc pending on that task.
   */
  void TaskFinished ();
  /**
   * Constructor based on a processor-id
   * @param Procid Id of processor where the object is being created
   */
  OOPTask();
  OOPTask (int Procid);
  OOPTask (const OOPTask & task);
  virtual ~ OOPTask ();
  /**
   * Returns the Id of current object
   */
  OOPObjectId Id();
  /**
   * Sets the dependency list of the task
   */
  void SetDependencyList(const OOPAccessTagList & deplist)
  {
    fDependRequest = deplist;
  }
  /**
   * Sets the id of current object
   * @param id Id to be set
   */
  void    SetTaskId (OOPObjectId id)
  {
    fTaskId = id;
  }

  /**
   * Set the associated task manager
   */
  void SetTaskManager(TPZAutoPointer<OOPTaskManager> TM);

  /**
   * return the TaskManager
   */
  TPZAutoPointer<OOPTaskManager> TM();

  /**
   * Processor where the task should execute
   */
  int GetProcID ();
  /**
   * Sets the processor Id where the task should be executed
   * @param proc Processor Id
   */
  void SetProcID (int proc)
  {
    fProc = proc;
  }
  /**
   * Changes the task priority
   * @param newpriority New priority to be set
   */
  void ChangePriority (int newpriority);	// execute this task
  /**
   * Returns the priority of the current task
   */
  int Priority ();
  /**
  * Adds a data dependency to the task. It is assumed that
  * write-access implies that this procedure update the object
  * @param depend Dependency which will be appended
  */
  void AddDependentData (const OOPAccessTag & depend);
  /**
  * Execute the task, verifying that all needed data acesses are satisfied.
  */
  virtual OOPMReturnType Execute ();
  /**
  * Submits the task to the TM, TaskId is returned.
  */
  //virtual OOPObjectId Submit ();
  /**
  * Returns last created Id.
  */
  virtual int ClassId () const
  {
    return TTASK_ID;
  }
  /**
  * Virtual Unpack function.
  * Defines the necessary interface for task communication along the network.
  * @param * buf Buffer for data manipulation.
  */
  virtual void Read (TPZStream & buf, void * context);
  virtual void Write (TPZStream & buf, int withclassid);
// Apenas para DEBUG.
//  virtual void Work()  { Debug( "\nTSaveable::Work." ); }
//  virtual void Print() { Debug( "  TSaveable::Print." ); }
  /**
   * Returns the recurrence information
   */
  int IsRecurrent ();
  /**
   * Sets the recurrence parameter
   * @param recurrence Holds the recurrence value
   * @since 12/06/2003
   */
  void    SetRecurrence (bool recurrence = true);
  int NumDepObj()
  {
    return fDependRequest.Count();
  }
  /**
   * Return the pointer to the ith object from which this task depends
   */
  TPZSaveable * GetDepObjPtr(int idepend);

	/**
	 * Return the dependency tag
	 * @param i : The ith dependency tag
	 */
protected:
	OOPAccessTag GetTag(int i);
  /**
  * Processor where the task should be executed
  */
  int     fProc;
  /**
  * Id of current task.
  * The Id is assigned to the task after having been submitted.
  */
  OOPObjectId fTaskId;
  /**
  * List of objects of type MDepend.
  * The current task depends on all entries of the list with an specific access/version state
  */
  OOPAccessTagList fDependRequest;

  /**
   * Holds a brief description of the task purpose
   */
  string fLabel;

  /**
   * The task manager to which this task was submitted
   */
  TPZAutoPointer<OOPTaskManager> fTM;
};


/**
 * Prototype for an instantaneous task
 */

class OOPDaemonTask:public OOPTask
{
public:
  OOPDaemonTask():OOPTask() {}
  OOPDaemonTask (int ProcId);
    OOPDaemonTask (const OOPDaemonTask & task):OOPTask (task)
  {
  }
  long    ExecTime ();	// always returns 0
  int     CanExecute ();	// always returns 1
  virtual int ClassId () const
  {
    return TDAEMONTASK_ID;
  }
  virtual void Write(TPZStream & buf, int withclassid){
          OOPTask::Write(buf, withclassid);
  }
  virtual void Read(TPZStream & buf, void * context){
          OOPTask::Read(buf, context);
  }
  static TPZSaveable *Restore (TPZStream & buf, void * context = 0);
};
#endif
