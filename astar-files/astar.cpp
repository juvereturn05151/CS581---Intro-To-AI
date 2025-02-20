#include "astar.h"

template <typename GraphType, typename Heuristic>
std::vector<typename GraphType::Edge> Astar<GraphType, Heuristic>::search(size_t s, size_t g) 
{
    start_id = s;
    goal_id  = g;
    openlist = {};  
    solution.clear();
    
    Heuristic heuristic;

    using Edge = typename GraphType::Edge;
    using Vertex = typename GraphType::Vertex;

    std::unordered_map<size_t, double> g_score;
    std::unordered_map<size_t, double> f_score;
    std::unordered_map<size_t, size_t> came_from;

    Vertex const& start_vertex = graph.GetVertex(start_id);
    Vertex const& goal_vertex  = graph.GetVertex(goal_id);

    g_score[start_id] = 0.0;
    f_score[start_id] = heuristic(graph, start_vertex, goal_vertex);

    openlist.push({f_score[start_id], start_id});

    while (!openlist.empty()) 
    {
        size_t current_id = openlist.top().second;
        openlist.pop();

        if (current_id == goal_id) 
        {
            size_t trace_id = goal_id;
            while (trace_id != start_id) 
            {
                size_t prev_id = came_from[trace_id];
                for (const Edge& edge : graph.GetOutEdges(prev_id)) 
                {
                    if (edge.GetID2() == trace_id) 
                    {
                        solution.push_back(edge);
                        break;
                    }
                }
                trace_id = prev_id;
            }

            callback.OnFinish(*this);
            return solution;
        }

        for (const Edge& neighbor : graph.GetOutEdges(current_id)) 
        {
            size_t neighbor_id = neighbor.GetID2();

            double tentative_g = g_score[current_id] + neighbor.GetWeight();

            if (g_score.find(neighbor_id) == g_score.end() || tentative_g < g_score[neighbor_id]) 
            {
                g_score[neighbor_id] = tentative_g;
                f_score[neighbor_id] = tentative_g + heuristic(graph, graph.GetVertex(neighbor_id), goal_vertex);
                came_from[neighbor_id] = current_id;
                
                openlist.push({f_score[neighbor_id], neighbor_id});
            }
        }

        callback.OnIteration(*this);
    }

    callback.OnFinish(*this);
    return solution;
}