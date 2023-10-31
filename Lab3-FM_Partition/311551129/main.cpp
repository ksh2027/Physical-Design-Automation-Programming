// Designed by Yu Chen Keng 2022/11/16
#include "Legalization.cpp"
#include <float.h>
using namespace std;

int main(int argc, const char* argv[]){
    clock_t a,b; 
	a=clock();
	cout << "---code start---" << endl<<endl;

    if(argc!=3){ cerr <<"Error: the input file is not opened!"<<endl; exit(1); }
    FM fm;
    Legalization lg;
    fm.parser(argv[1]);
    double cost, cost_best=DBL_MAX;
    do{
        fm.initial();
        fm.build_graph();
        fm.partition();
        lg.initial();
        lg.get_data(&fm);
        lg.Build_rowlist();
        lg.Abacus(lg.chip0_list);
        lg.Abacus(lg.chip1_list);
        cost = fm.overlap_rem*lg.total_offset;
        if(cost<cost_best){
            cost_best = cost;
            cout<<cost_best<<endl;
            //legalization output
            sort(lg.cell_list.begin(), lg.cell_list.end(), [](Cell2* a, Cell2* b){
                return a->id < b->id;
            });
            fstream fout;
            fout.open(argv[2],fstream::out);
            if(!fout.is_open()){ cerr << "Error: the output file is not opened!" << endl; exit(1);}
            for(auto cell:lg.cell_list){
                fout<<cell->name<<" "<<cell->x_coor_best<<" "<<cell->y_coor_best<<" "<<cell->chip<<endl;
                //cout<<cell->name<<" "<<cell->x_coor_best<<" "<<cell->y_coor_best<<" "<<cell->chip<<endl;
            }
            fout.close();
        }
        b=clock();
    }while(double(b-a)/CLOCKS_PER_SEC<150);
	

    b=clock();
	cout<<endl<<"Time "<<double(b-a)/CLOCKS_PER_SEC<<endl;
	cout<<endl<<"---code end---"<<endl;
	return 0;
}