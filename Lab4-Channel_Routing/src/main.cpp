// Designed by nmsl 2022/12/20
#include "gcr.cpp"
#include "io.hpp"
#include <fstream>
#include <ostream>
#include <ctime>
#include <iostream>
using namespace std;

int main(int argc, const char* argv[]) {
	clock_t a,b; 
	a=clock();
	//GCR g(12, 1, 0);
	GCR g(20, 1, 0);
	
	
	cout << "---code start---" << endl;
	//ifstream inFile("Deutsch difficult.txt", ios::in);
	ifstream inFile(argv[1], ios::in);
	if (!inFile) { cerr << "Cannot open input file" << endl; return 0; }
	inFile >> g;
	inFile.close();
	
	g.initialize();
	for(int i=-1; i<2; i++){
		cout<<"round "<<i+1<<endl;
		for(g.now=0; g.now<g.column_num; g.now++){
			int situ = g.step_A();	//3:t,b failed 2:t failed 1:b failed 0:no failed
			if(situ!=-1){
				g.step_B();
				g.step_C();
				if(i>=0 && g.now==i)
					g.split();
				g.step_D();
				g.step_E(situ);
			}
			g.step_F();
		}
		//g.remov();
		g.printer();
		g.get_output();
		if(!g.resault())
			break;
		g.reset();
	}

	//rerout
	/*g.initialize_r2l();
	for(g.now=g.column_num-1; g.now>=0; g.now--){
		if(g.now)
			g.set_track();
		int situ = g.step_A();
		if(situ!=-1){
			g.step_B();
			g.step_C();
			g.step_D();
		}
		if(g.now)
			g.step_F_2();
	}
	g.printer();*/
	//g.get_output();
	
	fstream fout;
	fout.open(argv[2],fstream::out);
	if(!fout.is_open()){ cerr << "Error: the output file is not opened!" << endl; exit(1);}
	for(int i=1; i<static_cast<int>(g.out.size()); i++){
		auto net = g.out[i];
		if(net.empty())
			continue;
		//cout<<".begin "<<i<<endl;
		fout<<".begin "<<i<<endl;
		for(auto line:net){
			if(line.first){
				//cout<<".V ";
				fout<<".V ";
			}
			else{
				//cout<<".H ";
				fout<<".H ";
			}
			tuple<int, int, int> coor = line.second;
			//cout<<get<0>(coor)<<" "<<get<1>(coor)<<" "<<get<2>(coor)<<endl;
			fout<<get<0>(coor)<<" "<<get<1>(coor)<<" "<<get<2>(coor)<<endl;
		}
		//cout<<".end"<<endl;
		fout<<".end"<<endl;
	}
	fout.close();
	
	if(argc == 4)
		io::drawNets(argv[1], argv[2], argv[3]);
	b=clock();
	cout<<endl<<"Time "<<double(b-a)/CLOCKS_PER_SEC<<endl;
	cout<<endl<<"---code end---"<<endl;
	return 0;
}


