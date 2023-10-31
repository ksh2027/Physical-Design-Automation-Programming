// Designed by Yu Chen Keng 2022/10/25 
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <unordered_map>
#include <sstream>
#include <math.h>
#include <queue>
#include <algorithm>
#include <climits>
using namespace std;

struct Node {
	string name;
	int x_coor=0, y_coor=0, width, height;
	struct Node* l_child=nullptr;
	struct Node* r_child=nullptr;
	struct Node* parent=nullptr;
};

struct Net {
	int degree;
	vector<string> node_list;
	vector<pair<int, int>> terminal_list;
};

class Floorplanning {
	public:
		Floorplanning(double a);
		void parser(char const * block_file, char const * net_file);
		void initial();
		void BstarT_to_plan();
		void simulated_annealing(int k, double r);
		void set();
		pair<double, double> output_cost();
		vector<Node*> blocks;
		vector<Node*> blocks_past;
		vector<Node*> blocks_best;
		vector<Net*> nets;
		int outputbest=INT_MAX, outputarea, outputwire; 
		int blocks_num, nets_num;
		int outline_w, outline_h;
		int max_w=0, max_h=0;
		double alpha;
		int bound_top_best, bound_right_best;
	private:
		void recc(Node* n);
		vector<int> contour;
		double ratio_cal();
		void perturb();
		unordered_map<string, pair<int, int>> terminals;
		unordered_map<Node*, pair<int, int>> Skeep;
		Node* root;
		Node* head = new Node;
		int bound_top, bound_right;
		double Costbest;
};

Floorplanning::Floorplanning(double a){
	alpha = a;
	return;
}

void Floorplanning::parser(char const * block_file, char const * net_file){
	ifstream fin(block_file, ios::in);
	if (!fin) { cerr << "Cannot open input file" << endl; exit(1); }
	string reg, token;
	stringstream ss;
	while(getline(fin, reg)){
		if(reg.length()==0 || reg.length()==1 || reg==" ")
			continue;
		ss << reg;
		ss >> token;
		//cout<<token<<endl;
		if(token=="Outline:"){
			ss >> token;
			outline_w = stoi(token);
			ss >> token;
			outline_h = stoi(token);
		} 
		else if(token=="NumBlocks:"){
			ss >> token;
			blocks_num = stoi(token);
		}
		else if(token=="NumTerminals:"){
			ss >> token;
		}
		else{
			string name;
			int a, b;
			name = token;
			ss >> token;
			if(token=="terminal"){
				ss >> token;
				a = stoi(token);
				ss >> token;
				b = stoi(token);
				terminals[name] = make_pair(a, b);
			}
			else{
				a = stoi(token);
				ss >> token;
				b = stoi(token);
				Node* n = new Node;
				n->name = name;
				n->width = a;
				n->height = b;
				blocks.push_back(n);
				max_w+=a;
				max_h+=b;
			}
		}
		ss.clear();
		ss.str("");
	}
	fin.close();
	
	ifstream fin2(net_file, ios::in);
	if (!fin2) { cerr << "Cannot open input file" << endl; exit(1); }
	getline(fin2, reg);
	ss << reg;
	ss >> token;
	ss >> token;
	ss.clear();
	nets_num = stoi(token);
	Net* net;
	getline(fin2, reg);
	while(fin2.good()){
		ss << reg;
		ss >> token;
		if(token=="NetDegree:"){
			net = new Net;
			nets.push_back(net);
			//cout<<nets.size()<<endl;
			ss >> token;
			net->degree = stoi(token);
		}
		else{
			int is_terminal=1;
			for(auto block:blocks){
				if(block->name==token){
					net->node_list.push_back(block->name);
					is_terminal = 0;
					break;
				}
			}
			if(is_terminal){
				net->terminal_list.push_back(terminals[token]);
			}
		}	
		ss.clear();
		getline(fin2, reg);
	}
	fin2.close();
	
	//cout<<blocks.size()<<" "<<terminals.size()<<endl; 
	return;
}

