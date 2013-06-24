int row[100],col[100],diag1[100],diag2[100];
int N = 14;
int ans;
void try(int c)
{
if(c==N) ans++;
else{
int r;
for(r=0;r<N;r++){
if(!row[r] && !diag1[r+c] && !diag2[r+N-1-c]){
row[r]=diag1[r+c]=diag2[r+N-1-c]=1;
col[c]=r;
try(c+1);
row[r]=diag1[r+c]=diag2[r+N-1-c]=0;
}
}
}
}
main()
{
try(0);
printf("%d\n",ans);
}