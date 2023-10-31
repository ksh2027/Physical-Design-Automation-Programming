// Designed by Yu Chen Keng 2022/12/20
#include <iostream>
#include <vector>
#include <cstdlib>
#include <set>
#include <tuple>
#include <queue>
using namespace std;

class GCR {
		friend istream& operator>>(istream&, GCR&);
	public:
		GCR(int a, int b, int c);
		void initialize();
		void reset();
		int step_A();
		void step_B();
		void step_C();
		void step_D();
		void step_E(int situ);
		void step_F();
		void printer();
		int resault();
		void get_output();
		void remov();
		//void initialize_r2l();
		//void set_track();
		//void step_F_2();
		void split();
		vector<int> column_t, column_b;
		vector<vector<pair<bool, tuple<int, int, int>>>> out;
		int net_num=0, column_num=0;
		int now;
		
	private:
		void add_track(int y);
		int ICW, MJL, SNC;
		int via=0, wire=0;
		vector<char> t_state, b_state, net_now_state;
		vector<int> t_next, b_next, net_now_next, end_col;
		vector<set<int> > Y_n;
		vector<bool> track, finished, ended;
		vector<vector<int> > grid_h, grid_v, grid_d;
};

GCR::GCR(int a, int b, int c){
	ICW = a;
	MJL = b;
	SNC = c;
} 

void GCR::initialize(){
	t_state.assign(column_num, 's');
	b_state.assign(column_num, 's');
	t_next.assign(column_num, 0);
	b_next.assign(column_num, 0);
	end_col.resize(net_num+1);
	vector<pair<int, bool> > arr(net_num+1, make_pair(-1, 0));
	vector<bool> steady(net_num+1, 0);
	//state setting
	for(int i=column_num-1; i>=0; i--){
		int x = column_t[i], y = column_b[i];
		if(x==0)
			t_state[i] = ' ';
		else if(arr[x].first==-1){
			arr[x] = make_pair(i, 1);
			t_state[i] = 'e';
			end_col[x] = i;
		}
		else{
			if(steady[x] == 1)
				t_state[i] = 's';
			else
				t_state[i] = (arr[x].second == 1)? 'r':'f';
			steady[x] = (arr[x].first-i <= SNC && arr[x].second==0)? 1:0;
			t_next[i] = arr[x].first;
			arr[x] = make_pair(i, 1);
		}
		if(y==0)
			b_state[i] = ' ';
		else if(y==x){
			b_state[i] = t_state[i];
			steady[y] = 1;
		}
		else if(arr[y].first==-1){
			arr[y] = make_pair(i, 0);
			b_state[i] = 'e';
			end_col[y] = i;
		}
		else{
			if(steady[y] == 1)
				b_state[i] = 's';
			else
				b_state[i] = (arr[y].second == 0)? 'f':'r';
			steady[y] = (arr[y].first-i <= SNC && arr[y].second==1)? 1:0;
			b_next[i] = arr[y].first;
			arr[y] = make_pair(i, 0);
		}
	}
	//
	Y_n.resize(net_num+1);
	track.assign(ICW+1, 0);
	finished.assign(net_num+1, 0);
	ended.assign(net_num+1, 0);
	net_now_state.resize(net_num+1);
	net_now_next.resize(net_num+1);
	grid_h.assign(ICW+1, vector<int>(column_num, 0));
	grid_v.assign(ICW+1, vector<int>(column_num, 0));
	grid_d.assign(ICW+1, vector<int>(column_num, 0));
	out.resize(net_num+1);
	/*
	track[ICW/2] = 1;
	Y_n[1].insert(ICW/2);
	if(steady[1] == 1)
		net_now_state[1] = 's';
	else
		net_now_state[1] = (arr[1].second == 1)? 'r':'f';
	t_next[1] = arr[1].first;
	*/
}

void GCR::reset(){
	Y_n.clear();
	Y_n.resize(net_num+1);
	track.assign(ICW+1, 0);
	finished.assign(net_num+1, 0);
	ended.assign(net_num+1, 0);
	net_now_state.clear();
	net_now_state.resize(net_num+1);
	net_now_next.clear();
	net_now_next.resize(net_num+1);
	grid_h.assign(ICW+1, vector<int>(column_num, 0));
	grid_v.assign(ICW+1, vector<int>(column_num, 0));
	grid_d.assign(ICW+1, vector<int>(column_num, 0));
	out.clear();
	out.resize(net_num+1);
}

