#include "espprc.h"

Espprc::Espprc(int n, int start, int end, int step, int lt, int ut, int max_capacity, std::vector< std::vector<int> >& data) :\
        n(n), start(start), end(end), step(step), lower_time(lt), upper_time(ut), max_capacity(max_capacity), G(data) {
    lower_bound_matrix = std::vector< std::vector<Label> >(unsigned(((ut - lt)/step) + 10), std::vector<Label>(unsigned(n + 10)));
    naive_dual_bound = double(max_num);
    overall_best_cost = primal_bound = 0.0;
}

double Espprc::reduced_cost(const std::vector<int>& path) {
    if (path.empty()) return 0.0;
    double total_cost = 0.0;
    for (int i = 0; i < path.size() - 1; i++) {
        int ix = G.get_edge(path[i], path[i + 1]);
        total_cost += G.edge_list[ix].cost;
    }
    return total_cost;
}

double Espprc::capacity_consumption(const std::vector<int>& path) {
    double total_demand = 0.0;
    for (int i : path)
        total_demand += G.node_list[i].demand;
    return total_demand;
}

double Espprc::time_consumption(const std::vector<int>& path) {
    if (path.empty()) return 0.0;
    double total_time = 0.0;
    for (int i = 0; i < path.size() - 1; i++) {
        int ix = G.get_edge(path[i], path[i + 1]);
        total_time += G.edge_list[ix].time + G.node_list[path[i]].service_time;
        total_time = std::max(total_time, double(G.node_list[path[i + 1]].twl));
    }
    return total_time;
}

void Espprc::cal_naive_dual_bound() {
    for (Edge e : G.edge_list) {
        if (e.time != 0 &&  e.cost / (double)e.time < naive_dual_bound)
            naive_dual_bound = e.cost / (double)e.time;
    }
}

void Espprc::bound_order() {
    bound_generation_order = std::vector<int>(unsigned(n - 1));
    std::iota(bound_generation_order.begin(), bound_generation_order.end(), 1);
}

bool Espprc::is_feasible(int cur, double capacity, double time) {
    if (capacity + G.node_list[cur].demand > max_capacity || time > G.node_list[cur].twu) //prune condition
        return false;
    return true;
}

bool Espprc::check_bounds(int root, int cur, double time, double cost, bool flag) {
    double lower_bound = 0;
    if (time < time_incumbent + step) {
        double diff_time = time_incumbent + step - time;
        if (diff_time > 0) lower_bound = diff_time * naive_dual_bound + overall_best_cost;
        else std::cout << "Contradictory!\n";
    }
    else {
        try {
            Label lb = lower_bound_matrix.at(unsigned((upper_time - time) / step)).at(unsigned(cur));
            if (!lb.vis) return true;
            lower_bound = lb.best_cost;
            if (fabs(lower_bound - max_num) < err)
                return false;
        } catch(const std::out_of_range& e) {
            std::cout << "Out of range error!\n";
        }
    }
    double best_cost = flag ? primal_bound : G.node_list[root].best_cost;
    if (cost + lower_bound >= best_cost) //prune condition
        return false;
    return true;
}

bool Espprc::rollback(int cur, double cost, const std::vector<int>& path) {
    if (path.size() < 2 || G.get_edge(*(path.end() - 2), cur) == -1)
        return true;
    std::vector<int> alt_path(path.begin(), path.end() - 1);
    alt_path.push_back(cur);
    if (cost >= reduced_cost(alt_path)) //prune condition
        return false;
    return true;
}

void Espprc::dynamic_update(int cur, const std::vector<int>& opt_path) {
    double path_cost = reduced_cost(std::vector<int>(std::find(opt_path.begin(), opt_path.end(), cur), opt_path.end()));
    if (path_cost < G.node_list[cur].best_cost)
        G.node_list[cur].best_cost = path_cost;
}

void Espprc::pulse_procedure(int root, int cur, double cost, double capacity, double time, std::vector<int>& path, bool flag) {
    if (time < G.node_list[cur].twl) time = G.node_list[cur].twl;
    if (!is_feasible(cur, capacity, time) || !check_bounds(root, cur, time, cost, flag) || !rollback(cur, cost, path))
        return;

    std::vector<int> opt_path;
    path.push_back(cur);
    double nx_cost = 0.0, nx_capacity = capacity + G.node_list[cur].demand, nx_time = 0.0;
    for (int edge_ix : G.node_list[cur].outgoing_index) {
        std::vector<int> new_path(path.begin(), path.end());
        int successor = G.edge_list[edge_ix].to;
        nx_cost = cost + G.edge_list[edge_ix].cost;
        nx_time = std::max(double(G.node_list[successor].twl), time + G.node_list[cur].service_time + G.edge_list[edge_ix].time);
        if (G.node_list[successor].label) {
            G.node_list[successor].label = false;
            pulse_procedure(root, successor, nx_cost, nx_capacity, nx_time, new_path, flag);
            G.node_list[successor].label = true;
        }
        if (!new_path.empty() && new_path.back() == end && ((opt_path.empty()) || reduced_cost(new_path) < reduced_cost(opt_path))) {
            opt_path = new_path;
            dynamic_update(cur, opt_path);
        }
    }
    if (path.back() != end) { path = opt_path; }

    if (!path.empty() && path.back() == end) {
        double tmp = reduced_cost(path);
        G.node_list[root].best_cost = std::min(G.node_list[root].best_cost, tmp);
        if (tmp < primal_bound) {
            primal_bound = tmp;
            //std::cout<<"primal_bound: "<<primal_bound<<std::endl;
        }
    }
}

void Espprc::bounding_scheme() {
    bound_order();
    cal_naive_dual_bound();
    int bound_index = 0;
    time_incumbent = upper_time - step;
    std::vector< std::vector<int> > pre_path(n);
    while (time_incumbent >= lower_time) {
        //std::cout<<"bound time: "<<time_incumbent<<std::endl;
        for (int root : bound_generation_order) {
            G.reset_label();
            std::vector<int> path;
            G.node_list[root].label = false;
            pulse_procedure(root, root, 0.0, 0.0, double(time_incumbent), path, false);
            if (!pre_path[root].empty() && path.empty()) path = pre_path[root];
            lower_bound_matrix[bound_index][root] =\
                Label(G.node_list[root].best_cost, time_consumption(path), capacity_consumption(path), path);
            pre_path[root] = path;
        }

        overall_best_cost = primal_bound;
        time_incumbent -= step;
        bound_index += 1;
    }
}

void Espprc::espprc() {
    //std::cout<<"---------bound begin---------\n";
    bounding_scheme();
    //std::cout<<"---------bound end---------\n";
    G.reset_label();
    G.node_list[start].label = false;
    primal_bound = 0.0;
    std::vector<int> opt_path;
    pulse_procedure(start, start, 0.0, 0.0, 0.0, opt_path, true);

    std::cout<<"min cost: "<<reduced_cost(opt_path)<<std::endl<<"optimal path: ";

    for (int i : opt_path)
        std::cout<<i<<" ";
    std::cout<<std::endl;
}


void Espprc::print_para() {
    std::cout<<"n,start,end,step,lt,ut,max_capacity: "<<n<<" "<<start<<" "<<\
        end<<" "<<step<<" "<<lower_time<<" "<<upper_time<<" "<<max_capacity<<std::endl;
}





























