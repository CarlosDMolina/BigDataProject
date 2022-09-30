#include <iostream>
using namespace std;
#include "foundation.h"
#include "csv.h"
#include "trace.h"
#include <vector>
class vec
{
public:
    uint32 i; // J value
    uint32 count; //Number of elements in current vector
    real64 variance;
    real64 mean;
    real64 M2;
    real64 S;

    vec()
    {
        i=0;
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
void read_CSV_file(CSVfile F, uint32 offset,bool view_data )
{

  if (view_data) {
      tm_stdout.print("BLOCK-BASED\n");
      F.view();
      F.print_header();
  }
    F.seek_block(offset);
    F.read_block();
    F.view_block();


}
struct mean_variance{
    uint64 count=0;
    real64 variance=0;
    real64 mean=0;


};
typedef struct mean_variance mean_variance;
int32 i_available(uint32 L,vector<vec> X)
{
    for(int j=0;j<X.size();j++)
        if(X[j].i==L)
            return j;


    return -1;
}
vector<vec> calculate_vector_mean(const char* path,uint32 offset)
{
    CSVfile F(path);
    vector<vec> answers;
    //  F.view();
    //F.print_header();
    uint32 start_offset=offset;
    uint64 count=0;
    real64 mean=0;
    real64 Mean=0;
    real64 M2=0;
    real64 variance=0;
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
            int32 zz=i_available(stoi(b),answers);
            if(zz==-1)
            {
                vec T;
                T.i= stoi(b);
                T.update_mean_variance(stod(v));
                answers.push_back(T);
                //answers.push_back()
            } else
            {
                answers[zz].update_mean_variance(stod(v));

            }

        }
        offset++;

    }
    return answers;
}


mean_variance calculate_mean(const char* path,uint32 offset)
{
    CSVfile F(path);
  //  F.view();
    //F.print_header();
    uint32 start_offset=offset;
    uint64 count=0;
    real64 mean=0;
    real64 Mean=0;
    real64 M2=0;
    real64 variance=0;
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
            count++;
            real64 delta = stod(v) - Mean;
            Mean += delta / (count);
            M2 += delta * (stod(v) - Mean);
            variance = M2 / (count-1);


            mean = mean + ((stod(v) - mean) / count);
        }
offset++;

    }
    mean_variance  answer;
    answer.count=count;
    answer.mean=mean;
    answer.variance=variance;
    return answer;

}

int main(int argc, char *argv[]) {

   // read_CSV_file(F, stoi(argv[2]), false);
   string G="open_mode=read;file_name=";
   G+=argv[1];
   G+=";";
vector<vec> ANS= calculate_vector_mean(G.c_str(),1);
for( int32 IT=0 ; IT<ANS.size();IT++  )
{
    printf("j=%d, elements Count= %d, Mean:%f, Variance:%f\n",ANS[IT].i,ANS[IT].count,ANS[IT].mean,ANS[IT].variance);


}
    return 0;
}
