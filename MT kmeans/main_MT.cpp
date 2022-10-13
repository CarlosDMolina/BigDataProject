#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <omp.h>
using namespace std;
class Point{
public:
    double i, j,v;     // coordinates
    int cluster;     // no default cluster
    double minDist;  // default infinite dist to nearest cluster

    Point() {
        i=0;
        j=0;
        v=0;
        cluster=0;
        minDist=__DBL_MAX__;
    }

    Point(double x, double y) {
        j=x;
        v=y;
        cluster=-1;
        minDist=__DBL_MAX__;
    }
    double distance(Point p) {
        return (p.j - j) * (p.j - j) + (p.v - v) * (p.v - v);
    }



};
std::vector<std::string> splitString(std::string str, char splitter){
    std::vector<std::string> result;
    std::string current = "";
    for(int i = 0; i < str.size(); i++){
        if(str[i] == splitter){
            if(current != ""){
                result.push_back(current);
                current = "";
            }
            continue;
        }
        current += str[i];
    }
    if(current.size() != 0)
        result.push_back(current);
    return result;
}
long int sz;
vector<Point> read_dataset(string file_name)
{
    string line;
    vector<Point> ds;
    ifstream myfile (file_name);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            vector<string> splt= splitString(line,' ');
            if (splt.size()==1){
                sz = stoi(splt[0]);

            } else{
                ds.push_back(Point(stoi(splt[1]), stoi(splt[2])));


            }
        }
        myfile.close();
    }

    return ds;
}
vector<Point> read_dataset_w(string file_name,long int start_point)
{
    string line;
    vector<Point> ds;
    ifstream myfile (file_name);
    if (myfile.is_open())
    {
        long int cntr=0;
        while ( getline (myfile,line) )
        {
        if(cntr >start_point){
            vector<string> splt= splitString(line,' ');
            if (splt.size()==1){
                sz = stoi(splt[0]);

            } else{
                ds.push_back(Point(stoi(splt[1]), stoi(splt[2])));


            }}
            else
            cntr++;
        }
        myfile.close();
    }
    
    return ds;
}

