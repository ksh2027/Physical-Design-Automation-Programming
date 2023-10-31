// Designed by Yu Chen Keng 2022/11/16
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <list>
using namespace std;
stringstream ss;

struct Cell{
    string name;
    int id;
    int x_coor, y_coor, width, height;
    unordered_map<Cell*, int> neighbors;
    bool chip, chip_best; //0:left 1:right
    int gain=0;
    bool free=1;
};

class FM{
    public:
        FM(){};
        void parser(char const * file_path);
        void initial();
        void build_graph();
        void partition();
        vector<Cell*> cell_list, term_list;
        int chip_w, chip_h, row_h, row_num, term_num, cell_num;
        double overlap_rem=0;
    private:
        double left_area=0, right_area=0;
        int left_num=0, right_num=0;
        long long cutsize=0, cutsize_best=0;
        double ratio, ratio_best=0;
        double balance_ratio=0;
        double init_overlap=0, part_overlap=0;
};

void FM::parser(char const * file_path){
    ifstream fin(file_path, ios::in);
    if (!fin) { cerr << "Cannot open input file" << endl; exit(1); }
	string reg, token;
    vector<string> tokens;
    bool is_cell = 0;
    int count=0;
    while(getline(fin, reg)){
        ss.clear(); ss.str(reg);
        tokens.clear(); tokens.resize(0);
		while(true){
            ss >> token;
            if(ss.fail()) break;
            tokens.push_back(token);
        }
        if(!tokens.size())
            continue;
        if(tokens[0]=="DieSize"){
			chip_w = stoi(tokens[1]);
            chip_h = stoi(tokens[2]);
		} 
		else if(tokens[0]=="DieRows"){
			row_h = stoi(tokens[1]);
            row_num = stoi(tokens[2]);
		}
		else if(tokens[0]=="Terminal"){
			term_num = stoi(tokens[1]);
		}
        else if(tokens[0]=="NumCell"){
            is_cell = 1;
            cell_num = stoi(tokens[1]);
        }
		else{
            Cell* cell = new Cell;
            cell->name = tokens[0];
            cell->x_coor = stoi(tokens[1]);
            cell->y_coor = stoi(tokens[2]);
            cell->width = stoi(tokens[3]);
            cell->height = stoi(tokens[4]);
			if(is_cell){
                cell->id=count++;
                cell_list.push_back(cell);
            }
            else
                term_list.push_back(cell);
            //cout<<cell->name<<" "<<cell->x_coor<<" "<<cell->y_coor<<" "<<cell->width<<" "<<cell->height<<endl;
		}
    }
    sort(cell_list.begin(), cell_list.end(), [](Cell* a, Cell* b){
        return a->x_coor < b->x_coor;
    });
}

void FM::initial(){
    left_num = 0;
    right_num = 0;
    left_area=0; 
    right_area=0;
    ratio_best=0;
    cutsize=0; 
    cutsize_best=0;
    init_overlap=0;
    part_overlap=0;
    overlap_rem=0;
}

void FM::build_graph(){
    for(auto cell:cell_list){
        bool left = rand()%2;
        if(left){
            cell->chip=0;
            cell->chip_best=0;
            left_area+=cell->width*cell->height;
            left_num++;
        }
        else{
            cell->chip=1;
            cell->chip_best=1;
            right_area+=cell->width*cell->height;
            right_num++;
        }
        cell->gain = 0;
        cell->free = 1;
    }
    for(int i=0; i<cell_list.size(); i++){
        for(int j=i+1; j<cell_list.size(); j++){ 
            Cell* c1 = cell_list[i];
            Cell* c2 = cell_list[j];
            if(c2->x_coor > c1->x_coor+c1->width)
                break;
            int del_x, del_y;
            del_x = min(c1->x_coor+c1->width, c2->x_coor+c2->width)-max(c1->x_coor, c2->x_coor);
            del_y = min(c1->y_coor+c1->height, c2->y_coor+c2->height)-max(c1->y_coor, c2->y_coor);
            int overlap = del_x*del_y;
            if(del_x>0 && del_y>0){
                c1->neighbors[c2] = overlap;
                c2->neighbors[c1] = overlap;
                //initialize gain, cutsize
                if(c1->chip == c2->chip){
                    c1->gain += overlap;
                    c2->gain += overlap;
                }
                else{
                    c1->gain -= overlap;
                    c2->gain -= overlap;
                    cutsize += overlap;
                    cutsize_best += overlap;
                }
                init_overlap+=overlap;
            }
        }
    }
}

void FM::partition(){
    //map<int, Cell*, greater<int>> gain_list;
    map<int, list<Cell*>, greater<int>> gain_list;
    for(auto cell:cell_list){
        /*while(!gain_list.insert(pair<int, Cell*>(cell->gain, cell)).second){
            cell->gain++;
        }*/
        gain_list[cell->gain].push_back(cell);
    }
    Cell* move;
    double area;
    bool chip;
    while(gain_list.size()){
        for(auto& g:gain_list){
            /*area = g.second->width*g.second->height;
            chip = g.second->chip;
            //if not balance
            //if((!chip && (left_area-area)/(right_area+area)<balance_ratio) || (chip && (right_area-area)/(left_area+area)<balance_ratio))
            //    continue;
            move = g.second;
            gain_list.erase(move->gain);*/
            move = g.second.back();
            chip = move->chip;
            gain_list[move->gain].pop_back();
            if(gain_list[move->gain].empty())
                gain_list.erase(move->gain);
            break;
        }
        move->free=0;
        for(auto& n:move->neighbors){
            if(!n.first->free)
                continue;
            //gain_list.erase(n.first->gain);
            gain_list[n.first->gain].remove(n.first);
            if(gain_list[n.first->gain].empty())
                gain_list.erase(n.first->gain);
            if(n.first->chip == chip)
                n.first->gain -= n.second*2;
            else
                n.first->gain += n.second*2;
            /*while(!gain_list.insert(pair<int, Cell*>(n.first->gain, n.first)).second){
                n.first->gain++;
            }*/
            gain_list[n.first->gain].push_back(n.first);
        }
        if(!chip){
            move->chip = 1;
            //left_area -= move->width*move->height;
            //right_area += move->width*move->height;
            left_num--;
            right_num++;
        }
        else{
            move->chip = 0;
            //left_area += move->width*move->height;
            //right_area -= move->width*move->height;
            left_num++;
            right_num--;
        }
        cutsize += move->gain;
        ratio = (left_num<=right_num)? left_num/right_num : right_num/left_num;
        //update best partition
        if(cutsize > cutsize_best || (cutsize == cutsize_best && ratio>ratio_best)){
            cutsize_best = cutsize;
            ratio_best = ratio;
            for(auto cell:cell_list)
                cell->chip_best = cell->chip;
        }
        //cout<<move->name<<endl;
    }
    part_overlap = init_overlap-cutsize_best;
    overlap_rem = part_overlap/init_overlap;
}
