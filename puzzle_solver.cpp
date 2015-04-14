#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <string>
#include <queue>
#include <stack>
#include <map>
#include <math.h>
#include <stdlib.h>
using namespace std;
#define maxN 15
#define maxM 15

int row[4] = {-1,0,1,0};
int col[4] = {0,1,0,-1};

struct state
{
    string board;
    int gcost;
    int hcost;
};
struct compare
{
    bool operator() (const state& l,const state& r)
    {
        return (l.gcost+l.hcost)>(r.gcost+r.hcost);
    }
};

int puzzle[maxN][maxM],hand[maxN][maxM],goal[maxN][maxM];
int parent[maxN*maxM*maxN];
int n,m;
priority_queue <state,vector<state>,compare> pq;
map <string,int> bindex;
map <string,int> ::iterator bit;
map <int , pair<int,int> > goalmap;
map <int , pair<int,int> > ::iterator git;
map <int,string> revbindex;
map <int,string> ::iterator revit;
pair <int,int> zero;
vector <int> visited;
map <int,int> hashtonum;   // stores original number for each hashed value

char numtochar(int a)
{
    int offset = a-10;
    return 'a'+offset;
}
int chartonum(char a)
{
    int offset = a - 'a';
    return 10+offset;
}
string convert()
{
    const int sz = n+m+3;
    char cp[sz];
    int k = 0;
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<m; j++)
        {
            if(hand[i][j] > 9)
            {
                cp[k] = numtochar(hand[i][j]);
            }
            else cp[k] = hand[i][j] + '0';
            k++;
        }
    }
    cp[k] = '\0';
    string s = cp;
    return s;
}

void extract(string st)
{
    const int sz = n + m + 3;
    char cp[sz];
    strcpy(cp,st.c_str());
    int k = 0;
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<m; j++)
        {
            if(cp[k]>63) puzzle[i][j] = chartonum(cp[k]);
            else puzzle[i][j] = cp[k] - '0';
            if(puzzle[i][j]==0) zero.first = i, zero.second = j;
            k++;
        }
    }
}
void copy_board()
{
    //printf("copy board\n");
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<m; j++)
        {
            hand[i][j] = puzzle[i][j];
            //printf("%d ",hand[i][j]);
        }
        //printf("%d\n");
    }
}
bool check()
{
    int i,j;
    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++)
        {
            if(puzzle[i][j] != goal[i][j]) return false;
        }
    }
    return true;
}
void swap_blank(int x1,int y1,int x2,int y2)
{
    int temp = hand[x1][y1];
    hand[x1][y1] = hand[x2][y2];
    hand[x2][y2] = temp;
}
int abdistance(int x1,int y1,int x2,int y2)
{
    return abs(x1-x2) + abs(y1-y2);
}
int heuristic()
{
    int sum = 0;
    int i,j,k;
    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++)
        {
            k = hand[i][j];
            pair <int,int> pos;
            pos = goalmap[k];
            sum += abdistance(i,j,pos.first,pos.second);
        }
    }
    return sum;
}

