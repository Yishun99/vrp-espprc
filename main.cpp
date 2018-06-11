#include "espprc.h"
#include <sstream>
#include <fstream>
#include <ctime>

void reader(std::vector< std::vector<int> >& data, std::string& filename, int& max_capacity) {
    std::ifstream infile(filename);
    std::string line;
    int tmp;
    for (int i = 0; i < 9; i++) {
        std::getline(infile, line);
        if (i == 4) {
            std::istringstream iss(line);
            iss >> tmp >> max_capacity;
        }
    }
    while (std::getline(infile, line)) {
        if (line.size() < 2) break;
        std::istringstream iss(line);
        std::vector<int> tv;
        iss >> tmp;
        while (iss >> tmp)
            tv.push_back(tmp);
        data.push_back(tv);
    }
}


int main(int args, char *argv[]) {
    std::string filename = argv[1];
    int max_capacity;
    std::vector< std::vector<int> > data;
    reader(data, filename, max_capacity);

    int node_number = int(data.size()) + 1, step = 10, lt = int(0.1 * data[0][4]), ut = data[0][4];
    Espprc model(node_number, 0, node_number - 1, step, lt, ut, max_capacity, data);
    //model.G.show_graph(); model.print_para();

    double duration;
    std::clock_t start = std::clock();
    model.espprc();
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    std::cout<<"duration: "<<duration<<std::endl;
    return 0;
}