void Floorplanning::initial(){
    random_shuffle(blocks.begin(), blocks.end());
	root = blocks[0];
	head->l_child = root;
	root->parent = head;
	root->x_coor=0;
	root->y_coor=0;
	for(int i=0; i<blocks.size(); i++){
		if(i*2+1<blocks.size()){
			blocks[i]->l_child = blocks[i*2+1];
			blocks[i]->l_child->parent = blocks[i];
		}
		else
			blocks[i]->l_child=nullptr;
		if(i*2+2<blocks.size()){
			blocks[i]->r_child = blocks[i*2+2];
			blocks[i]->r_child->parent = blocks[i];
		}
		else
			blocks[i]->r_child=nullptr;
	}
	BstarT_to_plan();
	if(blocks_best.empty()){
		for(auto i:blocks){
			Node* n1 = new Node;
			Node* n2 = new Node;
			*n1 = *i;
			*n2 = *i;
			blocks_past.push_back(n1);
			blocks_best.push_back(n2);
		}
	}
	else{
		for(int i=0; i<blocks.size(); i++)
			*blocks_past[i] = *blocks[i];
	}
}

void Floorplanning::BstarT_to_plan(){
	contour.clear();
	contour.resize(max_w, 0);
	for(int i=0; i<root->width; i++)
		contour[i]=root->height;
	bound_right = root->width;
	bound_top = root->height;
	recc(root);
}

void Floorplanning::recc(Node* n){
	if(n->l_child){
		n->l_child->x_coor = n->x_coor+n->width;
		int y_max=0;
		for(int i=n->l_child->x_coor; i<n->l_child->x_coor+n->l_child->width; i++)
			y_max = max(y_max, contour[i]);
		n->l_child->y_coor = y_max;
		for(int i=n->l_child->x_coor; i<n->l_child->x_coor+n->l_child->width; i++)
			contour[i]=y_max+n->l_child->height;
		bound_right = max(bound_right, n->l_child->x_coor+n->l_child->width);
		bound_top = max(bound_top, n->l_child->y_coor+n->l_child->height);
		recc(n->l_child);
	}
	if(n->r_child){
		n->r_child->x_coor = n->x_coor;
		int y_max=0;
		for(int i=n->x_coor; i<n->x_coor+n->r_child->width; i++)
			y_max = max(y_max, contour[i]);
		n->r_child->y_coor = y_max;
		for(int i=n->x_coor; i<n->x_coor+n->r_child->width; i++)
			contour[i]=y_max+n->r_child->height;
		bound_right = max(bound_right, n->r_child->x_coor+n->r_child->width);
		bound_top = max(bound_top, n->r_child->y_coor+n->r_child->height);
		recc(n->r_child);
	}
	return;
}

void Floorplanning::perturb(){
	int per = rand()%100, cas;
	if(per<25)
		cas=1;
	else if(per>50)
		cas=2;
	else
		cas=3;
	Node* choose_b1;
	Node* choose_b2;
	if(cas==1){
		choose_b1 = blocks[rand()%blocks.size()];
		swap(choose_b1->width, choose_b1->height);
	}
	else if(cas==2){
		Node* move = blocks[rand()%blocks.size()];
		string name_reg = move->name;
		int width_reg = move->width;
		int height_reg = move->height;
		Node* des;
		bool lr;
		while(move->l_child || move->r_child){
			des = move;
			if(move->l_child && move->r_child)
				lr = rand()%2;
			else
				lr = (move->l_child)? 1:0;	
			if(lr)
				move = move->l_child;
			else
				move = move->r_child;
			des->name = move->name;
			des->width = move->width;
			des->height = move->height;
		}
		if(move==move->parent->l_child)
			move->parent->l_child = nullptr;
		else
			move->parent->r_child = nullptr;
		Node* des_p;
		do{
			des_p = blocks[rand()%blocks.size()];
		}while((des_p->l_child && des_p->r_child)||des_p==move);
		if(!des_p->l_child && !des_p->r_child)
			lr = rand()%2;
		else
			lr = (des_p->l_child)? 0:1;
		if(lr)
			des_p->l_child = move;
		else
			des_p->r_child = move;
		move->parent = des_p;
		move->name = name_reg;
		move->width = width_reg;
		move->height = height_reg;	
	}
	else{
		choose_b1 = blocks[rand()%blocks.size()];
		choose_b2 = blocks[rand()%blocks.size()];
		swap(choose_b1->name, choose_b2->name);
		swap(choose_b1->width, choose_b2->width);
		swap(choose_b1->height, choose_b2->height);	
	}
}