int GCR::step_A(){
	int x = column_t[now], y = column_b[now], w = track.size();
	int empty_t, empty_b, temp_t=-1, temp_b=-1;
	//for r2l
	if(ended[x])
		x=0;
	if(ended[y])
		y=0;
	//
	if(x==0 && y==0)
		return -1;
	if(t_state[now]=='e')
		finished[x] = 1;
	if(b_state[now]=='e')
		finished[y] = 1;
	net_now_state[x] = t_state[now];
	net_now_state[y] = b_state[now];
	net_now_next[x] = t_next[now];
	net_now_next[y] = b_next[now];
	
	for(empty_b=1; empty_b<w; empty_b++)
		if(track[empty_b]==0)
			break;
	for(empty_t=w-1; empty_t>0; empty_t--)
		if(track[empty_t]==0)
			break;
	if(x==0);
	else if(empty_t!=0){
		if(Y_n[x].empty())
			temp_t = empty_t;
		else
			temp_t = (empty_t > *Y_n[x].rbegin())? empty_t:*Y_n[x].rbegin();
	}
	else if(!Y_n[x].empty())
		temp_t = *Y_n[x].rbegin();
	if(y==0);
	else if(empty_b!=w){
		if(Y_n[y].empty())
			temp_b = empty_b;
		else
			temp_b = (empty_b < *Y_n[y].begin())? empty_b:*Y_n[y].begin();
	}
	else if(!Y_n[y].empty())
		temp_b = *Y_n[y].begin();
	
	if(x==y && t_state[now]=='e'){
		for(int i=0; i<w; i++)
			grid_v[i][now] = x;
		for(auto it=Y_n[x].begin(); it!=Y_n[x].end(); it++){
			track[*it] = 0;
			grid_d[*it][now] = x;
		}
		return 0;
	}
	if(temp_t==-1 && temp_b==-1){
		if(x==0)
			return 1;
		else if(y==0)
			return 2;
		return 3;
	}
	else if(temp_t==-1){
		if(finished[y] && temp_b==*Y_n[y].begin()){
			track[temp_b] = 0;
			Y_n[y].clear();
		}
		else{
			track[temp_b] = 1;
			Y_n[y].insert(temp_b);
		}
		for(int i=0; i<temp_b; i++)
			grid_v[i][now] = y;
		grid_d[temp_b][now] = y;
		if(x!=0)
			return 2;
		else
			return 0;
	}	
	else if(temp_b==-1){
		if(finished[x] && temp_t==*Y_n[x].begin()){
			track[temp_t] = 0;
			Y_n[x].clear();
		}
		else{
			track[temp_t] = 1;
			Y_n[x].insert(temp_t);	
		}
		for(int i=w-1; i>=temp_t; i--)
			grid_v[i][now] = x;
		grid_d[temp_t][now] = x;
		if(y!=0)
			return 1;
		else
			return 0;
	}
	
	if(temp_t>temp_b){
		if(finished[x] && temp_t==*Y_n[x].begin()){
			track[temp_t] = 0;
			Y_n[x].clear();
		}
		else{
			track[temp_t] = 1;
			Y_n[x].insert(temp_t);	
		}
		for(int i=w-1; i>=temp_t; i--)
			grid_v[i][now] = x;
		grid_d[temp_t][now] = x;
		if(finished[y] && temp_b==*Y_n[y].begin()){
			track[temp_b] = 0;
			Y_n[y].clear();
		}
		else{
			track[temp_b] = 1;
			Y_n[y].insert(temp_b);
		}
		for(int i=0; i<temp_b; i++)
			grid_v[i][now] = y;
		grid_d[temp_b][now] = y;
	}
	else if(temp_t<=temp_b && x!=y){
		if(w-temp_t < temp_b){
			if(finished[x] && temp_t==*Y_n[x].begin()){
				track[temp_t] = 0;
				Y_n[x].clear();
			}
			else{
				track[temp_t] = 1;
				Y_n[x].insert(temp_t);	
			}
			for(int i=w-1; i>=temp_t; i--)
				grid_v[i][now] = x;
			grid_d[temp_t][now] = x;
			return 1;
		}
		else{
			if(finished[y] && temp_b==*Y_n[y].begin()){
				track[temp_b] = 0;
				Y_n[y].clear();
			}
			else{
				track[temp_b] = 1;
				Y_n[y].insert(temp_b);
			}
			for(int i=0; i<temp_b; i++)
				grid_v[i][now] = y;
			grid_d[temp_b][now] = y;
			return 2;
		}
	}
	else{
		if(finished[x] && temp_t==*Y_n[x].begin()){
			track[temp_t] = 0;
			Y_n[x].clear();
		}
		else{
			track[temp_t] = 1;
			Y_n[x].insert(temp_t);	
		}
		for(int i=0; i<w; i++)
			grid_v[i][now] = x;
		grid_d[temp_t][now] = x;
	}
	
	return 0;
}

