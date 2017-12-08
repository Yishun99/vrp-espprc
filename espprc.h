#include "graph.h"

class Label {
public:
    double best_cost, time_consumption, capacity_consumption;
    bool vis;
    std::vector<int> path;
    Label() : vis(false) {}
    Label(const double& bc, const double& tc, const double& cc, const std::vector<int>& path) :\
        best_cost(bc), time_consumption(tc), capacity_consumption(cc), path(path), vis(true) {}
};

class Espprc {
private:
    int n, start, end, step, lower_time, upper_time, max_capacity, time_incumbent;
    double naive_dual_bound, overall_best_cost, primal_bound;

    std::vector<int> bound_generation_order;
    std::vector< std::vector<Label> > lower_bound_matrix;

    double reduced_cost(const std::vector<int>& path);
    double capacity_consumption(const std::vector<int>& path);
    double time_consumption(const std::vector<int>& path);

    bool is_feasible(int cur, double capacity, double time);
    bool check_bounds(int root, int cur, double time, double cost, bool flag);
    bool rollback(int cur, double cost, const std::vector<int>& path);

    bool intersection(const std::vector<int>& path1, const std::vector<int>& path2);
    bool concat(int root, int cur, double time, double cost, double capacity, std::vector<int>& path, bool flag);

    void pulse_procedure(int root, int cur, double cost, double capacity, double time, std::vector<int>& path, bool flag);
    void dynamic_update(int cur, const std::vector<int>& opt_path);
    void bounding_scheme();
    void cal_naive_dual_bound();
    void bound_order();
public:
    Graph G;
    Espprc(int n, int start, int end, int step, int lt, int ut, int max_capacity, std::vector< std::vector<int> >& data);

    void print_para();
    void espprc();

};