int solve()
{
    copy_board();
    string s = convert();
    state states;
    states.board = s;
    states.gcost = 0;
    states.hcost = heuristic();
    pq.push(states);
    bit = bindex.find(s);
    if(bit == bindex.end()) bindex[s] = bindex.size();
    revbindex[bindex[s]] = s;
    //parent[bindex[s]] = 0;
    visited.push_back(bindex[s]);
    while(pq.empty()==false)
    {
        //printf("1\n");
        states = pq.top();
        pq.pop();
        int gcst = states.gcost;
        int totalcost = gcst + states.hcost;
        string curboard = states.board;
        //cout<<states.board<<endl;
        extract(curboard); // convert string to puzzle board
        if(check())
        {
            printf("Goal reached with cost %d\n",totalcost);
            return bindex[curboard];
        }
        for(int i=0; i<4; i++)
        {
            copy_board();  //copy puzzle array to hand array
            int j = row[i] + zero.first;
            int k = col[i] + zero.second;
            if(j>=0 && j<n && k>=0 && k<m)
            {
                swap_blank(j,k,zero.first,zero.second);
                string ns = convert();
                bit = bindex.find(ns);
                if(bit!=bindex.end())
                {
                    if(visited.size()==bindex[ns]);
                    else
                    {
                        int newgcost = gcst + 1;
                        int newhcost = heuristic();
                        state nstate;
                        nstate.board = ns;
                        nstate.gcost = newgcost;
                        nstate.hcost = newhcost;
                        pq.push(nstate);
                        //visited[bindex[ns]] = true;
                        //parent[bindex[ns]] = bindex[curboard];
                    }
                }
                else
                {
                    bindex[ns] = bindex.size();
                    //cout<<bindex[ns]<<endl;
                    revbindex[bindex[ns]] = ns;
                    int newgcost = gcst + 1;
                    state nstate;
                    nstate.board = ns;
                    nstate.gcost = newgcost;
                    nstate.hcost = heuristic();
                    pq.push(nstate);
                    visited.push_back(bindex[ns]);
                    //parent[bindex[ns]] = bindex[curboard];
                }
            }
        }
        //printf("2\n");
    }
    return -1;
}
void printpuzzle()
{
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<m; j++) printf("%d ",hashtonum[puzzle[i][j]]);
        puts("");
    }
    puts("");
}
void printSolution(int index)
{
    stack <int> order;
    //cout<<index<<endl;
    int par = parent[index];
    order.push(index);
    order.push(par);
    //cout<<par<<endl;
    while(par!=1)
    {
        par = parent[par];
        order.push(par);
        //cout<<par<<endl;
    }
    while(order.empty()==false)
    {
        int nd = order.top();
        order.pop();
        string s = revbindex[nd];
        //cout<<s<<endl;
        extract(s);
        printpuzzle();
    }
}
struct hashhelper
{
    int x,y;
    int val;
};
vector <int> numtohash[maxN*maxM];  // stores the hashed value against each original numbers
void hashconvertpuzzle()
{
    int i,j;
    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++)
        {
            int k = puzzle[i][j];
            if(numtohash[k].size()==1)
            {
                puzzle[i][j] = numtohash[k][0];
            }
            else if(numtohash[k].size()>1)
            {
                int mini = n*m;
                int savex,savey,savval,savindex;
                for(int r=0; r<numtohash[k].size(); r++)
                {
                    int w = numtohash[k][r];
                    pair <int,int> xyofw = goalmap[w];
                    int dist = abdistance(i,j,xyofw.first,xyofw.second);
                    if(dist<mini)
                    {
                        savindex = r;
                        mini = dist;
                        savex = xyofw.first;
                        savey = xyofw.second;
                        savval = w;
                    }
                }
                puzzle[i][j] = savval;
                numtohash[k].erase(numtohash[k].begin()+savindex);
            }
        }
    }
}
void hashconvertgoal()
{
    queue <hashhelper> waiting;
    int i,j;
    bool hashed[maxM*maxN];
    memset(hashed,false,sizeof(hashed));
    int mod = n*m;
    for(i=0; i<mod; i++) numtohash[i].clear();
    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++)
        {
            int k = goal[i][j];
            k = k % mod;
            if(hashed[k]==false)
            {
                hashed[k] = true;
                numtohash[k].push_back(k);
                hashtonum[k] = k;
                goalmap[k] = make_pair(i,j);
            }
            else
            {
                hashhelper hh;
                hh.x = i;
                hh.y = j;
                hh.val = k;
                waiting.push(hh);
            }
        }
    }
    while(waiting.empty()==false)
    {
        hashhelper hh = waiting.front();
        waiting.pop();
        for(int v = hh.val+1; v<mod; v++)
        {
            if(hashed[v]==false)
            {
                hashed[v] = true;
                goal[hh.x][hh.y] = v;
                numtohash[hh.val].push_back(v);
                hashtonum[v] = hh.val;
                goalmap[v] = make_pair(hh.x,hh.y);
                break;
            }
        }
    }
}

int main()
{
    freopen("data.txt","r",stdin);
    scanf("%d%d",&n,&m);
    int i,j;
    /* initializations */
    visited.clear();
    memset(parent,0,sizeof(parent));
    bindex.clear();
    while(pq.empty()==false) pq.pop();
    goalmap.clear();

    /* take input */
    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++) scanf("%d",&puzzle[i][j]);
    }

    for(i=0; i<n; i++)
    {
        for(j=0; j<m; j++)
        {
            scanf("%d",&goal[i][j]);
            //goalmap[goal[i][j]] = make_pair(i,j);
        }
    }
    /* hash convert to support repeats */
    hashconvertgoal();
    hashconvertpuzzle();

    /* start solving */
    int solution_index = solve();
    if(solution_index>0)
    {
        //printSolution(solution_index);
    }
    else printf("Failed\n");

    return 0;
}