struct cmp{
    bool operator()(tuple<int, int, int, int> a, tuple<int, int, int, int> b) {
        if(get<0>(a)!=get<0>(b))
        	return get<0>(a) < get<0>(b);
        else if(get<2>(a)!=get<2>(b))
        	return get<2>(a) > get<2>(b);
        return get<1>(a) > get<1>(b);
    }
};

void GCR::step_B(){
	int w = track.size(); 
	//float mid = (w+1)/2;
	vector<set<int> > subY(net_num+1), track_use(w), Y_rem(net_num+1);
	set<int> candidate;
	priority_queue <tuple<int, int, int, int> , vector<tuple<int, int, int, int> >, cmp> pq;
	subY.clear();
	Y_rem.clear();
	for(int i=1; i<net_num+1; i++){
		//cout<<"第"<<i<<endl;
		if(Y_n[i].size()<2)
			continue;
		subY[i]=Y_n[i];
		for(auto it=subY[i].begin(); subY[i].size()>=2;){
			if(grid_v[*it][now]!=0){
				subY[i].erase(it);
				Y_rem[i].insert(*it);
				continue;	
			}
			break;
		}
		//cout<<"k1"<<endl;
		for(auto it=subY[i].rbegin(); subY[i].size()>=2;){
			if(grid_v[*it-1][now]!=0){
				subY[i].erase(next(it).base());
				Y_rem[i].insert(*it);
				continue;	
			}
			break;
		}
		//cout<<"k2"<<endl;
		if(subY[i].size()<2)
			continue;
		//cout<<"candidate: "<<i<<endl;
		int a = (finished[i] && Y_rem[i].empty())? subY[i].size():subY[i].size()-1;
		int b = *subY[i].rbegin()-*subY[i].begin();
		int c = (w-*subY[i].rbegin() <= *subY[i].begin())? w-*subY[i].rbegin():*subY[i].begin();
		pq.push(make_tuple(a, b, c, i));
		candidate.insert(i);
		for(int j=*subY[i].begin(); j<*subY[i].rbegin(); j++)
			track_use[j].insert(i);	
	}
	//cout<<"k3"<<endl;
	while(!pq.empty()){
		int net = get<3>(pq.top());
		//cout<<net<<"減少"<<get<0>(pq.top())<<endl;
		pq.pop();
		if(!candidate.count(net))
			continue;
		for(int j=*subY[net].begin(); j<*subY[net].rbegin(); j++)
			grid_v[j][now]=net;
		for(auto it=subY[net].begin(); it!=subY[net].end(); it++){
			if(grid_h[*it][now-1]!=0)
				grid_d[*it][now]=net;
			else
				grid_d[*it][now]=0;
			track[*it]=0;
		}
		Y_n[net]=Y_rem[net];
		if(finished[net] && Y_n[net].empty());
		else{
			int stay = (w-*subY[net].rbegin() <= *subY[net].begin())? *subY[net].begin():*subY[net].rbegin();
			Y_n[net].insert(stay);
			grid_d[stay][now]=net;
			track[stay]=1;
		}
		for(int i=1; i<w; i++){
			if(track_use[i].count(net)){
				for(auto it=track_use[i].begin(); it!=track_use[i].end(); it++)
					candidate.erase(*it);
			}
		}
	}
	//cout<<"Step_B done"<<endl;
}

