#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

extern int solveMatrix(double *A_in, int n, double *b_in, double *x_out);

using namespace std;
int main(int argc, char *argv[]){
  ifstream mtx(argv[1]);
  ifstream vec(argv[2]);
  vector<double> A;
  vector<double> b;
  string line;
  int n=0;
  double token;
  while(getline(mtx,line)){
    stringstream input;
    input.str(line);
    int count = 0;
    while(input>>token){
      A.push_back(token);
      count++;
    }
    if(n==0) n=count;
  }
  while(vec>>token){
    b.push_back(token);
  }
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      cout<<A[4*i+j]<<"\t";
    }
    cout<<endl;
  }
  cout<<endl;
  for(int i=0;i<n;i++){
    cout<<b[i]<<endl;
  }
  cout<<endl;
  
  solveMatrix(&A[0], n, &b[0], &b[0]);
  for(int i=0;i<n;i++){
    cout<<b[i]<<endl;
  }

  return 0;
}
