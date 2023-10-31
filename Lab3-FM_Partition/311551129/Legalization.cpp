// Designed by Yu Chen Keng 2022/11/16
#include "FM_partition.cpp"
#include <limits.h>
using namespace std;

struct Cell2{
    string name;
    int id;
    bool chip;
    int x_coor, y_coor, width, height;
    int x_coor_best, y_coor_best;
    int x_coor_orig, y_coor_orig;
};

struct terminal{
    string name;
    int x_coor, y_coor, width, height;
};

struct Row{
    int x_coor, y_coor, width;
    vector<Cell2*> rowcell_list;
};

struct Cluster{
    vector<Cell2*> clustercell_list;
    int xc, ec=0, wc=0, qc=0;
};

class Legalization{
    public:
    Legalization(){};
    void initial();
    void get_data(FM* fm);
    void Build_rowlist();
    void Abacus(vector<Cell2*>& chip_list);
    vector<Cell2*> chip0_list, chip1_list, cell_list;
    vector<terminal*> term_list;
    int chip_w, chip_h, row_h, row_num;
    double total_offset=0;
    private:
    bool PlaceRow(Row* row);
    void AddCell(Cluster* cluster, Cell2* cell);
    bool Collapse(Row* row, vector<Cluster*>& cluster_list, int p);
    vector<vector<Row*>> row_list;
};

void Legalization::initial(){
    cell_list.clear();
    chip0_list.clear();
    chip1_list.clear();
    term_list.clear();
    row_list.clear();
    total_offset=0;
}

void Legalization::get_data(FM* fm){
    vector<Cell*> cell_list_fm = fm->cell_list;
    vector<Cell*> term_list_fm = fm->term_list;
    for(auto cell_fm:cell_list_fm){
        Cell2* cell = new Cell2;
        cell->name = cell_fm->name;
        cell->id = cell_fm->id;
        cell->chip = cell_fm->chip_best;
        cell->x_coor = cell_fm->x_coor;
        cell->y_coor = cell_fm->y_coor;
        cell->width = cell_fm->width;
        cell->height = cell_fm->height;
        cell->x_coor_best = cell_fm->x_coor;
        cell->y_coor_best = cell_fm->y_coor;
        cell->x_coor_orig = cell_fm->x_coor;
        cell->y_coor_orig = cell_fm->y_coor;
        if(cell_fm->chip_best)
            chip1_list.push_back(cell);
        else
            chip0_list.push_back(cell);
        cell_list.push_back(cell);
    }
    for(auto term_fm:term_list_fm){
        terminal* term = new terminal;
        term->name = term_fm->name;
        term->x_coor = term_fm->x_coor;
        term->y_coor = term_fm->y_coor;
        term->width = term_fm->width;
        term->height = term_fm->height;
        term_list.push_back(term);
    }
    chip_w = fm->chip_w;
    chip_h = fm->chip_h;
    row_h = fm->row_h;
    row_num = fm->row_num;
}

void Legalization::Build_rowlist(){
    row_list.resize(row_num);
    for(int i=0; i<row_num; i++){
        Row* row = new Row;
        row->x_coor = 0;
        row->y_coor = i*row_h;
        row->width = chip_w;
        row_list[i].push_back(row);
    }
    for(auto term:term_list){
        int slip_t = (term->y_coor+term->height)/row_h;
        int slip_b = term->y_coor/row_h;
        int term_l = term->x_coor;
        int term_r = term->x_coor+term->width;
        for(int i=slip_b; i<=slip_t; i++){
            for(int j=0; j<row_list[i].size(); j++){
                int row_l = row_list[i][j]->x_coor;
                int row_r = row_list[i][j]->x_coor+row_list[i][j]->width;
                if(row_r<=term_l || row_l>=term_r)
                    continue;
                else if(row_l>=term_l && row_r<=term_r){
                    vector<Row*>::iterator it = row_list[i].begin()+j;
                    row_list[i].erase(it);
                }
                else if(row_l<term_l && row_r>term_r){
                    Row* row = new Row;
                    row->x_coor = term_r;
                    row->y_coor = row_list[i][j]->y_coor;
                    row->width = row_r-term_r;
                    row_list[i].push_back(row);
                    row_list[i][j]->width = term_l-row_l;     
                }
                else if(row_l<term_l && row_r>term_l)
                    row_list[i][j]->width = term_l-row_l;
                else if(row_r>term_r && row_l<term_r){
                    row_list[i][j]->x_coor = term_r;
                    row_list[i][j]->width = row_r-term_r;
                }
            }
        }
    }
}

