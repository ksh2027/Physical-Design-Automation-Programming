// Designed by Yu Chen Keng 2022/09/29
#include <iostream>
#include <fstream>
#include <vector>
#include<cstdlib>
using namespace std;

struct tile {
	int x, y;
	struct tile* rt;
	struct tile* tr;
	struct tile* lb;
	struct tile* bl;
	bool type;
	int index;
};

class CornerStitching {
	public:
		CornerStitching();
		tile* point_finding(tile* t, int des_x, int des_y);
		tile* creation(pair<pair<int, int>, pair<int, int> > rct, tile* toptile, tile* bottomtile);
		void area_enum(tile* t);
		//void area_enum2(tile* t);
		int l_bound=0, r_bound, t_bound, b_bound=0;
		int enum_count = 0, enum_total_tile=0;
		tile* now;
		vector<pair<int, pair<int, int> > > output;
		//vector<pair<int, pair<pair<int, int>, pair<int, int> > > > output2;
	private:
		void horizontal_split(tile* t, int y);
		tile* vertical_split(tile* t, int x);
		bool can_merge(tile* t1, tile* t2);
		void vertical_merge(tile* t1, tile* t2);
		tile* plane;
};

CornerStitching::CornerStitching() {
	plane = new tile;
	plane->x = l_bound;
	plane->y = b_bound;
	plane->bl = NULL;
	plane->lb = NULL;
	plane->tr = NULL;
	plane->rt = NULL;
	plane->type = 0;
	now = plane;
}

tile* CornerStitching::point_finding(tile* t, int des_x, int des_y) {
	//cout<<"point_finding"<<endl;
	while(t->x > des_x || (t->tr && t->tr->x <= des_x) || t->y > des_y || (t->rt && t->rt->y <= des_y)) {
		while(t->y > des_y || (t->rt && t->rt->y <= des_y)) {
			if(t->y > des_y)
				t = t->lb;
			else
				t = t->rt;
		}
		while(t->x > des_x || (t->tr && t->tr->x <= des_x)) {
			if(t->x > des_x)
				t = t->bl;
			else
				t = t->tr;
		}
	}
	//cout<<"point_finding end"<<endl;
	return t;
}

void CornerStitching::horizontal_split(tile* t, int y) {
	//total_tile++;
	tile* new_t = new tile;
	new_t->type = 0;
	new_t->x = t->x;
	new_t->y = t->y;
	t->y = y+1;
	new_t->bl = t->bl;
	new_t->lb = t->lb;
	new_t->tr = NULL;
	tile* adj_t = t->tr;
	while(adj_t && adj_t->y >= t->y)
		adj_t = adj_t->lb;
	new_t->tr = adj_t;
	while(adj_t && adj_t->bl == t) {
		adj_t->bl = new_t;
		adj_t = adj_t->lb;
	}
	adj_t = t->bl;
	while(adj_t) {
		if((adj_t->rt && adj_t->rt->y-1 >= t->y) || !adj_t->rt) {
			t->bl = adj_t;
			break;
		}
		adj_t->tr = new_t;
		adj_t = adj_t->rt;
	}

	adj_t = t->lb;
	while(adj_t && adj_t->rt == t) {
		adj_t->rt = new_t;
		adj_t = adj_t->tr;
	}
	new_t->rt = t;
	t->lb = new_t;
}

tile* CornerStitching::vertical_split(tile* t, int x) {
	//total_tile++;
	tile* new_t = new tile;
	new_t->type = 0;
	new_t->x = t->x;
	new_t->y = t->y;
	new_t->bl = t->bl;
	new_t->lb = t->lb;
	t->x = x;
	new_t->rt = NULL;
	tile* adj_t = t->rt;
	while(adj_t && adj_t->x >= x)
		adj_t = adj_t->bl;
	new_t->rt = adj_t;
	while(adj_t && adj_t->lb==t) {
		adj_t->lb = new_t;
		adj_t = adj_t->bl;
	}
	adj_t = t->bl;
	while(adj_t && adj_t->tr==t) {
		adj_t->tr = new_t;
		adj_t = adj_t->rt;
	}
	adj_t = t->lb;
	while(adj_t) {
		if((adj_t->tr && adj_t->tr->x-1 >= x) || !adj_t->tr) {
			t->lb = adj_t;
			break;
		}
		adj_t->rt = new_t;
		adj_t = adj_t->tr;
	}
	new_t->tr = t;
	t->bl = new_t;
	return new_t;
}

bool CornerStitching::can_merge(tile* t1, tile* t2) {
	if(t1->x!=t2->x || t1->type==1 || t2->type==1)
		return 0;
	if((t1->tr && t2->tr && t1->tr->x==t2->tr->x)||(!t1->tr && !t2->tr))
		return 1;
	return 0;
}

void CornerStitching::vertical_merge(tile* t1, tile* t2) {
	//total_tile--;
	tile* adj_t = t2->tr;
	while(adj_t && adj_t->bl==t2) {
		adj_t->bl = t1;
		adj_t = adj_t->lb;
	}
	adj_t = t2->rt;
	while(adj_t && adj_t->lb==t2) {
		adj_t->lb = t1;
		adj_t = adj_t->bl;
	}
	adj_t = t2->bl;
	while(adj_t && adj_t->tr==t2) {
		adj_t->tr = t1;
		adj_t = adj_t->rt;
	}
	t1->rt = t2->rt;
	t1->tr = t2->tr;
	delete t2;
}

