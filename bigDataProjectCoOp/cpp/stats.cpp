#include <iostream>
#include <iomanip>
#include "foundation.h"
#include "csv.h"
#include "trace.h"

using namespace std;
class vec
{
public:
    uint32 j=0; // J value
    uint32 count=0; //Number of elements in current vector
    real64 variance=0;
    real64 mean=0;
    real64 sum=0;
    bool init =false;
    bool padding = true;
    real64 M2=0;
    real64 S=0;
    ///////////////////////////////////test mode vars

    vec()
    {
        j=0;
        count=0;
        variance=0;
        mean=0;
        sum=0;
        init= false;

        ///////////////////////////////////test mode vars
        padding =true;
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

void Compute_Mean_Stddev(string file_path,string num_rows, string outfile_path);
void PrintOutput(vec* ans, string output_filename,uint32 sz);
uint32 i_available(uint32 L,vec* X,uint32 sz);
void Help();
uint32 calculate_vector_mean(vec* &ans,const char* path,uint32 offset);
int32 j_available(uint32 L,vec* X,uint32 sz);
int main(int argc, char *argv[])
{
    string outFile;
    string blk_sz;
    vec* ANS;
    uint32 j_count;
    if (argc ==2)
    {
        blk_sz="2";
        outFile = "output.csv";
    }
    else if (argc == 3)
    {
        blk_sz=argv[2];
        outFile = "output.csv";
    }
    else if (argc == 4)
    {
        blk_sz=argv[2];
         outFile = argv[3];
    }
    else if (argc == 5 && string (argv[4])=="yes")
    {
        blk_sz=argv[2];
         outFile = argv[3];
        string G="open_mode=read;file_name=";
        G+=argv[1];

        G+=";";

        j_count = calculate_vector_mean(ANS,G.c_str(),1);
        for( int32 IT=1 ; IT<j_count;IT++  )
        {
            //string GX= to_string(ANS[IT].variance);

            printf("j=%d, elements Count= %d, Mean:%f, Variance:%f\n",ANS[IT].j,ANS[IT].count,ANS[IT].mean,ANS[IT].variance);
        }

    }
    else if (argc == 5 && string (argv[3])!="yes")
    {
        blk_sz=argv[2];
         outFile = argv[3];

    }
    else
    {
        Help();
        return 1;
    }
    Compute_Mean_Stddev(argv[1],blk_sz ,outFile);
    return 0;
}

void Help()
{
    printf("PROGRAM HELP\n");
    printf("Proper Terminal Arguments:\n");
    printf("stats <input_file_name> <block_num_rows(default=2)> <optional(mandatory if in test mode): output_file_name> <optional:for test mode enter yes>");
    printf("***<input file path> is required and can only accept ijv formats at the moment.***\n");
    printf("***The number you put in the block_num_rows argument determines the block size by rows. Keep in mind that if the number of rows is not a multiple of rows in the input file, then you may get an incomplete last block***\n");
    printf( "***You must include the .csv extenson to save in the proper format. If you leave this argument empty an output.csv file will be created automatically.***\n");
}

void Compute_Mean_Stddev(string file_path,string num_rows, string outfile_path)
{
    CSVfile F(("open_mode=read;"+num_rows+";file_name="+file_path).c_str()); // strings, record blocks
    uint32 i;
    vec* ans;
    uint32 sz =3;
    ans = new vec[sz];
    uint32 head=1;

    real64 * col_vals=new real64[4];
    uint32 col_val_ptr=1;
    string temp;

   // tm_stdout.print("BLOCK-BASED\n");
    // F.set_block_parameters(); // in constructor?
    //F.view();
    // F.print_header();
    i = 1;
    F.seek_block(i);
    while (!F.eof())
    {
        F.read_block(); // last block may be incomplete
        if (!F.eof())
        {


            for (int indx = 1; indx <= F.block_num_rows; indx++)
            {
                // Parse each columns value into array col_vals
                for (int ch = 0; ch < F.fixed_row_size; ch++)
                {
                    if (F.block_row[indx].s[ch] != ',')
                    {
                        temp += F.block_row[indx].s[ch];
                    }

                    if (F.block_row[indx].s[ch] == ',' || F.block_row[indx].s[ch] == '\n')
                    {
                        col_vals[col_val_ptr]= stod(temp);
                        col_val_ptr++;
                        temp.clear();
                    }
                }

                // For each unique j we start summing each of its values and count the number of times it occurs
                //for (int j = 1; j < F.num_columns; j++)
                int32 X=i_available(col_vals[2],ans,sz);
                if(X!=-1){
                    ans[X].sum += col_vals[3];
                    ans[X].count++;
                    }
                else{
                    vec T;
                    T.j=col_vals[2];
                    T.sum+=col_vals[3];
                    T.count++;
                    T.init= true;
                    if (head ==sz-1)
                    {

                        ans[head]=T;
                        vec* t= new vec[sz*2];
                        copy(ans,ans+sz,t);
                        delete[] ans;
                        ans=t;
                        sz*=2;
                        head++;

                    } else
                    {
                        ans[head]=T;
                        head+=1;
                    }

                }
                col_vals[2]=0;
                col_vals[1]=0;
                col_vals[3]=0;
                col_val_ptr=1;

            }
        }
        i++;
    }

    for (uint32 i=1;i<sz;i++)
    {
        if(ans[i].init)
        {
            ans[i].mean=ans[i].sum/ans[i].count;
        }

    }

    i = 1;
    F.seek_block(i);

    while (!F.eof())
    {
        F.read_block(); // last block may be incomplete
        if (!F.eof())
        {
            for (int indx = 1; indx <= F.block_num_rows; indx++)
            {
                // Parse each columns value into array col_vals
                for (int ch = 0; ch < F.fixed_row_size; ch++)
                {
                    if (F.block_row[indx].s[ch] != ',')
                    {
                        temp += F.block_row[indx].s[ch];
                    }

                    if (F.block_row[indx].s[ch] == ',' || F.block_row[indx].s[ch] == '\n')
                    {
                        //col_vals.push_back(stod(temp));
                        col_vals[col_val_ptr]=stod(temp);
                        col_val_ptr++;
                        temp.clear();
                    }
                }

                // On the second pass, we calculate the standard deviation now that we have the mean. In the next phase I will try to get it to a single pass.

                    int32 X=i_available(col_vals[2],ans,sz);
                ans[X].variance+=pow(col_vals[3]-ans[X].mean,2);
                col_vals[1]=0;
                col_vals[2]=0;
                col_vals[3]=0;

                col_val_ptr=1;
            }
        }
        i++;
    }

    for (uint32 i=1;i<sz;i++)
    {
        if(ans[i].init)
        {
            ans[i].variance/=(ans[i].count-1);
        }

    }
    //for (const auto &e : std_dev)
    PrintOutput(ans, outfile_path,sz);
    delete [] col_vals;
}

void PrintOutput(vec* ans, string output_filename,uint32 sz)
{
    string path;
    path = output_filename;

    FILE* ptr = fopen(path.c_str(), "w");

    string XX = "j  uint32";
    string mean = "Mean  real64", stddev = "Standard Deviation  real64";

    string head1((30 - XX.length()), ' ');
    head1 += XX;
    string head2((30 - mean.length()), ' ');
    head2 += mean;
    string head3((30 - stddev.length()), ' ');
    head3 += stddev;
    string header = head1 + ' ' + head2 + ' ' + head3 + '\n';

    printf("\n%s", header.c_str());
    fputs(header.c_str(), ptr);

    for (uint32 i=1;i<sz;i++)
    {
        if(ans[i].init){
            string s1((30 - to_string(ans[i].j).length()), ' ');
            s1 += to_string(ans[i].j);
            string s2((30 - to_string(ans[i].mean).length()), ' ');
            s2 += to_string(ans[i].mean);
            string s3((30 - to_string(ans[i].variance).length()), ' ');
            s3 += to_string(ans[i].variance);
            string final = s1 + "," + s2 + "," + s3 + "\n";

            printf("%s", final.c_str());
            fputs(final.c_str(), ptr);
        }
    }
    delete[] ans;
    fclose(ptr);
}
////////////////////////////////////
uint32 calculate_vector_mean(vec* &ans,const char* path,uint32 offset)
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
            int32 zz=j_available(stoi(b),ans,curr_pointer);
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
uint32 i_available(uint32 L,vec* X,uint32 sz)
{
    for(uint32 j=1;j<sz;j++)
        if(X[j].j==L&& X[j].init==true)
            return j;


    return -1;
}
int32 j_available(uint32 L,vec* X,uint32 sz)
{
    for(uint32 j=1;j<sz;j++)
        if(X[j].j==L && !X[j].padding)
            return j;


    return -1;
}
