#include <iostream>
using namespace std;
#include "foundation.h"
#include "csv.h"
#include <vector>
class vec
{
public:
    uint32 j=0; // J value
    uint32 count=0; //Number of elements in current vector
    real64 variance=0;
    real64 mean=0;
    real64 M2=0;
    real64 S=0;
    bool padding=true;
    vec()
    {
        padding = true;
        j=0;
        count=0;
        variance=0;
        mean=0;
        M2=0;
        S=0;

    }
    void update_mean_variance(real64 v)
    {


        count++;
        real64 old_M=M2;
        M2+=(v-M2)/count;
        S=S+(v-M2)*(v-old_M);
        if(count>1)
        variance=S/(count-1);
        real64 delta = v - mean;
        mean += delta / (count);
    }


};
struct mean_variance{
    uint64 count=0;
    real64 variance=0;
    real64 mean=0;


};
typedef struct mean_variance mean_variance;
int32 i_available(uint32 L,vec* X,uint32 sz)
{
    for(uint32 j=1;j<sz;j++)
        if(X[j].j==L && !X[j].padding)
            return j;


    return -1;
}
int calculate_vector_mean(vec* &ans,const char* path,uint32 offset)
{
    CSVfile F(path);
    uint32 sz=3;
    ans = new vec[sz];
    uint32 curr_pointer=1;
    uint32 start_offset=offset;
    F.seek_block(start_offset);
    while (!F.eof())
    {

        F.read_block();
        uint32 i =0;
        string t;
        string a,b,v;
        uint32 number=1;

        while((int)F.block_row[1].s[i]!=10&&F.block_row[1].s[i]!=0)
        {
            if((int)F.block_row[1].s[i]!=32&& (int)F.block_row[1].s[i]!=44)
                t+=F.block_row[1].s[i];
            else if ((int)F.block_row[1].s[i]==44)
            {
                if(number ==1)
                {
                    a=t;
                    number++;
                    t="";

                }
                else if(number ==2)
                {
                    b=t;
                    number++;
                    t="";
                }
            }
            i++;
        }
        v=t;
        if(v.length()>0) {
            int32 zz=i_available(stoi(b),ans,curr_pointer);
            if(zz==-1)
            {
                vec T;
                T.j= stoi(b);
                T.update_mean_variance(stod(v));
                T.padding=false;
                if((sz-1) == curr_pointer)
                {
                    ans[curr_pointer]=T;
                    vec* t= new vec[sz*2];
                    copy(ans,ans+sz,t);
                    delete[] ans;
                    ans=t;
                    sz*=2;
                    curr_pointer++;
                }
                else
                {
                    ans[curr_pointer]=T;
                    curr_pointer+=1;


                }
            } else
            {
                ans[zz].update_mean_variance(stod(v));

            }

        }
        offset++;

    }
    return curr_pointer;
}



int main(int argc, char *argv[]) {

   // read_CSV_file(F, stoi(argv[2]), false);
   string G="open_mode=read;file_name=";
   G+=argv[1];
   G+=";";
   vec* ANS;
 uint32 j_count = calculate_vector_mean(ANS,G.c_str(),1);
    FILE* ptr = fopen(argv[2],"w");
 string head="           i  uint32,           c  uint32,           m real64,          v real64\n";
for( int32 IT=1 ; IT<j_count;IT++  )
{
    //string GX= to_string(ANS[IT].variance);

    printf("j=%d, elements Count= %d, Mean:%f, Variance:%f\n",ANS[IT].j,ANS[IT].count,ANS[IT].mean,ANS[IT].variance);
    string s1((20- to_string(ANS[IT].j).length()),' ');
    s1+=to_string(ANS[IT].j);
    string s2((20- to_string(ANS[IT].count).length()),' ');
    s2+=to_string(ANS[IT].count);
    string s3((20- to_string(ANS[IT].mean).length()),' ');
    s3+=to_string(ANS[IT].mean);
    string s4((20- to_string(ANS[IT].variance).length()),' ');
    s4+=to_string(ANS[IT].variance);
    string final = s1+","+s2+","+s3+","+s4+"\n";
    fputs(final.c_str(),ptr);
}




delete[] ANS;
    fclose(ptr);
    return 0;
}
