#include "astar.h"

template <typename GraphType, typename Heuristic> 
std::vector<typename GraphType::Edge> Astar<GraphType, Heuristic>::search(size_t s, size_t g) 
{
    start_id = s;
    goal_id  = g;
    openlist.clear();
    closedlist.clear();
    solution.clear();
    Heuristic heuristic;
            
    using Edge = typename GraphType::Edge;
    using Vertex = typename GraphType::Vertex;

    std::map<size_t, double> g_score;
    std::map<size_t, double> f_score;
    std::map<size_t, size_t> came_from;
            
    Vertex const& start_vertex = graph.GetVertex(start_id);
    Vertex const& goal_vertex  = graph.GetVertex(goal_id);
            
    g_score[start_id] = 0.0;
    f_score[start_id] = heuristic(graph, start_vertex, goal_vertex);
    openlist.insert(start_id);

    while (openlist.size() > 0) 
    {
        size_t current_id = *(openlist.begin());

        for (const auto& t : openlist) 
        {
            size_t t_heu = heuristic(graph, graph.GetVertex(t), goal_vertex);
            size_t current_heu = heuristic(graph, graph.GetVertex(current_id), goal_vertex);

            if(f_score[t] > f_score[current_id] ||( f_score[t] == f_score[current_id] && t_heu < current_heu))
            {
                current_id = t;
            }
        }

        openlist.erase(openlist.begin());
        
       // std::cout<<"current id"<<current_id<<"graph"<<graph.GetVertex(current_id).ID()<<std::endl;

        if (current_id == goal_id) 
        {
            //std::cout<<"goal_id"<<goal_id<<std::endl;
            size_t trace_id = goal_id;
            while (trace_id != start_id) 
            {
                size_t prev_id = came_from[trace_id];
                const auto &edges = graph.GetOutEdges(prev_id);
                for (const Edge &edge : edges) 
                {
                    if (edge.GetID2() == trace_id) 
                    {
                        //std::cout<<"edge.GetID()"<<edge.GetID1()<<std::endl;
                        solution.push_back(edge);
                        break;
                    }
                }
                trace_id = prev_id;
            }
            std::reverse(solution.begin(), solution.end()); 

            callback.OnFinish(*this);
            return solution;
        }
        
        closedlist.insert(current_id);
        
        for (const Edge& neighbor : graph.GetOutEdges(current_id)) 
        {
            size_t neighbor_id = neighbor.GetID2();
                
            if (closedlist.find(neighbor_id) != closedlist.end())
            {
                continue;
            }

            double tentative_g = g_score[current_id] + neighbor.GetWeight();

            if (g_score.find(neighbor_id) == g_score.end() || tentative_g < g_score[neighbor_id]) 
            {
                g_score[neighbor_id] = tentative_g;
                f_score[neighbor_id] = tentative_g + heuristic(graph, graph.GetVertex(neighbor_id), goal_vertex);
                came_from[neighbor_id] = current_id; 
                openlist.insert(neighbor_id);
            }
        }

        callback.OnIteration(*this);
    }

    callback.OnFinish( *this );
    return solution;
}