struct cmp2{
    bool operator()(tuple<int, int, int, int> a, tuple<int, int, int, int> b) {
        if(get<0>(a)!=get<0>(b))
        	return get<0>(a) < get<0>(b);
        else if(get<1>(a)!=get<1>(b))
        	return get<1>(a) > get<1>(b);
        return get<2>(a) > get<2>(b);
    }
};

void GCR::step_C(){
	int w = track.size(); 
	vector<set<int> > track_use(w);
	//priority_queue <pair<int, int>> pq;
	priority_queue <tuple<int, int, int, int>, vector<tuple<int, int, int, int> >, cmp2> pq;
	set<int> candidate;
	vector<pair<int, int> > reg(net_num+1);
	for(int i=1; i<net_num+1; i++){
		if(Y_n[i].size()<2)
			continue;
		int orig_t=*Y_n[i].rbegin(), orig_b=*Y_n[i].begin(), des_t=orig_t, des_b=orig_b, move_b=orig_b, move_t=orig_t;
		while((grid_v[move_b][now]==0 || grid_v[move_b][now]==i) && move_b<w-1){
			move_b++;
			if(!track[move_b] && !grid_d[move_b][now])
				des_b = move_b;
		}
		while((grid_v[move_t-1][now]==0 || grid_v[move_t-1][now]==i) && move_t>1){
			move_t--;
			if(!track[move_t] && !grid_d[move_t][now])
				des_t = move_t;
		}
		int b;
		if(orig_t-des_t >= des_b-orig_b){
			reg[i].first = orig_t;
			reg[i].second = des_t;
			b = w-orig_t;
		}
		else{
			reg[i].first = orig_b;
			reg[i].second = des_b;
			b = orig_b;
		}
		if(abs(reg[i].second-reg[i].first) >= MJL){
			int a = abs(reg[i].second-reg[i].first);
			//int c = end_col[i];
			int c = net_now_next[i];
			pq.push(make_tuple(a, b, c, i));
			candidate.insert(i);
			int up = (reg[i].second>reg[i].first)? reg[i].second:reg[i].first, down = (reg[i].second>reg[i].first)? reg[i].first:reg[i].second;
			for(int j=down; j<=up; j++)
				track_use[j].insert(i);
		}
	}
	while(!pq.empty()){
		int net = get<3>(pq.top());
		pq.pop();
		if(!candidate.count(net))
			continue;
		int s = reg[net].first, d = reg[net].second;
		int up = (d>s)? d:s, down = (d>s)? s:d;
		for(int j=down; j<up; j++)
			grid_v[j][now]=net;
		if(grid_h[s][now-1]!=0 && now!=0)
			grid_d[s][now]=net;
		else
			grid_d[s][now]=0;
		track[s]=0;
		grid_d[d][now]=net;
		track[d]=1;
		Y_n[net].erase(s);
		Y_n[net].insert(d);
		for(int i=1; i<w; i++){
			if(track_use[i].count(net)){
				for(auto it=track_use[i].begin(); it!=track_use[i].end(); it++)
					candidate.erase(*it);
			}
		}
	}
}