void Legalization::Abacus(vector<Cell2*>& chip_list){
    /*sort(chip_list.begin(), chip_list.end(), [](Cell2* a, Cell2* b){
        return a->x_coor < b->x_coor;
    });*/
    for(auto cell:chip_list){
        int Cbest = INT_MAX, Ccost;
        Row* Rbest;
        int startrow = cell->y_coor_orig/row_h;
        for(int i=startrow; i<row_list.size(); i++){
            if(abs(row_list[i][0]->y_coor-cell->y_coor_orig)>Cbest)
                break;
            for(auto row:row_list[i]){
                row->rowcell_list.push_back(cell);
                if(!PlaceRow(row)){
                    row->rowcell_list.pop_back();
                    continue;
                }                    
                Ccost = abs(cell->x_coor-cell->x_coor_orig)+abs(cell->y_coor-cell->y_coor_orig);
                if(Ccost<Cbest){
                    Cbest = Ccost;
                    Rbest = row;
                }
                row->rowcell_list.pop_back();
            }
        }
        for(int i=startrow; i>=0; i--){
            if(abs(row_list[i][0]->y_coor-cell->y_coor_orig)>Cbest)
                break;
            for(auto row:row_list[i]){
                row->rowcell_list.push_back(cell);
                if(!PlaceRow(row)){
                    row->rowcell_list.pop_back();
                    continue;
                }                    
                Ccost = abs(cell->x_coor-cell->x_coor_orig)+abs(cell->y_coor-cell->y_coor_orig);
                if(Ccost<Cbest){
                    Cbest = Ccost;
                    Rbest = row;
                }
                row->rowcell_list.pop_back();
            }
        }
        Rbest->rowcell_list.push_back(cell);
        PlaceRow(Rbest);
        cell->y_coor_best = Rbest->y_coor;
        for(auto cell_modify:Rbest->rowcell_list)
            cell_modify->x_coor_best = cell_modify->x_coor;
    }

    for(auto cell:chip_list){
        total_offset += (abs(cell->x_coor_best-cell->x_coor_orig)+abs(cell->y_coor_best-cell->y_coor_orig));
    }
    //clear rowcell_list
    for(auto rowi:row_list)
        for(auto row:rowi)
            row->rowcell_list.clear();
}

bool Legalization::PlaceRow(Row* row){
    vector<Cluster*> cluster_list;
    for(auto cell:row->rowcell_list){
        Cluster* cluster_last;
        if(!cluster_list.empty())
            cluster_last = cluster_list.back();
        if(cell==row->rowcell_list[0] || cluster_last->xc+cluster_last->wc<=cell->x_coor_orig){
            Cluster* cluster = new Cluster;
            cluster_list.push_back(cluster);
            cluster->xc = cell->x_coor_orig;
            AddCell(cluster, cell);
        }
        else{
            AddCell(cluster_last, cell);
            //cout<<cell->name<<" "<<row->x_coor<<" "<<row->y_coor<<endl;
        }
        if(!Collapse(row, cluster_list, cluster_list.size()-1))
            return 0;
    }
    for(auto cluster:cluster_list){
        Cell2* cell_pre = cluster->clustercell_list[0];
        cell_pre->x_coor = cluster->xc;
        cell_pre->y_coor = row->y_coor;
        for(int i=1; i<cluster->clustercell_list.size(); i++){
            Cell2* cell = cluster->clustercell_list[i];
            cell->x_coor = cell_pre->x_coor+cell_pre->width;
            cell->y_coor = row->y_coor;
            cell_pre = cell;
        }
    }
    return 1;
}

void Legalization::AddCell(Cluster* cluster, Cell2* cell){
    cluster->clustercell_list.push_back(cell);
    cluster->ec ++;
    cluster->qc += (cell->x_coor_orig-cluster->wc);
    cluster->wc += cell->width;
}

bool Legalization::Collapse(Row* row, vector<Cluster*>& cluster_list, int p){
    Cluster* cluster = cluster_list[p];
    cluster->xc = cluster->qc/cluster->ec;
    int row_l = row->x_coor, row_r = row->x_coor+row->width;
    if(cluster->xc < row_l)
        cluster->xc = row_l;
    if(cluster->xc > row_r-cluster->wc)
        cluster->xc = row_r-cluster->wc;
    //can't insert this row
    if(cluster->xc < row_l || cluster->xc > row_r-cluster->wc)
        return 0;
    if(p==0)
        return 1;
    Cluster* cluster_pre = cluster_list[p-1];
    if(cluster_pre->xc+cluster_pre->wc > cluster->xc){
        cluster_pre->clustercell_list.insert(cluster_pre->clustercell_list.end(), cluster->clustercell_list.begin(), cluster->clustercell_list.end());
        cluster_pre->ec += cluster->ec;
        cluster_pre->qc += cluster->qc - cluster->ec*cluster_pre->wc;
        cluster_pre->wc += cluster->wc;
        vector<Cluster*>::iterator it = cluster_list.begin()+p;
        cluster_list.erase(it);
        if(!Collapse(row, cluster_list, p-1))
            return 0;
    }
    return 1;
}