double Floorplanning::ratio_cal(){
	double ratio = (double)(bound_top*outline_w) / (double)(bound_right*outline_h);
	if(ratio<1)
		ratio = 1/ratio;
	if(bound_top>outline_h)
		ratio *= 1.1;
	if(bound_right>outline_w)
		ratio *= 1.1;
	if(bound_top<=outline_h && bound_right<=outline_w)
		ratio *= 0.8;
	return ratio;
}

void Floorplanning::simulated_annealing(int k, double r){
	double new_cost;
	Costbest=INT_MAX;
	//initial T
	/*int t=50;
	int total_uphill=0;
	for(int i=0; i<t; i++){
		orig_cost = cost_cal();
		perturb();
		BstarT_to_plan();
		new_cost = cost_cal();
		delta_cost = new_cost-orig_cost;
		if(delta_cost>0)
			total_uphill+=delta_cost;
		for(int i=0; i<blocks.size(); i++)
			*blocks[i] = *blocks_past[i];
	}
	int avg_uphill=total_uphill/t;*/
	//start SA
	//int T=avg_uphill*(-1)/log(0.999);
	double T=4000000000;
	do{
		for(int i=0; i<k; i++){
			//orig_cost = (double)bound_top*(double)bound_right*ratio_cal();
			perturb();
			BstarT_to_plan();
			new_cost = (double)bound_top*(double)bound_right*ratio_cal();
			if(new_cost>Costbest && (double)rand()/RAND_MAX >= exp(new_cost*(-1)/T)){
				for(int i=0; i<blocks.size(); i++)
					*blocks[i] = *blocks_past[i];
			}
			else{
				for(int i=0; i<blocks.size(); i++)
					*blocks_past[i] = *blocks[i];
				Costbest = new_cost;
				pair<double, double> pdd=output_cost();
				double new_outputbest = alpha*pdd.first+(1-alpha)*pdd.second;
				if(new_outputbest<outputbest && bound_top<=outline_h && bound_right<=outline_w){
					for(int i=0; i<blocks.size(); i++)
						*blocks_best[i] = *blocks[i];
					bound_top_best = bound_top;
					bound_right_best = bound_right;
					outputbest = round(new_outputbest);
					outputarea = pdd.first;
					outputwire = pdd.second;
				}
			}
		T *= r;
		}	
	}while(0);
}

pair<double, double> Floorplanning::output_cost(){
	double area = bound_top*bound_right;
	double wire_length=0;
	int t, b, r, l;
	for(auto net:nets){
		t=INT_MIN;
		b=INT_MAX;
		r=INT_MIN;
		l=INT_MAX;
		for(string node_name:net->node_list){
			Node* node;
			for(auto block:blocks){
				if(block->name==node_name){
					node = block;
					break;
				}
			}
			t = max(t, node->y_coor+node->height/2);
			b = min(b, node->y_coor+node->height/2);
			r = max(r, node->x_coor+node->width/2);
			l = min(l, node->x_coor+node->width/2);
		}
		for(auto terminal:net->terminal_list){
			t = max(t, terminal.second);
			b = min(b, terminal.second);
			r = max(r, terminal.first);
			l = min(l, terminal.first);
		}
		int hpwl = t-b+r-l;
		wire_length+=hpwl;
	}
	//cost = alpha*area+(1-alpha)*wire_length;
	return make_pair(area, wire_length);
}

