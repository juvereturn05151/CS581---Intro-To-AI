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
            
    Vertex const& start_vertex = graph.GetVertex(start_id);
    Vertex const& goal_vertex  = graph.GetVertex(goal_id);
            
    g_score[start_id] = 0.0;
    f_score[start_id] = heuristic(graph, start_vertex, goal_vertex);
    openlist.insert(start_id);
    
    while (openlist.size() > 0) 
    {
        size_t current_id = *(openlist.begin());
        openlist.erase(openlist.begin());
        
        if (current_id == goal_id) 
        {
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
                openlist.insert(neighbor_id);
            }
        }

        callback.OnIteration(*this);
    }

    callback.OnFinish( *this );
    return solution;
}