
#include "../Flux/TPartitionRelation.h"
#include <iostream>


int main() {

  TPartitionRelation *partition = TPartitionRelation::CreateRandom(5);
  cout << "Number of partitions " << partition->GetNPartitions() << endl;
  partition->Print(cout);
  delete partition;
  return 0;

}
