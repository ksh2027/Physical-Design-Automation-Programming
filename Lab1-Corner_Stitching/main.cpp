// Designed by Yu Chen Keng 2022/09/29 
#include "corner_stitching.cpp"
#include <fstream>
#include <ostream>
#include <ctime>
#include <sstream>
#include <algorithm>
using namespace std;

int main(int argc, const char* argv[]) {
	clock_t a,b; 
	a=clock();
	CornerStitching cs;
	cout << "---code start---" << endl<<endl;
	
	if(argc!=3){ cerr <<"Error: the input file is not opened!"<<endl; exit(1); }
	ifstream fin(argv[1], ios::in);
	if (!fin) { cerr << "Cannot open input file" << endl; return 0; }
	
	string reg;
	stringstream ss;
	vector<pair<int, int> >pf_output;
	getline(fin, reg);
	ss << reg;
	ss >> cs.r_bound;
	ss >> cs.t_bound;
	ss.clear();
	//Creation
	while(fin.good()){
		getline(fin, reg);
		if(reg.empty())
			break;
		ss.str("");
		ss << reg;
		ss >> reg;
		if(reg=="P"){
			int des_x, des_y;
			ss >> des_x;
			ss >> des_y;
			tile* t = cs.point_finding(cs.now, des_x, des_y);
			pf_output.push_back(make_pair(t->x, t->y));
		}
		else{
			int l, b, w, h, r, t;
			ss >> l;
			ss >> b;
			ss >> w;
			ss >> h;
			r = l+w;
			t = b+h;
			pair<pair<int, int>, pair<int, int> > rec = make_pair(make_pair(l, b), make_pair(r, t));
			tile* top_tile = cs.point_finding(cs.now, r-1, t-1);
			tile* bottom_tile = cs.point_finding(top_tile, l, b);
			cs.now = cs.creation(rec, top_tile, bottom_tile);
			cs.now->index = stoi(reg);
		}
		ss.clear();
	}
	fin.close();
	
	//Area enumeration
	cs.now = cs.point_finding(cs.now, cs.l_bound, cs.t_bound-1);
	cs.area_enum(cs.now);
	
	//Output
	fstream fout;
    fout.open(argv[2],fstream::out);
    if(!fout.is_open()){ cerr << "Error: the output file is not opened!" << endl; exit(1);}
    fout<<cs.enum_total_tile<<endl;
	cout<<cs.enum_total_tile<<endl;
	sort(cs.output.begin(), cs.output.end());
	for(auto i:cs.output){
		fout<<i.first<<" "<<i.second.first<<" "<<i.second.second<<endl;
		cout<<i.first<<" "<<i.second.first<<" "<<i.second.second<<endl;
	}
	for(auto i:pf_output){
		fout<<i.first<<" "<<i.second<<endl;
		cout<<i.first<<" "<<i.second<<endl;	
	}
	fout.close();
	
	b=clock();
	cout<<endl<<"Time "<<b-a<<endl;
	cout<<endl<<"---code end---"<<endl;
	return 0;
}