/*
struct cmp {
    bool operator()(pair<int, int> a, pair<int, int> b) {
    	int cmp1=sqrt(pow(a.first, 2)+pow(a.second, 2));
		int cmp2=sqrt(pow(b.first, 2)+pow(b.second, 2));
        return cmp1 > cmp2;
    }
};

void Floorplanning::initial(){
	random_device rd;
    default_random_engine rng(rd());
    shuffle(blocks.begin(), blocks.end(), rng);
	root = blocks[0];
	head->l_child = root;
	root->parent = head;
	root->x_coor=0;
	root->y_coor=0;
	vector<Node*> contour_h, contour_v;
	vector<int> value_h, value_v;
	contour_h.resize(max_w, nullptr);
	value_h.resize(max_w, 0);
	contour_v.resize(max_h, nullptr);
	value_v.resize(max_h, 0);
	for(int i=0; i<root->width; i++){
		contour_h[i]=root;
		value_h[i]=root->height;
	}
	for(int i=0; i<root->height; i++){
		contour_v[i]=root;
		value_v[i]=root->width;
	}
	priority_queue<pair<int, int>, vector<pair<int, int>>, cmp> pq;
	pq.push(make_pair(root->x_coor, root->y_coor+root->height));
	pq.push(make_pair(root->x_coor+root->width, root->y_coor));
	
	pair<int, int> pii;
	for(int k=1; k<blocks.size(); k++){
		Node* now = blocks[k];
		Node* left;
		Node* bottom;
		while(1){
			pii = pq.top();
			pq.pop();
			while(pq.top()==pii)
				pq.pop();
			left = contour_v[pii.second];
			bottom = contour_h[pii.first];	
			if((left && left->x_coor>pii.first)&&(bottom && (bottom->y_coor>pii.second || bottom->x_coor!=pii.first)))
				continue;
			break;
		}
		
		for(int i=pii.second; i<pii.second+now->height; i++){
			if(contour_v[i] &&(value_v[i] > left->x_coor+left->width))
				left = contour_v[i];
		}
		if(left && left->x_coor<=pii.first && !left->l_child){
			if(value_h[left->x_coor+left->width+1]<pii.second){
				k--;
				continue;
			}
			left->l_child = now;
			now->parent = left;
			now->x_coor = left->x_coor+left->width;
			now->y_coor = pii.second;
		}
		else{
			int y_max=value_h[pii.first];
			for(int i=bottom->x_coor; i<bottom->x_coor+now->width; i++){
				if(contour_h[i])
					y_max = max(y_max, value_h[i]);
			}
			if(value_v[y_max]<pii.first){
				k--;
				continue;
			}
			bottom->r_child = now;
			now->parent = bottom;
			now->x_coor = bottom->x_coor;
			now->y_coor = y_max;
		}

		for(int i=now->x_coor; i<now->x_coor+now->width; i++){
			if(now->y_coor+now->height>value_h[i]){
				contour_h[i]=now;
				value_h[i]=now->y_coor+now->height;
			}
		}
		for(int i=now->y_coor; i<now->y_coor+now->height; i++){
			if(now->x_coor+now->width>value_v[i]){
				contour_v[i]=now;
				value_v[i]=now->x_coor+now->width;
			}
		}
		
		int i=now->x_coor;
		if(!now->x_coor){
			pq.push(make_pair(0, now->y_coor+now->height));
			i++;
		}	
		for(; i<now->x_coor+now->width; i++){
			if(value_h[i-1]>value_h[i])
				pq.push(make_pair(i, value_h[i]));
		}
		i=now->y_coor;
		if(!now->y_coor){
			pq.push(make_pair(now->x_coor+now->width, 0));
			i++;
		}	
		for(; i<now->y_coor+now->height; i++){
			if(value_v[i-1]>value_v[i])
				pq.push(make_pair(value_v[i], i));
		}
	}
	BstarT_to_plan();
	for(auto i:blocks){
		Node* n1 = new Node;
		Node* n2 = new Node;
		*n1 = *i;
		*n2 = *i;
		blocks_past.push_back(n1);
		blocks_best.push_back(n2);
	}
	Costbest = cost_cal();
	bound_top_best = bound_top;
	bound_right_best = bound_right;
}*/
