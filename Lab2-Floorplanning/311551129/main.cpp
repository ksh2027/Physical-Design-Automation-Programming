// Designed by Yu Chen Keng 2022/10/25 
#include "floorplanning.cpp"
#include <fstream>
#include <iostream>
#include <ctime>
#include <algorithm>
using namespace std;

int main(int argc, const char* argv[]) {
	clock_t a,b; 
	a=clock();
	cout << "---code start---" << endl<<endl;
	
	if(argc!=5){ cerr <<"Error: the input file is not opened!"<<endl; exit(1); }
	double alpha = atof(argv[1]);
	Floorplanning fp(alpha);
	fp.parser(argv[2], argv[3]);
	do{
		fp.initial();
		fp.simulated_annealing(100000, 0.85);
		b=clock();
	}while(b-a<150000);
	cout<<endl;
		
	//Output(draw)
	/*fstream fout;
    fout.open(argv[4],fstream::out);
    if(!fout.is_open()){ cerr << "Error: the output file is not opened!" << endl; exit(1);}
    fout<<fp.blocks_num<<endl;
	cout<<fp.blocks_num<<endl;
	fout<<fp.bound_right_best<<" "<<fp.bound_top_best<<endl;
	cout<<fp.bound_right_best<<" "<<fp.bound_top_best<<endl;
	for(auto i:fp.blocks_best){
		fout<<1<<" "<<i->x_coor<<" "<<i->y_coor<<" "<<i->width<<" "<<i->height<<endl;
		cout<<1<<" "<<i->x_coor<<" "<<i->y_coor<<" "<<i->width<<" "<<i->height<<endl;
	}
	fout.close();
	cout<<fp.outputbest<<endl;*/
	
	//Output
	fstream fout;
	fout.open(argv[4],fstream::out);
    if(!fout.is_open()){ cerr << "Error: the output file is not opened!" << endl; exit(1);}
    fout<<fp.outputbest<<endl;
    cout<<fp.outputbest<<endl;
    fout<<fp.outputwire<<endl;
    cout<<fp.outputwire<<endl;
    fout<<fp.outputarea<<endl;
    cout<<fp.outputarea<<endl;
    fout<<fp.bound_right_best<<" "<<fp.bound_top_best<<endl;
	cout<<fp.bound_right_best<<" "<<fp.bound_top_best<<endl;
	string runtime = to_string(b-a);
	fout<<runtime.substr(0, 3)<<"."<<runtime.substr(3)<<endl;
	cout<<runtime.substr(0, 3)<<"."<<runtime.substr(3)<<endl;
	for(auto i:fp.blocks_best){
		fout<<i->name<<" "<<i->x_coor<<" "<<i->y_coor<<" "<<i->x_coor+i->width<<" "<<i->y_coor+i->height<<endl;
		cout<<i->name<<" "<<i->x_coor<<" "<<i->y_coor<<" "<<i->x_coor+i->width<<" "<<i->y_coor+i->height<<endl;
	}
	
	b=clock();
	cout<<endl<<"Time "<<b-a<<endl;
	cout<<endl<<"---code end---"<<endl;
	return 0;
}