void kMeansClustering_seq(vector<Point> points, int epochs, int k,int thread_count,bool test_mode)
{

    vector<Point> centroids;
    srand(4);  // need to set the random seed
    for (int i = 0; i < k; ++i) {
        centroids.push_back(points.at(rand() % points.size()));
    }

    //// points for clusters
    
    for(int X=0;X<epochs;X++){
auto st=omp_get_wtime();
        for (vector<Point>::iterator c = begin(centroids);c != end(centroids); ++c) {
            // quick hack to get cluster index
            int clusterId = c - begin(centroids);

#pragma omp parallel for
            for (vector<Point>::iterator it = points.begin(); it != points.end(); ++it) {
                Point p = *it;
                double dist = c->distance(p);
                if (dist < p.minDist) {
                    p.minDist = dist;
                    p.cluster = clusterId;
                }
                *it = p;
            }

        }
        vector<vector<int>>nPoints_threads;
        vector<vector<double>>sumX_threads,sumY_threads;
        vector<int> nPoints;
        vector<double> sumX, sumY;

// Initialise with zeroes
        for (int j = 0; j < k; ++j) {
            nPoints.push_back(0);
            sumX.push_back(0.0);
            sumY.push_back(0.0);
        }
        for(int th=0;th<thread_count;th++)
        {
        nPoints_threads.push_back(nPoints);
        sumX_threads.push_back(sumX);
        sumY_threads.push_back(sumY);
        }
        #pragma omp parallel for
        for (vector<Point>::iterator it = points.begin();
             it != points.end(); ++it) {
            int clusterId = it->cluster;
            
            nPoints_threads[omp_get_thread_num()][clusterId] += 1;
            sumX_threads[omp_get_thread_num()][clusterId] += it->j;
            sumY_threads[omp_get_thread_num()][clusterId] += it->v;
            it->minDist = __DBL_MAX__;  // reset distance
        }
// Compute the new centroids
        for(int th=0;th<thread_count;th++)
        {
            for(long int cx=0;cx<nPoints_threads[th].size();cx++)
            {
                nPoints[cx]+=nPoints_threads[th][cx];
                sumX[cx]+=sumX_threads[th][cx];
                sumY[cx]+=sumY_threads[th][cx];
            }
        
        }
        for (vector<Point>::iterator c = begin(centroids);
             c != end(centroids); ++c) {
            int clusterId = c - begin(centroids);
            c->j = sumX[clusterId] / nPoints[clusterId];
            c->v = sumY[clusterId] / nPoints[clusterId];
        }
    if(X%50==0&& test_mode)
    cout<<"EPOCH:"<<X<<"  ITERATION TIME:"<<omp_get_wtime()-st<<endl;
    }

    ofstream myfile;
    myfile.open("output.csv");
    myfile << "x,y,c" << endl;

    for (vector<Point>::iterator it = points.begin();
         it != points.end(); ++it) {
        myfile << it->j << "," << it->v << "," << it->cluster << endl;
    }
    myfile.close();
}
int main(int argc, char const *argv[])
{
int epochs,k,num_threads;
string fp;
bool window_mode = false;
int window_size=10;
bool test_mode=true;
if(argc <2)
{
cout<<"WRONG INPUTS"<<endl;
cout<< "usage: input_filename epochs(default=200) k(default=5) num_threads(default=4) test_mode(show iteration time use f to not show)  window_mode(enter number for window size if not it will be whole file)(output always in output.csv)"<<endl;
return 1;
}
if(argc ==2)
{
    fp=string(argv[1]);
    epochs=200;
    k=5;
    num_threads=4;
}
else if(argc ==3)
{
    fp=string(argv[1]);
    epochs=stoi(string(argv[2]));
    k=5;
    num_threads=4;
}
else if(argc ==4)
{
    fp=string(argv[1]);
    epochs=stoi(string(argv[2]));
    k=stoi(string(argv[3]));
    num_threads=4;
}
else if(argc ==5)
{
    fp=string(argv[1]);
    epochs=stoi(string(argv[2]));
    k=stoi(string(argv[3]));
    num_threads=stoi(string(argv[4]));
}
else if(argc ==6)
{

fp=string(argv[1]);
    epochs=stoi(string(argv[2]));
    k=stoi(string(argv[3]));
    num_threads=stoi(string(argv[4]));
    if(string(argv[5])=="f")
    test_mode=false;
    else
    {
        cout<<"invalid set to default as true"<<endl;
    
    }



}
else if(argc ==7)
{

fp=string(argv[1]);
    epochs=stoi(string(argv[2]));
    k=stoi(string(argv[3]));
    num_threads=stoi(string(argv[4]));
    if(string(argv[5])=="f")
    test_mode=false;
    else
    {
        cout<<"invalid set to default as true"<<endl;
    
    }
    window_size=stoi(string(argv[6]));
    window_mode = true;


}
else
{
    cout<<"TOO MANY INPUTS"<<endl;
    return 1;
}

omp_set_num_threads(num_threads);
    cout<<"FILE NAME:"<<fp<<endl;
    cout<<"epoch count:"<<epochs<<endl;
    cout<<"k:"<<k<<endl;
    cout<<"thread count:"<<num_threads<<endl;
    auto st=omp_get_wtime();
    if (window_mode == false){
    vector<Point>ds=read_dataset(fp);
    kMeansClustering_seq(ds,epochs,k,num_threads,test_mode);
    }
    else
    {
        int start_point=0;
        while(start_point<sz-start_point||sz==0)
        {
            vector<Point>ds=read_dataset_w(fp,start_point);
            kMeansClustering_seq(ds,epochs,k,num_threads,test_mode);
            start_point+=window_size;
        }
    }
    cout<<"TOTAL WTIME TIME:"<<omp_get_wtime()-st<<endl;
    return 0;
}
