/* Generated by Together */

#include "TParAnalysis.h"
void TParAnalysis::ComputeFlux(){
        // message #1.1 to pc:TParCompute
        TParCompute * pc = new TParCompute(Procid);

        // message #1.2 to rhs:TParRhs
        TParRhs * rhs = new TParRhs(ProcId, Trace);

        // message #1.3 to state:TParState
        TParState * state = new TParState(ProcId, Trace);

        // message #1.4 to mesh:TParMesh
        TParMesh * mesh = new TParMesh(ProcId, Trace);

        // message #1.5 to ver:OOPDataVersion
        OOPDataVersion * ver = new OOPDataVersion();

        // message #1.6 to ver:OOPDataVersion
        ver->SetLevelVersion(level, version);

        // message #1.7 to DM:OOPDataManager
        OOPDataManager * DM = null;
        OOPObjectId rhsId = DM->SubmitObject(rhs, 1);

        // message #1.8 to pc:TParCompute
        pc->SetRhsId(rhsId);

        // message #1.9 to DM:OOPDataManager
        OOPObjectId stateId = DM->SubmitObject(state);

        // message #1.10 to pc:TParCompute
        pc->SetStateId(stateId);

        // message #1.11 to DM:OOPDataManager
        OOPObjectId meshId = DM->SubmitObject(mesh);

        // message #1.12 to pc:TParCompute
        pc->SetMeshId(meshId);

        // message #1.13 to pc:TParCompute
        pc->AddDependentData(meshId, st, version);

        // message #1.14 to pc:TParCompute
        pc->AddDependentData(stateId, st, version);

        // message #1.15 to pc:TParCompute
        pc->AddDependentData(rhsId, st, version);

        // message #1.16 to pc:TParCompute
        OOPObjectId oOPObjectId = pc->Submit();
    }