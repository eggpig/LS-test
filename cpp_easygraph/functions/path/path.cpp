#include "path.h"

#include "../../classes/graph.h"
#include "../../common/utils.h"
#include "../../classes/linkgraph.h"
#include<ext/pb_ds/priority_queue.hpp>

Graph_L graph_to_linkgraph(Graph &G, bool if_directed, std::string weight_key){
    int node_num = G.node.size();
    const std::vector<graph_edge>& edges = G._get_edges();
    int edges_num = edges.size();
    Graph_L G_l(node_num, if_directed);
    for(register int i = 0; i < edges_num; i++){
        graph_edge e = edges[i];
        edge_attr_dict_factory& edge_attr = e.attr;
        G_l.add_edge(e.u,e.v, edge_attr[weight_key]);
        if (!if_directed){
            G_l.add_edge(e.v, e.u, edge_attr[weight_key]);
        }
            
    }
    return G_l;

}

std::vector<float> _dijkstra(Graph_L &G_l, int source, std::string weight, int target) {
    int N = G_l.n;
    std::vector<float> dis(N+1,INFINITY);
    std::vector<bool> vis(N+1,false);
    __gnu_pbds::priority_queue<compare_node> q;
    dis[source] = 0;
    q.push(compare_node(source, dis[source]));
    std::vector<LinkEdge>& E = G_l.edges;
    std::vector<int>& head = G_l.head;
    while(!q.empty()) {
        int u=q.top().x;
        q.pop();
        if (vis[u]) 
            continue;
        vis[u] = true;
        if(u == target){
            break;
        }
        for(register int p = head[u]; p != -1; p = E[p].next) {
            int v = E[p].to;
            if (dis[v] > dis[u] + E[p].w) {
                dis[v] = dis[u] + E[p].w;
                q.push(compare_node(v, dis[v]));
            }
            
        }
    }
    
    return dis;

}
py::object _dijkstra_multisource(py::object G,py::object sources, py::object weight, py::object target) {
    py::list res_lst = py::list();
    Graph& G_ = G.cast<Graph&>();
    bool is_directed = G.attr("is_directed")().cast<bool>();
    node_t target_id = G_.node_to_id.attr("get")(target, -1).cast<node_t>();
    std::string weight_key = weight_to_string(weight);
    Graph_L G_l = graph_to_linkgraph(G_, is_directed, weight_key);
    int N = G_l.n;
    py::list sources_list = py::list(sources);
    int sources_list_len = py::len(sources_list);
    for(register int i = 0; i < sources_list_len; i++){
        node_t source_id = sources_list[i].cast<node_t>();
        const std::vector<float>& dis = _dijkstra(G_l,source_id,weight_key,target_id);
        py::list pydist = py::list();
        for(int i = 1;i<=N;i++){
            pydist.append(py::cast(dis[i]));
        }
        res_lst.append(pydist);
    }
    
    return res_lst;
}


py::object _spfa(py::object G, py::object source, py::object weight) {
    Graph& G_ = G.cast<Graph&>();
    bool is_directed = G.attr("is_directed")().cast<bool>();
    std::string weight_key = weight_to_string(weight);
    Graph_L G_l = graph_to_linkgraph(G_, is_directed, weight_key);
    int N = G_.node.size();

    int Q[N+10];
    memset(Q, 0, sizeof(int) * (N+10));
    std::vector<double> dis(N+1,INFINITY);
    std::vector<bool> vis(N+1,false);

    int l = 0, r = 1;
    node_t S = G_.node_to_id[source].cast<node_t>();
	Q[0] = S; vis[S] = true; dis[S] = 0;
    std::vector<LinkEdge>& E = G_l.edges;

    std::vector<int>& head = G_l.head;
    while (l != r) {
    	if (r != 0 && dis[Q[l]] >= dis[Q[r - 1]])
    		std::swap(Q[l], Q[r - 1]);
        int u = Q[l++]; 
        if (l >= N) l -= N; 
        vis[u] = true;

        for(register int p = head[u]; p != -1; p = E[p].next) {
            int v=E[p].to; 
            if (dis[v]>dis[u]+E[p].w) {
                dis[v]=dis[u]+E[p].w; 
                if (!vis[v]) {
                    vis[v]=true;
                    if (l == 0 || dis[v] >= dis[Q[l]])
						Q[r++]=v;
					else
						Q[--l]=v;
                    if (r >= N) r -= N;
                }
            }
        }
	}
    py::list pydist = py::list();
    for(int i = 1; i <= N; i++){
        pydist.append(dis[i]);
    }
    return pydist;
}