void GCR::step_D(){
	int w = track.size(); 
	vector<set<int> > track_use(w);
	//priority_queue <pair<int, int>> pq;
	priority_queue <tuple<int, int, int, int>, vector<tuple<int, int, int, int> >, cmp2> pq;
	set<int> candidate;
	vector<pair<int, int> > reg(net_num+1);
	for(int i=1; i<net_num+1; i++){
		if(Y_n[i].size()!=1)
			continue;
		int orig=*Y_n[i].begin(), des=orig, move=orig;
		//cout<<"net"<<i<<endl;
		if(net_now_state[i]=='r'){
			while((grid_v[move][now]==0 || grid_v[move][now]==i) && move<w-1){
				move++;
				if(!track[move] && (grid_d[move][now]==0 || grid_d[move][now]==i))
					des = move;
			}
		}
		else if(net_now_state[i]=='f'){
			while((grid_v[move-1][now]==0 || grid_v[move-1][now]==i) && move>1){
				move--;
				if(!track[move] && (grid_d[move][now]==0 || grid_d[move][now]==i)){
					des = move;
					//cout<<"des"<<des<<endl;
				}
				//cout<<"move"<<move<<endl;
			}
		}
		reg[i].first = orig;
		reg[i].second = des;
		if(abs(reg[i].second-reg[i].first) >= MJL){
			int a = abs(reg[i].second-reg[i].first);
			int b = min(orig, w-orig);
			//int c = end_col[i];
			int c = net_now_next[i];
			pq.push(make_tuple(a, b, c, i));
			candidate.insert(i);
			int up = (orig>des)? orig:des, down = (orig>des)? des:orig;
			for(int j=down; j<=up; j++)
				track_use[j].insert(i);
		}
	}
	while(!pq.empty()){
		int net = get<3>(pq.top());
		pq.pop();
		if(!candidate.count(net))
			continue;
		//cout<<"net"<<net<<endl;
		int s = reg[net].first, d = reg[net].second;
		int up = (d>s)? d:s, down = (d>s)? s:d;
		for(int j=down; j<up; j++)
			grid_v[j][now]=net;
		if(grid_h[s][now-1]!=0 && now!=0)
			grid_d[s][now]=net;
		else
			grid_d[s][now]=0;
		track[s]=0;
		grid_d[d][now]=net;
		track[d]=1;
		Y_n[net].erase(s);
		Y_n[net].insert(d);
		for(int i=1; i<w; i++){
			if(track_use[i].count(net)){
				for(auto it=track_use[i].begin(); it!=track_use[i].end(); it++)
					candidate.erase(*it);
			}
		}
	}	
} 

void GCR::add_track(int new_track){
	track.push_back(1);
	int w = track.size();
	for(int i=w-1; i>=new_track; i--)
		track[i] = track[i-1];
	track[new_track] = 1;
	for(int i=track.size()-1; i>=new_track; i--){
		for(int j=1; j<net_num+1; j++){
			if(Y_n[j].count(i)){
				Y_n[j].insert(i+1);
				Y_n[j].erase(i); 
			}
		}
	}
	cout<<"new_track"<<new_track<<endl;
	cout<<"new_track_size"<<track.size()<<endl;
	grid_v.resize(w);
	grid_h.resize(w);
	grid_d.resize(w);			
	for(int i=w-1; i>=new_track; i--)
		grid_v[i] = grid_v[i-1];
	for(int i=w-1; i>new_track; i--)
		grid_h[i] = grid_h[i-1];
	grid_h[new_track].assign(column_num, 0);
	for(int i=w-1; i>new_track; i--)
		grid_d[i] = grid_d[i-1];
	grid_d[new_track].assign(column_num, 0);	
}

void GCR::step_E(int situ){
	int x = column_t[now], y = column_b[now], w = track.size();
	int new_track;
	if(situ==3){
		new_track=w-1;
		while(grid_v[new_track-1][now]==0 && new_track>w/2)
			new_track--;
		add_track(++new_track);
		w = track.size();
		for(int i=w-1; i>=new_track; i--)
			grid_v[i][now] = x;
		grid_d[new_track][now] = x;
		Y_n[x].insert(new_track);
		if(x==y){
			for(int i=0; i<new_track; i++)
				grid_v[i][now] = x;
			return;
		}
		
		new_track=0;
		while(grid_v[new_track+1
		][now]==0 && new_track<(w-1)/2)
			new_track++;
		//if(new_track==1)
		//	new_track--;
		add_track(++new_track);
		for(int i=0; i<new_track; i++)
			grid_v[i][now] = y;
		grid_d[new_track][now] = y;
		Y_n[y].insert(new_track);
	}
	else if(situ==2){
		new_track=w-1;
		while(grid_v[new_track-1][now]==0 && new_track>w/2)
			new_track--;
		add_track(++new_track);
		w = track.size();
		for(int i=w-1; i>=new_track; i--)
			grid_v[i][now] = x;
		grid_d[new_track][now] = x;
		Y_n[x].insert(new_track);
	}
	else if(situ==1){
		new_track=0;
		while(grid_v[new_track+1][now]==0 && new_track<(w-1)/2)
			new_track++;
		//if(new_track==1)
		//	new_track--;
		add_track(++new_track);
		for(int i=0; i<new_track; i++)
			grid_v[i][now] = y;
		grid_d[new_track][now] = y;
		Y_n[y].insert(new_track);
	}
	return;
}

