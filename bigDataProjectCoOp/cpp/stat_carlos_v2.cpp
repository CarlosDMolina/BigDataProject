#include <iostream>
#include <iomanip>
#include "foundation.h"
#include "csv.h"
#include "trace.h"
#include <map>
#include <vector>

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
    vec()
    {
        j=0;
        count=0;
        variance=0;
        mean=0;
        sum=0;
        init= false;
    }
};
uint32 i_available(uint32 L,vec* X,uint32 sz)
{
    for(uint32 j=1;j<sz;j++)
        if(X[j].j==L&& X[j].init==true)
            return j;


    return -1;
}
void Compute_Mean_Stddev(string file_path, string &outfile_path);
void PrintOutput(vec* ans, string &output_filename,uint32 sz);
void Help();
int main(int argc, char *argv[])
{
    string outFile;
    if (argc == 2)
    {
        outFile = "output.csv";
    }
    else if (argc == 3)
    {
        string outFile = argv[2];
    }
    else
    {
        Help();
        return 1;
    }
    Compute_Mean_Stddev(argv[1], outFile);
    return 0;
}

void Help()
{
    printf("PROGRAM HELP\n");
    printf("Proper Terminal Arguments:\n");
    printf("stats \"open_mode=read;file_name=<input file path>;block_num_rows=<user choice>;\" <output file name>\n\n");
    printf("***<input file path> is required and can only accept ijv formats at the moment.***\n");
    printf("***The number you put in the block_num_rows argument determines the block size by rows. Keep in mind that if the number of rows is not a multiple of rows in the input file, then 
you may get an incomplete last block***\n");
    printf( "***You must include the .csv extenson to save in the proper format. If you leave this argument empty an output.csv file will be created automatically.***\n");
}

void Compute_Mean_Stddev(string file_path, string &outfile_path)
{
    CSVfile F(("open_mode=read;file_name="+file_path).c_str()); // strings, record blocks
    uint32 i;
    vec* ans;
    uint32 sz =3;
    ans = new vec[sz];
    uint32 head=1;

    map<uint32, real64> sums;
    map<uint32, real64> counts;
    map<uint32, real64> means;
    map<uint32, real64> std_dev;
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

void PrintOutput(vec* ans, string &output_filename,uint32 sz)
{
    string path;
    path = output_filename;

    FILE *ptr = fopen(path.c_str(), "w");

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