// py::object _dijkstra(py::object G,py::object sources, py::object weight, py::object target) {
//     Graph& G_ = G.cast<Graph&>();
//     bool is_directed = G.attr("is_directed")().cast<bool>();
//  //   std::cout<<"is_directed:"<<is_directed<<std::endl;
//     node_t target_id = G_.node_to_id.attr("get")(target, -1).cast<node_t>();
//  //   printf("target_id%d\n",target_id);
//     std::string weight_key = weight_to_string(weight);
//     Graph_L G_l = graph_to_linkgraph(G_, is_directed, weight_key);
//     int N = G_.node.size();
//  //   printf("N:%d\n",N);
//     std::vector<double> dis(N+1,INFINITY);
//     std::vector<bool> vis(N+1,false);
    
//     __gnu_pbds::priority_queue<compare_node> q;
    
//     py::list sources_list = py::list(sources);
//     int sources_list_len = py::len(sources_list);
//     for (register int i = 0; i < sources_list_len; i++) {
//         node_t source = G_.node_to_id[sources_list[i]].cast<node_t>();
//       //  printf("source_id%d\n",source);
//         dis[source] = 0;
//         q.push(compare_node(source, dis[source]));
//     //    printf("dis[source]:%2f\n",dis[source]);
//     }

//     std::vector<LinkEdge>& E = G_l.edges;
//    // printf("E_len:%d\n",E.size());
//     std::vector<int>& head = G_l.head;
//     while(!q.empty()) {
//         // printf("enter\n");
//         int u=q.top().x;
//         // printf("u:%d\n",u);
//         q.pop();
//         if (vis[u]) 
//             continue;
//         vis[u] = true;
//         if(u == target_id){
//             break;
//         }
//         for(register int p = head[u]; p != -1; p = E[p].next) {
//             int v = E[p].to;
//             // printf("v:%d p:%d\n",v,p);
//             if (dis[v] > dis[u] + E[p].w) {
//                 // printf("enter2\n");
//                 dis[v] = dis[u] + E[p].w;
//                 q.push(compare_node(v, dis[v]));
//                 // printf("enter3\n");
//             }
            
//         }
//     }
//     // printf("finish_first\n");
//     py::dict pydist = py::dict();
//     for(int i = 1; i <= N; i++){
//         pydist[G_.id_to_node[py::cast(i)]] = dis[i];
//     }
//     return pydist;

// }

// py::object _dijkstra_multisource(py::object G, py::object sources, py::object weight, py::object target) {
//     Graph& G_ = G.cast<Graph&>();
//     std::string weight_key = weight_to_string(weight);
//     node_t target_id = G_.node_to_id.attr("get")(target, -1).cast<node_t>();
//     std::map<node_t, weight_t> dist, seen;
//     std::priority_queue<std::pair<weight_t, node_t>, std::vector<std::pair<weight_t, node_t>>, std::greater<std::pair<weight_t, node_t>>> Q;
//     py::list sources_list = py::list(sources);
//     int sources_list_len = py::len(sources_list);
//     for (int i = 0; i < sources_list_len; i++) {
//         node_t source = G_.node_to_id[sources_list[i]].cast<node_t>();
//         seen[source] = 0;
//         Q.push(std::make_pair(0, source));
//     }
//     while (!Q.empty()) {
//         std::pair<weight_t, node_t> node = Q.top();
//         Q.pop();
//         weight_t d = node.first;
//         node_t v = node.second;
//         if (dist.count(v)) {
//             continue;
//         }
//         dist[v] = d;
//         if (v == target_id) {
//             break;
//         }
//         adj_dict_factory& adj = G_.adj;
//         for (auto& neighbor_info : adj[v]) {
//             node_t u = neighbor_info.first;
//             weight_t cost = neighbor_info.second.count(weight_key) ? neighbor_info.second[weight_key] : 1;
//             weight_t vu_dist = dist[v] + cost;
//             if (dist.count(u)) {
//                 if (vu_dist < dist[u]) {
//                     PyErr_Format(PyExc_ValueError, "Contradictory paths found: negative weights?");
//                     return py::none();
//                 }
//             } else if (!seen.count(u) || vu_dist < seen[u]) {
//                 seen[u] = vu_dist;
//                 Q.push(std::make_pair(vu_dist, u));
//             } else {
//                 continue;
//             }
//         }
//     }
//     py::dict pydist = py::dict();
//     for (const auto& kv : dist) {
//         pydist[G_.id_to_node[py::cast(kv.first)]] = kv.second;
//     }
//     return pydist;
// }

