#include "graph.h"

Graph::Graph(std::vector<std::vector<int>>& data) {
    int node_number = int(data.size()) + 1;
    std::vector<int> rd;
    rng(rd, node_number - 1, 0, 20);
    for (int i = 0; i < node_number - 1; i++) {
        std::vector<int> tv = data[i];
        node_list.push_back(Node(tv[0], tv[1], tv[2], tv[3], tv[4], tv[5]));
    }
    node_list.push_back(Node(data[0][0], data[0][1], data[0][2], data[0][3], data[0][4], data[0][5]));
    int index = 0;
    head = std::vector<int>(110, -1);
    for (int i = 0; i < node_number; i++) {
        for (int j = 0; j < node_number; j++) {
            if (i == j || j == 0 || i == node_number - 1 || (i == 0 && j == node_number - 1))
                continue;
            double dist = euclid_dist(node_list[i].x, node_list[i].y, node_list[j].x, node_list[j].y);
            if (node_list[i].twl + node_list[i].service_time + dist > node_list[j].twu)
                continue;
            //edge_list.push_back(Edge(i, j, dist - rd[i], dist, head, index));
            edge_list.push_back(Edge(i, j, dist - fixed_rand[i], dist, head, index));
        }
    }
    outgoing_edge_sort();
}

void Graph::outgoing_edge_sort() {
    for(int i = 0; i < node_list.size(); i++) {
        std::vector<T> ix;
        int edge_ix = head[i];
        while (edge_ix != -1) {
            ix.push_back(T(edge_ix, edge_list[edge_ix].cost));
            edge_ix = edge_list[edge_ix].next;
        }
        std::sort(ix.begin(), ix.end(), [](T const &a, T const &b) { return a.cost < b.cost; });
        std::vector<int> ans;
        for (auto t : ix) ans.push_back(t.ix);
        node_list[i].outgoing_index = ans;
    }
}

int Graph::get_edge(int u, int v) {
    int edge_ix = head[u];
    while (edge_ix != -1) {
        if (v == edge_list[edge_ix].to)
            return edge_ix;
        edge_ix = edge_list[edge_ix].next;
    }
    return -1;
}

double Graph::euclid_dist(int x1, int y1, int x2, int y2) {
    return sqrt(pow(double(x2 - x1), 2.0) + pow(double(y2 - y1), 2.0));
}

void Graph::rng(std::vector<int>& rd, int num, int l, int r) { // Random Number Generator
    int range = r - l + 1;
    while (num--) {
        rd.push_back(rand() % range + l);
    }
}

void Graph::reset_label() {
    for (int i = 0; i < node_list.size(); i++)
        node_list[i].label = true;
}

void Graph::show_graph() {
    std::cout<<"node number: "<<node_list.size()<<"  edge number: "<<edge_list.size()<<std::endl;

    /*for (Node i : node_list) {

        for (auto j : i.outgoing_index)
            std::cout<<j<<" ";
        std::cout<<"\n\n";
    }
    /*
    for (Edge i : edge_list) {
        std::cout<<i.cost<<" ";
    }
    */

    double cnt = 0.0;
    for (Edge e : edge_list)
        if (e.cost < 0)
            cnt += 1;
    std::cout<<"negative cost edge ratio: "<<cnt / edge_list.size()<<"\n";

}