void GCR::step_F(){
	for(int i=1; i<static_cast<int>(track.size()); i++){
		if(track[i]==1){
			if(grid_d[i][now])
				grid_h[i][now] = grid_d[i][now];
			else
				grid_h[i][now] = grid_h[i][now-1];
		}
	}
}

void GCR::remov(){
	set<int> non_f;
	ended.assign(net_num+1, 1);
	for(int i=1; i<static_cast<int>(track.size()); i++){
		if(grid_h[i][now-1]){
			non_f.insert(grid_h[i][now-1]);
			ended[grid_h[i][now-1]] = 0;
		}
	}
	for(auto k:non_f)
		cout<<"not end: "<<k<<endl;
	for(int i=0; i<static_cast<int>(grid_d.size()); i++){
		for(int j=0; j<static_cast<int>(grid_d[0].size()); j++)
			for(auto k:non_f){
				if(grid_d[i][j]==k)
					grid_d[i][j]=0;
				if(grid_v[i][j]==k)
					grid_v[i][j]=0;
				if(grid_h[i][j]==k)
					grid_h[i][j]=0;
			}
	}
}

void GCR::split(){
	int x = column_t[now], y = column_b[now], w = track.size();
	int t_top=*Y_n[x].begin()-1, t_bot=*Y_n[y].begin();
	if(x){
		for(; t_top>=w/2; t_top--){
			/*if(now && grid_h[t_top-1][now-1])
				break;*/
			grid_v[t_top][now] = x;
		}
		t_top++;
		grid_d[t_top][now] = x;
		track[t_top] = 1;
		Y_n[x].insert(t_top);
	}
	if(y){
		for(; t_bot<w/2-1; t_bot++){
			/*if(now && grid_h[t_bot][now-1])
				break;*/
			grid_v[t_bot][now] = y;
		}
		grid_d[t_bot][now] = y;
		track[t_bot] = 1;
		Y_n[y].insert(t_bot);
	}
}

/*void GCR::initialize_r2l(){
	t_state.assign(column_num, 's');
	b_state.assign(column_num, 's');
	t_next.assign(column_num, 0);
	b_next.assign(column_num, 0);
	vector<pair<int, bool> > arr(net_num+1, make_pair(-1, 0));
	vector<bool> steady(net_num+1, 0);
	for(int i=0; i<column_num; i++){
		int x = column_t[i], y = column_b[i];
		if(x==0)
			t_state[i] = ' ';
		else if(arr[x].first==-1){
			arr[x] = make_pair(i, 1);
			t_state[i] = 'e';
			end_col[x] = i;
		}
		else{
			if(steady[x] == 1)
				t_state[i] = 's';
			else
				t_state[i] = (arr[x].second == 1)? 'r':'f';
			steady[x] = (i-arr[x].first <= SNC && arr[x].second==0)? 1:0;
			t_next[i] = arr[x].first;
			arr[x] = make_pair(i, 1);
		}
		if(y==0)
			b_state[i] = ' ';
		else if(y==x){
			b_state[i] = t_state[i];
			steady[y] = 1;
		}
		else if(arr[y].first==-1){
			arr[y] = make_pair(i, 0);
			b_state[i] = 'e';
			end_col[y] = i;
		}
		else{
			if(steady[y] == 1)
				b_state[i] = 's';
			else
				b_state[i] = (arr[y].second == 0)? 'f':'r';
			steady[y] = (i-arr[y].first <= SNC && arr[y].second==1)? 1:0;
			b_next[i] = arr[y].first;
			arr[y] = make_pair(i, 0);
		}
	}
	Y_n.clear();
	Y_n.resize(net_num+1);
	finished.assign(net_num+1, 0);
}

void GCR::set_track(){
	for(int i=1; i<static_cast<int>(track.size()); i++){
		track[i] = grid_h[i][now-1];
		if(grid_h[i][now] && !ended[grid_h[i][now]])
			track[i] = 1;
	}
}

void GCR::step_F_2(){
	for(int i=1; i<static_cast<int>(track.size()); i++){
		if(track[i]==1){
			if(grid_d[i][now])
				grid_h[i][now-1] = grid_d[i][now];
			else
				grid_h[i][now-1] = grid_h[i][now];
		}
	}
}*/