py::object Prim(py::object G, py::object weight) {
    std::unordered_map<node_t, std::unordered_map<node_t, weight_t>> res_dict;
    py::dict result_dict = py::dict();
    Graph& G_ = G.cast<Graph&>();
    adj_dict_factory adj = G_.adj;
    std::vector<node_t> selected;
    std::vector<node_t> candidate;
    node_dict_factory& node_list = G_.node;
    std::string weight_key = weight_to_string(weight);
    for (node_dict_factory::iterator i = node_list.begin(); i != node_list.end(); i++) {
        node_t node_id = i->first;
        result_dict[G_.id_to_node[py::cast(node_id)]] = py::dict();
        if (selected.size() == 0) {
            selected.emplace_back(node_id);
        } else {
            candidate.emplace_back(node_id);
        }
    }
    while (candidate.size() > 0) {
        node_t start_id = -1;
        node_t end_id = -1;
        weight_t min_weight = INFINITY;
        int selected_len = selected.size();
        int candidate_len = candidate.size();
        for (int i = 0; i < selected_len; i++) {
            for (int j = 0; j < candidate_len; j++) {
                adj_attr_dict_factory node_adj = G_.adj[selected[i]];
                edge_attr_dict_factory edge_attr;
                weight_t edge_weight = INFINITY;
                bool j_exist = false;
                if (node_adj.find(candidate[j]) != node_adj.end()) {
                    edge_attr = node_adj[candidate[j]];
                    edge_weight = edge_attr.find(weight_key) != edge_attr.end() ? edge_attr[weight_key] : 1;
                    j_exist = true;
                }
                if ((node_list.find(selected[i]) != node_list.end()) &&
                    j_exist &&
                    (edge_weight < min_weight)) {
                    start_id = selected[i];
                    end_id = candidate[j];
                    min_weight = edge_weight;
                }
            }
        }
        if (start_id != -1 && end_id != -1) {
            res_dict[start_id][end_id] = min_weight;
            selected.emplace_back(end_id);
            std::vector<node_t>::iterator temp_iter;
            temp_iter = std::find(candidate.begin(), candidate.end(), end_id);
            candidate.erase(temp_iter);
        } else {
            break;
        }
    }
    for (std::unordered_map<node_t, std::unordered_map<node_t, weight_t>>::iterator k = res_dict.begin();
         k != res_dict.end(); k++) {
        py::object res_node = G_.id_to_node[py::cast(k->first)];
        for (std::unordered_map<node_t, weight_t>::iterator z = k->second.begin(); z != k->second.end(); z++) {
            py::object res_adj_node = G_.id_to_node[py::cast(z->first)];
            result_dict[res_node][res_adj_node] = z->second;
        }
    }
    return result_dict;
}
bool comp(const std::pair<std::pair<node_t, node_t>, weight_t>& a, const std::pair<std::pair<node_t, node_t>, weight_t>& b) {
    return a.second < b.second;
}
py::object Kruskal(py::object G, py::object weight) {
    std::unordered_map<node_t, std::unordered_map<node_t, weight_t>> res_dict;
    py::dict result_dict = py::dict();
    std::vector<std::vector<node_t>> group;
    Graph& G_ = G.cast<Graph&>();
    adj_dict_factory& adj = G_.adj;
    node_dict_factory& node_list = G_.node;
    std::vector<std::pair<std::pair<node_t, node_t>, weight_t>> edge_list;
    std::string weight_key = weight_to_string(weight);
    for (node_dict_factory::iterator i = node_list.begin(); i != node_list.end(); i++) {
        node_t i_id = i->first;
        result_dict[G_.id_to_node[py::cast(i_id)]] = py::dict();
        std::vector<node_t> temp_vector;
        temp_vector.emplace_back(i_id);
        group.emplace_back(temp_vector);
        adj_attr_dict_factory i_adj = adj[i_id];
        for (adj_attr_dict_factory::iterator j = i_adj.begin(); j != i_adj.end(); j++) {
            node_t j_id = j->first;
            weight_t edge_weight = adj[i_id][j_id].find(weight_key) != adj[i_id][j_id].end() ? adj[i_id][j_id][weight_key] : 1;
            edge_list.emplace_back(std::make_pair(std::make_pair(i_id, j_id), edge_weight));
        }
    }
    std::sort(edge_list.begin(), edge_list.end(), comp);
    node_t m, n;
    int group_size = group.size();
    for (auto edge : edge_list) {
        for (int i = 0; i < group_size; i++) {
            int group_i_size = group[i].size();
            for (int j = 0; j < group_i_size; j++) {
                if (group[i][j] == edge.first.first) {
                    m = i;
                    break;
                }
            }
            for (int j = 0; j < group_i_size; j++) {
                if (group[i][j] == edge.first.second) {
                    n = i;
                    break;
                }
            }
        }
        if (m != n) {
            res_dict[edge.first.first][edge.first.second] = edge.second;
            std::vector<node_t> temp_vector;
            group[m].insert(group[m].end(), group[n].begin(), group[n].end());
            group[n].clear();
        }
    }
    for (std::unordered_map<node_t, std::unordered_map<node_t, weight_t>>::iterator k = res_dict.begin();
         k != res_dict.end(); k++) {
        py::object res_node = G_.id_to_node[py::cast(k->first)];
        for (std::unordered_map<node_t, weight_t>::iterator z = k->second.begin(); z != k->second.end(); z++) {
            py::object res_adj_node = G_.id_to_node[py::cast(z->first)];
            result_dict[res_node][res_adj_node] = z->second;
        }
    }
    return result_dict;
}