tile* CornerStitching::creation(pair<pair<int, int>, pair<int, int> > rct, tile* toptile, tile* bottomtile) {
	tile* start = toptile;
	if((toptile->rt && toptile->rt->y-1 >= rct.second.second) || (!toptile->rt && rct.second.second<t_bound)) {
		//cout<< "toptile_split "<<endl;
		horizontal_split(toptile, rct.second.second-1);
		start = toptile->lb;
		if(toptile==bottomtile)
			bottomtile = toptile->lb;
	}
	if(bottomtile->y < rct.first.second) {
		//cout<< "bottomtile_split "<<endl;
		horizontal_split(bottomtile, rct.first.second-1);
	}
	//cout<< "horizen_split_end"<<endl;
	tile* left_t;
	tile* right_t;
	int end = bottomtile->y-1;
	bool lock = 0;
	tile* pos = start;
	tile* reg = new tile;
	while(pos->y>end) {
		if(pos->x < rct.first.first)
			left_t = vertical_split(pos, rct.first.first);
		else
			left_t = NULL;
		if(left_t && left_t->rt && can_merge(left_t, left_t->rt))
			vertical_merge(left_t, left_t->rt);
		if(left_t && left_t->lb && can_merge(left_t, left_t->lb))
			vertical_merge(left_t->lb, left_t);
			
		if((pos->tr && pos->tr->x-1 >= rct.second.first) || (!pos->tr && rct.second.first<r_bound)) {
			right_t = vertical_split(pos, rct.second.first)->tr;
			pos = right_t->bl;
		} 
		else 
			right_t = NULL;
		if(right_t && right_t->rt && can_merge(right_t, right_t->rt))
			vertical_merge(right_t, right_t->rt);
		if(right_t && right_t->lb && can_merge(right_t, right_t->lb))
			vertical_merge(right_t->lb, right_t);
			
		if(lock)
			vertical_merge(pos, pos->rt);
		else
			lock = 1;
		reg = pos;
		if(pos->y==b_bound)
			break;
		pos = point_finding(pos->lb, rct.first.first, pos->y-1);
	}
	reg->type = 1;
	//cout << "create at "<<reg->x<<" "<<reg->y<<" "<<reg->type<<endl;
	return reg;
}

void CornerStitching::area_enum(tile* t) {
	enum_total_tile++;
	if(t->type) {
		int adj_block=0, adj_space=0;
		tile* adj_t = t->tr;
		while(adj_t) {
			if(adj_t->type)
				adj_block++;
			else
				adj_space++;
			if(adj_t->y <= t->y)
				break;
			adj_t = adj_t->lb;
		}
		adj_t = t->rt;
		while(adj_t) {
			if(adj_t->type)
				adj_block++;
			else
				adj_space++;
			if(adj_t->x <= t->x)
				break;
			adj_t = adj_t->bl;
		}
		adj_t = t->bl;
		while(adj_t) {
			if(adj_t->type)
				adj_block++;
			else
				adj_space++;
			int adj_t_top = (adj_t->rt)? adj_t->rt->y-1:t_bound-1;
			int t_top = (t->rt)? t->rt->y-1:t_bound-1;
			if(adj_t_top >= t_top)
				break;
			adj_t = adj_t->rt;
		}
		adj_t = t->lb;
		while(adj_t) {
			if(adj_t->type)
				adj_block++;
			else
				adj_space++;
			int adj_t_right = (adj_t->tr)? adj_t->tr->x-1:r_bound-1;
			int t_right = (t->tr)? t->tr->x-1:r_bound-1;
			if(adj_t_right >= t_right)
				break;
			adj_t = adj_t->tr;
		}
		output.push_back(make_pair(t->index, make_pair(adj_block, adj_space)));
	}
	if(!t->tr && t->x!=l_bound)
		return;
	tile* adj_t = t->tr;
	while(adj_t && adj_t->bl==t) {
		area_enum(adj_t);
		adj_t = adj_t->lb;
	}
	if(t->x==l_bound && t->y!=b_bound)
		area_enum(point_finding(t, t->x, t->y-1));
	return;
}

//for drawing
/*void CornerStitching::area_enum2(tile* t) {
	enum_total_tile++;
	int index = (t->type)? 1:-1;
	int l=t->x, b=t->y, w=(t->tr)? t->tr->x - t->x:r_bound - t->x, h=(t->rt)? t->rt->y - t->y:t_bound - t->y;
	output2.push_back(make_pair(index, make_pair(make_pair(l, b), make_pair(w, h))));

	if(!t->tr && t->x!=l_bound)
		return;
	tile* adj_t = t->tr;
	while(adj_t && adj_t->bl==t) {
		area_enum2(adj_t);
		adj_t = adj_t->lb;
	}
	if(t->x==l_bound && t->y!=b_bound)
		area_enum2(point_finding(t, t->x, t->y-1));
	return;
}*/ 