void GCR::printer(){
	int sta =0 , fin = column_num;
	//int sta = 39 , fin = 79;
	//int sta = column_num-38 , fin = column_num;
	for(int i=sta; i<fin; i++)
		cout<<t_state[i]<<"   ";
	cout<<endl;
	for(int i=sta; i<fin; i++){
		cout<<column_t[i];
		if(column_t[i]>9)
			cout<<"  ";
		else
			cout<<"   ";
	}
	cout<<endl;
	cout<<"■ "; 
	for(int i=sta+1; i<fin; i++)
		cout<<"— ■ ";
	cout<<endl;
	
	for(int i=grid_v.size()-1; i>=0; i--){
		for(int j=sta; j<fin; j++){
			if(grid_v[i][j]){
				cout<<"｜  ";
				wire++;
			}
			else
				cout<<"    ";
		}
		cout<<endl;
		if(i==0)
			break;
		for(int j=sta; j<fin; j++){
			if(grid_d[i][j]){
				cout<<"■ ";
				via++;
			}
			else if(grid_h[i][j]!=0 && grid_v[i][j]!=0){
				cout<<"＋";
				wire+=2;
			}
			else if(grid_h[i][j]!=0){
				cout<<"— ";
				wire++;
			}
			else if(grid_v[i][j]!=0){
				cout<<"｜";
				wire++;
			}
			else
				cout<<"  ";
			if(grid_h[i][j]){
				cout<<"— ";
				wire++;
			}
			else
				cout<<"  ";
		}
		cout<<endl;
	}
	
	cout<<"■ "; 
	for(int i=sta+1; i<fin; i++)
		cout<<"— ■ ";
	cout<<endl;
	for(int i=sta; i<fin; i++){
		cout<<column_b[i];
		if(column_b[i]>9)
			cout<<"  ";
		else
			cout<<"   ";
	}
	cout<<endl;
	for(int i=sta; i<fin; i++)
		cout<<b_state[i]<<"   ";
	cout<<endl;
}

int GCR::resault(){
	cout<<"channel_width: "<<track.size()-1<<endl;
	cout<<"via_used: "<<via<<endl;
	cout<<"wire_length: "<<wire<<endl;
	int non_finished=0;
	for(int i=1; i<static_cast<int>(track.size()); i++)
		if(track[i])
			non_finished++;
	cout<<"non_finished: "<<non_finished<<endl;
	return non_finished;
} 

istream& operator>>(istream& is, GCR& g) {
	vector<int> arr;
	string reg;
	while(is.good()){
		is >> reg;
		arr.push_back(atoi(reg.c_str()));
		g.net_num = max(g.net_num, arr.back());
	}
	g.column_num = arr.size()/2;
	g.column_t.assign(arr.begin(), arr.begin()+g.column_num);
	g.column_b.assign(arr.begin()+g.column_num, arr.end());
	cout<< "column_num: "<<g.column_num<<endl;
	cout<< "net_num: "<<g.net_num<<endl;
	return is;
}

void GCR::get_output(){
	for(int i=0; i<static_cast<int>(grid_v[0].size()); i++){
		int net = grid_v[0][i];
		int coory = 0;
		for(int j=1; j<static_cast<int>(grid_v.size()); j++){
			if(grid_v[j][i]!=net || j==static_cast<int>(grid_v.size())-1){
				if(net){
					out[net].push_back(make_pair(1, make_tuple(i, coory, j)));
				}
				net = grid_v[j][i];
				coory = j;
			}
		}
		if(net){
			out[net].push_back(make_pair(1, make_tuple(i, coory, grid_v.size())));
		}
	}
	for(int i=1; i<static_cast<int>(grid_h.size()); i++){
		int net = grid_h[i][0];
		int coorx = 0;
		for(int j=0; j<static_cast<int>(grid_h[0].size()); j++){
			if(grid_h[i][j]!=net || j==static_cast<int>(grid_h[0].size())-1){
				if(net){
					out[net].push_back(make_pair(0, make_tuple(coorx, i, j)));
				}
				net = grid_h[i][j];
				coorx = j;
			}
		}
	}
}