py::object Floyd(py::object G, py::object weight) {
    std::unordered_map<node_t, std::unordered_map<node_t, weight_t>> res_dict;
    Graph& G_ = G.cast<Graph&>();
    adj_dict_factory& adj = G_.adj;
    py::dict result_dict = py::dict();
    node_dict_factory& node_list = G_.node;
    std::string weight_key = weight_to_string(weight);
    for (node_dict_factory::iterator i = node_list.begin(); i != node_list.end(); i++) {
        result_dict[G_.id_to_node[py::cast(i->first)]] = py::dict();
        adj_attr_dict_factory temp_key = adj[i->first];
        for (node_dict_factory::iterator j = node_list.begin(); j != node_list.end(); j++) {
            if (temp_key.find(j->first) != temp_key.end()) {
                if (adj[i->first][j->first].count(weight_key) == 0) {
                    adj[i->first][j->first][weight_key] = 1;
                }
                res_dict[i->first][j->first] = adj[i->first][j->first][weight_key];
            } else {
                res_dict[i->first][j->first] = INFINITY;
            }
            if (i->first == j->first) {
                res_dict[i->first][i->first] = 0;
            }
        }
    }

    for (node_dict_factory::iterator k = node_list.begin(); k != node_list.end(); k++) {
        for (node_dict_factory::iterator i = node_list.begin(); i != node_list.end(); i++) {
            for (node_dict_factory::iterator j = node_list.begin(); j != node_list.end(); j++) {
                weight_t temp = res_dict[i->first][k->first] + res_dict[k->first][j->first];
                weight_t i_j_weight = res_dict[i->first][j->first];
                if (i_j_weight > temp) {
                    res_dict[i->first][j->first] = temp;
                }
            }
        }
    }

    for (std::unordered_map<node_t, std::unordered_map<node_t, weight_t>>::iterator k = res_dict.begin();
         k != res_dict.end(); k++) {
        py::object res_node = G_.id_to_node[py::cast(k->first)];
        for (std::unordered_map<node_t, weight_t>::iterator z = k->second.begin(); z != k->second.end(); z++) {
            py::object res_adj_node = G_.id_to_node[py::cast(z->first)];
            result_dict[res_node][res_adj_node] = z->second;
        }
    }
    return result_dict;
}
