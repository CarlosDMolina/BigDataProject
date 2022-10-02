#include <iostream>
#include <iomanip>
#include "foundation.h"
#include "csv.h"
#include "trace.h"
#include "datasettxt.h"
#include <map>
#include <vector>

using namespace std;

void Compute_Mean_Stddev(string file_path, string &outfile_path);
void PrintOutput(map<uint32, real64> &mp, map<uint32, real64> &mp2, string &output_filename);
void Help();

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        string outFile = "output.csv";
        Compute_Mean_Stddev(argv[1], outFile);
    }
    else if (argc == 3)
    {
        string outFile = argv[2];
        Compute_Mean_Stddev(argv[1], outFile);
    }
    else
    {
        Help();
        return 0;
    }

    return 0;
}

void Help()
{
    cout << "PROGRAM HELP\n"
         << endl;
    cout << "Proper Terminal Arguments:\n"
         << endl;
    cout << "stats \"open_mode=read;file_name=<input file path>;block_num_rows=<user choice>;\" <output file name>\n\n"
         << endl;
    cout << "***<input file path> is required and can only accept ijv formats at the moment.***\n"
         << endl;
    cout << "***The number you put in the block_num_rows argument determines the block size by rows. Keep in mind that if the number of rows is not a multiple of rows in the input file, then you may get an incomplete last block***\n"
         << endl;
    cout << "***You must include the .csv extenson to save in the proper format. If you leave this argument empty an output.csv file will be created automatically.***\n"
         << endl;
}

void Compute_Mean_Stddev(string file_path, string &outfile_path)
{
    CSVfile F(file_path.c_str()); // strings, record blocks
    // CSVfile F("open_mode=read;file_name=table2_str.csv;block_num_rows=2;");  // strings, record blocks
    // CSVfile F("open_mode=read;file_name=table3_str.csv;block_num_rows=2;");  // strings, record blocks
    // CSVfile F("open_mode=read;file_name=table4_str.csv;block_num_rows=5;");  // large
    uint32 i;
    map<uint32, real64> sums;
    map<uint32, real64> counts;
    map<uint32, real64> means;
    map<uint32, real64> std_dev;

    vector<real64> col_vals;
    string temp;

    tm_stdout.print("BLOCK-BASED\n");
    // F.set_block_parameters(); // in constructor?
    F.view();
    // F.print_header();
    i = 1;
    F.seek_block(i);
    while (!F.eof())
    {
        F.read_block(); // last block may be incomplete
        if (!F.eof())
        {
            // F.view_block();

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
                        col_vals.push_back(stod(temp));
                        temp.clear();
                    }
                }

                // For each unique j we start summing each of its values and count the number of times it occurs
                for (int j = 1; j < F.num_columns - 1; j++)
                {
                    sums[col_vals[j]] += col_vals[j + 1];
                    counts[col_vals[j]]++;
                }
                col_vals.clear();
            }
        }
        i++;
    }

    for (const auto &e : sums)
    {
        means[e.first] = e.second / (counts.at(e.first));
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
                        col_vals.push_back(stod(temp));
                        temp.clear();
                    }
                }

                // On the second pass, we calculate the standard deviation now that we have the mean. In the next phase I will try to get it to a single pass.
                for (int j = 1; j < F.num_columns - 1; j++)
                {
                    std_dev[col_vals[j]] += pow(col_vals[j + 1] - means.at(col_vals[j]), 2);
                }
                col_vals.clear();
            }
        }
        i++;
    }

    for (const auto &e : std_dev)
    {
        std_dev[e.first] = sqrt(e.second / (counts.at(e.first) - 1));
    }

    PrintOutput(means, std_dev, outfile_path);
}

void PrintOutput(map<uint32, real64> &mp, map<uint32, real64> &mp2, string &output_filename)
{
    string path = "/home/caki/Desktop/";
    path += output_filename;

    FILE *ptr = fopen(path.c_str(), "w");

    char j = 'j';
    string mean = "Mean", stddev = "Standard Deviation";

    string head1((20 - 1), ' ');
    head1 += j;
    string head2((20 - mean.length()), ' ');
    head2 += mean;
    string head3((20 - stddev.length()), ' ');
    head3 += stddev;
    string header = head1 + ' ' + head2 + ' ' + head3 + '\n';

    printf("\n%s", header.c_str());
    fputs(header.c_str(), ptr);

    for (auto it = mp.begin(), its = mp2.begin(); it != mp.end(), its != mp2.end(); ++it, ++its)
    {
        string s1((20 - to_string(it->first).length()), ' ');
        s1 += to_string(it->first);
        string s2((20 - to_string(it->second).length()), ' ');
        s2 += to_string(it->second);
        string s3((20 - to_string(its->second).length()), ' ');
        s3 += to_string(its->second);
        string final = s1 + "," + s2 + "," + s3 + "\n";

        printf("%s", final.c_str());
        fputs(final.c_str(), ptr);
    }

    fclose(ptr);
}