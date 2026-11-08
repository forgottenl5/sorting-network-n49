// Exact 0-1 verifier for sorting networks with up to 64 wires.
// Idea: take the longest initial run of comparators whose wire-groups (connected components) each have <= MAXC wires.
// For each group, enumerate its outputs over ALL 2^size inputs (so the set of reachable group states is exact).
// The reachable states of the whole prefix are exactly the PRODUCT of the groups' reachable sets (groups are independent).
// Then run the remaining comparators bit-sliced (64 vectors per word) over the whole product and check every output is sorted.
// This is an exact check of all 2^n 0-1 inputs (0-1 principle), not a sample.
// usage: xverify file [MAXC=16] [limit_product=4e10]     file: first line n, then "a b" per comparator (in network order)
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <numeric>
using namespace std;
typedef uint64_t u64;
int n; vector<pair<int,int>> C;
int par[64]; int fnd(int x){ return par[x]==x?x:par[x]=fnd(par[x]); }
int main(int argc,char**argv){
  FILE*f=fopen(argv[1],"r"); if(!f){puts("cannot open");return 2;}
  int MAXC=argc>2?atoi(argv[2]):16; double LIM=argc>3?atof(argv[3]):4e10;
  if(fscanf(f,"%d",&n)!=1) return 2; int a,b; while(fscanf(f,"%d %d",&a,&b)==2){ if(a>b) swap(a,b); if(a<0||b>=n||a==b){puts("bad comparator");return 2;} C.push_back({a,b}); }
  // longest prefix keeping all components <= MAXC
  iota(par,par+64,0); vector<int> sz(64,1); size_t P=0;
  for(;P<C.size();P++){ int x=fnd(C[P].first), y=fnd(C[P].second); if(x!=y){ if(sz[x]+sz[y]>MAXC) break; par[x]=y; sz[y]+=sz[x]; } }
  // groups
  vector<vector<int>> groups; { vector<int> gid(n,-1); for(int w=0;w<n;w++){ int r=fnd(w); if(gid[r]<0){gid[r]=groups.size(); groups.push_back({});} groups[gid[r]].push_back(w);} }
  int G=groups.size(); vector<vector<u64>> states(G); // each state: bitmask over the group's wires (bit k = wire groups[g][k])
  double prod=1;
  for(int g=0;g<G;g++){
    int m=groups[g].size(); vector<int> idx(n,-1); for(int k=0;k<m;k++) idx[groups[g][k]]=k;
    vector<char> seen(1u<<m,0);
    for(u64 x=0;x<(1ull<<m);x++){ u64 v=x; for(size_t p=0;p<P;p++){ int i=idx[C[p].first], j=idx[C[p].second]; if(i<0||j<0) continue; u64 bi=(v>>i)&1,bj=(v>>j)&1; if(bi>bj){ v^=(1ull<<i)|(1ull<<j);} } seen[v]=1; }
    for(u64 v=0;v<(1ull<<m);v++) if(seen[v]) states[g].push_back(v);
    prod*=states[g].size();
  }
  printf("n=%d comparators=%zu prefix=%zu groups=%d product of reachable states=%.4g\n",n,C.size(),P,G,prod);
  if(prod>LIM){ puts("product too large, refusing (raise limit or lower MAXC)"); return 3; }
  u64 total=(u64)prod; vector<int> digit(G,0); u64 done=0; u64 bad=0; u64 firstbad=0; bool haveBad=false;
  vector<u64> W(n);
  while(done<total){
    u64 lanes=min<u64>(64,total-done);
    for(int w=0;w<n;w++) W[w]=0;
    for(u64 l=0;l<lanes;l++){
      for(int g=0;g<G;g++){ u64 s=states[g][digit[g]]; for(size_t k=0;k<groups[g].size();k++) if((s>>k)&1) W[groups[g][k]]|=1ull<<l; }
      // increment mixed-radix counter
      for(int g=0;g<G;g++){ if(++digit[g]<(int)states[g].size()) break; digit[g]=0; }
    }
    for(size_t p=P;p<C.size();p++){ u64 x=W[C[p].first], y=W[C[p].second]; W[C[p].first]=x&y; W[C[p].second]=x|y; }
    u64 mask = lanes==64?~0ull:((1ull<<lanes)-1), viol=0;
    for(int w=0;w+1<n;w++) viol|=W[w]&~W[w+1];
    viol&=mask; if(viol){ if(!haveBad){haveBad=true;firstbad=done;} bad+=__builtin_popcountll(viol); }
    done+=lanes;
  }
  printf("checked %llu reachable prefix states (covers all 2^%d inputs): %s (%llu failing)\n",(unsigned long long)total,n,bad?"NOT A SORTER":"SORTS ALL INPUTS",(unsigned long long)bad);
  return bad?1:0;
}
