
#include "ooptaskcontrol.h"
#include "ooptask.h"

OOPTaskControl::OOPTaskControl(OOPTask *task): fTask(task) {
  if(task) {
    fDepend = task->GetDependencyList();
    fDepend.ClearPointers();
  }
}

OOPTaskControl::~OOPTaskControl() {

  delete fTask;

}
